// by navewindre
// github.com/navewindre

#pragma once
#include "util.h"
#include "typedef.h"

constexpr U8 CON_WIDTH = 52;
constexpr U8 CON_HEIGHT = 16;
constexpr U8 CON_MAX_WIDTH  = CON_WIDTH - 6;
constexpr U8 CON_MAX_HEIGHT = CON_HEIGHT - 5;
constexpr U8 CON_TITLE_HEIGHT = 4;
constexpr U8 LINE_INACTIVE = 0xff;

enum ConsoleColFg_t {
  CONFG_BLACK = 0,
  CONFG_DARKBLUE,
  CONFG_DARKGREEN,
  CONFG_LIGHTBLUE,
  CONFG_RED,
  CONFG_PURPLE,
  CONFG_GOLD,
  CONFG_LIGHTGRAY,
  CONFG_DARKGRAY,
  CONFG_BLUE,
  CONFG_LIGHTGREEN,
  CONFG_CYAN,
  CONFG_LIGHTRED,
  CONFG_MAGENTA,
  CONFG_YELLOW,
  CONFG_WHITE
};

enum ConsoleColBg_t {
  CONBG_BLACK = 0,
  CONBG_DARKBLUE = 0x10,
  CONBG_DARKGREEN = 0x20,
  CONBG_LIGHTBLUE = 0x30,
  CONBG_RED = 0x40,
  CONBG_PURPLE = 0x50,
  CONBG_GOLD = 0x60,
  CONBG_LIGHTGRAY = 0x70,
  CONBG_DARKGRAY = 0x80,
  CONBG_BLUE = 0x90,
  CONBG_LIGHTGREEN = 0xa0,
  CONBG_CYAN = 0xb0,
  CONBG_LIGHTRED = 0xc0,
  CONBG_MAGENTA = 0xd0,
  CONBG_YELLOW = 0xe0,
  CONBG_WHITE = 0xf0
};

enum LineActionType_t {
  LINE_ACTION_ENTER,
  LINE_ACTION_LEFTARROW,
  LINE_ACTION_RIGHTARROW
};

typedef void( *LINE_CALLBACK )( struct CON_LINE* self, U8 action );

struct CON_LINE {
  char text[CON_MAX_WIDTH + 1];
  char subtext[12];

  U8 text_col = 15;
  U8 subtext_col = 15;
  I8 line_num;

  bool active;
  LINE_CALLBACK callback;
};


extern CON_LINE* con_lines;
extern U8        con_selected_line;

extern void con_print_title();
extern void con_resize( U8 w, U8 h );
extern void con_setpos( U8 x, U8 y );
extern void con_set_line_text(
  U8 line,
  const char* text,
  bool selected = false,
  U8 fg = CONFG_WHITE,
  U8 bg = CONBG_BLACK
);

extern void con_set_line_subtext(
  U8 line,
  const char* text,
  bool selected = false,
  U8 fg = CONFG_WHITE,
  U8 bg = CONBG_BLACK
);

extern void con_set_line(
  U8 line,
  const char* text,
  const char* subtext,
  bool selected = false,
  U8 fg = CONFG_WHITE,
  U8 bg = CONBG_BLACK
);

extern void con_init();
extern void con_set_line_callback( U8 line, LINE_CALLBACK cb );
extern void con_set_bottomline_text( const char* text, ... );
extern void con_set_bottomline_text( U8 color, const char* text, ... );
extern void con_clear_bottomline_text();
extern void con_clear_line( U8 line );
extern void con_clear();
extern void con_refresh();
extern void con_print( U8 color, const char* text, ... );
extern void con_print_line( U8 line );

inline void con_print_colors() {
  for( I16 i = 0; i <= 255; ++i )
    con_print( (U8)i, "%003d", i );
}

extern STR<64> con_progressbar( F32 progress, U8 width = CON_MAX_WIDTH );
