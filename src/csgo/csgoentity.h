//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include "../util.h"

#include "sdk.h"
#include "netvar.h"

#define OFFSET( name, prop, table, type, off )                \
  type name() { \
    static U32 offset = netvar_find( csgop, table, prop ) + off; \
    return get<type>( offset ); } \
  void name( type v ) { \
    static U32 offset = netvar_find( csgop, table, #name ) + off; \
    return set<type>( offset, v ); } \

#define NETVAR( name, table, type )                \
  type name() { \
    static U32 offset = netvar_find( csgop, table, #name ); \
    return get<type>( offset ); } \
  void name( type v ) { \
    static U32 offset = netvar_find( csgop, table, #name ); \
    return set<type>( offset, v ); } \


class CSGOENTITY {
public:
  static CSGO* csgop;
  
public:
  CSGOENTITY( U32 ptr ) : base( ptr ) {};
  CSGOENTITY( const CSGOENTITY& other ) : base( other.base ) {}

  inline operator U32&() { return base; }

  template <typename t>
  t get( U32 offset ) { return csgop->read<t>( base + offset ); }

  template <typename t>
  void set( U32 offset, t v ) { return csgop->write<t>( base + offset, v ); }

public:
  CSGO_CLIENT_CLASS get_clientclass() {
    U32 networkable = get<U32>( 0x8 );
    U32 create_fn   = csgop->read<U32>( networkable + 0x8 );
    U32 clientclass = csgop->read<U32>( create_fn + 0x1 );

    return csgop->read<CSGO_CLIENT_CLASS>( clientclass );
  }
  
  NETVAR( m_fFlags, "DT_CSPlayer", I32 );
  OFFSET( m_iCrosshairID, "m_bHasDefuser", "DT_CSPlayer", I32, 92 );
  OFFSET( m_dwBoneMatrix, "m_nForceBone", "DT_BaseAnimating", U32, 28 );

  static CSGOENTITY from_list( I32 idx ) {
    static U32 entlist = csgop->read<U32>(
      csgop->code_match(
        csgop->client, "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8"
      ) + 1
    );

    return csgop->read<U32>(
      entlist + idx * 0x10
    );
  }

  bool is_weapon() {
    CSGO_CLIENT_CLASS cl = get_clientclass();
    return ( cl.index >= CWeaponAug && cl.index <= CWeaponXM1014 )
          || cl.index == 1; // for ak
  }
  
public:
  U32 base;
};