//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include <Windows.h>
#include <winternl.h>
#include <vector>
#include "x86.h"
#include "winintern.h"
#include "asmutil.h"
#include "util.h"

struct MODULE_EXPORT {
  const char* name;
  void*       base;
};

struct MODULE_EXPORT64 {
  STR<256> name;
  U64 base;
};

inline void* nt_get_address() {
  U32 fs_offset = (U32)&( (NT_TIB*)nullptr )->Self;

  PTEB teb = (PTEB)( __readfsdword( fs_offset ) );
  PEB* peb = teb->ProcessEnvironmentBlock;

  auto ldr_entry = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink[0].Flink;
  auto data_entry = ldr_entry->Reserved2[0]; 

  return data_entry;
}

inline U64 nt_get_address64() {
  REG64 _ret;

  heavens_gate_enter();

  {
    db( 0x65 ) db( 0x48 ) db( 0x8b ) db( 0x04 ) db( 0x25 )
    db( 0x30 ) db( 0x00 ) db( 0x00 ) db( 0x00 ) // mov rax, qword ptr[gs:0x30]  // get TEB
    db( 0x48 ) db( 0x8b ) db( 0x40 ) db( 0x60 ) // mov rax, qword ptr[rax+0x60] // TEB->PEB
    db( 0x48 ) db( 0x8b ) db( 0x40 ) db( 0x18 ) // mov rax, qword ptr[rax+0x18] // PEB->LDR
    db( 0x48 ) db( 0x8b ) db( 0x40 ) db( 0x10 ) // mov rax, qword ptr[rax+0x10] // LDR->InMemList
    db( 0x48 ) db( 0x8b ) db( 0x00 )            // mov rax, qword ptr[rax]      // *InMemList.Flink
    db( 0x48 ) db( 0x8b ) db( 0x40 ) db( 0x30 ) // mov rax, qword ptr[rax+0x30] // entry->DllBase
  }

  __asm {
    rex_w mov _ret.u32[0], eax
  }
  
  heavens_gate_exit();

  return _ret.u64;
}

inline std::vector< MODULE_EXPORT > module_get_exports( void* module_base ) {
  auto dos_header = (IMAGE_DOS_HEADER*)( module_base );
  auto nt_headers = (IMAGE_NT_HEADERS*)( (U32)module_base + dos_header->e_lfanew );
  auto data_dir   = nt_headers->OptionalHeader.DataDirectory[0].VirtualAddress;
  auto export_dir = (IMAGE_EXPORT_DIRECTORY*)( data_dir + (U32)module_base );

  U32* names = (U32*)( (U32)module_base + export_dir->AddressOfNames );
  U32* funcs = (U32*)( (U32)module_base + export_dir->AddressOfFunctions );
  U16* ords  = (U16*)( (U32)module_base + export_dir->AddressOfNameOrdinals );

  std::vector< MODULE_EXPORT > ret;
  for( U32 i = 0; i < export_dir->NumberOfNames; ++i ) {
    const char* name = (const char*)( (U32)module_base + names[i] );
    void*       func = (void*)( (U32)module_base + funcs[ords[i]] );

    ret.push_back( { name, func } );
  }

  return ret;
};

extern NTSTATUS64 nt_create_thread64(
  REG64* thread,
  ACCESS_MASK mask,
  _OBJECT_ATTRIBUTES64* object_attributes,
  HANDLE process_handle,
  LPTHREAD_START_ROUTINE start_routine,
  void* parameter,
  U32 suspended,
  U32 stack_zero_bits = 0,
  U32 stack_commit = 0,
  U32 stack_reserve = 0
);

extern NTSTATUS64 nt_close64(
  REG64 handle
);

inline NTSTATUS64 nt_close64( HANDLE handle ) {
  return nt_close64( (U64)handle );
}

extern NTSTATUS64 nt_open_process64(
  HANDLE* handle,
  U32 desired_access,
  _OBJECT_ATTRIBUTES64* obj_attrbitues,
  _CLIENT_ID_T<U64>* client_id
);

extern NTSTATUS64 nt_write_vm64(
  HANDLE handle,
  U64 address, // imagine lmfao
  void* value,
  ULONG size,
  U64* out_ret_bytes = 0
);

extern NTSTATUS64 nt_read_vm64(
  HANDLE handle,
  U64 address,
  void* buffer,
  ULONG size,
  U64* out_ret_bytes = 0
);

extern NTSTATUS64 nt_query_vm64(
  HANDLE handle,
  U64 address,
  WIN32_MEMORY_INFORMATION_CLASS information_class,
  void* memory_information,
  U64 memory_information_length,
  U64* return_length = 0
);

extern NTSTATUS64 nt_allocate_vm64(
  HANDLE handle,
  U64* allocated_address,
  ULONG zero_bits,
  U64* region_size,
  ULONG allocation_type,
  ULONG protect
);

extern NTSTATUS64 nt_free_vm64(
  HANDLE handle,
  U64* address,
  U64* size,
  ULONG free_type
);

extern NTSTATUS64 nt_query_system_information64(
  SYSTEM_INFORMATION_CLASS info_class,
  void* system_information,
  ULONG system_infromation_length,
  ULONG* return_length
);

extern NTSTATUS64 nt_query_information_process64(
  HANDLE handle,
  PROCESSINFOCLASS info_class,
  void* process_information,
  ULONG process_information_length,
  ULONG* out_information_length
);

extern NTSTATUS64 nt_delay_execution64(
  BOOLEAN alterable,
  LARGE_INTEGER* delay_interval
);

extern NTSTATUS64 nt_query_object64(
  HANDLE handle,
  I32 info_class,
  void* object_information,
  ULONG object_information_length,
  ULONG* return_length
);

inline std::vector< MODULE_EXPORT64 > module_get_exports( U32 module_base, HANDLE proc = (HANDLE)-1 ) {
  std::vector< MODULE_EXPORT64 > ret;
  IMAGE_DOS_HEADER               dos_header;
  IMAGE_NT_HEADERS               nt_headers;
  U32                            data_dir;
  IMAGE_EXPORT_DIRECTORY         export_dir;

  nt_read_vm64( proc, module_base, &dos_header, sizeof( dos_header ) );
  nt_read_vm64( proc, module_base + dos_header.e_lfanew, &nt_headers, sizeof( nt_headers ) );
  data_dir = nt_headers.OptionalHeader.DataDirectory[0].VirtualAddress;
  nt_read_vm64( proc, module_base + data_dir, &export_dir, sizeof( export_dir ) );

  U32 names = module_base + export_dir.AddressOfNames;
  U32 funcs = module_base + export_dir.AddressOfFunctions;
  U32 ords = module_base + export_dir.AddressOfNameOrdinals;

  char name[256]{};
  U32  func = 0;
  U16  ord;
  U32  str_ptr = 0;

  for( U32 i = 0; i < export_dir.NumberOfNames; ++i ) {
    nt_read_vm64( proc, names + 0x4 * i, &str_ptr, 0x4 );
    nt_read_vm64( proc, module_base + str_ptr, name, 256 );

    nt_read_vm64( proc, ords + 0x2 * i, &ord, 0x2 );
    nt_read_vm64( proc, funcs + 0x4 * ord, &func, 0x4 );
    
    name[255] = 0;
    ret.push_back( { STR<256>( name ), module_base + func } );
  }

  return ret;
}

inline std::vector< MODULE_EXPORT64 > module_get_exports64( U64 module_base, HANDLE proc = (HANDLE)-1 ) {
  std::vector< MODULE_EXPORT64 > ret;
  IMAGE_DOS_HEADER               dos_header;
  IMAGE_NT_HEADERS64             nt_headers;
  U64                            data_dir;
  IMAGE_EXPORT_DIRECTORY         export_dir;

  nt_read_vm64( proc, module_base, &dos_header, sizeof( dos_header ) );
  nt_read_vm64( proc, module_base + dos_header.e_lfanew, &nt_headers, sizeof( nt_headers ) );
  data_dir = nt_headers.OptionalHeader.DataDirectory[0].VirtualAddress;
  nt_read_vm64( proc, module_base + data_dir, &export_dir, sizeof( export_dir ) );

  U64 names = module_base + export_dir.AddressOfNames;
  U64 funcs = module_base + export_dir.AddressOfFunctions;
  U64 ords = module_base + export_dir.AddressOfNameOrdinals;

  char name[256]{};
  U64  func = 0;
  U16  ord;
  U64  str_ptr = 0;

  for( U32 i = 0; i < export_dir.NumberOfNames; ++i ) {
    nt_read_vm64( proc, names + 0x4 * i, &str_ptr, 0x4 );
    nt_read_vm64( proc, module_base + str_ptr, name, 256, 0 );

    nt_read_vm64( proc, ords + 0x2 * i, &ord, 0x2 );
    nt_read_vm64( proc, funcs + 0x4 * ord, &func, 0x4 );

    
    ret.push_back( { STR<128>( name ), module_base + func } );
  }

  return ret;
}