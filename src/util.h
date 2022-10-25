// by navewindre
// github.com/navewindre

#pragma once

#include <windows.h>

#include "syscall.h"

template < U32 size >
struct STR {
  STR() = default;
  STR( const char* other ) { strcpy_s< size >( data, other ); }
  template < U32 size2 >
  STR( STR< size2 >&& other ) { strcpy_s< size >( data, other ); }

  operator char*() { return data; }
  
  char data[size]{};
};

inline ULONG u_thread_create( LPTHREAD_START_ROUTINE routine, void* param = 0 ) {
  _OBJECT_ATTRIBUTES64 attr{};
  REG64                thread;
  ULONG                ret_id;

  attr.Length = sizeof( attr );

  nt_create_thread64( &thread, 0xffff, &attr, GetCurrentProcess(), routine, param, 0 ); 
  ret_id = GetThreadId( (HANDLE)thread.u32[0] );
  
  
  return ret_id;
}

template < typename t >
STR< 32 > u_num_to_string_hex( t num ) {
  STR< 32 > ret;

  sprintf( ret.data, "%08X", num );
  return ret;
}