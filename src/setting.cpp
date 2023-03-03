#include "setting.h"

const char* get_path() {
  static char out_dir[260]{};

  if( !out_dir[0] ) {
    strcat( out_dir, getenv( "USERPROFILE" ) );
    strcat( out_dir, "/" );
    strcat( out_dir, ".godword" );
  }

  return out_dir;
}

// credits: gcc
size_t getline(char **lineptr, size_t *n, FILE *stream) {
  char *bufptr = NULL;
  char *p = bufptr;
  size_t size;
  int c;

  if (lineptr == NULL) {
    return -1;
  }
  if (stream == NULL) {
    return -1;
  }
  if (n == NULL) {
    return -1;
  }
  bufptr = *lineptr;
  size = *n;

  c = fgetc(stream);
  if (c == EOF) {
    return -1;
  }
  if (bufptr == NULL) {
    bufptr = (char*)malloc(128);
    if (bufptr == NULL) {
      return -1;
    }
    size = 128;
  }
  p = bufptr;
  while(c != EOF) {
    if ((p - bufptr) > int(size - 1)) {
      size = size + 128;
      bufptr = (char*)realloc(bufptr, size);
      if (bufptr == NULL) {
        return -1;
      }
    }
    *p++ = c;
    if (c == '\n') {
      break;
    }
    c = fgetc(stream);
  }

  *p++ = '\0';
  *lineptr = bufptr;
  *n = size;

  return p - bufptr - 1;
}

void setting_save( const char* name, const void* src, U32 size ) {
  char*     buffer = (char*)alloca( size * 2 + 1 );
  const U8* data = (const U8*)( src );

  memset( buffer, 0, size * 2 + 1 );

  for( U32 i = 0; i < size; ++i ) {
    sprintf( &buffer[2 * i], "%02x", data[i] );
  }

  FILE* f = fopen( get_path(), "a" );
  if( !f )
    return;

  fprintf( f, "%s = %s\n", name, buffer );
  fflush( f );

  fclose( f );
}

void setting_load( const char* name, const void* dst, U32 size ) {
  char* buffer = (char*)alloca( size * 2 + 1 );
  U8*   data = (U8*)( dst );

  memset( buffer, 0, size * 2 + 1 );

  FILE* f = fopen( get_path(), "r+" );
  if( !f )
    return;

  char  read_name[64]{};
  char* line = nullptr;
  U32   len = 128;

  /*
   * probably not the proper way
   * don't wanna fuck with an entire ini parser
  */
  while( getline( &line, &len, f ) != -1 ) {
    if( line ) {
      sscanf( line, "%s = %s", read_name, buffer );
      if( !strcmp( name, read_name ) )
        break;

      free( line );
      line = nullptr;
    }
  }

  fclose( f );

  if( !buffer[0] )
    return;

  for( U32 i = 0; i < size; ++i ) {
    unsigned temp;
    sscanf( &buffer[2 * i], "%02x", &temp );
    data[i] = temp;
  }
}
