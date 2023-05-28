//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "menu.h"
#include "conin.h"

#include "csgo/hack.h"

CSGO* csgop;

const I8 MENU_PAGE_MIN = 0;
const I8 MENU_PAGE_MAX = 2;
I8 menu_page = 1;

typedef void(*CON_PAGE_FN)();

struct MENU_PAGE {
  const char* name;
  CON_PAGE_FN page_fn;
};

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
  con_set_line_text( 0, "load config", false );
  con_set_line_subtext( 0, "[ENTER]", false, CONFG_LIGHTBLUE );
  con_set_line_callback( 0, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      settings.load();
    }
    } );

  con_set_line_text( 1, "save config", false );
  con_set_line_subtext( 1, "[ENTER]", false, CONFG_LIGHTBLUE );
  con_set_line_callback( 1, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      settings.save();
    }
    } ); 
}

void show_page_1() {
  static SETTING<bool>& bhop_active = *settings.find<bool>( "bhop_active"fnv );
  static SETTING<bool>& chams_active = *settings.find<bool>( "chams_active"fnv );
  static SETTING<bool>& glow_active = *settings.find<bool>( "glow_active"fnv );
  static SETTING<bool>& nightmode_active = *settings.find<bool>( "nightmode_active"fnv );
  static SETTING<bool>& clantag_active = *settings.find<bool>( "clantag_active"fnv );

  con_set_line_text( 0,"bhop",false );
  con_set_line_subtext(
    0,
    bhop_active? "[on]" : "[off]",
    con_selected_line == 0,
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

  con_set_line_text(1, "chams", false);
  con_set_line_subtext(
    1,
    chams_active? "[on]" : "[off]",
    false,
    chams_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 1, []( CON_LINE *self,U8 action ) {
    chams_active = !chams_active;
    hack_run_chams( csgop );
    con_set_line_subtext(
      1,
      chams_active? "[on]" : "[off]",
      self->active,
      chams_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  con_set_line_text(2, "glow", false);
  con_set_line_subtext(
    2,
    glow_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 2, []( CON_LINE *self,U8 action ) {
    glow_active = !glow_active;
    con_set_line_subtext(
      2,
      glow_active? "[on]" : "[off]",
      self->active,
      glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  con_set_line_text(3, "nightmode", false);
  con_set_line_subtext(
    3,
    nightmode_active? "[on]" : "[off]",
    false,
    nightmode_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 3, []( CON_LINE *self,U8 action ) {
    nightmode_active = !nightmode_active;
    con_set_line_subtext(
      3,
      nightmode_active? "[on]" : "[off]",
      self->active,
      nightmode_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  con_set_line_text( 4, "clantag changer" );
  con_set_line_subtext(
    4,
    clantag_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 4, []( CON_LINE* self, U8 action ) {
    clantag_active = !clantag_active;
    con_set_line_subtext(
      4,
      clantag_active? "[on]" : "[off]",
      self->active,
      clantag_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    ); 
    } );

  con_set_line_text( 5, "dump all classes to classes.dump", false );
  con_set_line_subtext(
    5,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 5, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      csgo_dump_classes( csgop );
    } ); 


  con_set_line_text( 6, "dump syscalls to syscall_arch.dump", false );
  con_set_line_subtext(
    6,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 6, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      syscall_dump_to_file();
    } );

  con_set_line_text( 7, "dump interfaces to interfaces.dump", false );
  con_set_line_subtext(
    7,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 7, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      csgo_dump_ifaces_to_file( csgop );
    } );
}

void show_page_2() {
  static SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );
  static SETTING<I32>& triggerbot_key = *settings.find<I32>( "triggerbot_key"fnv );

  con_set_line_text( 0,"aim assist",false );
  con_set_line_subtext(
    0,
    aim_active? "[on]" : "[off]",
    con_selected_line == 0,
    aim_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback(0,[]( CON_LINE *self,U8 action ) {
    aim_active = !aim_active;
    con_set_line_subtext(
      0,
      aim_active? "[on]" : "[off]",
      self->active,
      aim_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
  });

  con_set_line_text( 1, "triggerbot", false);
  con_set_line_subtext(
    1,
    key_titles[triggerbot_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 1, []( CON_LINE *,U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      con_update_hotkey( 1, triggerbot_key.v );
    }
    } );
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