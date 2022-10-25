// by navewindre
// github.com/navewindre

#include <thread>

#include "conin.h"
#include "process.h"
#include "sdk.h"
#include "conout.h"
#include "util.h"

const char* LOCALPLAYER_SIG = "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF";
const char* FORCEATTACK_SIG = "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04";
const char* GLOWSTRUCT_SIG  = "A1 ? ? ? ? A8 01 75 4B";
const char* JUMP_SIG        = "8B 0D ? ? ? ? 8B D6 8B C1 83 CA 02";
const U32   FLAGS_OFF       = 0x104;
const U32   TEAM_OFF        = 0xF4;
const U32   CROSSHAIRID_OFF = 0x11838;

using namespace std::chrono_literals;

static U32 localplayer_ptr;
static U32 jump_ptr;
static U32 attack_ptr;
static U32 glow_ptr;

static bool bhop_active = true;
static I32  trigger_key = 0x06;
static bool glow_active = true;

void hack_run_bhop( CSGO* p ) {
  if( !bhop_active || !( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) ) 
    return;

  assert( !!localplayer_ptr );
  assert( !!jump_ptr );

  U32 player_base = p->read< U32 >( localplayer_ptr );
  if( !player_base )
    return;
  
  I32 player_flags = p->read< I32 >( player_base + FLAGS_OFF );
  bool  air = !( player_flags & 1 << 0 );

  p->write< I32 >( jump_ptr, air ? 4 : 6 );
}

void hack_run_trigger( CSGO* p ) {
  if( !( GetAsyncKeyState( trigger_key ) & 0x8000 ) )
    return;

  assert( !!localplayer_ptr );
  assert( !!attack_ptr );

  U32 player_base = p->read< U32 >( localplayer_ptr );
  I32 crosshairid = p->read< I32 >( player_base + CROSSHAIRID_OFF );
  if( crosshairid > 0 && crosshairid < 65 )
    p->write< I32 >( attack_ptr, 6 );
}

void hack_run_glow( CSGO* p ) {
  if( !glow_active )
    return;
  
  U32 player_base = p->read< U32 >( localplayer_ptr );
  if( !player_base )
    return;

  U32 local_team = p->read< I32 >( player_base + TEAM_OFF );

  glow_obj_manager_t glow;
  p->read( glow_ptr, &glow, sizeof( glow_obj_manager_t ) );

  if( !glow.m_count )
    return;
  
  glow_object_t* glow_objects = (glow_object_t*)malloc( sizeof( glow_object_t ) * glow.m_count );
  p->read( (U32)( glow.m_objects ), glow_objects, sizeof( glow_object_t ) * glow.m_count );

  for( U32 i = 0; i < glow.m_count; ++i ) {
    glow_object_t& o = glow_objects[i];

    if( !o.m_ent || o.m_ent == player_base )
      continue;

    I32 team = p->read< I32 >( o.m_ent + TEAM_OFF );
    if( team == local_team || (team != 2 && team != 3) )
      continue;

    
    COLOR color = ( team == 2 ) ?
      COLOR{ 1.0f, 0.17f, 0.37f, 0.7f } : 
      COLOR{ 0.17f, 0.67f, 0.8f, 0.8f };

    o.m_rwo = true;
    o.m_rwuo = false;
    o.m_bloom_amt = 0.7f;
    o.m_full_bloom = false;
    o.m_color = color;

    U32 obj_address = (U32)glow.m_objects + i * sizeof( glow_object_t );
    p->write( obj_address + 0x8, (void*)( (U32)&glow_objects[i] + 0x8 ), sizeof( glow_object_t ) - 0x16 );
  }

  free( glow_objects );
}

void hack_print_offset( U8 line, const char* name, ULONG offset ) {
  con_set_line_text( line, name );
  U8 color = offset > 0x1000 ? CONFG_WHITE : CONFG_RED;
  
  con_set_line_subtext( line, u_num_to_string_hex( offset ), false, color );
}

void hack_show_ui( CSGO* p ) {
  con_clear();
  con_capturing_input = true;
  
  con_set_line_text( 0, "bhop", false );
  con_set_line_subtext(
    0,
    bhop_active? "[on]" : "[off]",
    true,
    bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 0, []( CON_LINE* self, U8 action ) {
    bhop_active = !bhop_active;
    con_set_line_subtext(
      0,
      bhop_active? "[on]" : "[off]",
      self->active,
      bhop_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    ); 
  } );

  con_set_line_text( 1, "glow", false );
  con_set_line_subtext(
    1,
    glow_active? "[on]" : "[off]",
    false,
    glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
  );

  con_set_line_callback( 1, []( CON_LINE* self, U8 action ) {
    glow_active = !glow_active;
    con_set_line_subtext(
      1,
      glow_active? "[on]" : "[off]",
      self->active,
      glow_active? CONFG_LIGHTGREEN : CONFG_LIGHTRED
    );
  } );

  
  con_set_line_text( 2, "triggerbot", false );
  con_set_line_subtext(
    2,
    "[mouse5]",
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_text( 3, "display color pallette" );
  con_set_line_subtext(
    3,
    "[enter]",
    false,
    CONFG_LIGHTBLUE
  );

  con_set_line_callback( 3, []( CON_LINE*, U8 action ) {
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

  con_set_bottomline_text(
    "LOCALPLAYER: %08X | FLAGS: %08X | menu",
    p->read< U32 >( localplayer_ptr ), 0x0
  );
}

void hack_init( CSGO* p ) {
  con_clear();
  
  p->client = p->get_module( "client.dll" );
  p->engine = p->get_module( "engine.dll" );

  con_set_bottomline_text( "searching for signatures..." );
  
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  hack_print_offset( 1, "jump", jump_ptr );
  hack_print_offset( 2, "attack", attack_ptr );
  hack_print_offset( 3, "glow", glow_ptr );
  
  localplayer_ptr = p->read< U32 >( p->code_match( p->client, LOCALPLAYER_SIG ) + 3 ) + 4;
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  jump_ptr        = p->read< U32 >( p->code_match( p->client, JUMP_SIG ) + 2 );
  hack_print_offset( 1, "jump", jump_ptr );
  attack_ptr      = p->read< U32 >( p->code_match( p->client, FORCEATTACK_SIG ) + 2 );
  hack_print_offset( 2, "attack", attack_ptr ); 
  glow_ptr        = p->read< U32 >( p->code_match( p->client, GLOWSTRUCT_SIG ) + 1 ) + 4;
  hack_print_offset( 3, "glow", glow_ptr );
}