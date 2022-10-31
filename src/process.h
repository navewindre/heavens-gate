#pragma once
#include <Windows.h>
#include <inttypes.h>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>

#include "ntutil.h"
#include "winintern.h"
#include "typedef.h"
#include "fnv.h"
#include "syscall.h"

class PROCESS32 {
private:
  HANDLE      m_base{};
  I32         m_id{};
  char        m_name[256]{};

private:
  U8 binary_match( U8* code, U8* pattern, U32 size ) {
    for( U32 i = 0; i < size; ++i ) {
      if( pattern[i] && code[i] != pattern[i] )
        return 0;
    }

    return 1;
  }

  U8* parse_signature( const char* sig, U32* out_len ) {
    U8* sig_bytes = (U8*)malloc( strlen( sig ) );

    U32 i, byte;
    for( i = 0, byte = 0; i < strlen( sig ); ++byte ) {
      if( sig[i] == ' ' )
        return 0;

      if( sig[i] == '?' ) {
        sig_bytes[byte] = 0;
        i += 2;
        continue;
      }

      unsigned long temp;
      sscanf( &sig[i], "%02x", &temp );

      sig_bytes[byte] = (U8)( temp & 0xff );
      i += 3;
    }

    if( out_len )
      *out_len = byte;
    return sig_bytes;
  }
  
public:
  PROCESS32( const char* name ) {
    memset( m_name, 0, 256 );
    memcpy( m_name, name, strlen( name ) );
  };

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

  U32 get_module32( FNV1A name, U32* out_size = 0 ) {
    U64        peb32_addr;
    NTSTATUS64 status;

    if( !m_id )
      return U32{};

    ULONG out_ret = 0;
    status = nt_query_information_process64(
      m_base,
      ProcessWow64Information,
      &peb32_addr,
      sizeof( U64 ),
      &out_ret
    );

    if( status != STATUS_SUCCESS )
      return 0;

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
      FNV1A module_hash = fnv1a( module_name );

      if( module_hash == name ) {
        if( out_size )
          *out_size = *(U32*)&data_table.Reserved3[0];
        return (U32)data_table.Reserved2[0];
      }
    }

    return U32{};
  }

  U32 code_match( U32 module_base, const char* sig ) {
    U32 sig_length;
    U8* sig_bytes = parse_signature( sig, &sig_length );
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
        if( binary_match( buffer + i, sig_bytes, sig_length ) ) {
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

class CSGO : public PROCESS32 {
public:
  CSGO() : PROCESS32( "csgo.exe" ) {};

  U32 client;
  U32 engine;
};