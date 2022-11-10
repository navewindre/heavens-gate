// by navewindre
// github.com/navewindre
#pragma once

#include <time.h>

#include "../conin.h"
#include "../process.h"
#include "../conout.h"
#include "../util.h"
#include "../vars.h"

#include "../mathutil.h"
#include "../menu.h"
#include "../setting.h"

#include "csgoentity.h"
#include "csgoplayer.h"


extern SETTING_HOLDER settings_holder;

extern SETTING<I32>  triggerbot_key;
extern SETTING<bool> bhop_active;
extern SETTING<bool> glow_active;

extern F64  perf_ipt;
extern F64  perf_tps;

const char* const LOCALPLAYER_SIG = "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF";
const char* const FORCEATTACK_SIG = "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04";
const char* const GLOWSTRUCT_SIG  = "A1 ? ? ? ? A8 01 75 4B";
const char* const JUMP_SIG        = "8B 0D ? ? ? ? 8B D6 8B C1 83 CA 02";

extern U32 localplayer_ptr;

extern void hack_run_bhop( CSGO* p );
extern void hack_run_trigger( CSGO* p );
extern void hack_run_glow( CSGO* p );
extern CSGO* hack_init();

static void hack_run( PROCESS32* p ) {
  static U32 last_tick;
  
  struct timespec time;
  timespec_get( &time, TIME_UTC );

  U32 tick = time.tv_nsec;
  
  CSGO* csgo = (CSGO*)p;

  hack_run_bhop( csgo );
  hack_run_trigger( csgo );
  hack_run_glow( csgo );

  perf_ipt = (tick - last_tick) * 0.000000001; 
  perf_tps = 1.0 / perf_ipt;


  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  con_set_bottomline_text(
    "local: 0x%08x | flags: 0x%03x | tps: %.f",
    local.base,
    local.m_fFlags(),
    perf_tps
  );
  
  last_tick = tick;
}