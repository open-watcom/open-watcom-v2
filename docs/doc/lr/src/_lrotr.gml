.func _lrotr
.synop begin
#include <stdlib.h>
unsigned long _lrotr( unsigned long value,
                      unsigned int shift );
.ixfunc2 '&Rotate' &funcb
.synop end
.desc begin
The
.id &funcb.
function rotates the unsigned long integer, determined by
.arg value
.ct , to the right by the number of bits specified in
.arg shift
.ct .li .
.desc end
.return begin
The rotated value is returned.
.return end
.see begin
.seelist _lrotl _rotl _rotr
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

unsigned long mask = 0x12345678;

void main()
  {
    mask = _lrotr( mask, 4 );
    printf( "%08lX\n", mask );
  }
.exmp output
81234567
.exmp end
.class WATCOM
.system
