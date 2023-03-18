//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include "../process.h"
#include "../conout.h"
#include "interface.h"


class CSGO : public PROCESS32 {
public:
  CSGO() : PROCESS32( "csgo.exe" ) {};

  void dump_interfaces() {
    interfaces = srceng_get_interfaces( this );
  }

  U32 client;
  U32 engine;

  VECTOR<IFACE_ENTRY> interfaces;
};