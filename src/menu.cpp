//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "menu.h"
#include "conin.h"

#include "csgo/hack.h"

CSGO* csgop;

I8 menu_page = 1;
MENU_PAGE menu_pages[MENU_PAGE_MAX - MENU_PAGE_MIN + 1];

void show_paging( U8 num ) {
  char line_str[CON_MAX_WIDTH + 3]{};

  U32 cur_char = 0;
  if( num > MENU_PAGE_MIN ) {
    sprintf( line_str, "<-%s", menu_pages[num-1].name );
    cur_char = strlen( line_str );
  }

  char mid_str[16];
  sprintf( mid_str, "[%d] %s", num, menu_pages[num].name );
  I32 mid_len = strlen( mid_str );

  I32 diff = CON_MAX_WIDTH / 2 - (I32)cur_char - mid_len / 2;

  memset( line_str + cur_char, ' ', diff );
  cur_char += diff;

  strcat( line_str, mid_str );
  cur_char += mid_len;

  if( num < MENU_PAGE_MAX ) {
    char next_str[16];
    sprintf( next_str, "%s->", menu_pages[num+1].name );

    I32 next_len = strlen( next_str );
    I32 diff = CON_MAX_WIDTH - (I32)cur_char - next_len;

    memset( line_str + cur_char, ' ', diff );
    strcat( line_str, next_str );
  }

  con_set_line(
    CON_MAX_HEIGHT - 1,
    line_str,
    "",
    con_selected_line == CON_MAX_HEIGHT - 1,
    CONFG_MAGENTA
  );

  con_set_line_callback(
    CON_MAX_HEIGHT - 1,
    []( CON_LINE* self, U8 action ) {
      if( action == LINE_ACTION_LEFTARROW && menu_page > MENU_PAGE_MIN )
        --menu_page;
      if( action == LINE_ACTION_RIGHTARROW && menu_page < MENU_PAGE_MAX )
        ++menu_page;

      con_clear();
      menu_pages[menu_page].page_fn();
      show_paging( menu_page );
    } );
}

void show_page_0() {
  I32 line_idx = 0;

  con_set_line_text( line_idx, "load config", false );
  con_set_line_subtext( line_idx, "[ENTER]", false, CONFG_LIGHTBLUE );
  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      settings.load();
    }
    } );

  ++line_idx;

  con_set_line_text( line_idx, "save config", false );
  con_set_line_subtext( line_idx, "[ENTER]", false, CONFG_LIGHTBLUE );
  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      settings.save();
    }
    } );

  ++line_idx;

  con_set_line_text( line_idx, "perf_tickrate", false );
  con_set_line_subtext( line_idx, u_num_to_string_dec( perf_tickrate ), false, CONFG_BLUE );
  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_LEFTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
        if( perf_tickrate > 10 )
          perf_tickrate -= 10;
        else
          perf_tickrate = 0;
      } else {
        if( perf_tickrate > 0 )
          --perf_tickrate;
      }
    }
    if( action == LINE_ACTION_RIGHTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
        if( perf_tickrate < 2048 )
          perf_tickrate += 10;
        else
          perf_tickrate = 2048;
      } else {
        if( perf_tickrate < 2048 )
          ++perf_tickrate;
      }
    }

    con_set_line_subtext( self->line_num, u_num_to_string_dec( perf_tickrate ), true, CONFG_BLUE );
  } );

  line_idx += 4;

  con_set_line_text( line_idx, "dump all classes to classes.dump", false );
  con_set_line_subtext(
    6,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( line_idx, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      csgo_dump_classes( csgop );
    } ); 

  ++line_idx;

  con_set_line_text( line_idx, "dump syscalls to syscall_arch.dump", false );
  con_set_line_subtext(
    line_idx,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( line_idx, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      syscall_dump_to_file();
    } );

  ++line_idx;

  con_set_line_text( line_idx, "dump interfaces to interfaces.dump", false );
  con_set_line_subtext(
    line_idx,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( line_idx, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      csgo_dump_ifaces_to_file( csgop );
    } );
}

void show_page_1() {
  static SETTING<bool>& bhop_active = *settings.find<bool>( "bhop_active"fnv );
  static SETTING<bool>& chams_active = *settings.find<bool>( "chams_active"fnv );
  static SETTING<bool>& glow_active = *settings.find<bool>( "glow_active"fnv );
  static SETTING<bool>& glowteam_active = *settings.find<bool>( "glowteam_active"fnv );

  static SETTING<bool>& nightmode_active = *settings.find<bool>( "nightmode_active"fnv );
  static SETTING<bool>& noflash_active = *settings.find<bool>( "noflash_active"fnv );
  static SETTING<bool>& clantag_active = *settings.find<bool>( "clantag_active"fnv );
  
  I32 line_idx = 0;

  con_set_line_text( line_idx,"bhop",false );
  con_set_line_subtext(
    line_idx,
    bhop_active? "[on]" : "[off]",
    con_selected_line == 0,
    bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback(line_idx,[]( CON_LINE *self,U8 action ) {
    bhop_active = !bhop_active;
    con_set_line_subtext(
      self->line_num,
      bhop_active? "[on]" : "[off]",
      self->active,
      bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "chams", false );
  con_set_line_subtext(
    line_idx,
    chams_active? "[on]" : "[off]",
    false,
    chams_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    chams_active = !chams_active;
    con_set_line_subtext(
      self->line_num,
      chams_active? "[on]" : "[off]",
      self->active,
      chams_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "glow", false );
  con_set_line_subtext(
    line_idx,
    glow_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    glow_active = !glow_active;
    con_set_line_subtext(
      self->line_num,
      glow_active? "[on]" : "[off]",
      self->active,
      glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "team glow", false );
  con_set_line_subtext(
    line_idx,
    glowteam_active? "[on]" : "[off]",
    false,
    glowteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    glowteam_active = !glowteam_active;
    con_set_line_subtext(
      self->line_num,
      glowteam_active? "[on]" : "[off]",
      self->active,
      glowteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "nightmode", false );
  con_set_line_subtext(
    line_idx,
    nightmode_active? "[on]" : "[off]",
    false,
    nightmode_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    nightmode_active = !nightmode_active;
    con_set_line_subtext(
      self->line_num,
      nightmode_active? "[on]" : "[off]",
      self->active,
      nightmode_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "no flash", false );
  con_set_line_subtext(
    line_idx,
    noflash_active? "[on]" : "[off]",
    false,
    noflash_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    noflash_active = !noflash_active;
    con_set_line_subtext(
      self->line_num,
      noflash_active? "[on]" : "[off]",
      self->active,
      noflash_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "clantag changer" );
  con_set_line_subtext(
    line_idx,
    clantag_active? "[on]" : "[off]",
    false,
    clantag_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    clantag_active = !clantag_active;
    con_set_line_subtext(
      self->line_num,
      clantag_active? "[on]" : "[off]",
      self->active,
      clantag_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    ); 
    } );
}

void show_page_2() {
  static SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );
  static SETTING<F32>& aim_fov = *settings.find<F32>( "aim_fov"fnv );
  static SETTING<I32>& aim_strength = *settings.find<I32>( "aim_strength"fnv );
  static SETTING<bool>& aimteam_active = *settings.find<bool>( "aimteam_active"fnv );
  static SETTING<bool>& crosshair_active = *settings.find<bool>( "crosshair_active"fnv );
  static SETTING<bool>& rcs_active = *settings.find<bool>( "rcs_active"fnv );
  static SETTING<I32>& triggerbot_key = *settings.find<I32>( "triggerbot_key"fnv );
  static SETTING<bool>& triggerteam_active = *settings.find<bool>( "triggerteam_active"fnv );

  I32 line_idx = 0;

  con_set_line_text( line_idx,"aim assist",false );
  con_set_line_subtext(
    line_idx,
    aim_active? "[on]" : "[off]",
    con_selected_line == 0,
    aim_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx,[]( CON_LINE *self,U8 action ) {
    aim_active = !aim_active;
    con_set_line_subtext(
      self->line_num,
      aim_active? "[on]" : "[off]",
      self->active,
      aim_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
  });

  ++line_idx;

  con_set_line_text( line_idx, "aim assist team", false );
  con_set_line_subtext(
    line_idx,
    aimteam_active? "[on]" : "[off]",
    false,
    aimteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    aimteam_active = !aimteam_active;
    con_set_line_subtext(
      self->line_num,
      aimteam_active? "[on]" : "[off]",
      self->active,
      aimteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "aim assist fov", false );
  con_set_line_subtext( line_idx, u_num_to_string_float( aim_fov ), false, CONFG_BLUE );
  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_LEFTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
        if( aim_fov > 0 )
          aim_fov -= 1;
        else
          aim_fov = 0;
      } else {
        if( aim_fov > 0 )
          aim_fov -= 0.1;
      }
    }
    if( action == LINE_ACTION_RIGHTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
        if( aim_fov < 360 )
          aim_fov += 1;
        else
          aim_fov = 360;
      } else {
        if( aim_fov < 360 )
          aim_fov += 0.1;
      }
    }

    if( aim_fov > 360.f )
      aim_fov -= 360.f;
    else if( aim_fov < 0.f )
      aim_fov += 360.f;

    con_set_line_subtext( self->line_num, u_num_to_string_float( aim_fov ), true, CONFG_BLUE );
    } );

  ++line_idx;

  con_set_line_text( line_idx, "aim assist strength", false );
  con_set_line_subtext( line_idx, u_num_to_string_int( aim_strength ), false, CONFG_BLUE );
  con_set_line_callback( line_idx, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_LEFTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
        aim_strength -= 10;
      else
        aim_strength -= 1;
    }
    if( action == LINE_ACTION_RIGHTARROW ) {
      if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
        aim_strength += 10;
      else
        aim_strength += 1;
    }

    if( aim_strength > 100 )
      aim_strength -= 101;
    else if( aim_strength < 0 )
      aim_strength += 101;

    con_set_line_subtext( self->line_num, u_num_to_string_int( aim_strength ), true, CONFG_BLUE );
    } );

  ++line_idx;

  con_set_line_text( line_idx, "rcs xhair", false );
  con_set_line_subtext(
    line_idx,
    crosshair_active? "[on]" : "[off]",
    false,
    crosshair_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    crosshair_active = !crosshair_active;
    con_set_line_subtext(
      self->line_num,
      crosshair_active? "[on]" : "[off]",
      self->active,
      crosshair_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "rcs", false );
  con_set_line_subtext(
    line_idx,
    rcs_active? "[on]" : "[off]",
    false,
    rcs_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    rcs_active = !rcs_active;
    con_set_line_subtext(
      self->line_num,
      rcs_active? "[on]" : "[off]",
      self->active,
      rcs_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  ++line_idx;

  con_set_line_text( line_idx, "triggerbot", false );
  con_set_line_subtext(
    line_idx,
    key_titles[triggerbot_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      con_update_hotkey( self->line_num, triggerbot_key.v );
    }
    } );

  ++line_idx;

  con_set_line_text( line_idx, "trigger team", false );
  con_set_line_subtext(
    line_idx,
    triggerteam_active? "[on]" : "[off]",
    false,
    triggerteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( line_idx, []( CON_LINE *self,U8 action ) {
    triggerteam_active = !triggerteam_active;
    con_set_line_subtext(
      self->line_num,
      triggerteam_active? "[on]" : "[off]",
      self->active,
      triggerteam_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });
}

void menu_show_ui( PROCESS32 *p ) {
  csgop = (CSGO*)p;

  con_clear();
  con_capturing_input = true;

  menu_pages[0] = { "config", &show_page_0 };
  menu_pages[1] = { "general", &show_page_1 };
  menu_pages[2] = { "aim", &show_page_2 };

  show_page_1();
  show_paging( 1 );

  con_set_bottomline_text(
    "LOCALPLAYER: %08X | FLAGS: %08X | menu",
    p->read<U32>( localplayer_ptr ),0x0
  );
}