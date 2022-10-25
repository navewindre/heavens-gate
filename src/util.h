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
  REG64                thread;
  ULONG                ret_id;

  nt_create_thread64( &thread, 0x1fffff, 0, GetCurrentProcess(), routine, param, 0 ); 
  ret_id = GetThreadId( (HANDLE)thread.u32[0] );

  nt_close64( thread );
  
  return ret_id;
}

template < typename t >
STR< 32 > u_num_to_string_hex( t num ) {
  STR< 32 > ret;

  sprintf( ret.data, "%08X", num );
  return ret;
}