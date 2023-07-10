#include "hack.h"

#include "convar.h"
#include "netvar.h"


VEC3 calc_dist( VEC3 v, F32 distance ) {
  F32 sqr1 = sinf( v.x * M_PI / 180.f ) * distance;
  F32 sqr2 = sinf( v.y * M_PI / 180.f ) * distance;
  return { abs( sqr1 ), abs( sqr2 ), 0.f };
}

bool aim_check_player( CSGOPLAYER player, CSGO* p ) {
  if( !player )
    return true; // if no player
  if( player.get_clientclass().index != CCSPlayer )
    return true; // if not player

  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  static SETTING<bool>& aimteam_active = *settings.find<bool>( "aimteam_active"fnv );
  
  if( player.base == local.base )
    return true;
  if( player.m_iTeamNum() == local.m_iTeamNum() && !aimteam_active.v )
    return true;
  if( player.m_bDormant() )
    return true;
  if( player.m_lifeState() )
    return true;
  if( !player.m_bSpottedByMask() )
    return true;
  return false;
}

void aim_set( CSGO* p, F32 val ) {
  convar_set( p, pitch_ptr, val );
  convar_set( p,   yaw_ptr, val );
  return;
}

void hack_run_aim( CSGO* p ) {
  static SETTING<bool>& aim_active = *settings.find<bool>( "aim_active"fnv );
  static SETTING<F32>& aim_fov = *settings.find<F32>( "aim_fov"fnv );
  static SETTING<I32>& aim_strength = *settings.find<I32>( "aim_strength"fnv );

  if( !aim_active )
    return;

  F32 sens;

  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  if( local.m_iHealth( ) < 1 || !local )
    aim_set( p, 0.022f );

  CSGOENTITY wep = CSGOENTITY::from_list(
    ( ( local.m_hActiveWeapon() & 0xFFF ) - 1 )
  );

  if( !wep.is_weapon( ) )
    aim_set( p, 0.022f );
  
  F32 fov_sqr{ aim_fov * aim_fov };
  F32 lowest_dist{ fov_sqr };
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
    VEC3 v_distance = calc_dist(
      ( local_view - target_ang ),
      local_pos.dist_to( target_pos )
    );

    F32 distance = v_distance.length2dsqr();

    if( distance > lowest_dist )
      continue;
    
    lowest_dist = distance;
    closest = player;
  }

  if( !closest )
    aim_set( p, 0.022f );

  F32 factor = lowest_dist / fov_sqr;
  if( factor > 1.f )
    factor = 1.f;

  // change this for how aggressively the aim 'comes on'.
  // lower values = less assist on outer edge of fov, more on inner.
  factor = pow( factor, 3.f ); // 1 -> 5 ( test )

  const F32 min_sens = 0.0001f + ( ( 0.011f - 0.0001f ) / 100 ) * ( abs( aim_strength - 100 ) );

  sens = min_sens + ( 0.022f - min_sens ) * factor;
  
  aim_set( p, sens );
}