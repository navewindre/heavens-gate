// by navewindre
// github.com/navewindre
#pragma once

#include <thread>

#include "../conin.h"
#include "../process.h"
#include "../conout.h"
#include "../util.h"
#include "../vars.h"

#include "sdk.h"

static bool bhop_active = true;
static bool glow_active = true;

static I32  triggerbot_key = 0x06;

const char* const LOCALPLAYER_SIG = "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF";
const char* const FORCEATTACK_SIG = "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04";
const char* const GLOWSTRUCT_SIG  = "A1 ? ? ? ? A8 01 75 4B";
const char* const JUMP_SIG        = "8B 0D ? ? ? ? 8B D6 8B C1 83 CA 02";
const U32         FLAGS_OFF       = 0x104;
const U32         TEAM_OFF        = 0xF4;
const U32         CROSSHAIRID_OFF = 0x11838;

using namespace std::chrono_literals;

static U32 localplayer_ptr;
static U32 jump_ptr;
static U32 attack_ptr;
static U32 glow_ptr;

inline void hack_run_bhop( CSGO* p ) {
  if( !bhop_active || !( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) ) 
    return;

  assert( !!localplayer_ptr );
  assert( !!jump_ptr );

  U32 player_base = p->read<U32>( localplayer_ptr );
  if( !player_base )
    return;
  
  I32 player_flags = p->read<I32>( player_base + FLAGS_OFF );
  bool  air = !( player_flags & 1 << 0 );

  p->write<I32>( jump_ptr, air ? 4 : 6 );
}

inline void hack_run_trigger( CSGO* p ) {
  if( !( GetAsyncKeyState( triggerbot_key ) & 0x8000 ) )
    return;

  assert( !!localplayer_ptr );
  assert( !!attack_ptr );

  U32 player_base = p->read<U32>( localplayer_ptr );
  I32 crosshairid = p->read<I32>( player_base + CROSSHAIRID_OFF );
  if( crosshairid > 0 && crosshairid < 65 )
    p->write< I32 >( attack_ptr, 6 );
}

inline void hack_run_glow( CSGO* p ) {
  if( !glow_active )
    return;
  
  U32 player_base = p->read< U32 >( localplayer_ptr );
  if( !player_base )
    return;

  U32 local_team = p->read< I32 >( player_base + TEAM_OFF );

  GLOW_OBJ_MANAGER glow;
  p->read( glow_ptr, &glow, sizeof( GLOW_OBJ_MANAGER ) );

  if( !glow.count )
    return;
  
  GLOW_OBJECT* glow_objects = (GLOW_OBJECT*)malloc( sizeof( GLOW_OBJECT ) * glow.count );
  p->read( (U32)( glow.objects ), glow_objects, sizeof( GLOW_OBJECT ) * glow.count );

  for( U32 i = 0; i < glow.count; ++i ) {
    GLOW_OBJECT& o = glow_objects[i];

    if( !o.ent || o.ent == player_base )
      continue;

    I32 team = p->read<I32>( o.ent + TEAM_OFF );
    if( team == local_team || (team != 2 && team != 3) )
      continue;

    
    COLOR color = ( team == 2 ) ?
      COLOR{ 1.0f, 0.17f, 0.37f, 0.7f } : 
      COLOR{ 0.17f, 0.67f, 0.8f, 0.8f };

    o.rwo = true;
    o.rwuo = false;
    o.bloom_amt = 0.7f;
    o.full_bloom = false;
    o.color = color;

    U32 obj_address = (U32)glow.objects + i * sizeof( GLOW_OBJECT );
    p->write( obj_address + 0x8, (void*)( (U32)&glow_objects[i] + 0x8 ), sizeof( GLOW_OBJECT ) - 0x16 );
  }

  free( glow_objects );
}

inline void hack_print_offset( U8 line, const char* name, ULONG offset ) {
  con_set_line_text( line, name );
  U8 color = offset > 0x1000 ? CONFG_WHITE : CONFG_RED;
  
  con_set_line_subtext( line, u_num_to_string_hex( offset ), false, color );
}

inline void hack_init( CSGO* p ) {
  con_clear();
  
  p->client = p->get_module32( "client.dll"fnv );
  p->engine = p->get_module32( "engine.dll"fnv );

  con_set_bottomline_text( "searching for signatures..." );
  
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  hack_print_offset( 1, "jump", jump_ptr );
  hack_print_offset( 2, "attack", attack_ptr );
  hack_print_offset( 3, "glow", glow_ptr );
  
  localplayer_ptr = p->read<U32>( p->code_match( p->client, LOCALPLAYER_SIG ) + 3 ) + 4;
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  jump_ptr        = p->read<U32>( p->code_match( p->client, JUMP_SIG ) + 2 );
  hack_print_offset( 1, "jump", jump_ptr );
  attack_ptr      = p->read<U32>( p->code_match( p->client, FORCEATTACK_SIG ) + 2 );
  hack_print_offset( 2, "attack", attack_ptr ); 
  glow_ptr        = p->read<U32>( p->code_match( p->client, GLOWSTRUCT_SIG ) + 1 ) + 4;
  hack_print_offset( 3, "glow", glow_ptr );
}