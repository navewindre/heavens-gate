// by navewindre
// github.com/navewindre

#pragma once

#include "conout.h"

enum ConEventType_t {
  EVENT_NONE = -1,
  EVENT_KEYDOWN = 0,
  EVENT_KEYUP = 1,
};

struct CON_EVENT {
  I32 keycode;
  I32 state;
};

extern U8* con_key_states;
extern bool con_capturing_input;

extern void con_update_hotkey( U8 line , I32 &var );

extern ULONG __stdcall con_handler( void* );
extern ULONG __stdcall con_hook_handler( void * );
