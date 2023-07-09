#include "hack.h"
#include "../menu.h"


struct CMD_TOGGLE {
  const char* name;
  const char* desc;
  SETTING<bool>& var;
};


void hack_toggle( CMD_TOGGLE cmd ) {
  con_clear();
  cmd.var = !cmd.var;
  
  menu_pages[menu_page].page_fn();
  show_paging( menu_page );
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
    { "hg_bhop"       , "toggles aim assist", bhop_active },
    { "hg_chams"      , "toggles bhop", chams_active },
    { "hg_glow"       , "toggles chams", glow_active },
    { "hg_night"      , "toggles clantag", nightmode_active },
    { "hg_flash"      , "toggles no flash", noflash_active },
    { "hg_clan"       , "toggles glow", clantag_active },
    { "hg_aim"        , "toggles nightmode", aim_active },
    { "hg_xhair"      , "toggles standalone rcs", crosshair_active },
    { "hg_rcs"        , "toggles team triggerbot", rcs_active },
    { "hg_triggerteam", "toggles recoil crosshair", triggerteam_active }
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

    u_sleep( 1 * T_SEC / 5 );
    for( auto& cmd : cmd_toggle ) {
      sprintf( buf, "echo \"%s : %s\"", cmd.name, cmd.desc );
      
      COPYDATASTRUCT hconsole_out;
      hconsole_out.cbData = strlen( buf ) + 1;
      hconsole_out.dwData = 0;
      hconsole_out.lpData = ( void* )buf;
      SendMessageA( hconsole,
        WM_COPYDATA, 0,
        ( LPARAM )&hconsole_out
      );

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