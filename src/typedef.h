//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#pragma once
#ifdef __cplusplus
#include <vector>
template<typename t>
using VECTOR = std::vector<t>;
#endif

typedef char      I8;
typedef short     I16;
typedef int       I32;
typedef long long I64;

typedef unsigned char      U8;
typedef unsigned short     U16;
typedef unsigned int       U32;
typedef unsigned long long U64;

typedef float  F32;
typedef double F64;

typedef unsigned long PTR;


#define assert( x ) if( !x ) con_set_assert( "ASSERTION FAILED: %s() (line: %d)", __func__, __LINE__ )