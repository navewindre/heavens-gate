//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include "typedef.h"

#define M_PI 3.141592653589793238f

struct VEC3 {
  F32 x, y, z;

  VEC3() { x = y = z = 0.0f; }
  VEC3( F32 X, F32 Y, F32 Z ) { x = X; y = Y; z = Z; }
  VEC3( const F32* v ) { x = v[0]; y = v[1]; z = v[2]; }
  VEC3( const VEC3& v ) { x = v.x; y = v.y; z = v.z; }

  VEC3& operator=( const VEC3& v ) { x = v.x; y = v.y; z = v.z; return *this; }
  F32& operator[]( I32 i ) { return ( (F32*)this )[i]; }
  F32 operator[]( I32 i ) const { return ( (F32*)this )[i]; }

  VEC3& operator+=( const VEC3& v ) { x += v.x; y += v.y; z += v.z; return *this; }
  VEC3& operator-=( const VEC3& v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
  VEC3& operator*=( const VEC3& v ) { x *= v.x; y *= v.y; z *= v.z; return *this; }
  VEC3& operator/=( const VEC3& v ) { x /= v.x; y /= v.y; z /= v.z; return *this; }

  VEC3 operator+( const VEC3& v ) const { return VEC3( x + v.x, y + v.y, z + v.z ); }
  VEC3 operator-( const VEC3& v ) const { return VEC3( x - v.x, y - v.y, z - v.z ); }
  VEC3 operator*( const VEC3& v ) const { return VEC3( x * v.x, y * v.y, z * v.z ); }
  VEC3 operator/( const VEC3& v ) const { return VEC3( x / v.x, y / v.y, z / v.z ); }

  operator bool() const { return !is_zero(); }

  F32 length() const {
    return sqrtf( x * x + y * y + z * z );
  }

  F32 lengthsqr() const {
    return ( x * x + y * y + z * z );
  }

  F32 length2d() const {
    return sqrtf( x * x + y * y );
  }

  F32 length2dsqr() const {
    return ( x * x + y * y );
  }

  F32 dist_to( const VEC3& v ) const {
    return ( *this - v ).length();
  }

  F32 dist_to_sqr( const VEC3& v ) const {
    return ( *this - v ).lengthsqr();
  }

  F32 dot( const VEC3& v ) const {
    return ( x * v.x + y * v.y + z * v.z );
  }

  F32 angle_between( const VEC3& to ) { 
    const F32 from_length = length( );
    const F32 to_length = to.length( );

    if( from_length && to_length ) {
      return acosf( dot( to ) / from_length * to_length );
    }

    return 0.f;
  }

  VEC3 cross( const VEC3& v ) const {
    return VEC3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
  }

  bool is_zero() const {
    return ( x > -FLT_EPSILON && x < FLT_EPSILON &&
      y > -FLT_EPSILON && y < FLT_EPSILON &&
      z > -FLT_EPSILON && z < FLT_EPSILON );
  }

  inline void normalize_in_place() {
    F32 iradius = 1.f / ( length() + FLT_EPSILON ); //FLT_EPSILON

    x *= iradius;
    y *= iradius;
    z *= iradius;
  }

  VEC3 abs() const {
    return VEC3{ ::abs( x ), ::abs( y ), ::abs( z ) };
  }

  VEC3 clamp() {
    for( U32 axis{}; axis < 2; axis++ ) {
      auto &cur_axis = operator[]( axis );
      if( !isfinite( cur_axis ) )
        cur_axis = 0.f;
      else
        cur_axis = remainderf( cur_axis, 360.f );     
    }

    x = x > 89.f ? 89.f : x < -89.f ? -89.f : x;
    y = y > 180.f ? 180.f : y < -180.f ? -180.f : y;
    z = 0.f;

    return *this;
  }

};

_forceinline VEC3 vector_angles( const VEC3& start, const VEC3& end ) {
  VEC3 delta	= end - start;

  float magnitude = sqrtf( ( delta.x * delta.x ) + ( delta.y * delta.y ) );
  float pitch		= atan2f( -delta.z, magnitude ) * 57.295779513082f; 
  float yaw		= atan2f( delta.y, delta.x ) * 57.295779513082f;

  VEC3 angle( pitch, yaw, 0.0f );
  return angle.clamp( );
}