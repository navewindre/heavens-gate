//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include "csgoentity.h"

class CSGOPLAYER : public CSGOENTITY {
public:
  CSGOPLAYER( U32 base ) : CSGOENTITY( base ) {}
  CSGOPLAYER( const CSGOENTITY& other ) : CSGOENTITY( other.base ) {}
  CSGOPLAYER() = default;
  
  // this doesnt need to be a part of the aimbot.
  VEC3 get_bone_pos( I32 bone_id ) {
    U32 bonematrix = get<U32>( 0x26a8 );

    // why read 3 times when u can read once?
    return get<VEC3>( bonematrix + 0x30 * bone_id + 0x0c );
  }

  
  NETVAR( m_iTeam, "DT_CSPlayer", I32 );
};