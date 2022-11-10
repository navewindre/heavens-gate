#pragma once
#include "sdk.h"
#include "../util.h"
#include "../process.h"

#define OFFSET( name, offset, type )                     \
  type name() { return get<type>( offset ); }            \
  void name( type v ) { return set<type>( offset, v ); }

class CSGOENTITY {
public:
  static CSGO* csgop;
  

public:
  CSGOENTITY( U32 ptr ) : base( ptr ) {};
  CSGOENTITY( CSGOENTITY&& other ) : base( other.base ) {}

  inline operator U32&() { return base; }
  
  template <typename t>
  t get( U32 offset ) { return csgop->read<t>( base + offset ); }

  template <typename t>
  void set( U32 offset, t v ) { return csgop->write<t>( base + offset, v ); }

  CSGO_CLIENT_CLASS get_clientclass() {
    U32 networkable = get<U32>( 0x8 );
    U32 create_fn   = csgop->read<U32>( networkable + 0x8 );
    U32 clientclass = csgop->read<U32>( create_fn + 0x1 );

    return csgop->read<CSGO_CLIENT_CLASS>( clientclass );
  }
  
  OFFSET( m_fFlags, 0x104, I32 )

public:
  U32 base;
};