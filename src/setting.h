#pragma once
#include <vector>

#include "fnv.h"

extern void setting_save( const char* name, const void* src, U32 size );
extern void setting_load( const char* name, const void* dst, U32 size );

class SETTING_NODE {
public:
  virtual void load() {};
  virtual void save() {};
};

class SETTING_HOLDER : SETTING_NODE {
public:
  SETTING_HOLDER() = default;
  SETTING_HOLDER( SETTING_HOLDER* other ) { other->register_( this ); }
  
  void register_( SETTING_NODE* node ) { nodes.push_back( node ); }

  void save() override {
    for( auto& it : nodes )
      it->save();
  }

  void load() override {
    for( auto& it : nodes )
      it->load();
  }

private:
  std::vector<SETTING_NODE*> nodes;
};

template < typename t >
class SETTING : SETTING_NODE {
public:
  SETTING( SETTING_HOLDER* owner, const char* name, t _default = t{} ) :
    m_owner( owner ),
    m_name( name ),
    v( _default )
  {
    owner->register_( this );  
  };

  __forceinline SETTING( const SETTING<t>& other ) : v( other.v ) {}
  __forceinline SETTING( const t& other ) : v( other ) {}
  
  void save() override { setting_save( m_name, &v, sizeof( t ) ); }
  void load() override { setting_load( m_name, &v, sizeof( t ) ); }

  operator t&() { return v; }
  t* operator&() { return &v; }
  
  t v;
private:
  SETTING_HOLDER* m_owner;
  const char*     m_name;
};