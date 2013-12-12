.func jistojms
#include <jstring.h>
unsigned short jistojms( unsigned short c );
.ixfunc2 '&CharTest' &func
.synop end
.desc begin
The &func converts a JIS character set code to a shift-JIS character
set code.
If the argument is out of range, &func returns 0.
Valid JIS double-byte characters are those in which the first and
second byte fall in the range 0x21 - 0x7E.
This is summarized in the following diagram.
.millust begin
   [ 1st byte ]    [ 2nd byte ]
    0x21-0x7E       0x21-0x7E
.millust end
.np
.us Note:
The JIS character set code is a double-byte character set defined by
JIS, the Japan Industrial Standard Institutes.
Shift-JIS is another double-byte character set.
It is defined by Microsoft for MS/DOS and is based on the JIS code.
The first byte and the second byte of JIS codes can have values less
than 0x80.
Microsoft has designed shift-JIS code so that it can be mixed in
strings with single-byte alpha-numeric codes.
Thus the double-byte shift-JIS codes are greater than or equal to
0x8140.
.desc end
.return begin
The &func function returns zero if the argument is not in the range;
otherwise, the corresponding shift-JIS code is returned.
.return end
.see begin
.seelist &function. jistojms jmstojis
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    unsigned short c;
.exmp break
    c = jistojms( 0x2152 );
    printf( "%x\n", c );
  }
.exmp output
8171
.exmp end
.class WATCOM
.system
