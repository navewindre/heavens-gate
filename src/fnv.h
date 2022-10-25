#pragma once

#include "typedef.h"

typedef U32 FNV1A;

enum : FNV1A {
  FNV1A_PRIME = 0x1000193,
  FNV1A_BASIS = 0x811C9DC5
};

inline constexpr U32 strlen_ct( const char* str ) {
  U32 s = 0;
  for( ; !!str[s]; ++s );

  return s;
}

inline constexpr FNV1A fnv1a( const U8* data, const U32 size ) {
  FNV1A out = FNV1A_BASIS;

  for( U32 i = 0; i < size; ++i )
    out = ( out ^ data[i] ) * FNV1A_PRIME;

  return out;
}

inline constexpr FNV1A fnv1a( const char* str ) {
  U32 len = strlen_ct( str );

  return fnv1a( (const U8*)str, len );
}

#pragma warning( disable : 4455 )
constexpr FNV1A operator""fnv( const char* str, U32 len ) {
  return fnv1a( (const U8*)str, len );
}