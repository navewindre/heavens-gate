// by navewindre
// github.com/navewindre

#pragma once
#include <vector>
#include "typedef.h"
#include "asmutil.h"
#include "process.h"
#include "ntutil.h"

enum X86Regs_t : U32 {
  eax = 0,
  ecx,
  edx,
  ebx,
  esp,
  ebp,
  esi,
  edi
};

struct SYSCALL_ENTRY {
  const char* name;
  void*       base;
  I32         idx;
};

constexpr U32 x86_encode_mov_imm32( U32 reg ) { return ( 0xb8 + reg ); }
constexpr U32 x86_encode_push_reg( U32 reg ) { return 0x50 | ( reg & 7 ); }
constexpr U32 x86_encoded_pop_reg( U32 reg ) { return 0x58 | ( reg & 7 ); }

inline U32 syscall_wrapper_size( const U8* fn, U16* out_ret_bytes = 0 ) {
  for( U32 off = 0; off < 0x30; ++off ) {
    if( fn[off] == 0xc3 /* retn */ ) {
      if( out_ret_bytes )
        *out_ret_bytes = 0;

      return off + 1;
    }
    if( fn[off] == 0xc2 /* retn imm16 */ ) {
      if( out_ret_bytes )
        *out_ret_bytes = *(U16*)( &fn[off + 1] );

      return off + 3;
    }
  }

  return 0;
}

inline U8 syscall_is_syscall( const U8* fn, U32 fn_size ) {
  if( fn[0] != x86_encode_mov_imm32( X86Regs_t::eax ) )
    return false;

  for( U32 off = 0; off < fn_size; ++off ) {
    if( *(U16*)( &fn[off] ) == 0xffd2 /* call edx */ )
      return 1;
  }

  return 0;
}

inline U32 syscall_get_index( const U8* fn, U16* out_ret_bytes = 0 ) {
  U16 ret_bytes{};
  U32 wrapper_size = syscall_wrapper_size( fn, &ret_bytes );

  if( !syscall_is_syscall( fn, wrapper_size ) )
    return 0;
  
  if( out_ret_bytes )
    *out_ret_bytes = ret_bytes;

  for( U32 off = 0; off < wrapper_size; ++off ) {
    if( *( fn + off ) == x86_encode_mov_imm32( X86Regs_t::eax ) )
      return *(U32*)( fn + off + 1 );
  }

  return 0;
}

inline std::vector< SYSCALL_ENTRY > syscall_dump() {
  std::vector< SYSCALL_ENTRY > ret;
  std::vector< MODULE_EXPORT > nt_exports;

  void* nt = nt_get_address();
  nt_exports = module_get_exports( nt );

  for( auto exp : nt_exports ) {
    U32 size = syscall_wrapper_size( (U8*)exp.base );
    if( !size )
      continue;

    if( !syscall_is_syscall( (U8*)exp.base, size ) )
      continue;

    U32 idx = syscall_get_index( (U8*)exp.base );

    SYSCALL_ENTRY e;
    e.base = exp.base;
    e.name = exp.name;
    e.idx = idx;
    
    ret.push_back( e );
  }

  return ret;
}

__declspec( naked ) void NULLSUB_000() {};

union REG64 {
  U32 u32[2];
  U64 u64;
};

typedef U64( __cdecl* fn_type )( U64* );

template < typename ... argt >
NTSTATUS syscall_execute( U64 idx, argt ... args ) {
  U64 args64[] = { (U64)(args)... };

  constexpr U8 stub[] = {
    0x55,                               // 0:   push ebp
    0x89, 0xe5,                         // 2:   mov ebp, esp

    0x83, 0xe4, 0xf0,                   // 5:   and esp, 0xfffffff0
  
    // enter long mode
    0x6a, 0x33,                         // 7:   push 0x33
    0xe8, 0x00, 0x00, 0x00, 0x00,       // 12:  call rel32 0x0
    0x83, 0x04, 0x24, 0x05,             // 16:  add dword ptr[esp], 0x5
    0xcb,                               // 17:  retf

    // copy first 4 args to registers
    0x67, 0x48, 0x8b, 0x4d, 8,          // 22:  mov r10, [ebp + 8]
    0x67, 0x48, 0x8B, 0x55, 16,         // 27:  mov rdx, [ebp + 16]
    0x67, 0x4C, 0x8B, 0x45, 24,         // 32:  mov r8, [ebp + 24]
    0x67, 0x4C, 0x8B, 0x4D, 32,         // 37:  mov r9, [ebp + 32]
    
    0x48, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE,
    0x00, 0x00, 0x00, 0x00,             // 47:  mov rax, param_count

    // check if copy needed
    0xa8, 0x01,                         // 49:  test al, 1
    0x75, 0x04,                         // 51:  jne 6
    0x48, 0x83, 0xec, 0x08,             // 55:  sub rsp, 8

    0x57,                               // 56:  push rdi
    0x67, 0x48, 0x8b, 0x7d, 40,         // 61:  mov rdi, [ebp + 40]
    0x75, 0x06,                         // 63:  jne 8 
    0x74, 0x16, 0x48, 0x8d, 0xc7, 0xf8, // 69:  lea rdi, [rdi+rax*8-8]

    // set up variable width for x64 syscall
    0x48, 0x85, 0xc0,                   // 72:  test rax, rax
    0x74, 0x0c,                         // 74:  je 0c
    0xff, 0x37,                         // 76:  push [rdi]
    0x48, 0x83, 0xef, 0x08,             // 80:  sub rdi, 8
    0x48, 0x83, 0xe8, 0x01,             // 84:  sub rax, 1
    0xeb, 0xef,                         // 86:  jmp rel

    0x48, 0xBF, 0xEF, 0xBE, 0xAD, 0xDE,
    0x00, 0x00, 0x00, 0x00,             // 96:  mov rdi, return_ptr
    0x47, 0x83, 0xec, 0x20,             // 100: sub rsp, 0x20

    // do the syscall
    0x4c, 0x8b, 0xd1,                   // 103: mov r10, rcx
    0xb8, 0xde, 0xad, 0xbe, 0xef,       // 108: mov eax, syscall_index
    0x0f, 0x05,                         // 110: syscall

    // un-fuck the stack
    0x48, 0x89, 0x07,                   // 113: mov [rdi], rax
    0x67, 0x48, 0x8b, 0x4d, 64,         // 119: mov rcx, [ebp+64]
    0x48, 0x8d, 0x64, 0xcc, 0x20,       // 125: lea rsp, [rsp+rcx*8+64]
    0x5f,                               // 126: pop rdi

    // exit long mode
    0xe8, 0x00, 0x00, 0x00, 0x00,       // 131: call rel32 0x0
    0xc7, 0x44, 0x24, 0x04,
    0x23, 0x00, 0x00, 0x00,             // 139: mov dword ptr[rsp+4], 0x23
    0x83, 0x04, 0x24, 0x0d,             // 143: retf

    0x66, 0x8c, 0xd8,                   // 146: mov ax, ds
    0x8e, 0xd0,                         // 148: mov ss, eax

    0x89, 0xec,                         // 150: mov esp, ebp
    0x5d,                               // 151: pop ebp
    0xcc                                // 152: ret
  };

  void* stub_alloc = VirtualAlloc(
    0,
    sizeof( stub ),
    MEM_COMMIT | MEM_RESERVE,
    PAGE_EXECUTE_READWRITE
  );

  REG64 ret{};
  
  memcpy( stub_alloc, stub, sizeof( stub ) );
  *(U64*)( &stub_alloc[41] ) = sizeof...( args );
  *(U32*)( &stub_alloc[98] ) = (U32)idx;  
  ( (REG64*)&stub_alloc[88] )->u32[0] = &ret;
  
  fn_type fn = ( fn_type )( stub_alloc );
  fn( args64 );

  VirtualFree( stub_alloc, sizeof( stub ), MEM_FREE );

  return (NTSTATUS)ret.u64;
}
