// by navewindre
// github.com/navewindre

#pragma once
#include <vector>
#include "x86.h"
#include "fnv.h"
#include "asmutil.h"
#include "conout.h"
#include "ntutil.h"
#include "winintern.h"

struct SYSCALL_ENTRY {
  STR<64>     name;
  FNV1A       hash;
  U64         base;
  I32         idx;
};


inline U32 syscall_wrapper_size( const U8* fn, U16* out_ret_bytes = 0 ) {
  for( U32 off = 0; off < 0x18; ++off ) {
    // x64
    {
      if( *(U16*)(&fn[off]) == 0x050f     /* syscall */
       && *(U16*)(&fn[off + 3]) == 0x2ecd /* int 2e */
      ) {
        if( out_ret_bytes )
          *out_ret_bytes = 0;
        return off + 5;
      }
    }

    // x86
    {
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
  }

  return 0;
}

inline U8 syscall_is_syscall( const U8* fn, U32 fn_size ) {
  // x64
  if( fn[0] == 0x4c && fn[1] == 0x8b && fn[2] == 0xd1 ) { // mov r10, rcx
    return ( fn_size > 0 && fn_size < 0x20 ) ? 2 : 0;
  }
  
  // x86
  {
    if( fn[0] != x86_encode_mov_imm32( X86Regs_t::eax ) )
      return false;

    for( U32 off = 0; off < fn_size; ++off ) {
      if( *(U16*)( &fn[off] ) == 0xd2ff /* call edx */ )
        return 1;
    }
  }
    
  return 0;
}

inline U32 syscall_get_index( const U8* fn, U32 size, U16* out_ret_bytes = 0 ) {
  U16 ret_bytes{};
  U8  syscall_type = syscall_is_syscall( fn, size );

  if( !syscall_type )
    return 0;
  
  if( out_ret_bytes )
    *out_ret_bytes = ret_bytes;

  // same for x86/x64
  for( U32 off = 0; off < size; ++off ) {
    if( *( fn + off ) == x86_encode_mov_imm32( X86Regs_t::eax ) )
      return *(U32*)( fn + off + 1 );
  }

  return 0;
}

inline std::vector< SYSCALL_ENTRY > syscall_dump() {
  static std::vector< SYSCALL_ENTRY > ret;
  static std::vector< MODULE_EXPORT > nt_exports;

  // ntdll can't change during runtime
  if( !ret.empty() )
    return ret;
  
  void* nt = nt_get_address();
  if( nt_exports.empty() )
    nt_exports = module_get_exports( nt );

  for( auto exp : nt_exports ) {
    U32 size = syscall_wrapper_size( (U8*)exp.base );
    if( !size )
      continue;
    
    if( !syscall_is_syscall( (U8*)exp.base, size ) )
      continue;

    U32 idx = syscall_get_index( (U8*)exp.base, size );

    SYSCALL_ENTRY e;
    e.base = (U64)exp.base;
    e.name = exp.name;
    e.hash = fnv1a( e.name );
    e.idx = idx;
    
    ret.push_back( e );
  }

  return ret;
}

inline std::vector< SYSCALL_ENTRY > syscall_dump64() {
  static std::vector< SYSCALL_ENTRY >   ret{};
  static std::vector< MODULE_EXPORT64 > nt_exports{};

  if( !ret.empty() )
    return ret;

  U64 nt64 = nt_get_address64();
  if( nt_exports.empty() ) 
    nt_exports = module_get_exports64( nt64 );

  U8* syscall_wrapper = (U8*)VirtualAlloc(
    0,
    4096,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_READWRITE
  );
  
  for( auto exp : nt_exports ) {
    nt_read_vm64( (HANDLE)-1, exp.base, syscall_wrapper, 0x30 );

    U32 size = syscall_wrapper_size( syscall_wrapper );
    if( !size )
      continue;

    if( !syscall_is_syscall( syscall_wrapper, size ) )
      continue;

    U32 idx = syscall_get_index( syscall_wrapper, size );

    SYSCALL_ENTRY e;
    e.base = exp.base;
    e.name = exp.name.data;
    e.hash = fnv1a( exp.name );
    e.idx = idx;

    ret.push_back( e );
  }

  VirtualFree( syscall_wrapper, 4096, MEM_FREE );
  return ret;
}

inline SYSCALL_ENTRY syscall_find_syscall( FNV1A syscall_hash ) {
  static std::vector< SYSCALL_ENTRY > syscalls = syscall_dump();
  
  for( auto& syscall : syscalls ) {
    if( syscall.hash == syscall_hash )
      return syscall;
  }

  return {};
}

inline SYSCALL_ENTRY syscall_find_syscall64( FNV1A syscall_hash ) {
  std::vector< SYSCALL_ENTRY > syscalls = syscall_dump64();

  for( auto& syscall : syscalls ) {
    if( syscall.hash == syscall_hash )
      return syscall;
  }

  return {}; 
}


template < typename ... argt >
NTSTATUS64 __cdecl syscall_execute( U32 idx, argt ... args ) {
  REG64 args64[] = { (REG64)(args)... };
  
  U64   argc = sizeof...( argt );
  U16   arg  = 0;
  REG64 _rcx = arg < argc? (REG64)args64[arg++] : 0;
  REG64 _rdx = arg < argc? (REG64)args64[arg++] : 0;
  REG64 _r8  = arg < argc? (REG64)args64[arg++] : 0;
  REG64 _r9  = arg < argc? (REG64)args64[arg++] : 0;
  REG64 _rax = {};

  REG64 argc64 = ( argc - arg );
  REG64 argp = (U64)&args64[arg];

  REG64 idx64 = (U64)idx;

  U32 _esp;
  U16 _fs;

  // backup fs and esp to make restoring simpler
  __asm {
    mov _fs, fs
    mov _esp, esp
    
    mov eax, 0x2b
    mov fs, ax

    // align stack to 16
    and esp, 0xfffffff0
  }

  heavens_gate_enter();

  // x64 syscall calling convention
  // standard fastcall, first 4 args go in registers
  __asm {
    rex_wr mov edx, _rcx.u32[0] // mov r10, a0 
    rex_w  mov edx, _rdx.u32[0] // mov edx, a1
    rex_w  mov ecx, _rcx.u32[0]
    rex_wr mov eax, _r8.u32[0]  // mov r8, a2
    rex_wr mov ecx, _r9.u32[0]  // mov r9, a3
  }

  __asm {
    rex_w  mov eax, argc64.u32[0]
           test al, 1
           jnz stack_ok
           // most syscalls are fine with the stack being aligned to 16 bytes
           // but few specific ones crash
           // adjust based on no. of args
           sub esp, 8
  }
  
stack_ok:

  __asm {
           push edi
    rex_w  mov edi, argp.u32[0] // mov rdi, argp
    rex_w  lea edi, dword ptr[edi + 8 * eax - 8]
  }

arg_loop:

  __asm {
    rex_w  test eax, eax       // test rax, rax
           jz loop_end
           push dword ptr[edi]
    rex_w  sub edi, 8          // sub rdi, 8
    rex_w  sub eax, 1          // sub rax, 1
           jmp arg_loop
  }
loop_end:

  __asm {
    rex_w  mov eax, idx64.u32[0]
    // make stack space for syscall
    rex_w  sub esp, 0x28 // this fucker cost me a night

    // do the epic
           db( 0x0f ) db( 0x05 ) // syscall
  }

  //unfuck the stack
  __asm {
    rex_w  mov ecx, argc64.u32[0]
    rex_w  lea esp, dword ptr[esp + 8 * ecx + 0x28]

    pop edi

    // store 64 bits of rax on the stack
    rex_w  mov _rax.u32[0], eax
  }

  heavens_gate_exit();

  __asm {
    // restore stack segment
    mov ax, ds
    mov ss, ax

    // restore esp and fs
    mov esp, _esp
    mov ax, _fs
    mov fs, ax
  }

  return _rax.u64;
}
