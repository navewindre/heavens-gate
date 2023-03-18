//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#include "typedef.h"


template < typename t >
inline t u_clamp( t in, const t& min, const t& max ) {
  return in < min ? min : in > max ? max : in;
}

template < typename t >
inline t u_max( const t& t1, const t& t2 ) {
  return t1 > t2 ? t1 : t2;
}

template < typename t >
inline t u_min( const t& t1, const t& t2 ) {
  return t1 > t2 ? t2 : t1;
}