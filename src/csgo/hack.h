//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once

#include <time.h>

#include "../conout.h"
#include "../setting.h"

#include "csgo.h"
#include "csgoentity.h"
#include "csgoplayer.h"


extern SETTING_HOLDER settings;
extern F64  perf_ipt;
extern F64  perf_tps;
static I64  perf_drift;
extern U64  perf_tickrate;

const char* const LOCALPLAYER_SIG = "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF";
const char* const GLOWSTRUCT_SIG  = "A1 ? ? ? ? A8 01 75 4B";

extern U32 localplayer_ptr;

extern void hack_run_aim( CSGO* p );
extern void hack_run_bhop( CSGO* p );
extern void hack_run_trigger( CSGO* p );
extern void hack_run_chams( CSGO* p );
extern void hack_run_glow( CSGO* p );
extern void hack_run_nightmode( CSGO* p );
extern void hack_run_crosshair( CSGO* p );
extern void hack_run_clantag( CSGO* p );
extern CSGO* hack_init();

inline U64 hack_calc_perf_metrics( U64 tickrate ) {
  static U64 last_tick;
  U64 tick = u_tick();

  static I64 last_tick_delta;
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
  hack_run_glow( csgo );
  hack_run_nightmode( csgo );
  hack_run_clantag( csgo );
  
  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  con_set_bottomline_text(
    "local: 0x%08x | flags: 0x%03x | tps: %.0f",
    local.base,
    local.m_fFlags(),
    (F32)perf_tps
  );

  return csgo->valid();
}