#pragma once
#include "typedef.h"

enum X86Regs_t : U8 {
  eax = 0,
  ecx,
  edx,
  ebx,
  esp,
  ebp,
  esi,
  edi
};

enum X64Regs_t : U8 {
  rax = 0,
  rcx,
  rdx,
  rbx,
  rsp,
  rbp,
  rsi,
  rdi,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15
};

union REG64 {
  REG64() = default;
  REG64( U64 x ) : u64( x ) {}
  REG64( U32 x ) { u32[0] = x; }

  REG64( I64 x ) : u64( *(U64*)( &x ) ) {}
  REG64( I32 x ) { u32[0] = *(U32*)( &x ); } 
  
  U32 u32[2];
  U64 u64;
};

constexpr U32 x86_encode_mov_imm32( U32 reg ) { return ( 0xb8 + reg ); }
constexpr U32 x86_encode_push_reg( U32 reg ) { return 0x50 | ( reg & 7 ); }
constexpr U32 x86_encoded_pop_reg( U32 reg ) { return 0x58 | ( reg & 7 ); }