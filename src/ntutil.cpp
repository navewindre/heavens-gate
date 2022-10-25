#include "ntutil.h"
#include "syscall.h"

// it's big nigga season

NTSTATUS64 nt_create_thread64(
  REG64* thread,
  ACCESS_MASK mask,
  _OBJECT_ATTRIBUTES64* object_attributes,
  HANDLE process_handle,
  LPTHREAD_START_ROUTINE start_routine,
  void* parameter,
  U32 suspended,
  U32 stack_zero_bits,
  U32 stack_commit,
  U32 stack_reserve
) {
  static SYSCALL_ENTRY nt_create_thread = syscall_find_syscall( "NtCreateThreadEx"fnv );

  REG64 thread_handle_ptr;
  REG64 access64;
  REG64 parameter64;
  REG64 suspended64;
  REG64 stack_zero_bits64;
  REG64 stack_commit64;
  REG64 stack_reserve64;
  REG64 process64;
  REG64 unk64{};
  
  REG64 start;
  REG64 object_attributes_ptr;

  start.u64 = (U64)start_routine;
  access64.u64 = mask;
  process64.u64 = (U64)GetCurrentProcess();
  thread_handle_ptr.u64 = { (U64)thread };
  object_attributes_ptr.u64 = (U64)object_attributes;
  suspended64.u64 = suspended;
  parameter64.u64 = (U64)parameter;
  
  stack_zero_bits64.u64 = stack_zero_bits;
  stack_commit64.u64 = stack_commit;
  stack_reserve64.u64 = stack_reserve;
  
  NTSTATUS64 status = syscall_execute( nt_create_thread.idx,
    thread_handle_ptr,
    access64,
    object_attributes_ptr,
    process64,
    start,
    parameter64,
    suspended64,
    stack_zero_bits64,
    stack_commit64,
    stack_reserve64,
    unk64
  );

  return status;
}

NTSTATUS64 nt_close64( REG64 handle ) {
  static SYSCALL_ENTRY nt_close = syscall_find_syscall( "NtClose"fnv );

  NTSTATUS64 status = syscall_execute( nt_close.idx, handle );
  return status;
}

NTSTATUS64 nt_open_process64(
  HANDLE* handle,
  U32 desired_access,
  _OBJECT_ATTRIBUTES64* obj_attrbitues,
  _CLIENT_ID_T<U64>* client_id
) {
  static SYSCALL_ENTRY nt_open_process = syscall_find_syscall( "NtOpenProcess"fnv );

  REG64 handle64 = (U64)handle;
  REG64 desired_access64 = (U64)desired_access;
  REG64 object_attributes64 = (U64)obj_attrbitues;
  REG64 client_id64 = (U64)client_id;

  NTSTATUS64 status = syscall_execute( nt_open_process.idx,
    handle64,
    desired_access64,
    object_attributes64,
    client_id64
  );

  return status;
}
