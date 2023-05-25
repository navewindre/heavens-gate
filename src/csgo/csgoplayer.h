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
    return VEC3{
      csgop->read<F32>( m_dwBoneMatrix( ) + 0x30 * bone_id + 0x0c ),
      csgop->read<F32>( m_dwBoneMatrix( ) + 0x30 * bone_id + 0x1c ),
      csgop->read<F32>( m_dwBoneMatrix( ) + 0x30 * bone_id + 0x2c )
    };
  }

  NETVAR( m_iTeamNum, "DT_CSPlayer", I32 );
  NETVAR( m_clrRender, "DT_CSPlayer", BYTECOLOR );
};