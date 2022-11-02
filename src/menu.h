#pragma once
#include <thread>

#include "process.h"
#include "util.h"
#include "vars.h"

#include "csgo/interface.h"

static void syscall_dump_to_file() {
  static std::vector< SYSCALL_ENTRY > syscalls = syscall_dump();
  static std::vector< SYSCALL_ENTRY > syscalls64 = syscall_dump64();
  static std::vector< MODULE_EXPORT64 > nt64_exports = module_get_exports64( nt_get_address64() );
  char* syscall_str = (char*)malloc( 100000 );

  memset( syscall_str, 0, 100000 );
  
  char line_buf[256]{};
  for( auto& it : syscalls ) {
    sprintf_s<256>( line_buf, "%d %s -> %llx\n", it.idx, it.name.data, it.base );
    strcat( syscall_str, line_buf );
  }

  FILE* dump = fopen( "./syscalls_32.dump", "w" );
  fwrite( syscall_str, strlen( syscall_str ), 1, dump );
  fclose( dump );

  memset( syscall_str, 0, 100000 );
  
  for( auto& it : syscalls64 ) {
    sprintf_s<256>( line_buf, "%d %s -> %llx\n", it.idx, it.name.data, it.base );
    strcat( syscall_str, line_buf );
  }

  dump = fopen( "./syscalls_64.dump", "w" );
  fwrite( syscall_str, strlen( syscall_str ), 1, dump );
  fclose( dump );

  memset( syscall_str, 0, 100000 );
  
  for( auto& it : nt64_exports ) {
    sprintf_s<256>( line_buf, "%s -> %llx\n", it.name.data, it.base );
    strcat( syscall_str, line_buf );
  }

  dump = fopen( "./nt64.dump", "w" );
  fwrite( syscall_str, strlen( syscall_str ), 1, dump );
  fclose( dump ); 

  free( syscall_str );
}

static void csgo_dump_ifaces_to_file( CSGO* p ) {
  std::vector< IFACE_ENTRY > ifaces = srceng_get_interfaces( p );
  static char iface_str[999999]{};

  memset( iface_str, 0, sizeof( iface_str ) );
  char line_buf[256]{};
  for( auto& it : ifaces ) {
    sprintf( line_buf, "%s -> %08x in [%s]\n", it.name.data, it.ptr, it.module_name.data );
    strcat( iface_str, line_buf );
  }

  FILE* dump = fopen( "./interfaces.dump", "w" );
  fwrite( iface_str, strlen( iface_str ), 1, dump );
  fclose( dump ); 
}

extern void menu_show_ui( CSGO* p );