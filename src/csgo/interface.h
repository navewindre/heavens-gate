//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include "../process.h"
#include "../util.h"
#include "../typedef.h"

struct IFACE_ENTRY {
  U32     ptr;
  STR<64> name;
  U32     module;
  STR<64> module_name;
};

struct IFACE_REG {
  void*       create_fn;
  const char* name;
  U32         next;
};

inline bool iface_is_createinterface_export( PROCESS32* proc, U32 exp ) {
  U8 buf[12];

  proc->read( exp, buf, 12 );

  return(
    buf[0] == 0x55 &&
    buf[4] == 0xe9 &&
    buf[9] == 0xcc &&
    buf[10] == 0xcc
  );
}

inline bool iface_is_createinterface( PROCESS32* proc, U32 fn ) {
  U8 buf[12];

  proc->read( fn, buf, 12 );

  return (
    buf[0] == 0x55 &&
    buf[4] == 0x8b &&
    buf[10] == 0x57
  );
}

inline U32 iface_follow_createinterface( PROCESS32* proc, U32 exp ) {
  U32 jmp = exp + 0x4;
  U32 rel = proc->read<U32>( jmp + 0x1 );

  return jmp + rel + 0x5;
}

inline U32 iface_get_list( PROCESS32* proc, U32 exp ) {
  return proc->read<U32>( proc->read<U32>( exp + 0x6 ) );
}

static VECTOR< IFACE_ENTRY > srceng_get_interfaces( PROCESS32* proc ) {
  VECTOR< MODULE_EXPORT64 > exports;
  VECTOR< MODULE_ENTRY >    modules;
  VECTOR< IFACE_ENTRY >     ifaces;
  MODULE_EXPORT64*          create_interface_export;
  U32                       create_interface;

  modules = proc->dump_modules32();
  for( auto& module : modules ) {
    create_interface_export = 0;
    exports = module_get_exports( (U32)module.base, proc->get_base() );

    for( auto& it : exports ) {
      if( fnv1a( it.name ) == "CreateInterface"fnv &&
        iface_is_createinterface_export( proc, (U32)it.base )
      ) {
        create_interface_export = &it;
        break;
      }
    }

    if( !create_interface_export )
      continue;

    create_interface = iface_follow_createinterface( proc, (U32)create_interface_export->base );
    if( !create_interface || !iface_is_createinterface( proc, create_interface ) )
      continue;

    U32 list_ptr  = iface_get_list( proc, create_interface );
    if( !list_ptr )
      continue;
    
    IFACE_REG reg = proc->read<IFACE_REG>( list_ptr );
    STR<64> name{};
    do {
      memset( name.data, 0, 64 );
      proc->read( (U32)reg.name, name.data, 64 );
      name.data[63] = 0;

      IFACE_ENTRY e;
      e.module = (U32)module.base;
      e.module_name = module.name;
      e.name = name;
      e.ptr = proc->read<U32>( (U32)(reg.create_fn) + 0x1 );
    
      ifaces.push_back( e );

      reg = proc->read<IFACE_REG>( reg.next );
    } while( list_ptr != reg.next && reg.next );
  }

  return ifaces;
}