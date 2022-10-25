#pragma once
#include <Windows.h>
#include <inttypes.h>
#include <TlHelp32.h>
#include <string>
#include <Psapi.h>

#include "ntutil.h"
#include "winintern.h"
#include "typedef.h"

class PROCESS {
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
  PROCESS( const char* name ) {
    memset( m_name, 0, 256 );
    memcpy( m_name, name, strlen( name ) );
  };

  I8 open() {
    HANDLE          t32_snapshot;
    PROCESSENTRY32  proc_entry;

    t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    proc_entry.dwSize = sizeof( proc_entry );

    for ( Process32First( t32_snapshot, &proc_entry );
      Process32Next( t32_snapshot, &proc_entry );
      ) {
      if ( !strcmp( proc_entry.szExeFile, m_name ) ) {
        CloseHandle( t32_snapshot );
        m_id = proc_entry.th32ProcessID;

        _OBJECT_ATTRIBUTES64 obj_attributes{};
        _CLIENT_ID_T<U64> cid;

        cid.UniqueProcess = (U64)( UlongToHandle( m_id ) );
        cid.UniqueThread = 0;
        obj_attributes.Length = sizeof( obj_attributes );

        NTSTATUS64 status = nt_open_process64(
          &m_base,
          PROCESS_ALL_ACCESS, 
          &obj_attributes,
          &cid
        );

        return status == STATUS_SUCCESS;
      }
    }

    return 0;
  }

  U32 get_module( const char* name, U32* out_size = 0 ) {
    HANDLE    t32_snapshot;
    MODULEENTRY32 mod_entry;

    if( !m_id )
      return U32{};

    t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, m_id );
    mod_entry.dwSize = sizeof( mod_entry );

    for( Module32First( t32_snapshot, &mod_entry );
      Module32Next( t32_snapshot, &mod_entry );
    ) {
      if( !strcmp( mod_entry.szModule, name ) ) {
        CloseHandle( t32_snapshot );
        if( out_size )
          *out_size = mod_entry.modBaseSize;
        return U32( mod_entry.modBaseAddr );
      }
    }

    return U32{};
  }

  U32 code_match( U32 module_base, const char* sig ) {
    U32 sig_length;
    U8* sig_bytes = parse_signature( sig, &sig_length );
    if( !sig_bytes || sig_length <= 2 )
      return 0;

    MODULEINFO module{};
    U32        module_size;
    K32GetModuleInformation( m_base, (HMODULE)module_base, &module, sizeof( MODULEINFO ) );
    module_size = module.SizeOfImage;
    
    MEMORY_BASIC_INFORMATION64 mbi{0};

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

  template < typename t > void write( U32 address, const t& value ) {
    nt_write_vm64( m_base, address, (void*)&value, sizeof( t ) );
  }

  void write( U32 address, void* buffer, U32 size ) {
    nt_write_vm64( m_base, address, buffer, size );
  }

  template < typename t > t read( U32 address ) {
    t buffer{};
    read( address, &buffer, sizeof( t ) );

    return buffer;
  }

  void read( U32 address, void* out, U32 size ) {
    nt_read_vm64( m_base, address, out, size );
  }
};

class CSGO : public PROCESS {
public:
  CSGO() : PROCESS( "csgo.exe" ) {};

  U32 client;
  U32 engine;
};