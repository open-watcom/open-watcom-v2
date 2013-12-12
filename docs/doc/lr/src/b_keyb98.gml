.func _bios_keybrd for NEC PC98
.synop begin
#include <bios.h>
unsigned short _bios_keybrd( unsigned service,
                             unsigned char *keytable);
.ixfunc2 '&BiosFunc' &func
.synop end
.desc begin
The &func function accesses the BIOS keyboard services.
The possible values for
.arg service
are the following constants:
.begterm 17
.termhd1 Constant
.termhd2 Meaning
.term _KEYBRD_INITIALIZE
It initializes the keyboard interface. It initializes the keyboard
memory area and the controller. It enables input from the keyboard. It
returns no values.
.term _KEYBRD_READ
It reads key data code in the top of the key data buffer. It waits for
input if no data is in the key data buffer. The low-order byte of the
returned data is the ASCII code and the high-order byte is the scan code
.term _KEYBRD_READY
It checks whether there is a key code in the key data buffer. If key
data is in the key data buffer, it operates the same as
.kw _KEYBRD_READ
.ct .li .
But the status of the buffer is not changed. It returns zero if no
data.
.term _KEYBRD_SENSE
It checks the status of key input and returns it in the 16 bytes of
the area pointed to by the argument
.arg keytable
.ct .li .
Each byte corresponds to a code group.
.term _KEYBRD_SHIFTSTATUS
It checks whether any of the SHIFT, CAPS, KANA, GRPH, or CTRL keys are
pressed.
The low-order byte of the return value indicates which keys are
pressed.
.endterm
.desc end
.return begin
The return value depends on the
.arg service
requested.
.np
The
.kw _KEYBRD_READ
service return the character's ASCII value in the low-order byte
and the character's keyboard scan code in the high-order byte.
.np
The
.kw _KEYBRD_READY
services return zero if there was no character available, otherwise
it returns the same value returned by
.kw _KEYBRD_READ
.ct .li .
.np
The shift status is returned in the low-order byte with one bit
for each special key defined as follows:
.begnote $compact
.termhd1 Bit
.termhd2 Meaning
.setptnt 0 12
.sr ptntelmt = 0
.note bit 0 (0x01)
SHIFT is pressed
.note bit 1 (0x02)
CAPS is on
.note bit 2 (0x04)
KANA is on
.note bit 3 (0x08)
GRPH is pressed
.note bit 4 (0x10)
CTRL is pressed
.note bits 5-7
Not used
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
#include <bios.h>
#include <stdio.h>
#include <ctype.h>

void main()
  {
    unsigned key, shift, scan, ascii = 0;
    int     kread = _KEYBRD_READ;
    int     kready = _KEYBRD_READY;
    int     kshiftstatus = _KEYBRD_SHIFTSTATUS;

    /* Read and display keys until ESC is pressed */
    while( ascii != 27 ) {
      /*
       * Drain any keys in the keyboard type-ahead buffer,
       * then get the current key. If you want the last
       * key typed rather than the key currently being
       * typed, omit the initial loop.
       */
      while( _bios_keybrd( kready, NULL ) )
          _bios_keybrd( kread, NULL );
      key = _bios_keybrd( kread, NULL );

      /* Get shift state */
      shift = _bios_keybrd( kshiftstatus, NULL );

      /* Split key into scan and ascii parts */
      scan = key >> 8;
      ascii = key & 0x00ff;

      /* Categorize key */
      if( ( ascii == 0 ) || ( ascii == 0xE0 ) )
        printf( "ASCII: no\tChar: NA\t" );
      else if ( ascii < 32 )
        printf( "ASCII: yes\tChar: ^%c\t", ascii + '@' );
      else
        printf( "ASCII: yes\tChar: %c \t", ascii );
      printf( "Code: %.2X\tScan: %.2X\t Shift: %.4X\n",
              ascii, scan, shift );
    }
  }
.exmp end
.class BIOS
.system
