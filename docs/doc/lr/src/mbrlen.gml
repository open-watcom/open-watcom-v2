.func mbrlen _fmbrlen
#include <wchar.h>
int mbrlen( const char *s, size_t n, mbstate_t *ps );
.if &farfnc eq 1 .do begin
int _fmbrlen( const char far *s, size_t n, mbstate_t far *ps );
.do end
.funcend
.desc begin
The &func function determines the number of bytes comprising the
multibyte character pointed to by
.arg s
.ct .li .
The &func function is equivalent to the following call:
.millust begin
mbrtowc((wchar_t *)0, s, n, ps != 0 ? ps : &internal)
.millust end
where
.mono &internal
is the address of the internal
.kw mbstate_t
object for the &func function.
.im farparm
.im mbcrstrt
.desc end
.return begin
The &func function returns a value between -2 and
.arg n
.ct , inclusive.
The &func function returns the first of the following that applies:
.begnote
.termhd1 Value
.termhd2 Meaning
.note 0
if the next
.arg n
or fewer bytes form the multibyte character that corresponds to the
null wide character.
.note >0
if the next
.arg n
or fewer bytes form a valid multibyte character; the value returned is
the number of bytes that constitute that multibyte character.
.note -2
if the next
.arg n
bytes form an incomplete (but potentially valid) multibyte character,
and all
.arg n
bytes have been processed; it is unspecified whether this can occur
when the value of
.arg n
is less than that of the
.kw MB_CUR_MAX
macro.
.note -1
if an encoding error occurs (when the next
.arg n
or fewer bytes do not form a complete and valid multibyte character);
the value of the macro
.kw EILSEQ
will be stored in
.kw errno
.ct , but the conversion state will be unchanged.
.endnote
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <wchar.h>
#include <mbctype.h>
#include <errno.h>


const char chars[] = {
    ' ',
    '.',
    '1',
    'A',
    0x81,0x40, /* double-byte space */
    0x82,0x60, /* double-byte A */
    0x82,0xA6, /* double-byte Hiragana */
    0x83,0x42, /* double-byte Katakana */
    0xA1,      /* single-byte Katakana punctuation */
    0xA6,      /* single-byte Katakana alphabetic */
    0xDF,      /* single-byte Katakana alphabetic */
    0xE0,0xA1, /* double-byte Kanji */
    0x00
};
.exmp break
void main()
  {
    int         i, j, k;

    _setmbcp( 932 );
    j = 1;
    for( i = 0; j > 0; i += j ) {
      j = mbrlen( &chars[i], MB_CUR_MAX, NULL );
      printf( "%d bytes in character ", j );
      if( errno == EILSEQ ) {
        printf( " - illegal multibyte character\n" );
      } else {
        if( j == 0 ) {
          k = 0;
        } else if ( j == 1 ) {
          k = chars[i];
        } else if( j == 2 ) {
          k = chars[i]<<8 | chars[i+1];
        }
        printf( "(%#6.4x)\n", k );
      }
    }
  }
.exmp output
1 bytes in character (0x0020)
1 bytes in character (0x002e)
1 bytes in character (0x0031)
1 bytes in character (0x0041)
2 bytes in character (0x8140)
2 bytes in character (0x8260)
2 bytes in character (0x82a6)
2 bytes in character (0x8342)
1 bytes in character (0x00a1)
1 bytes in character (0x00a6)
1 bytes in character (0x00df)
2 bytes in character (0xe0a1)
0 bytes in character (  0000)
.exmp end
.class ANSI
.system
