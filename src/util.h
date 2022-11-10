// by navewindre
// github.com/navewindre

#pragma once
#include <windows.h>
#include <stdio.h>
#include "typedef.h"

extern ULONG u_thread_create( LPTHREAD_START_ROUTINE routine, void* param = 0 );

template < U32 size >
struct STR {
  STR() = default;
  STR( const char* other ) { strcpy_s< size >( data, other ); }
  template < U32 size2 >
  STR( STR< size2 >&& other ) { strcpy_s< size >( data, other ); }

  operator char*() { return data; }
  
  char data[size]{};
};


template < typename t >
STR< 32 > u_num_to_string_hex( t num ) {
  STR< 32 > ret;

  sprintf( ret.data, "%08X", num );
  return ret;
}

template < U32 size = 128 >
STR< size > u_widebyte_to_ansi( wchar_t* str ) {
  STR< size > ret;

  for( U32 i = 0; !!str[i]; ++i ) {
    ret.data[i] = str[i] & 0xff;
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