#pragma once
#include "csgoentity.h"

class CSGOPLAYER : public CSGOENTITY {
public:
  CSGOPLAYER( U32 base ) : CSGOENTITY( base ) {}

  NETVAR( m_iTeam, "DT_CSPlayer", I32 );
};