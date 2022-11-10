#pragma once
#include "csgoentity.h"

class CSGOPLAYER : public CSGOENTITY {
public:
  CSGOPLAYER( U32 base ) : CSGOENTITY( base ) {}

  OFFSET( m_iTeam, 0xf4, I32 );
  OFFSET( m_iCrosshairId, 0x11838, I32 );
};