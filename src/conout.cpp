// by navewindre
// github.com/navewindre

#include <windows.h>
#include <stdio.h>
#include <malloc.h>

#include "mathutil.h"
#include "util.h"
#include "conout.h"

CON_LINE* con_lines = (CON_LINE*)malloc( sizeof( CON_LINE ) * ( CON_MAX_HEIGHT + 1 ) );
U8        con_selected_line = 0;

void con_init() {
  AllocConsole();
  freopen( "CONOUT$","w",stdout );

  HWND con_handle = GetConsoleWindow();
  SetConsoleMode(
    con_handle,
    DISABLE_NEWLINE_AUTO_RETURN|ENABLE_WINDOW_INPUT|ENABLE_VIRTUAL_TERMINAL_INPUT
  );

  SetConsoleTitleA( "TESTING" );

  for( U8 i = 0; i < CON_MAX_HEIGHT; ++i ) {
    con_lines[i].line_num = -1;
  }
  
  con_setpos( 0, 0 );
  con_resize( CON_WIDTH, CON_HEIGHT );
  con_print_title();
  
  HANDLE stdo = GetStdHandle( STD_OUTPUT_HANDLE );

  CONSOLE_CURSOR_INFO info;
  GetConsoleCursorInfo( stdo, &info );
  info.bVisible = false;
  SetConsoleCursorInfo( stdo, &info );
}

void setc( U8 c ) {
  SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), c );
}

void con_resize( U8 w, U8 h ) {
  SMALL_RECT console_wnd{};
  console_wnd.Right = w;
  console_wnd.Bottom = h;

  HANDLE std_handle = GetStdHandle( STD_OUTPUT_HANDLE );
  SetConsoleWindowInfo( std_handle, true, &console_wnd );
  SetConsoleScreenBufferSize( std_handle, COORD( w + 1, h + 1 ) );
  SetConsoleWindowInfo( std_handle,true,&console_wnd );

  HWND con_handle = GetConsoleWindow();
  HMENU sys_menu  = GetSystemMenu( con_handle, false );

  if( sys_menu ) {
    DeleteMenu( sys_menu, SC_MAXIMIZE, MF_BYCOMMAND );
    DeleteMenu( sys_menu, SC_SIZE, MF_BYCOMMAND );
  }
}

void con_print( U8 color, const char* text, ... ) {
  SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), color );
  va_list args;
  va_start( args, text );
  vprintf_s( text, args );
  va_end( args );
}

void con_print_title() {
  con_print( 133, "  =============== [ heaven's gate ] ===============  " );
  con_print( 15, "                    [%s]\n", __DATE__ );
  con_print( 15, "\n" );
}

void con_setpos( U8 x, U8 y ) {
  HANDLE std_handle = GetStdHandle( STD_OUTPUT_HANDLE );
  SetConsoleCursorPosition( std_handle, COORD( x, y ) );
}

void con_print_line( U8 line ) {
  CON_LINE* l = &con_lines[line];
  l->line_num = line;
  
  con_setpos( 0, CON_TITLE_HEIGHT + line + 1 );

  U8 border_col = ( !l->active? CONFG_DARKGRAY : CONFG_WHITE );
  
  con_print( border_col, " = " );
  con_print( l->text_col, l->text );

  U8 spacing = CON_MAX_WIDTH - (U8)( strlen( l->text ) + strlen( l->subtext ) );
  char* spacer = (char*)_alloca( spacing + 1 );
  memset( spacer, ' ', spacing );
  spacer[spacing] = 0;

  con_print( 0, spacer );
  if( l->subtext_col > 15 || !l->active )
    con_print( l->subtext_col, l->subtext );
  else
    con_print( l->subtext_col + CONBG_DARKGRAY, l->subtext );
    
  con_print( border_col, " =" ); 
}

void con_set_line( U8 line, const char* text, bool selected, U8 fg, U8 bg ) {
  if( line > CON_MAX_HEIGHT )
    return;

  CON_LINE* l = &con_lines[line];
  strcpy( l->text, text );
  l->text_col = fg + bg;
  strcpy( l->subtext, text );
  l->subtext_col = fg + bg;
  l->active = selected;
  
  con_print_line( line );
}

void con_set_line_text( U8 line, const char* text, bool selected, U8 fg, U8 bg ) {
  if( line > CON_MAX_HEIGHT )
    return;

  CON_LINE* l = &con_lines[line];
  strcpy( l->text, text );
  l->text_col = fg + bg;
  l->active = selected;
  
  con_print_line( line );
}

void con_set_line_subtext( U8 line, const char* text, bool selected, U8 fg, U8 bg ) {
  if( line > CON_MAX_HEIGHT )
    return;

  CON_LINE* l = &con_lines[line];
  strcpy( l->subtext, text );
  l->subtext_col = fg + bg;
  l->active = selected;
  
  con_print_line( line );
}

void con_set_line_callback( U8 line, LINE_CALLBACK cb ) {
  if( line > CON_MAX_HEIGHT )
    return;

  con_lines[line].callback = cb;
}

void con_clear_line( U8 line ) {
  CON_LINE* l = &con_lines[line];

  l->line_num   = -1;
  l->active     = false;
  l->callback   = nullptr;
  l->subtext[0] = 0;
  l->text[0]    = 0;
  
  
  con_setpos( 0, CON_TITLE_HEIGHT + line + 1 );
  
  char* spacer = (char*)_alloca( CON_WIDTH + 1 );
  memset( spacer, ' ', CON_WIDTH );
  spacer[CON_WIDTH] = 0;
  
  con_print( 0, spacer );
}

void con_clear() {
  for( U8 i = 0; i < CON_MAX_HEIGHT; ++i )
    con_clear_line( i );
}

void con_set_bottomline_text( U8 color, const char* text, ... ) {
  con_setpos( 0, CON_HEIGHT );
  setc( color );

  char str_buffer[256]{};
  va_list args;
  va_start( args, text );
  vsprintf_s( str_buffer, 256, text, args );
  va_end( args );

  strcpy( con_lines[CON_MAX_HEIGHT].text, str_buffer );
  con_lines[CON_MAX_HEIGHT].text_col = color;
  
  U8 length =  (U8)strlen( str_buffer );
  U8 spacing = CON_WIDTH - length + 1;

  memset( str_buffer + length, ' ', spacing );
  str_buffer[CON_WIDTH] = '\0';
  WriteConsoleA( GetStdHandle( STD_OUTPUT_HANDLE ), str_buffer, CON_WIDTH, 0, 0 );
}

void con_set_bottomline_text( const char* text, ... ) {
  char str_buffer[256]{};
  va_list args;
  va_start( args, text );
  vsprintf_s( str_buffer, 256, text, args );
  va_end( args );

  con_set_bottomline_text( CONFG_BLACK + CONBG_LIGHTGRAY, str_buffer );
}

void con_clear_bottomline_text() {
  con_setpos( 0, CON_HEIGHT - 1 );

  char* spacer = (char*)_alloca( CON_WIDTH + 1 );
  memset( spacer, ' ', CON_WIDTH );
  spacer[CON_WIDTH] = 0;
  
  con_print( 0, spacer );
}

void con_refresh() {
  con_print( 0, "\r" );
  system( "cls" );
  con_setpos( 0, 0 );
  con_print_title();
  for( U8 i = 0; i < CON_MAX_HEIGHT; ++i ) {
    CON_LINE* line = &con_lines[i];
    
    if( line->line_num != -1 || !!strlen( line->text ) )
      con_print_line( i );
  }

  CON_LINE* bottom_line = &con_lines[CON_MAX_HEIGHT];
  con_set_bottomline_text( bottom_line->text_col, bottom_line->text );
}


STR<64> con_progressbar( F32 progress, U8 width ) {
  static U64  last_call = u_tick();
  static U8   cur_spinner = 0;
  static char spinner_anim[] = {
    '-',
    '\\',
    '|',
    '/',
    '-',
    '\\',
    '|',
    '/'
  };
  
  if( width <= 3 )
    return "";
  
  char* ret = (char*)_alloca( width + 1 );
  memset( ret, ' ', width );
  ret[width] = 0;

  ret[0] = '[';
  ret[width - 1] = ']';

  
  progress = u_clamp( progress, 0.f, 1.f ); 
  
  U8 total = (U8)( ( width - 2 ) * progress );
  for( size_t i = 1; i < total; ++i ) {
    ret[i] = '=';
  }


  if( progress < 1.0f ) {
    if( u_tick() - last_call > 200 ) {
      ++cur_spinner;
      last_call = u_tick();
    }
  
    ret[total - 1 + 1] = spinner_anim[cur_spinner];
  }

  return ret;
}