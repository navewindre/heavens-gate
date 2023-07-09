#include "csgo.h"

inline U32 convar_find( CSGO* p, const char* name ) {
  VECTOR<MODULE_ENTRY> modules = p->dump_modules32();

  for( auto& it : modules ) {
    U32 string_ptr = 0;
    do {
      string_ptr = p->code_match( (U32)it.base, (U8*)name, strlen( name ), string_ptr + 1 );
      if( !string_ptr )
        break;

      U8* ptr_bytes = (U8*)( &string_ptr );
      U8 pattern[] = {
        0x68, 0x00, 0x00, 0x00, 0x00, // cvar creation flags
        0x68, 0x00, 0x00, 0x00, 0x00, // defaultValue
        0x68,
        *ptr_bytes,
        *(ptr_bytes + 1),
        *(ptr_bytes + 2),
        *(ptr_bytes + 3),
        0xe8 // call create_cvar
      };
  
      U32 string_ref = p->code_match( (U32)it.base, pattern, sizeof( pattern ) );
      if( string_ref )
        return p->read<U32>( string_ref - 11 );

      // try with mov instead of call
      pattern[sizeof( pattern ) - 1] = 0xb9; // mov ecx, this
      string_ref = p->code_match( (U32)it.base, pattern, sizeof( pattern ) );
          
      if( !string_ref )
        continue;

      U32 convar = p->read<U32>( string_ref + sizeof( pattern ) );
      return convar;
    } while( true );  
  }

  return 0;
}

struct CVValue_t {
  char* m_pszString;
  int m_StringLength;
  float m_fValue;
  int m_nValue;
};

template <typename T>
inline void convar_set( CSGO* p, U32 convar, T _new ) {
  U32 val = *(U32*)&_new;
  val ^= convar;

  CVValue_t value = p->read<CVValue_t>( convar + 0x24 );
  value.m_nValue = val;
  *(U32*)(&value.m_fValue) = val;
  
  p->write<CVValue_t>( convar + 0x24, value );
}

template <typename T>
inline T convar_get( CSGO* p, U32 convar ) {
  U32 val = p->read<U32>( convar + 0x2c ) ^ convar;
  return *(T*)( &val );
}