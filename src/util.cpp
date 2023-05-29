//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "util.h"
#include "syscall.h"

ULONG u_thread_create( LPTHREAD_START_ROUTINE routine, void* param ) {
  REG64                thread;
  ULONG                ret_id;
  
  nt_create_thread64( &thread, 0x1fffff, 0, GetCurrentProcess(), routine, param, 0 ); 
  ret_id = GetThreadId( (HANDLE)thread.u32[0] );
  
  nt_close64( thread );
    
  return ret_id;
}
  
ULONG u_thread_create( HANDLE proc, LPTHREAD_START_ROUTINE routine, void* param ) {
  REG64                thread;
  ULONG                ret_id;
  
  nt_create_thread64( &thread, 0x1fffff, 0, proc, routine, param, 0 ); 
  ret_id = GetThreadId( (HANDLE)thread.u32[0] );
  
  nt_close64( thread );
    
  return ret_id; 
}

void u_sleep( U64 ns ) {
  static bool resolution_set = false;
  if( !resolution_set ) {
    ULONG timer_resolution;
    nt_set_timer_resolution64( 1, true, &timer_resolution );
    resolution_set = true;
  }

  LARGE_INTEGER interval;
  interval.QuadPart = -1 * ns;
  nt_delay_execution64( false, &interval );
}
