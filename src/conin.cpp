// by navewindre
// github.com/navewindre

#include <windows.h>
#include "conin.h"
#include "vars.h"
#include <cstdio>
#include "mathutil.h"

U8*  con_key_states = (U8*)malloc( 256 );
bool con_capturing_input = false;

U8   con_captured_key = NULL;
bool con_capturing_key = false;
bool con_enable_hook = false;
HHOOK con_keybdhook = NULL;
HHOOK con_mousehook = NULL;

void con_update_hotkey( U8 line , I32 &hk) {
  con_capturing_key = true;
  con_enable_hook = true;

  con_set_line_subtext(
    line,
    "[...]",
    true,
    CONFG_LIGHTRED );

  while( con_capturing_key ) {
    Sleep( 100 );
  }

  if( con_captured_key!=0x1B ) 
    hk = con_captured_key;

  con_set_line_subtext(
    line,
    key_titles[hk],
    true,
    CONFG_LIGHTBLUE );
}

I8 con_find_nearest_line( I8 cur, bool reverse ) {
  if( reverse ) {
    if( cur <= 0 )
      return -1;
      
    for( I8 i = cur - 1; i >= 0; --i ) {
      if( con_lines[i].line_num != -1 )
        return i;
    }
  }
  else {
    if( cur >= CON_MAX_HEIGHT )
      return -1;
      
    for( I8 i = cur + 1; i < CON_MAX_HEIGHT; ++i ) {
      if( con_lines[i].line_num != -1 )
        return i;
    }
  }

  return -1;
}

void con_line_change_callback( U8 prev, U8 current ) {
  con_lines[prev].active = false;
  con_lines[current].active = true;

  con_print_line( prev );
  con_print_line( current );
}

void con_handle_ui_event( CON_EVENT e ) {
  if( !con_capturing_input )
    return;

  if( con_key_states[e.keycode] || !e.state )
    return;

  I8 prev_line = con_selected_line;
  I8 line = -1;
  
  switch( e.keycode ) {
  case VK_UP:
    if( ( line = con_find_nearest_line( (I8)con_selected_line, true ) ) == -1 )
      return;

    con_line_change_callback( prev_line, con_selected_line = line );
    return;
  case VK_DOWN:
    if( ( line = con_find_nearest_line( (I8)con_selected_line, false ) ) == -1 )
      return;

    con_line_change_callback( prev_line, con_selected_line = line );
    return;
  case VK_LEFT:
  case VK_RIGHT:
  case VK_RETURN:
  {
    CON_LINE* line = &con_lines[con_selected_line];
    if( !line->callback || line->line_num == -1 )
      return;

    U8 con_event = (e.keycode == VK_RETURN)? LINE_ACTION_ENTER :
      (e.keycode == VK_LEFT)? LINE_ACTION_LEFTARROW : LINE_ACTION_RIGHTARROW;

    line->callback( line, con_event );
  }
  }
}

void con_parse_event( CON_EVENT e ) {
  switch( e.keycode ) {
  case VK_UP:
  case VK_DOWN:
  case VK_LEFT:
  case VK_RIGHT:
  case VK_RETURN:
    con_handle_ui_event( e );
  default:
    con_key_states[e.keycode] = e.state; break;  
  } 
}

void con_parse_events( HANDLE stdi ) {
  INPUT_RECORD records[64];
  ULONG count;

  if( !ReadConsoleInputA( stdi, records, 64, &count ) )
    return;

  for( U32 i = 0; i < count; ++i ) {
    INPUT_RECORD* r = &records[i];

    if( r->EventType != KEY_EVENT )
      continue;
    
    CON_EVENT e;

    e.state =   !r->Event.KeyEvent.bKeyDown;
    e.keycode = r->Event.KeyEvent.wVirtualKeyCode;

    con_parse_event( e );
  }
}

ULONG __stdcall con_handler( void * ) {
  HANDLE stdi = GetStdHandle( STD_INPUT_HANDLE );
  for( ;; ) {
    con_parse_events( stdi );

    Sleep( 1 );
  }
}

LRESULT CALLBACK con_khook_callback( int n,WPARAM w,LPARAM l )
{
  KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT *) l);
  if( con_capturing_key && w == WM_KEYDOWN ) {
    if( key.vkCode == 0x0D ||
        key.vkCode == 0x26 ||
        key.vkCode == 0x28 ) {
      return 1;
    }

    con_capturing_key = false;
    con_captured_key = key.vkCode;
    PostQuitMessage( 0 );
  }
  return CallNextHookEx( con_keybdhook, n, w, l );
}

LRESULT CALLBACK con_mhook_callback( int n,WPARAM w,LPARAM l )
{
  MSLLHOOKSTRUCT mos = *((MSLLHOOKSTRUCT *) l);
  if( con_capturing_key ) {
    switch( w ) {
    case WM_LBUTTONDOWN:
      con_captured_key = 1;
      con_capturing_key = false;
      break;

    case WM_RBUTTONDOWN:
      con_captured_key = 2;
      con_capturing_key = false;
      break;

    case WM_MBUTTONDOWN:
      con_captured_key = 4;
      con_capturing_key = false;
      break;

    case WM_XBUTTONDOWN:
      MSLLHOOKSTRUCT *info = reinterpret_cast<MSLLHOOKSTRUCT *>( l );
      I8 xb = HIWORD( info->mouseData );
      con_captured_key = xb + 0x4;
      con_capturing_key = false;
      break;
    }
    if( !con_capturing_key )
      PostQuitMessage( 0 ); 
  }
  return CallNextHookEx( con_mousehook, n, w, l );
}

ULONG __stdcall con_hook_handler( void* ) {
  HINSTANCE con_hinstance = GetModuleHandle( NULL );
  MSG con_inputMsg;

  for( ;;) {
    if( con_enable_hook ) {

      con_keybdhook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        con_khook_callback,
        con_hinstance,
        0 );

      con_mousehook = SetWindowsHookExW(
        WH_MOUSE_LL,
        con_mhook_callback,
        con_hinstance,
        0 );

      while( GetMessage( &con_inputMsg,NULL,0,0 ) )
      {
        TranslateMessage( &con_inputMsg );
        DispatchMessageA( &con_inputMsg );
      }

      UnhookWindowsHookEx( con_keybdhook );
      UnhookWindowsHookEx( con_mousehook );
      con_enable_hook = false;
    }
    Sleep( 1 );
  }

  return 0;
}
