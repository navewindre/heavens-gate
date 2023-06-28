#include "hack.h"
#include "../menu.h"

void hack_toggle( I32 line, SETTING<bool>& var, U8 page ) {
  con_clear( );

  menu_pages[ page ].page_fn( );

  show_paging( page );

  var = !var;

  con_set_line_subtext(
    line,
    var? "[on]" : "[off]",
    con_selected_line == line,
    var? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );
}

struct CMD_DATA {
  COPYDATASTRUCT data;
  const char *cstring;
};

#define gcon_var( name, string ) \
CMD_DATA name; \
name.cstring = string; \
name.data = { 0, strlen( string ) + 1, ( void* )string }; \
help_cmd.push_back( name )

#define gcon_match( name ) !strncmp( buf + 1, name, strlen( name ) )

void __cdecl game_hack_toggle( VECTOR<STR<64>> args ) {
  static SETTING<bool>& bhop_active = *settings.find<bool>( "bhop_active"fnv );
  static SETTING<bool>& chams_active = *settings.find<bool>( "chams_active"fnv );
  static SETTING<bool>& glow_active = *settings.find<bool>( "glow_active"fnv );
  static SETTING<bool>& nightmode_active = *settings.find<bool>( "nightmode_active"fnv );
  static SETTING<bool>& noflash_active = *settings.find<bool>( "noflash_active"fnv );
  static SETTING<bool>& clantag_active = *settings.find<bool>( "clantag_active"fnv );

  static SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );
  static SETTING<bool>& crosshair_active = *settings.find<bool>( "crosshair_active"fnv );
  static SETTING<bool>& triggerteam_active = *settings.find<bool>( "triggerteam_active"fnv );

  char buf[512]{};

  for( auto& it : args )
    sprintf( buf, "%s\n%s", buf, it.data );

  // there has to be a better way to do this, right ?
  // right .....
  if( gcon_match( "hg_bhop" ) )
    hack_toggle( 0, bhop_active, 1 );
  else if( gcon_match( "hg_chams" ) )
    hack_toggle( 1, chams_active, 1 );
  else if( gcon_match( "hg_glow" ) )
    hack_toggle( 2, glow_active, 1 );
  else if( gcon_match( "hg_night" ) )
    hack_toggle( 3, nightmode_active, 1 );
  else if( gcon_match( "hg_flash" ) )
    hack_toggle( 4, noflash_active, 1 );
  else if( gcon_match( "hg_clan" ) )
    hack_toggle( 5, clantag_active, 1 );
  else if( gcon_match( "hg_aim" ) )
    hack_toggle( 0, aim_active, 2 );
  else if( gcon_match( "hg_xhair" ) )
    hack_toggle( 1, crosshair_active, 2 );
  else if( gcon_match( "hg_triggerteam" ) )
    hack_toggle( 3, triggerteam_active, 2 );
  else if( gcon_match( "hg_help" ) ) {
    const HWND hconsole = FindWindowA( "Valve001", 0 );

    if( !hconsole )
      return;

    VECTOR<CMD_DATA> help_cmd;
    gcon_var( aim  , "echo \"hg_aim         : toggles aim assist\"" );
    gcon_var( bhop , "echo \"hg_bhop        : toggles bhop\"" );
    gcon_var( chams, "echo \"hg_chams       : toggles chams\"" );
    gcon_var( clan , "echo \"hg_clan        : toggles clantag ( buggy )\"" );
    gcon_var( flash, "echo \"hg_flash       : toggles no flash\"" );
    gcon_var( glow , "echo \"hg_glow        : toggles glow\"" );
    gcon_var( night, "echo \"hg_night       : toggles nightmode\"" );
    gcon_var( trigg, "echo \"hg_triggerteam : toggles team triggerbot\"" );
    gcon_var( xhair, "echo \"hg_xhair       : toggles recoil crosshair\"" );
    
    for( const auto& cmd : help_cmd )
      SendMessageA( hconsole, WM_COPYDATA, 0, ( LPARAM )&cmd.data );
  } else {
    const HWND hconsole = FindWindowA( "Valve001", 0 );
    if( !hconsole )
      return;
    COPYDATASTRUCT hconsole_out;
    hconsole_out.cbData = strlen( "echo \"invalid cmd, use \'hg_help\' for cmd list\"" ) + 1;
    hconsole_out.dwData = 0;
    hconsole_out.lpData = ( void* )"echo \"invalid cmd, use \'hg_help\' for cmd list\"";
    SendMessageA( hconsole,
      WM_COPYDATA, 0,
      ( LPARAM )&hconsole_out
    );
  }
  return;
}