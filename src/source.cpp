// by navewindre
// github.com/navewindre

#include "csgo/hack.h"
#include "util.h"
#include "conin.h"
#include "menu.h"

I32 __cdecl main() {
  con_init();
  PROCESS32* p = hack_init();

  u_thread_create( &con_hook_handler );
  u_thread_create( &con_handler );
  
  Sleep( 1000 );
  settings_holder.load();
  menu_show_ui( p );
  
  for( ;; ) {
    hack_run( p ); 
  }
}
