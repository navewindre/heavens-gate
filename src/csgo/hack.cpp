//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "hack.h"

#include "netvar.h"
#include "../disasm.h"

SETTING_HOLDER settings;
SETTING<I32>  triggerbot_key{ &settings, "triggerbot_key", 0x6 };
SETTING<bool> bhop_active{ &settings, "bhop_active", true };
SETTING<bool> glow_active{ &settings, "glow_active", false };
SETTING<bool> clantag_active{ &settings, "clantag_active", false };

F64  perf_ipt = .0;
F64  perf_tps = .0;

U32 localplayer_ptr;
U32 jump_ptr;
U32 attack_ptr;
U32 glow_ptr;
U32 clantag_ptr;
U32 clientstate_ptr;

void hack_run_bhop( CSGO* p ) {
  if( !bhop_active || !( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) ) 
    return;

  assert( !!localplayer_ptr );
  assert( !!jump_ptr );

  CSGOPLAYER player = p->read<U32>( localplayer_ptr );
  if( !player )
    return;
  
  I32  player_flags = player.m_fFlags();
  bool air = !( player_flags & 1 << 0 );

  if( !air )
    p->write<I32>( jump_ptr, 6 );
}

void hack_run_trigger( CSGO* p ) {
  if( !( GetAsyncKeyState( triggerbot_key ) & 0x8000 ) )
    return;

  assert( !!localplayer_ptr );
  assert( !!attack_ptr );

  CSGOPLAYER player = p->read<U32>( localplayer_ptr );
  I32 crosshairid   = player.m_iCrosshairID();
  if( crosshairid > 0 && crosshairid < 65 )
    p->write< I32 >( attack_ptr, 6 );
}

void hack_run_glow( CSGO* p ) {
  if( !glow_active )
    return;
  
  CSGOPLAYER local = p->read<U32>( localplayer_ptr );
  if( !local )
    return;

  U32 local_team = local.m_iTeam();

  GLOW_OBJ_MANAGER glow;
  p->read( glow_ptr, &glow, sizeof( GLOW_OBJ_MANAGER ) );

  if( !glow.count )
    return;
  
  GLOW_OBJECT* glow_objects = (GLOW_OBJECT*)malloc( sizeof( GLOW_OBJECT ) * glow.count );
  p->read( (U32)( glow.objects ), glow_objects, sizeof( GLOW_OBJECT ) * glow.count );

  for( U32 i = 0; i < glow.count; ++i ) {
    GLOW_OBJECT& o = glow_objects[i];

    if( !o.ent || local.base == o.ent )
      continue;

    CSGOPLAYER        e = o.ent;
    CSGO_CLIENT_CLASS cl = e.get_clientclass();
    COLOR             color;

    /* clientclass outdated af*/
    if( cl.index == CBasePlayer ) {
      I32 team = e.m_iTeam();
      if( team == local_team || (team != 2 && team != 3) )
        continue;
    
      color = ( team == 2 ) ?
        COLOR{ 1.0f, 0.17f, 0.37f, 0.7f } : 
        COLOR{ 0.17f, 0.67f, 0.8f, 0.8f };
    }
    else if( cl.index >= CWeaponAug && cl.index <= CWeaponXM1014 && !o.rwo ) {
      color = { 0.8f, 0.8f, 0.8f, 0.6f };
    }
    else continue;
      
    o.rwo = true;
    o.rwuo = false;
    o.bloom_amt = 0.7f;
    o.full_bloom = false;
    o.color = color;

    U32 obj_address = (U32)glow.objects + i * sizeof( GLOW_OBJECT );
    p->write( obj_address + 0x8, (void*)( (U32)&glow_objects[i] + 0x8 ), sizeof( GLOW_OBJECT ) - 0x16 );
  }

  free( glow_objects );
}

__declspec( naked ) void __stdcall setclantag_shellcode( void* string ) {
  __asm {
    push ebp
    mov ebp, esp
    sub esp, __LOCAL_SIZE
  }
  
  U32 clantag_offset;
  clantag_offset = 0xDADADADA;
  
  using set_clantag = int( __fastcall* )( const char*, const char* );
  ( (set_clantag)(clantag_offset) )( (const char*)string, (const char*)string );
  
  DISASM_SIG();

  __asm {
    mov esp, ebp
    pop ebp
    ret
  }
}

void hack_setclantag( CSGO* csgo, const char* str ) {
  static U64 func_address = 0;
  static U64 string_address = 0;
  
  if( !func_address || !string_address ) {
    DISASM_INFO disasm = disasm_function( &setclantag_shellcode );

    U8* func_copy = (U8*)malloc( disasm.func_length );
    memcpy( func_copy, disasm.func_start, disasm.func_length );
    
    for( U32 i = 0; i < disasm.func_length; ++i ) {
      if( *(U32*)( func_copy + i ) == 0xdadadada ) {
        *(U32*)( func_copy + i ) = clantag_ptr;
        break;
      }
    }

    func_address = csgo->allocate( disasm.func_length );
    string_address = csgo->allocate( 16, PAGE_READWRITE );
    csgo->write( func_address, func_copy, disasm.func_length );

    free( func_copy );
  }


  U32 len = strlen( str );
  assert( (len < 16) );
  
  csgo->write( string_address, str, len );
  u_thread_create(
    csgo->get_base(),
    (LPTHREAD_START_ROUTINE)(U32)func_address,
    (void*)(U32)string_address
  );
}

void hack_run_clantag( CSGO* csgo ) {
  if( !clantag_active || !localplayer_ptr )
    return;
  
  const char8_t* clantag[] = {
    u8"\u30FB\u2605*\u309C\u22C6",
    u8"\u309C\u30FB\u2605\u22C6*",
    u8"\u22C6\u309C\u30FB*\u2605",
    u8"*\u22C6\u309C\u2605\u30FB",
    u8"\u2605*\u22C6\u30FB\u309C"
  };

  static I32 counter = 0;
  static U64 last_tick = u_tick() + (rand() % 1000 - 500);
  U64 tick = u_tick();

  
  if( tick - last_tick > 1000 ) {
    counter = (++counter) % 5;

    hack_setclantag( csgo, (const char*)( clantag[counter] ) );
    last_tick = tick;
  }
}

inline void hack_print_offset( U8 line, const char* name, ULONG offset ) {
  con_set_line_text( line, name );
  U8 color = offset > 0x1000 ? CONFG_WHITE : CONFG_RED;
  
  con_set_line_subtext( line, u_num_to_string_hex( offset ), false, color );
}

inline U32 get_clantag_offset( CSGO* csgo ) {
  const char* const clantag_str = "Current clan ID for name decoration";
  U32 str = csgo->code_match( csgo->engine, (U8*)clantag_str, strlen( clantag_str ) );
  while( csgo->read<U8>( str - 1 ) != 0 )
    str = csgo->code_match( csgo->engine, (U8*)clantag_str, strlen( clantag_str ), str + 1 );
  
  U8 str_bytes[] = {
    0x68,
    *( (U8*)(&str) + 0 ),
    *( (U8*)(&str) + 1 ),
    *( (U8*)(&str) + 2 ),
    *( (U8*)(&str) + 3 )
  };

  U32 push_str = csgo->code_match( csgo->engine, str_bytes, sizeof( str_bytes ) );
  U8 func_buffer[100];
  csgo->read( push_str - 100, func_buffer, sizeof( func_buffer ) );

  U32 cvar_func = 0;
  for( U32 i = 0; i < 100; ++i ) {
    if( func_buffer[i] == 0x68 && func_buffer[i + 5] == 0x51 ) {
      cvar_func = *(U32*)( func_buffer + i + 1 );
      break;
    }
  }

  U8 cvar_func_buffer[256];
  csgo->read( cvar_func, cvar_func_buffer, sizeof( cvar_func_buffer ) );

  for( U32 i = 0; i < 256; ++i ) {
    if( cvar_func_buffer[i] == 0xe8
     && cvar_func_buffer[i + 5] == 0x5f
     && cvar_func_buffer[i + 6] == 0x5e
     && cvar_func_buffer[i + 7] == 0x5b ) {
      return *(U32*)( cvar_func_buffer + i + 1 ) + cvar_func + i + 5;
    }
  }
  
  return 0;
}

inline U32 get_jump_offset( CSGO* csgo ) {
  IFACE_ENTRY chl = u_vector_search<IFACE_ENTRY>( csgo->interfaces, []( IFACE_ENTRY* e ) {
    return !!strstr( e->name, "VClient0" );
  } );

  if( !chl.ptr )
    return 0;

  U32 chl_vtable = csgo->read<U32>( chl.ptr );
  U32 chl_vtable_16 = csgo->read<U32>( chl_vtable + 16 * sizeof(U32) );
  U32 input = csgo->read<U32>( chl_vtable_16 + 1 );
  
  U32 input_vtable = csgo->read<U32>( input );
  U32 vtable_3 = csgo->read<U32>( input_vtable + 2 * sizeof(U32) );

  U8 func_buffer[256];
  csgo->read( vtable_3, func_buffer, sizeof( func_buffer ) );

  U8 pattern[] = { 0x83, 0xca, 0x02, 0x24, 0x03 };
  for( U32 i = 0; i < sizeof( func_buffer ) - sizeof( pattern ); ++i ) {
    if( u_binary_match( func_buffer + i, pattern, sizeof( pattern ) ) ) {
      return *(U32*)( func_buffer + i - 8 );
    }
  }

  return 0;
}

inline U32 get_attack_offset( CSGO* csgo ) {
  IFACE_ENTRY chl = u_vector_search<IFACE_ENTRY>( csgo->interfaces, []( IFACE_ENTRY* e ) {
   return !!strstr( e->name, "VClient0" );
 } );

  if( !chl.ptr )
    return 0;

  U32 chl_vtable = csgo->read<U32>( chl.ptr );
  U32 chl_vtable_16 = csgo->read<U32>( chl_vtable + 16 * sizeof(U32) );
  U32 input = csgo->read<U32>( chl_vtable_16 + 1 );
  
  U32 input_vtable = csgo->read<U32>( input );
  U32 vtable_3 = csgo->read<U32>( input_vtable + 2 * sizeof(U32) );

  U8 func_buffer[256];
  csgo->read( vtable_3, func_buffer, sizeof( func_buffer ) );

  U8 pattern[] = { 0x83, 0xca, 0x01, 0x24, 0x03 };
  for( U32 i = 0; i < sizeof( func_buffer ) - sizeof( pattern ); ++i ) {
    if( u_binary_match( func_buffer + i, pattern, sizeof( pattern ) ) ) {
      return *(U32*)( func_buffer + i - 8 );
    }
  }

  return 0;
}

inline U32 get_clientstate_offset( CSGO* csgo ) {
  IFACE_ENTRY engine = u_vector_search<IFACE_ENTRY>( csgo->interfaces, []( IFACE_ENTRY* e ) {
    return !!strstr( e->name, "VEngineClient0" );
  } );

  if( !engine.ptr )
    return 0;

  U32 engine_vtable = csgo->read<U32>( engine.ptr );
  U32 engine_vtable_18 = csgo->read<U32>( engine_vtable + 18 * sizeof(U32) );

  U8 func_buffer[256];
  csgo->read( engine_vtable_18, func_buffer, sizeof( func_buffer ) );

  for( U32 i = 0; i < 256; ++i ) {
    if( func_buffer[i] == 0x8b
     && func_buffer[i+1] == 0x34
     && func_buffer[i+2] == 0x85 ) {
      return csgo->read<U32>( *(U32*)( func_buffer + i + 3 ) );
    }
  }

  return 0;
}

#define progress( x ) con_set_line( CON_MAX_HEIGHT - 1, con_progressbar( x ), "" )

CSGO* hack_init() {
  static CSGO p;
  con_clear();
  
  while( !p.open() ) {
    progress( 0.f );
    con_set_bottomline_text( "waiting for process..." );
    Sleep( 500 );
  }

  progress( .2f );
  do {
    p.client = p.get_module32( "client.dll"fnv );
    p.engine = p.get_module32( "engine.dll"fnv );
    if( p.client && p.engine )
      break;

    progress( .3f );
    con_set_bottomline_text( "waiting for modules..." );
    Sleep( 500 );
  } while( true );

  progress( .4f );
  con_set_bottomline_text( "dumping interfaces..." );

  do {
    p.dump_interfaces();
    if( p.interfaces.size() > 1 )
      break;

    progress( .4f );
    Sleep( 500 );
  } while( true );

  progress( .5f );
  // preload netvar tables
  netvar_get_table( &p, " " );
  progress( .6f );
  
  con_set_bottomline_text( "searching for offsets..." );

  con_set_line_text( 0, "found interfaces: " );
  con_set_line_subtext( 0, u_num_to_string_dec( p.interfaces.size() ), false, CONFG_CYAN );
  
  localplayer_ptr = p.read<U32>( p.code_match( p.client, LOCALPLAYER_SIG ) + 3 ) + 4;
  hack_print_offset( 1, "localplayer", localplayer_ptr ); progress( .65f );
  jump_ptr        = get_jump_offset( &p );
  hack_print_offset( 2, "jump", jump_ptr ); progress( .7f );
  attack_ptr      = get_attack_offset( &p );
  hack_print_offset( 3, "attack", attack_ptr ); progress( .75f );
  glow_ptr        = p.read<U32>( p.code_match( p.client, GLOWSTRUCT_SIG ) + 1 ) + 4;
  hack_print_offset( 4, "glow", glow_ptr ); progress( .8f );
  clantag_ptr = get_clantag_offset( &p );
  hack_print_offset( 5, "SetClanTag", clantag_ptr ); progress( .85f );
  clientstate_ptr = get_clientstate_offset( &p );
  hack_print_offset( 6, "clientstate", clientstate_ptr ); progress( .9f );
  
  progress( 1.f );
  CSGOENTITY::csgop = &p;
  
  return &p;
}

#undef progress