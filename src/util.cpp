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
  
