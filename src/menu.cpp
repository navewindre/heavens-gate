#include "menu.h"
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
      settings_holder.load();
    }
  } );

  con_set_line_text( 1, "save config", false );
  con_set_line_subtext( 1, "[ENTER]", false, CONFG_LIGHTBLUE );
  con_set_line_callback( 1, []( CON_LINE* self, U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      settings_holder.save();
    }
  } ); 
}

void show_page_1() {
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

  con_set_line_text( 5, "dump syscalls to syscall_arch.dump", false );
  con_set_line_subtext(
    5,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 5, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      syscall_dump_to_file();
  } );

  con_set_line_text( 6, "dump interfaces to interfaces.dump", false );
  con_set_line_subtext(
    6,
    key_titles[VK_RETURN],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 6, []( CON_LINE*, U8 action ) {
    if( action == LINE_ACTION_ENTER )
      csgo_dump_ifaces_to_file( csgop );
  } );
}

void show_page_2() {
  con_set_line_text( 0, "triggerbot", false);
  con_set_line_subtext(
    0,
    key_titles[triggerbot_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 0, []( CON_LINE *,U8 action ) {
    if( action == LINE_ACTION_ENTER ) {
      con_update_hotkey( 0, triggerbot_key );
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