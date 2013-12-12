.func _mbccpy _fmbccpy
.synop begin
#include <mbstring.h>
void _mbccpy( unsigned char *dest,
              const unsigned char *ch );
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
void _fmbccpy( unsigned char __far *dest,
               const unsigned char __far *ch );
.ixfunc2 '&Multibyte' &ffunc
.do end
.synop end
.desc begin
The
.id &func.
function copies one multibyte character from
.arg ch
to
.arg dest
.ct .li .
.im farparm
.desc end
.return begin
The
.id &func.
function does not return a value.
.return end
.see begin
.im seembc &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>
#include <mbstring.h>

unsigned char mb1[2] = {
    0x00, 0x00
};

unsigned char mb2[4] = {
    0x81, 0x42, 0x81, 0x41
};

void main()
  {
    _setmbcp( 932 );
    printf( "%#6.4x\n", mb1[0] << 8 | mb1[1] );
    _mbccpy( mb1, mb2 );
    printf( "%#6.4x\n", mb1[0] << 8 | mb1[1] );
  }
.exmp output
  0000
0x8142
.exmp end
.class WATCOM
.system
