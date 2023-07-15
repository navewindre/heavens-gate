#include "hack.h"
#include "../menu.h"

struct CMD_TOGGLE {
  const char* name;
  const char* desc;
  SETTING<bool>& var; // figure out how to parse non-bool
};


void hack_toggle( CMD_TOGGLE cmd ) {
  con_clear();
  cmd.var.v ^= 1;
  
  menu_pages[menu_page].page_fn();
  show_paging( menu_page );
}

void hack_off( CMD_TOGGLE cmd ) {
  con_clear();
  cmd.var.v = false;

  menu_pages[menu_page].page_fn();
  show_paging( menu_page );
}

#define gcon_match( name ) !strncmp( buf + 1, name, strlen( buf ) )
#define gcon_send( str ) \
COPYDATASTRUCT temp { 0, strlen( str ) + 1, ( void* )str }; \
SendMessageA( hconsole, WM_COPYDATA, 0, ( LPARAM )&temp )

bool __cdecl game_hack_toggle( VECTOR<STR<64>> args ) {
  char buf[512]{};

  for( auto& it : args )
    sprintf( buf, "%s\n%s", buf, it.data );

  // split char array @ spaces to ignore extraneous spaces after cmd
  // & add fov, perf_tps, and aim strength ( int support )
  static CMD_TOGGLE cmd_toggle[] = {
    { "hg_aim"        , "toggles aim assist"      , *settings.find<bool>(         "aim_active"fnv ) },
    { "hg_aimteam"    , "toggles team aim assist" , *settings.find<bool>(     "aimteam_active"fnv ) },
    { "hg_bhop"       , "toggles bhop"            , *settings.find<bool>(        "bhop_active"fnv ) },
    { "hg_chams"      , "toggles chams"           , *settings.find<bool>(       "chams_active"fnv ) },
    { "hg_clan"       , "toggles clantag (buggy)" , *settings.find<bool>(     "clantag_active"fnv ) },
    { "hg_flash"      , "toggles no flash"        , *settings.find<bool>(     "noflash_active"fnv ) },
    { "hg_glow"       , "toggles glow"            , *settings.find<bool>(        "glow_active"fnv ) },
    { "hg_glowteam"   , "toggles team glow"       , *settings.find<bool>(    "glowteam_active"fnv ) },
    { "hg_night"      , "toggles nightmode"       , *settings.find<bool>(   "nightmode_active"fnv ) },
    { "hg_rcs"        , "toggles standalone rcs"  , *settings.find<bool>(         "rcs_active"fnv ) },
    { "hg_triggerteam", "toggles team triggerbot" , *settings.find<bool>( "triggerteam_active"fnv ) },
    { "hg_xhair"      , "toggles recoil crosshair", *settings.find<bool>(   "crosshair_active"fnv ) }
  };

  static HWND hconsole = FindWindowA( "Valve001", 0 );
  if( !hconsole )
    return false;

  for( const auto& cmd : cmd_toggle ) {
    if( gcon_match( cmd.name ) ) {
      hack_toggle( cmd );

      sprintf( buf,
        "echo \"%s : %s\"",
        cmd.name,
        ( cmd.var.v? "true" : "false" )
      );

      gcon_send( buf );
      return false;
    }
  }

  // recoil xhair and nightmode dont toggle off
  if( gcon_match( "hg_panic" ) ) {
    for( auto& cmd : cmd_toggle )
      hack_off( cmd );
    return true;
  }

  if( gcon_match( "hg_help" ) ) {
    u_sleep( 200 * T_MS );
    for( auto& cmd : cmd_toggle ) {
      sprintf( buf,
        "echo \"%s : %s\"",
        cmd.name, cmd.desc
      );
      
      gcon_send( buf );
      u_sleep( 50 * T_MS );
    } 
    u_sleep( 250 * T_MS );
    gcon_send( "echo \"hg_panic : toggles all features off and closes heaven's gate. ( buggy )\"" );
    return false;
  }

  gcon_send( "echo \"invalid cmd, use \'hg_help\' for list\"" );
  return false;
}