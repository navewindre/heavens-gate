//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "csgo/hack.h"
#include "util.h"
#include "conin.h"
#include "menu.h"

bool run() {
  con_init();
  PROCESS32* p = hack_init();

  Sleep( 1000 );
  settings.load();
  menu_show_ui( p );

  for( ;; ) {
    if( !hack_run( p ) ) {
      nt_close64( p->get_base() );
      Sleep( 5000 );
      break;
    }
  }

  return false;
}
  
I32 __cdecl main() {
  u_set_debug_privilege();
  u_thread_create( &con_hook_handler );
  u_thread_create( &con_handler );

  for( ; !run(); );

  return 0;
}
