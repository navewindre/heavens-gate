// by navewindre
// github.com/navewindre

#include <thread>
#include "hack.h"
#include "util.h"
#include "conin.h"
#include "menu.h"
#include "vars.h"
#include "ntutil.h"

I32 main() {
  con_init();

  auto nt_dump = syscall_dump();

  u_thread_create( &con_handler ); 
  u_thread_create( &con_hook_handler );

  CSGO p{};

  while( !p.open() ) {
    con_set_bottomline_text( "waiting for process..." );
    Sleep( 100 );
  }
  
  hack_init( &p );
  Sleep( 1000 );
  menu_show_ui( &p );
  
  for ( ;; ) {
    hack_run_bhop( &p );
    hack_run_trigger( &p );
    hack_run_glow( &p );
    
    std::this_thread::sleep_for( 1ms );
  }
}
