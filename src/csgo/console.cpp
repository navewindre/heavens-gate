#include "hack.h"
#include "../menu.h"


struct CMD_TOGGLE {
  const char* name;
  int page;
  int line;
  SETTING<bool>& var;
};


void hack_toggle( CMD_TOGGLE cmd ) {
  con_clear();
  
  menu_pages[cmd.page].page_fn();
  show_paging( cmd.page );

  cmd.var = !cmd.var;

  con_set_line_subtext(
    cmd.line,
    cmd.var? "[on]" : "[off]",
    con_selected_line == cmd.line,
    cmd.var? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );
}

#define gcon_match( name ) !strncmp( buf + 1, name, strlen( name ) )

#define gcon_var( string ) { 0, strlen( string ) + 1, ( void* )string }

void __cdecl game_hack_toggle( VECTOR<STR<64>> args ) {
  static SETTING<bool>& bhop_active = *settings.find<bool>( "bhop_active"fnv );
  static SETTING<bool>& chams_active = *settings.find<bool>( "chams_active"fnv );
  static SETTING<bool>& glow_active = *settings.find<bool>( "glow_active"fnv );
  static SETTING<bool>& nightmode_active = *settings.find<bool>( "nightmode_active"fnv );
  static SETTING<bool>& noflash_active = *settings.find<bool>( "noflash_active"fnv );
  static SETTING<bool>& clantag_active = *settings.find<bool>( "clantag_active"fnv );

  static SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );
  static SETTING<bool>& crosshair_active = *settings.find<bool>( "crosshair_active"fnv );
  static SETTING<bool>& rcs_active = *settings.find<bool>( "rcs_active"fnv );
  static SETTING<bool>& triggerteam_active = *settings.find<bool>( "triggerteam_active"fnv );

  char buf[512]{};

  for( auto& it : args )
    sprintf( buf, "%s\n%s", buf, it.data );

  CMD_TOGGLE cmd_toggle[10] = {
    { "hg_bhop"       , 1, 0,        bhop_active },
    { "hg_chams"      , 1, 1,       chams_active },
    { "hg_glow"       , 1, 2,        glow_active },
    { "hg_night"      , 1, 3,   nightmode_active },
    { "hg_flash"      , 1, 4,     noflash_active },
    { "hg_clan"       , 1, 5,     clantag_active },
    { "hg_aim"        , 2, 0,         aim_active },
    { "hg_xhair"      , 2, 1,   crosshair_active },
    { "hg_rcs"        , 2, 2,         rcs_active },
    { "hg_triggerteam", 2, 4, triggerteam_active }
  };

  for( const auto& cmd : cmd_toggle ) {
    if( gcon_match( cmd.name ) ) {
      hack_toggle( cmd );
      return;
    }
  }

  if( gcon_match( "hg_help" ) ) {
    const HWND hconsole = FindWindowA( "Valve001", 0 );
    if( !hconsole )
      return;

    COPYDATASTRUCT help_cmd[] = {
      gcon_var( "echo \"hg_aim         : toggles aim assist\""        ),
      gcon_var( "echo \"hg_bhop        : toggles bhop\""              ),
      gcon_var( "echo \"hg_chams       : toggles chams\""             ),
      gcon_var( "echo \"hg_clan        : toggles clantag ( buggy )\"" ),
      gcon_var( "echo \"hg_flash       : toggles no flash\""          ),
      gcon_var( "echo \"hg_glow        : toggles glow\""              ),
      gcon_var( "echo \"hg_night       : toggles nightmode\""         ),
      gcon_var( "echo \"hg_rcs         : toggles standalone rcs\""    ),
      gcon_var( "echo \"hg_triggerteam : toggles team triggerbot\""   ),
      gcon_var( "echo \"hg_xhair       : toggles recoil crosshair\""  )
    };

    u_sleep( 1 * T_SEC / 5 );
    for( const auto& cmd : help_cmd ) {
      SendMessageA( hconsole, WM_COPYDATA, 0, ( LPARAM )&cmd );
      u_sleep( 1 * T_SEC / 20 );
    }
    return;
  }

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
  
  return;
}