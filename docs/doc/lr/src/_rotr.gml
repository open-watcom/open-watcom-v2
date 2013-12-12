.func _rotr
#include <stdlib.h>
unsigned int _rotr( unsigned int value,
                    unsigned int shift );
.ixfunc2 '&Rotate' &func
.synop end
.desc begin
The &func function rotates the unsigned integer, determined by
.arg value
.ct , to the right by the number of bits specified in
.arg shift
.ct .li .
If you port an application using &func between a 16-bit and a 32-bit
environment, you will get different results because of the difference
in the size of integers.
.desc end
.return begin
The rotated value is returned.
.return end
.see begin
.seelist _rotr _lrotl _lrotr _rotl
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

unsigned int mask = 0x1230;

void main()
  {
    mask = _rotr( mask, 4 );
    printf( "%04X\n", mask );
  }
.exmp output
0123
.exmp end
.class WATCOM
.system
