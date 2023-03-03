#define db( x ) __asm _emit x

#define heavens_gate_enter() \
db( 0x6a ) db( 0x33 ) /*push 0x33*/ \
db( 0xe8 ) db( 0x0 ) db( 0x0 ) db( 0x0 ) db( 0x0 ) /*call rel32 0x0*/ \
db( 0x83 ) db( 0x4 ) db( 0x24 ) db( 0x5 ) /*add dword ptr[ esp ], 0x5*/ \
db( 0xcb ) /*retf - pop ip and cs*/

#define heavens_gate_exit() \
db( 0xe8 ) db( 0x0 ) db( 0x0 ) db( 0x0 ) db( 0x0 ) /*call rel32 0x0*/ \
db( 0xc7 ) db( 0x44 ) db( 0x24 ) db( 0x4 ) \
db( 0x23 ) db( 0x0 ) db( 0x0 ) db( 0x0 ) /*mov dword ptr[ rsp + 0x4 ], 0x23 - return address hack (come back to wow64 cs)*/\
db( 0x83 ) db( 0x4 ) db( 0x24 ) db( 0xd ) /*add dword ptr[ rsp ], 0xd*/ \
db( 0xcb ) /*retf*/

#define rex_w  db( 0x48 ) __asm
#define rex_wb db( 0x49 ) __asm
#define rex_wr db( 0x4c ) __asm
#define x64_push( x ) db( 0x48 | ( ( x ) >> 3 ) ) db( 0x50 | ( ( x ) & 7 ) )
#define x64_pop( x ) db( 0x48 | ( ( x ) >> 3 ) ) db( 0x58 | ( ( x ) & 7 ) )
