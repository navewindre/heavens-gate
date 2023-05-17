﻿//|_   _   _.     _  ._  |_   _.  _ |
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

  // this also doesn't need to be a part of the aimbot.
  static CSGOENTITY from_list( I32 idx ) {
    // nice fucking arbitrary magic number
    const U32 entlist = 0x4dfef0c;

    return csgop->read<U32>(
      csgop->client + entlist + idx * 0x10
    );
  }

  // also doesn't have to be a part of the aimbot
  bool is_weapon() {
    CSGO_CLIENT_CLASS cl = get_clientclass();
    
    return cl.index >= CWeaponAug && cl.index <= CWeaponXM1014;
  }
  
public:
  U32 base;
};