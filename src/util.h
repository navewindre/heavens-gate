//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include <chrono>
#include <windows.h>
#include <stdio.h>
#include <thread>

#include "typedef.h"

constexpr U64 T_MS = 10000;
constexpr U64 T_SEC = 1000 * T_MS;
constexpr U64 T_MIN = 60 * T_SEC;
constexpr U64 T_HOUR = 60 * T_MIN;
constexpr U64 T_DAY = 24 * T_HOUR;

extern ULONG u_thread_create( LPTHREAD_START_ROUTINE routine, void* param = 0 );
extern ULONG u_thread_create( HANDLE proc, LPTHREAD_START_ROUTINE routine, void* param = 0 );
extern void  u_sleep( U64 ns );

template < U32 size >
struct STR {
  STR() = default;
  STR( const char* other ) { strcpy_s< size >( data, other ); }
  template < U32 size2 >
  STR( STR< size2 >&& other ) { strcpy_s< size >( data, other ); }

  operator char*() { return data; }
  
  char data[size]{};
};

template < U32 size >
struct WSTR {
  WSTR() = default;
  WSTR( const wchar_t* other ) { wstrcpy_s< size >( data, other ); }
  template < U32 size2 >
  WSTR( WSTR< size2 >&& other ) { wstrcpy_s< size >( data, other ); }

  operator wchar_t*() { return data; }
  
  wchar_t data[size]{};
};

template < typename t >
STR< 32 > u_num_to_string_hex( t num ) {
  STR< 32 > ret;

  sprintf( ret.data, "%08X", num );
  return ret;
}

template < typename t >
STR< 32 > u_num_to_string_dec( t num ) {
  STR< 32 > ret;

  sprintf( ret.data, "%lld", (I64)num );
  return ret;
}

template < U32 size = 128 >
STR< size > u_widebyte_to_ansi( const wchar_t* str ) {
  STR< size > ret;

  for( U32 i = 0; !!str[i]; ++i ) {
    ret.data[i] = str[i] & 0xff;
  }

  return ret;
}

template < U32 size = 128 >
WSTR< size > u_ansi_to_widebyte( const char* str ) {
  WSTR< size > ret;

  for( U32 i = 0; !!str[i]; ++i ) {
    ret.data[i] = str[i];
  }

  return ret;
}

inline U8 u_set_debug_privilege() {
  HANDLE           token;
  TOKEN_PRIVILEGES tkp{};

  if( !OpenProcessToken(
    GetCurrentProcess(),
    TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
    &token
  ) ) {
    return 0;
  }

  if( !LookupPrivilegeValueA( 0, SE_DEBUG_NAME, &tkp.Privileges->Luid ) ) {
    CloseHandle( token );
    return 0;
  }

  tkp.PrivilegeCount = 1;
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if( !AdjustTokenPrivileges( token, false, &tkp, 0, nullptr, 0 ) ) {
    CloseHandle( token );
    return 0;
  }

  CloseHandle( token );

  return 1;
}

inline U8 u_binary_match( U8* code, U8* pattern, U32 size ) {
  for( U32 i = 0; i < size; ++i ) {
    if( pattern[i] && (code[i] ^ pattern[i]) != 0 )
      return 0;
  }

  return 1;
}

inline U8* u_parse_signature( const char* sig, U32* out_len ) {
  U32 i, byte, len = strlen( sig );
  U8* sig_bytes = (U8*)malloc( len );

  for( i = 0, byte = 0; i < len; ++byte ) {
    if( sig[i] == ' ' )
      return 0;

    if( sig[i] == '?' ) {
      sig_bytes[byte] = 0;
      for( U32 i2 = i; i2 < len; ++i2 ) {
        if( sig[i2 + 1] == ' ' ) {
          i = i2 + 2;
          break;
        }
      }
        
      continue;
    }

    unsigned long temp;
    sscanf( &sig[i], "%02x", &temp );

    sig_bytes[byte] = (U8)( temp & 0xff );
    i += 3;
  }

  if( out_len )
    *out_len = byte;
  return sig_bytes;
}

template <typename t>
inline t u_vector_search( VECTOR<t> v, bool( *func)( t* t1 ) ) {
  for( auto& it : v ) {
    if( func( &it ) )
      return it;
  }

  return {};
}

inline U64 u_tick() {
  static LARGE_INTEGER freq;
  if( !freq.QuadPart )
    QueryPerformanceFrequency( &freq );

  LARGE_INTEGER ticks;
  QueryPerformanceCounter( &ticks );

  constexpr U64 wanted_precision = 10000000;
  if( wanted_precision == freq.QuadPart )
    return ticks.QuadPart;
  return (U64)(ticks.QuadPart / ((double)freq.QuadPart / wanted_precision) );
}