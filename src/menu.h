#include <thread>

#include "conin.h"
#include "process.h"
#include "conout.h"
#include "util.h"
#include "vars.h"

void menu_show_ui(CSGO *p) {
  con_clear();
  con_capturing_input=true;

  con_set_line_text(0,"bhop",false);
  con_set_line_subtext(
    0,
    bhop_active? "[on]" : "[off]",
    true,
    bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback(0,[](CON_LINE *self,U8 action) {
    bhop_active=!bhop_active;
    con_set_line_subtext(
      0,
      bhop_active? "[on]" : "[off]",
      self->active,
      bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });

  con_set_line_text(1,"glow",false);
  con_set_line_subtext(
    1,
    glow_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback(1,[](CON_LINE *self,U8 action) {
    glow_active=!glow_active;
    con_set_line_subtext(
      1,
      glow_active? "[on]" : "[off]",
      self->active,
      glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
    });


  con_set_line_text(2,"triggerbot",false);
  con_set_line_subtext(
    2,
    key_titles[triggerbot_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 2,[]( CON_LINE *,U8 action ) {
    if( action==LINE_ACTION_ENTER ) {
      con_update_hotkey( 2 , triggerbot_key);
    }
    } );

  con_set_line_text(3,"display color pallette");
  con_set_line_subtext(
    3,
    key_titles[0x0D],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback(3,[](CON_LINE *,U8 action) {
    static bool toggle;
    if(action==LINE_ACTION_ENTER) {
      if(!toggle)
        con_print_colors();
      else {
        con_refresh();
      }

      toggle=!toggle;
    }
    });

  /*
  con_set_line_text(4,"test input");
  con_set_line_subtext(
    4,
    key_titles[trigger_key],
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback(4,[](CON_LINE *,U8 action) {
    if( action==LINE_ACTION_ENTER ) {
      con_triggerkey_update( 4 );
    }
    });
    */

  con_set_bottomline_text(
    "LOCALPLAYER: %08X | FLAGS: %08X | menu",
    p->read< U32 >( localplayer_ptr ),0x0
  );
}