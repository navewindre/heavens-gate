// by navewindre
// github.com/navewindre

#pragma once

#include <windows.h>
#include "typedef.h"

template < U32 size >
struct STRING {
  STRING() = default;
  STRING( const char* other ) { strcpy_s< size >( data, other ); }
  template < U32 size2 >
  STRING( STRING< size2 >&& other ) { strcpy_s< size >( data, other ); }

  operator char*() { return data; }
  
  char data[size]{};
};

inline ULONG u_thread_create( LPTHREAD_START_ROUTINE routine, void* param = 0 ) {
  ULONG ret_id;
  HANDLE thread = CreateThread( 0, 0, routine, param, 0, &ret_id );
  CloseHandle( thread );
  
  return ret_id;
}

template < typename t >
STRING< 32 > u_num_to_string_hex( t num ) {
  STRING< 32 > ret;

  sprintf( ret.data, "%08X", num );
  return ret;
}