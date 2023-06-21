#include "hack.h"

#include "convar.h"
#include "netvar.h"

SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );

bool aim_check_player( CSGOPLAYER player, CSGO* p ) {
  if( !player )
    return true; // if no player
  if( player.base == p->read<U32>( localplayer_ptr ) )
    return true; // if player is you
  if( player.get<bool>( 0xed ) )
    return true; // dormant
  if( player.m_lifeState( ) )
    return true;
  if( player.m_bSpottedByMask( ) )
    return true;
  return false;
}

void hack_run_aim( CSGO* p ) {
  if( !aim_active )
    return;
  
  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  if( local.m_iHealth( ) < 1 )
    return;

  F32 m_pitch, m_yaw;

  CSGOENTITY wep = CSGOENTITY::from_list(
    ( ( local.m_hActiveWeapon( ) & 0xFFF ) - 1 )
  );

  if( !wep.is_weapon( ) ) {
    m_pitch = m_yaw = 0.022f;
    convar_set( p, pitch_ptr, m_pitch );
    convar_set( p,   yaw_ptr, m_yaw   );
    return;
  }

  F32 lowest_dist{ 3.33f };
  U32 closest{ };
  for( U32 index{}; index <= 64; ++index ) {
    CSGOPLAYER player = CSGOENTITY::from_list( index );

    if( !aim_check_player( player, p ) )
      continue;
    
    VEC3 local_pos  = local.m_vecOrigin( ) + local.m_vecViewOffset( );
    VEC3 local_view = p->read<VEC3>( clientstate_ptr + 0x4d90 );
    // could replace this magic number with pattern, but is it worth it ?

    VEC3 target_pos = player.get_bone_pos( 8 );
    VEC3 target_ang = vector_angles( local_pos, target_pos );

    F32 distance = ( local_view - target_ang ).clamp().length2d();

    if( distance > lowest_dist )
      continue;
    
    lowest_dist = distance;
    closest = player;
  }

  if( !closest ) {
    m_pitch = m_yaw = 0.022f;
    convar_set( p, pitch_ptr, m_pitch );
    convar_set( p,   yaw_ptr, m_yaw   );
    return;
  }

  m_pitch = 0.001f + ( 0.022f - 0.001f ) * ( lowest_dist / 3.33f ),
  m_yaw   = 0.001f + ( 0.022f - 0.001f ) * ( lowest_dist / 3.33f );

  convar_set( p, pitch_ptr, m_pitch );
  convar_set( p,   yaw_ptr, m_yaw   );
}