#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#include "ntutil.h"
#include "winintern.h"
#include "typedef.h"
#include "fnv.h"

struct MODULE_ENTRY {
  U64     base;
  U64     size;
  STR<64> name;
  FNV1A   hash;
};

class PROCESS32 {
private:
  HANDLE      m_base{};
  I32         m_id{};
  char        m_name[256]{};

private:

  
public:
  PROCESS32( const char* name ) {
    memset( m_name, 0, 256 );
    memcpy( m_name, name, strlen( name ) );
  };

  HANDLE get_base() { return m_base; }

  I8 open() {
    const U32 PINFO_ALLOC_SIZE = 0x400000;
    _SYSTEM_PROCESS_INFORMATION64* pinfo;
    ULONG received_bytes;

    pinfo = (_SYSTEM_PROCESS_INFORMATION64*)VirtualAlloc(
      0,
      PINFO_ALLOC_SIZE,
      MEM_COMMIT | MEM_RESERVE,
      PAGE_READWRITE
    );
  
    NTSTATUS64 status = nt_query_system_information64(
      SystemProcessInformation,
      pinfo,
      PINFO_ALLOC_SIZE,
      &received_bytes
    );

    if( status != STATUS_SUCCESS )
      return 0;

    do {
      if( pinfo->ImageName.Buffer ) {
        STR< 128 > pname = u_widebyte_to_ansi( (wchar_t*)(U32)pinfo->ImageName.Buffer );
        if( !strcmp( pname, m_name ) ) {
          m_id = (I32)pinfo->UniqueProcessId;
          break;
        }
      }

      pinfo = (decltype( pinfo ))( (U32)pinfo + pinfo->NextEntryOffset );
    } while( !!pinfo->NextEntryOffset );
  
    VirtualFree( pinfo, PINFO_ALLOC_SIZE, MEM_FREE );

    _OBJECT_ATTRIBUTES64 obj_attributes{};
    _CLIENT_ID_T<U64> cid;

    cid.UniqueProcess = (U64)( UlongToHandle( m_id ) );
    cid.UniqueThread = 0;
    obj_attributes.Length = sizeof( obj_attributes );

    status = nt_open_process64(
      &m_base,
      PROCESS_ALL_ACCESS, 
      &obj_attributes,
      &cid
    );

    return status == STATUS_SUCCESS;
  }

  U32 get_module_size32( U64 module_base ) {
   IMAGE_NT_HEADERS nt_headers;
   IMAGE_DOS_HEADER dos_header;

    read( module_base, &dos_header, sizeof( dos_header ) );
    read( module_base + dos_header.e_lfanew, &nt_headers, sizeof( nt_headers ) );

    return nt_headers.OptionalHeader.SizeOfImage;
  }

  U64 get_module_size64( U64 module_base ) {
    IMAGE_NT_HEADERS64 nt_headers;
    IMAGE_DOS_HEADER   dos_header;

    read( module_base, &dos_header, sizeof( dos_header ) );
    read( module_base + dos_header.e_lfanew, &nt_headers, sizeof( nt_headers ) );

    return nt_headers.OptionalHeader.SizeOfImage;
  }

  // someone finish this
  /*
  std::vector< MODULE_ENTRY > dump_modules64() {
    std::vector< MODULE_ENTRY >  ret;
    PROCESS_BASIC_INFORMATION64* pbi;
    ULONG                        pbi_len;
    NTSTATUS64                   status;

    status = nt_query_information_process64(
      m_base,
      ProcessBasicInformation,
      nullptr,
      0,
      &pbi_len
    );

    printf( "%08x %08x\n", status );
  }
  */

  std::vector< MODULE_ENTRY > dump_modules32() {
    std::vector< MODULE_ENTRY > ret;
    U64                         peb32_addr;
    NTSTATUS64                  status;

    if( !m_id )
      return ret;

    ULONG out_ret = 0;
    status = nt_query_information_process64(
      m_base,
      ProcessWow64Information,
      &peb32_addr,
      sizeof( U64 ),
      &out_ret
    );

    if( status != STATUS_SUCCESS )
      return ret;

    PEB* peb = (PEB*)VirtualAlloc(
      0,
      sizeof( PEB ),
      MEM_COMMIT | MEM_RESERVE,
      PAGE_READWRITE
    );

    read( peb32_addr, peb, sizeof( PEB ) );

    PEB_LDR_DATA ldr;
    read( (U32)peb->Ldr, &ldr, sizeof( ldr ) );

    VirtualFree( peb, sizeof( PEB64 ), MEM_FREE );

    U64 root = (U32)ldr.InMemoryOrderModuleList.Flink;
    for( U32 entry = read<U32>( root ); entry != root; entry = read<U32>( entry ) ) {
      LDR_DATA_TABLE_ENTRY data_table{};
      read( entry, &data_table, sizeof( data_table ) );
      
      if( !data_table.FullDllName.Buffer )
        continue;

      wchar_t module_buffer[256]{};
      read(
        (U64)data_table.FullDllName.Buffer,
        module_buffer, 256 * sizeof( wchar_t )
      );

      STR<256> module_name = u_widebyte_to_ansi<256>( module_buffer );
      FNV1A    module_hash = fnv1a( module_name );
      U64      module_base = (U32)data_table.Reserved2[0];
      U64      module_size = *(U32*)((U32)&data_table + 0x20);

      ret.push_back( {
        module_base,
        module_size,
        module_name.data,
        module_hash
      } );
    }

    return ret;
  }

  U32 get_module32( FNV1A name, U32* out_size = 0 ) {
    std::vector< MODULE_ENTRY > modules = dump_modules32();
    for( auto& it : modules ) {
      if( it.hash == name ) {
        if( out_size )
          *out_size = (U32)it.size;

        return (U32)it.base;
      }
    }

    return 0;
  }

  U32 code_match( U32 module_base, const char* sig ) {
    U32 sig_length;
    U8* sig_bytes = u_parse_signature( sig, &sig_length );
    if( !sig_bytes || sig_length <= 2 )
      return 0;

    MEMORY_BASIC_INFORMATION64 mbi{0};
    U32 module_size = get_module_size32( module_base );
    
    for( U64 off = 0; off < module_size; off += mbi.RegionSize ) {
      nt_query_vm64( m_base, module_base + off, MemoryRegionInfo, &mbi, sizeof( mbi ) );
      
      if( mbi.State == MEM_FREE )
        continue;
      
      U8* buffer = (U8*)malloc( (U32)mbi.RegionSize );
      read( (U32)mbi.BaseAddress, buffer, (U32)mbi.RegionSize );
      
      for( U32 i = 0; i < mbi.RegionSize - sig_length; ++i ) {
        if( u_binary_match( buffer + i, sig_bytes, sig_length ) ) {
          free( buffer );
          free( sig_bytes );
          return (U32)mbi.BaseAddress + i;
        }
      }
      
      free( buffer );
    }

    free( sig_bytes );
    return 0;
  }

  I32 get_id() { return m_id; }

  template < typename t > void write( U64 address, const t& value ) {
    nt_write_vm64( m_base, address, (void*)&value, sizeof( t ) );
  }

  void write( U64 address, void* buffer, U32 size ) {
    nt_write_vm64( m_base, address, buffer, size );
  }

  template < typename t > t read( U64 address ) {
    t buffer{};
    read( address, &buffer, sizeof( t ) );

    return buffer;
  }

  void read( U64 address, void* out, U32 size ) {
    nt_read_vm64( m_base, address, out, size );
  }
};
