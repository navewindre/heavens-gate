#pragma once
#pragma warning( disable: 4102 )

#include "util.h"
#include "asmutil.h"
#include "x86.h"

#define DISASM_SIG() _DISASM_SIG1( __COUNTER__ )
#define _DISASM_SIG1( x ) _DISASM_SIG( x )
#define _DISASM_SIG( x ) \
    db( 0xeb ) db( 0x12 ) db( 0xef ) db( 0xbe ) db( 0xad ) db( 0xde ) \
    __asm mov eax, ##x __asm mov eax, ##x^(##x/2) db( 0xff ) db( 0xff )


struct DISASM_INFO {
  U8* func_start;
  U32 func_length;
  U8* sig_ptr;
  I32 check;
};

const I32 DISASM_SIG_LENGTH = 18;

U8* disasm_find_sig_end( void* funptr, int* out_checknr = 0 ) {
  static const char* signature_str = "eb 12 ef be ad de b8 ? ? ? ? b8 ? ? ? ? ff ff";
  U8 *ret = 0, *ptr = (U8*)funptr;
  U32 sig_size;
  U8* sig_bytes = u_parse_signature( signature_str, &sig_size );

  for( U16 i = 0; i < 64; ++i ) {
    if( u_binary_match( ptr + i, sig_bytes, sig_size ) ) {
      ret = ptr + i; break;
    }
  }
  
  free( sig_bytes );
  if( !ret )
    return 0;
  
  I32* check1 = (I32*)( ret + 7 );
  I32* check2 = (I32*)( ret + 12 );
  if( out_checknr )
    *out_checknr = *check1;
  
  if( *check2 == ( (*check1) ^ (*check1 / 2) ) )
    return ret;

  return 0;
}

U8* disasm_find_ret_instruction( void* funptr ) {
  U8* ptr = (U8*)funptr;
  
  for( U16 i = 0; i < 64; ++i ) {
    if( ptr[i] == RET_FAR
     || ptr[i] == RET_NEAR
     || ptr[i] == RET_FAR_IMM16
     || ptr[i] == RET_NEAR_IMM16 ) {
      return ptr + i;
    }
  }

  return 0;
}

DISASM_INFO disasm_function( void* func ) {
  DISASM_INFO ret{};
  I32 checknr;
  U8* signature = disasm_find_sig_end( func, &checknr );
  if( !signature )
    return ret;

  U8* ret_byte = disasm_find_ret_instruction( func );
  if( !ret_byte )
    return ret;

  ret.func_start = (U8*)func;
  ret.func_length = ret_byte - (U8*)func;
  ret.check = checknr;
  ret.sig_ptr = signature;
  
  return ret;
}