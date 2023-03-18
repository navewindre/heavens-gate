#include "csgo.h"

class MATERIAL {
private:
  static IFACE_ENTRY* get_matsystem( CSGO* csgo ) {
    static IFACE_ENTRY* ret = u_vector_search<IFACE_ENTRY>( csgo->interfaces,
    []( IFACE_ENTRY i ) {
      return fnv1a( "materialsystem.dll" ) == fnv1a( i.module_name )
          && !!strcmp( i.name, "VMaterialSystem" );
    } );

    return ret;
  }

  static IFACE_ENTRY* get_matsystem_cvar( CSGO* csgo ) {
    static IFACE_ENTRY* ret = u_vector_search<IFACE_ENTRY>( csgo->interfaces,
    []( IFACE_ENTRY i ) {
      return fnv1a( "materialsystem.dll" ) == fnv1a( i.module_name )
          && !!strcmp( i.name, "VEngineCvar" );
    } );

    return ret;
  }
  
public:
  static U32 first_material( CSGO* csgo ) {
    IFACE_ENTRY* mat_system = get_matsystem( csgo );

    U16 mat_handle = csgo->read<U16>( mat_system->ptr + 0x250 );

    while( mat_handle != 0xffff ) {
      U32 handle_entries = csgo->read<U32>( mat_system->ptr + 0x244 );
      U16 next_handle = csgo->read<U16>( handle_entries + 16 * mat_handle );

      if( next_handle == 0xffff )
        return mat_handle;

      mat_handle = next_handle;
    }

    return 0;
  }

  static U16 next_material( CSGO* csgo, U16 mat ) {
    IFACE_ENTRY* mat_system = get_matsystem( csgo );

    if( mat == 0xffff )
      return 0;

    U32 handle_array = csgo->read<U32>( mat_system->ptr + 0x244 );
    U16 next_handle = csgo->read<U16>( handle_array + 16 + mat + 2 );
    if( next_handle == 0xffff )
      return 0xffff;

    for( U16 i = next_handle; i != 0xffff; i = csgo->read<U16>( handle_array * 16 + i ) ) {
      next_handle = i;
    }

    return next_handle;
  }
};