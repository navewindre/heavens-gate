#include "hack.h"

#include "netvar.h"

SETTING_HOLDER settings;
SETTING<I32>  triggerbot_key{ &settings, "triggerbot_key", 0x6 };
SETTING<bool> bhop_active{ &settings, "bhop_active", true };
SETTING<bool> glow_active{ &settings, "glow_active", false };

F64  perf_ipt = .0;
F64  perf_tps = .0;

U32 localplayer_ptr;
U32 jump_ptr;
U32 attack_ptr;
U32 glow_ptr;

void hack_run_bhop( CSGO* p ) {
  if( !bhop_active || !( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) ) 
    return;

  assert( !!localplayer_ptr );
  assert( !!jump_ptr );

  CSGOPLAYER player = p->read<U32>( localplayer_ptr );
  if( !player )
    return;
  
  I32  player_flags = player.m_fFlags();
  bool air = !( player_flags & 1 << 0 );

  if( !air )
    p->write<I32>( jump_ptr, 6 );
}

void hack_run_trigger( CSGO* p ) {
  if( !( GetAsyncKeyState( triggerbot_key ) & 0x8000 ) )
    return;

  assert( !!localplayer_ptr );
  assert( !!attack_ptr );

  CSGOPLAYER player = p->read<U32>( localplayer_ptr );
  I32 crosshairid   = player.m_iCrosshairId();
  if( crosshairid > 0 && crosshairid < 65 )
    p->write< I32 >( attack_ptr, 6 );
}

void hack_run_glow( CSGO* p ) {
  if( !glow_active )
    return;
  
  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  if( !local )
    return;

  U32 local_team = local.m_iTeam();

  GLOW_OBJ_MANAGER glow;
  p->read( glow_ptr, &glow, sizeof( GLOW_OBJ_MANAGER ) );

  if( !glow.count )
    return;
  
  GLOW_OBJECT* glow_objects = (GLOW_OBJECT*)malloc( sizeof( GLOW_OBJECT ) * glow.count );
  p->read( (U32)( glow.objects ), glow_objects, sizeof( GLOW_OBJECT ) * glow.count );

  for( U32 i = 0; i < glow.count; ++i ) {
    GLOW_OBJECT& o = glow_objects[i];

    if( !o.ent || local.base == o.ent )
      continue;

    CSGOPLAYER        e = o.ent;
    CSGO_CLIENT_CLASS cl = e.get_clientclass();
    COLOR             color;

    /* clientclass outdated af*/
    if( cl.index == 0x28 ) {
      I32 team = e.m_iTeam();
      if( team == local_team || (team != 2 && team != 3) )
        continue;
    
      color = ( team == 2 ) ?
        COLOR{ 1.0f, 0.17f, 0.37f, 0.7f } : 
        COLOR{ 0.17f, 0.67f, 0.8f, 0.8f };
    }
    else if( cl.index >= CWeaponAug && cl.index <= CWeaponXM1014 && !o.rwo ) {
      color = { 0.8f, 0.8f, 0.8f, 0.6f };
    }
    else continue;
      
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

CSGO* hack_init() {
  static CSGO p;
  con_clear();
  
  while( !p.open() ) {
    con_set_bottomline_text( "waiting for process..." );
    Sleep( 100 );
  }
  
  p.client = p.get_module32( "client.dll"fnv );
  p.engine = p.get_module32( "engine.dll"fnv );

  con_set_bottomline_text( "dumping interfaces..." );

  p.dump_interfaces();

  // preload netvar tables
  netvar_get_table( &p, " " );
  con_set_line_text( 0, "found interfaces: " );
  con_set_line_subtext( 0, u_num_to_string_hex( p.interfaces.size() ), false, CONFG_CYAN );

  Sleep( 200 );
  
  con_set_bottomline_text( "searching for signatures..." );
  
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  hack_print_offset( 1, "jump", jump_ptr );
  hack_print_offset( 2, "attack", attack_ptr );
  hack_print_offset( 3, "glow", glow_ptr );
  
  localplayer_ptr = p.read<U32>( p.code_match( p.client, LOCALPLAYER_SIG ) + 3 ) + 4;
  hack_print_offset( 0, "localplayer", localplayer_ptr );
  jump_ptr        = p.read<U32>( p.code_match( p.client, JUMP_SIG ) + 2 );
  hack_print_offset( 1, "jump", jump_ptr );
  attack_ptr      = p.read<U32>( p.code_match( p.client, FORCEATTACK_SIG ) + 2 );
  hack_print_offset( 2, "attack", attack_ptr ); 
  glow_ptr        = p.read<U32>( p.code_match( p.client, GLOWSTRUCT_SIG ) + 1 ) + 4;
  hack_print_offset( 3, "glow", glow_ptr );

  CSGOENTITY::csgop = &p;
  
  return &p;
}