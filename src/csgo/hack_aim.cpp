#include "hack.h"

#include "convar.h"
#include "netvar.h"

SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );

bool aim_check_player( CSGOPLAYER player, CSGO* p ) {
  if( !player )
    return true; // if no player
  if( player.get_clientclass().index != CCSPlayer )
    return true; // if not player
  if( player.base == p->read<U32>( localplayer_ptr ) )
    return true; // if player is you
  if( player.m_bDormant() )
    return true; // dormant
  if( player.m_lifeState() )
    return true;
  if( !player.m_bSpottedByMask() )
    return true;
  return false;
}

#define aim_fov 10.f

F32 calc_dist( VEC3 v, F32 distance ) {
  F32 sqr1 = sinf( v.x * M_PI / 180.f ) * distance;
  F32 sqr2 = sinf( v.y * M_PI / 180.f ) * distance;
  return sqrtf( 
    ( sqr1 * sqr1 ) + ( sqr2 * sqr2 )
  );
}

void hack_run_aim( CSGO* p ) {
  if( !aim_active )
    return;

  static U32 color_ptr = convar_find( p, "cl_crosshaircolor_r" );
  convar_set<I32>( p, color_ptr, 255 );
  
  F32 m_pitch, m_yaw;

  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  if( local.m_iHealth( ) < 1 || !local ) {
    m_pitch = m_yaw = 0.022f;
    convar_set( p, pitch_ptr, m_pitch );
    convar_set( p,   yaw_ptr, m_yaw   );
    return;
  }

  CSGOENTITY wep = CSGOENTITY::from_list(
    ( ( local.m_hActiveWeapon() & 0xFFF ) - 1 )
  );

  if( !wep.is_weapon( ) ) {
    m_pitch = m_yaw = 0.022f;
    convar_set( p, pitch_ptr, m_pitch );
    convar_set( p,   yaw_ptr, m_yaw   );
    return;
  }

  F32 lowest_dist{ aim_fov };
  U32 closest{ };
  for( U32 index{}; index <= 64; ++index ) {
    CSGOPLAYER player = CSGOENTITY::from_list( index );

    if( aim_check_player( player, p ) )
      continue;
    
    VEC3 local_pos  = local.m_vecOrigin( ) + local.m_vecViewOffset( );
    VEC3 local_view = p->read<VEC3>( clientstate_ptr + 0x4d90 );
    // could replace this magic number with pattern, but is it worth it ?
    VEC3 target_pos;
    if( wep.get_clientclass( ).index == CWeaponAWP )
      target_pos = player.get_bone_pos( 6 );
    else
      target_pos = player.get_bone_pos( 8 );
    VEC3 target_ang = vector_angles( local_pos, target_pos );

    //F32 distance = ( local_view - target_ang ).clamp().length2d(); // non-dynamic
    F32 distance = calc_dist(
      ( local_view - target_ang ),
      local_pos.dist_to( target_pos )
    );

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

  // change this to change strength. this is the minimum allowed by the game.
  const F32 min_sens = 0.0001f;
  F32 factor = ( lowest_dist / aim_fov );
  if( factor > 1.f )
    factor = 1.f;

  // change this for how aggressively the aim 'comes on'.
  // lower values = less assist on outer edge of fov, more on inner.
  factor = pow( factor, 3.f );

  
  m_pitch = min_sens + ( 0.022f - min_sens ) * factor,
  m_yaw   = min_sens + ( 0.022f - min_sens ) * factor;

  convar_set<I32>( p, color_ptr, 0 );
  convar_set( p, pitch_ptr, m_pitch );
  convar_set( p,   yaw_ptr, m_yaw   );
}