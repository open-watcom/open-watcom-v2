.func _bios_keybrd
#include <bios.h>
unsigned short _bios_keybrd( unsigned service );
.ixfunc2 '&BiosFunc' &func
.funcend
.desc begin
The &func function uses INT 0x16 to access the BIOS keyboard services.
The possible values for
.arg service
are the following constants:
.begterm 17
.termhd1 Constant
.termhd2 Meaning
.term _KEYBRD_READ
Reads the next character from the keyboard.
The function will wait until a character has been typed.
.term _KEYBRD_READY
Checks to see if a character has been typed.
If there is one, then its value will be returned, but it is
not removed from the input buffer.
.term _KEYBRD_SHIFTSTATUS
Returns the current state of special keys.
.term _NKEYBRD_READ
Reads the next character from an enhanced keyboard.
The function will wait until a character has been typed.
.term _NKEYBRD_READY
Checks to see if a character has been typed on an enhanced keyboard.
If there is one, then its value will be returned, but it is
not removed from the input buffer.
.term _NKEYBRD_SHIFTSTATUS
Returns the current state of special keys on an enhanced keyboard.
.endterm
.desc end
.return begin
The return value depends on the
.arg service
requested.
.np
The
.kw _KEYBRD_READ
and
.kw _NKEYBRD_READ
services return the character's ASCII value in the low-order byte
and the character's keyboard scan code in the high-order byte.
.np
The
.kw _KEYBRD_READY
and
.kw _NKEYBRD_READY
services return zero if there was no character available, otherwise
it returns the same value returned by
.kw _KEYBRD_READ
and
.kw _NKEYBRD_READ
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
Right SHIFT key is pressed
.note bit 1 (0x02)
Left SHIFT key is pressed
.note bit 2 (0x04)
CTRL key is pressed
.note bit 3 (0x08)
ALT key is pressed
.note bit 4 (0x10)
SCROLL LOCK is on
.note bit 5 (0x20)
NUM LOCK is on
.note bit 6 (0x40)
CAPS LOCK is on
.note bit 7 (0x80)
Insert mode is set
.endnote
.sr ptntelmt = 1
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short key_state;
.exmp break
    key_state = _bios_keybrd( _KEYBRD_SHIFTSTATUS );
    if( key_state & 0x10 )
        printf( "SCROLL LOCK is on\n" );
    if( key_state & 0x20 )
        printf( "NUM LOCK is on\n" );
    if( key_state & 0x40 )
        printf( "CAPS LOCK is on\n" );
  }
.exmp output
NUM LOCK is on
.exmp end
.class BIOS
.system
