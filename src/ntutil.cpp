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

  REG64 start = (U64)start_routine;
  REG64 access64 = (U64)mask;
  REG64 process64 = (U64)process_handle;
  REG64 thread_handle_ptr = (U64)thread;
  REG64 object_attributes_ptr = (U64)object_attributes;
  REG64 suspended64 = (U64)suspended;
  REG64 parameter64 = (U64)parameter;
  
  REG64 stack_zero_bits64 = stack_zero_bits;
  REG64 stack_commit64 = stack_commit;
  REG64 stack_reserve64 = stack_reserve;

  REG64 unk64{};
  
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

  REG64 handle64{}; 
  REG64 desired_access64 = (U64)desired_access;
  REG64 object_attributes64 = (U64)obj_attrbitues;
  REG64 client_id64 = (U64)client_id;

  NTSTATUS64 status = syscall_execute( nt_open_process.idx,
    (REG64)(U64)&handle64,
    desired_access64,
    object_attributes64,
    client_id64
  );

  *handle = (HANDLE)handle64.u32[0];
  return status;
}

NTSTATUS64 nt_write_vm64(
  HANDLE handle,
  U64 address,
  void* value,
  ULONG size,
  U64* out_ret_bytes
) {
  static SYSCALL_ENTRY nt_write_vm = syscall_find_syscall( "NtWriteVirtualMemory"fnv );

  REG64 handle64 = (U64)handle;
  REG64 address64 = address;
  REG64 value64 = (U64)value;
  REG64 size64 = (U64)size;
  REG64 out_ret_bytes64 = (U64)out_ret_bytes;

  NTSTATUS64 status = syscall_execute( nt_write_vm.idx,
    handle64,
    address64,
    value64,
    size64,
    out_ret_bytes64
  );

  return status;
}

NTSTATUS64 nt_read_vm64(
  HANDLE handle,
  U64 address,
  void* buffer,
  ULONG size,
  U64* out_ret_bytes
) {
  static SYSCALL_ENTRY nt_write_vm = syscall_find_syscall( "NtReadVirtualMemory"fnv );

  REG64 handle64 = (U64)handle;
  REG64 address64 = address;
  REG64 buffer64 = (U64)buffer;
  REG64 size64 = (U64)size;
  REG64 out_ret_bytes64 = (U64)out_ret_bytes; 

  NTSTATUS64 status = syscall_execute( nt_write_vm.idx,
    handle64,
    address64,
    buffer64,
    size64,
    out_ret_bytes64
  );

  return status;
}

NTSTATUS64 nt_query_vm64(
  HANDLE handle,
  U64 address,
  WIN32_MEMORY_INFORMATION_CLASS information_class,
  void* memory_information,
  U64 memory_information_length,
  U64* return_length
  ) {
  static SYSCALL_ENTRY nt_query_vm = syscall_find_syscall( "NtQueryVirtualMemory"fnv );

  REG64 handle64 = (U64)handle;
  REG64 address64 = address;
  REG64 info_class64 = (U64)information_class;
  REG64 memory_information64 = (U64)memory_information;
  REG64 memory_information_length64 = memory_information_length;
  REG64 return_length64 = (U64)return_length;

  NTSTATUS64 status = syscall_execute( nt_query_vm.idx,
    handle64,
    address64,
    info_class64,
    memory_information64,
    memory_information_length64,
    return_length64
  );

  return status;
}
