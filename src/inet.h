#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "conout.h"
#include "typedef.h"

#pragma comment( lib, "libssl.lib" )
#pragma comment( lib, "libcrypto.lib" )
#pragma comment( lib, "Ws2_32.lib" )

inline U32 net_create_socket( I32 family, I32 type, I32 protocol ) {
  static WSADATA wsa_data;
  static bool wsa_init = false;
  if( !wsa_init ) {
    if( WSAStartup( MAKEWORD( 2, 2 ), &wsa_data ) != 0 ) {
      assert( true, "WSAStartup() error" );
      return INVALID_SOCKET;
    }

    wsa_init = true;
  }
  
  return socket( family, type, protocol );
}

inline U32 net_connect( U32 socket, const char* ip, U16 port ) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons( port );
  inet_pton( AF_INET, ip, &addr.sin_addr.s_addr );
  
  return connect( socket, (sockaddr*)&addr, sizeof( addr ) );
}

inline SSL_CTX* ssl_create_context() {
  OpenSSL_add_all_algorithms();
#ifdef DEBUG
  SSL_load_error_strings();
#endif
  
  SSL_CTX* ctx = SSL_CTX_new( TLSv1_2_client_method() );
  if( !ctx )
    return nullptr;

  return ctx;
}

inline SSL* ssl_connect( SSL_CTX* ctx, int sock ) {
  SSL* ssl = SSL_new( ctx );
  if( !ssl )
    return nullptr;

  SSL_set_fd( ssl, sock );
  if( SSL_connect( ssl ) <= 0 )
    return nullptr;

  return ssl;
}

inline void ssl_destroy( SSL* ssl ) {
  if( ssl )
    SSL_free( ssl );
}

inline void ssl_destroy_context( SSL_CTX* ctx ) {
  if( ctx )
    SSL_CTX_free( ctx );
}

inline int ssl_send( SSL* ssl, const void* data, int len ) {
  return SSL_write( ssl, data, len );
}