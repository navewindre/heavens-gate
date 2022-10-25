// by navewindre
// github.com/navewindre

#include "typedef.h"

struct COLOR {
  COLOR()  = default;
  COLOR( float r1, float g1, float b1, float a1 ) : r( r1 ), g( g1 ), b( b1 ), a( a1 ) {}
  
  float r;
  float g;
  float b;
  float a;
};

struct glow_object_t {
  U8          m_pad00[ 4 ];
  U32   			m_ent;				//0000
  COLOR				m_color;
  U8				  pad_01[ 8 ];
  F32				  m_bloom_amt;
  U8			    pad_02[ 4 ];
  bool				m_rwo;	//0024
  bool				m_rwuo;	//0025
  bool				m_full_bloom;
  char        pad_002B[1]; //0x002B
  I32         m_fullbloom_stencil; //0x002C
  I32         iUnk; //0x0030
  I32 m_nSplitScreenSlot; //0x0034
};

struct glow_obj_manager_t {
  glow_object_t*	m_objects;
  U32		m_max;
  U32		unk02;
  U32		m_count;
  U32		m_data_ptr_back;
  U32		m_first_free_slot;
  U32		unk1;
  U32		unk2;
  U32		unk3;
  U32		unk4;
  U32		unk5;
};