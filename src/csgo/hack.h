//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once

#include <time.h>

#include "../conout.h"
#include "../setting.h"

#include "csgo.h"
#include "csgoentity.h"
#include "csgoplayer.h"

struct CMD_FUNC {
  using func_t = void( __cdecl* )( VECTOR<STR<64>> );
  func_t func;
  STR<64> name;
};

void __cdecl game_hack_toggle( VECTOR<STR<64>> args );
static CMD_FUNC g_hack_toggle = {
  &game_hack_toggle,
  "hg_"
};

static CMD_FUNC* cmd_funcs[] = {
  &g_hack_toggle,
};

extern SETTING_HOLDER settings;
extern F64  perf_ipt;
extern F64  perf_tps;
static I64  perf_drift;
extern U64  perf_tickrate;

const char* const LOCALPLAYER_SIG = "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF";
const char* const GLOWSTRUCT_SIG  = "A1 ? ? ? ? A8 01 75 4B";

extern U32 localplayer_ptr;
extern U32 clientstate_ptr;
extern U32 pitch_ptr;
extern U32 yaw_ptr;

extern void hack_run_aim( CSGO* p );
extern void hack_run_bhop( CSGO* p );
extern void hack_run_trigger( CSGO* p );
extern void hack_run_chams( CSGO* p );
extern void hack_run_glow( CSGO* p );
extern void hack_run_nightmode( CSGO* p );
extern void hack_run_noflash( CSGO* p );
extern void hack_run_crosshair( CSGO* p );
extern void hack_run_clantag( CSGO* p );
extern CSGO* hack_init();

inline U64 hack_calc_perf_metrics( U64 tickrate ) {
  static U64 last_tick;
  U64 tick = u_tick();

  static U64 last_tps_tick;
  static U64 tick_counter = 0;
  
  perf_ipt = (tick - last_tick) / (F64)T_SEC; 

  if( tick - last_tps_tick < T_SEC * 0.5 )
    ++tick_counter;
  else {
    perf_tps = (F64)tick_counter * 2;
    tick_counter = 0;
    last_tps_tick = tick;

    I64 tick_delta = (I64)tickrate - (I64)perf_tps;
    F64 tick_ratio = (F64)tick_delta / (F64)( tickrate ) * 10;
    if( tick_ratio < 1.0 )
      tick_ratio = 1.0;

    perf_drift += (I64)( 100.0 * tick_ratio ) * ( tick_delta < 0 ? 1 : -1 ); 
  }

  if( tickrate > 0 ) {
    U64 delay = (T_SEC / tickrate);
    u_sleep( delay + perf_drift );
  }
  else {
    u_sleep( 1 );
  }

  last_tick = tick;
  return perf_drift;
}

static bool hack_run( PROCESS32* p ) {
  hack_calc_perf_metrics( perf_tickrate );

  CSGO* csgo = (CSGO*)p;

  hack_run_aim( csgo );
  hack_run_bhop( csgo );
  hack_run_trigger( csgo );
  hack_run_chams( csgo );
  hack_run_glow( csgo );
  hack_run_nightmode( csgo );
  hack_run_noflash( csgo );
  hack_run_clantag( csgo );

  
  static U32 string_ptr = 0;
  if( !string_ptr ) {
    string_ptr = p->code_match( csgo->engine, "B9 ? ? ? ? E8 ? ? ? ? 84 C0 75 0E 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 04 83 05 ? ? ? ? ? 75 04" );
    string_ptr = p->read<U32>( string_ptr + 1 );
  }
    
  STR<64> buf;
  p->read( string_ptr, buf, sizeof( buf ) );

  for( U16 i = 0; i < 1; ++i ) {
    CMD_FUNC* fn = cmd_funcs[i];

    if( strncmp( fn->name.data, buf.data, strlen( fn->name.data ) ) == 0 ) {
      fn->func( { buf } );
      p->write<U8>( string_ptr, 0 );
    }
  }

  
  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  con_set_bottomline_text(
    "local: 0x%08x | flags: 0x%03x | tps: %.0f",
    local.base,
    local.m_fFlags(),
    (F32)perf_tps
  );

  return csgo->valid();
}