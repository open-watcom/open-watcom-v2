#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short key_state;

    key_state = _bios_keybrd( _KEYBRD_SHIFTSTATUS );
    if( key_state & 0x10 )
        printf( "SCROLL LOCK is on\n" );
    if( key_state & 0x20 )
        printf( "NUM LOCK is on\n" );
    if( key_state & 0x40 )
        printf( "CAPS LOCK is on\n" );
  }
