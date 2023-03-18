//|_   _   _.     _  ._  |_   _.  _ |
//| | (/_ (_| \/ (/_ | | | | (_| (_ |<

#include <windows.h>
#include <winternl.h>
#pragma comment( lib, "ntdll.lib" )

#include "util.h"

struct LOGITECH_MOUSE_IO {
  char button;
  char x;
  char y;
  char wheel;
private:
  char unk;
};

HANDLE nt_init_device( const char* device_name ) {
  UNICODE_STRING name;
  OBJECT_ATTRIBUTES attr;

  RtlInitUnicodeString(&name, u_ansi_to_widebyte( device_name ) );
  InitializeObjectAttributes( &attr, &name, 0, NULL, NULL );

  NTSTATUS status = NtCreateFile(&g_input, GENERIC_WRITE | SYNCHRONIZE, &attr, &g_io, 0,
    FILE_ATTRIBUTE_NORMAL, 0, 3, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);

  return status;
}

HANDLE mouse_open( )