#pragma once

#include "csgo.h"
#include "sdk.h"
#include "../conout.h"

struct NETVAR_TABLE {
  U32 ptr;
  STR<64> name;
};

inline U32 netvar_get_list( CSGO* csgo ) {
  IFACE_ENTRY* chl = u_vector_search<IFACE_ENTRY>(
    csgo->interfaces,
    []( IFACE_ENTRY in ) {
      return !!strstr( in.name, "VClient0" );
    }
  );
  
  if( !chl )
    return 0;

  U32 chl_vtable = csgo->read<U32>( chl->ptr );
  U32 vtable_8 = chl_vtable + 8 * sizeof(U32);

  U32 get_allclasses = csgo->read<U32>( vtable_8 );
  U32 class_ptr = csgo->read<U32>( csgo->read<U32>( get_allclasses + 0x1 ) );

  return class_ptr;
}

inline VECTOR<NETVAR_TABLE> netvar_get_tables( CSGO* csgo, U32 list ) {
  static VECTOR<NETVAR_TABLE> tables{};
  if( !tables.empty() )
    return tables;
  
  U32 ptr = list;
  STR<64> net_name;
  do {
    CSGO_CLIENT_CLASS cclass = csgo->read<CSGO_CLIENT_CLASS>( ptr );
    RECV_TABLE table = csgo->read<RECV_TABLE>( (U32)cclass.recv );
    csgo->read( (U32)table.table_name, net_name.data, 64 );

    tables.push_back( { (U32)cclass.recv, net_name } );
    ptr = (U32)cclass.next;
  } while( ptr && ptr != list );

  return tables;
}

inline U32 netvar_get_table( CSGO* csgo, const char* table_name ) {
  static U32 list_ptr = netvar_get_list( csgo );
  static VECTOR<NETVAR_TABLE> tables = netvar_get_tables( csgo, list_ptr );

  for( auto& it : tables ) {
    if( strstr( it.name.data, table_name ) )
      return it.ptr;
  }

  return 0;
}

inline I32 netvar_get_entry( CSGO* csgo, const char* name, U32 table_ptr ) {
  I32 ret{};
  RECV_TABLE table = csgo->read<RECV_TABLE>( table_ptr );

  RECV_PROP* props = (RECV_PROP*)malloc( table.prop_count * sizeof( RECV_PROP ) );
  csgo->read( (U32)table.props, props, sizeof( RECV_PROP ) * table.prop_count );

  for( I32 i = 0; i < table.prop_count; ++i ) {
    RECV_PROP* prop = &props[i];

    if( prop->table ) {
      RECV_TABLE child = csgo->read<RECV_TABLE>( (U32)prop->table );
      if( child.prop_count ) {
        U32 tmp = netvar_get_entry( csgo, name, (U32)prop->table );
        if( tmp ) ret += prop->offset + tmp;
      }
    }

    STR<64> prop_name;
    csgo->read( (U32)prop->varname, prop_name.data, 64 );
    if( !strstr( prop_name.data, name ) )
      continue;

    return prop->offset + ret;
  }

  return ret;
}

inline I32 netvar_find( CSGO* csgo, const char* table_name, const char* prop ) {
  I32 ret;
  U32 table = netvar_get_table( csgo, table_name );

  if( !table )
    return 0;
  
  ret = netvar_get_entry( csgo, prop, table );
  return ret;
}