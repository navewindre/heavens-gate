// by navewindre
// github.com/navewindre

#pragma once
#include <Windows.h>
#include <winternl.h>
#include <vector>
#include "x86.h"
#include "winintern.h"


struct MODULE_EXPORT {
  const char* name;
  void*       base;
};

inline void* nt_get_address() {
  U32 fs_offset = (U32)&( (NT_TIB*)nullptr )->Self;

  PTEB teb = (PTEB)( __readfsdword( fs_offset ) );
  PEB* peb = teb->ProcessEnvironmentBlock;

  auto ldr_entry = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink[0].Flink;
  auto data_entry = ldr_entry->Reserved2[0]; 

  return data_entry;
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
}

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

extern NTSTATUS64 nt_open_process64(
  HANDLE* handle,
  U32 desired_access,
  _OBJECT_ATTRIBUTES64* obj_attrbitues,
  _CLIENT_ID_T<U64>* client_id
);