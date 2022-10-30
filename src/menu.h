#include <thread>

#include "conin.h"
#include "process.h"
#include "conout.h"
#include "util.h"
#include "vars.h"

void syscall_dump_to_file() {
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

void menu_show_ui( CSGO *p ) {
  con_clear();
  con_capturing_input = true;

  con_set_line_text( 0,"bhop",false );
  con_set_line_subtext(
    0,
    bhop_active? "[on]" : "[off]",
    true,
    bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback(0,[]( CON_LINE *self,U8 action ) {
    bhop_active = !bhop_active;
    con_set_line_subtext(
      0,
      bhop_active? "[on]" : "[off]",
      self->active,
      bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
  });

  con_set_line_text(1, "glow", false);
  con_set_line_subtext(
    1,
    glow_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 1, []( CON_LINE *self,U8 action ) {
    glow_active = !glow_active;
    con_set_line_subtext(
      1,
      glow_active? "[on]" : "[off]",
      self->active,
      glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
  });


  con_set_line_text( 2, "triggerbot", false);
  con_set_line_subtext(
    2,
    key_titles[triggerbot_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 2,[]( CON_LINE *,U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      con_update_hotkey( 2 , triggerbot_key);
    }
    } );

  con_set_line_text( 3, "display color pallette" );
  con_set_line_subtext(
    3,
    key_titles[0x0D],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 3, []( CON_LINE *,U8 action ) {
    static bool toggle;
    if( action == LINE_ACTION_ENTER ) {
      if( !toggle )
        con_print_colors();
      else {
        con_refresh();
      }

      toggle = !toggle;
    }
  } );

  con_set_line_text( 4, "dump syscalls to syscall_arch.dump", false );
  con_set_line_subtext(
    4,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 4, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      syscall_dump_to_file();
  } );

  /*
  con_set_line_text(4,"test input");
  con_set_line_subtext(
    4,
    key_titles[trigger_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback(4,[](CON_LINE *,U8 action) {
    if( action==LINE_ACTION_ENTER ) {
      con_triggerkey_update( 4 );
    }
    });
    */

  con_set_bottomline_text(
    "LOCALPLAYER: %08X | FLAGS: %08X | menu",
    p->read< U32 >( localplayer_ptr ),0x0
  );
}