.func swab
.synop begin
#include <stdlib.h>
void swab( char *src, char *dest, int num );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function copies
.arg num
bytes (which should be even) from
.arg src
to
.arg dest
swapping every pair of characters.
This is useful for preparing binary data to be transferred to another
machine that has a different byte ordering.
.desc end
.return begin
The &func function has no return value.
.return end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *msg = "hTsim seasegi  swspaep.d";
#define NBYTES 24
.exmp break
void main()
  {
    auto char buffer[80];
.exmp break
    printf( "%s\n", msg );
    memset( buffer, '\0', 80 );
    swab( msg, buffer, NBYTES );
    printf( "%s\n", buffer );
  }
.exmp output
hTsim seasegi  swspaep.d
This message is swapped.
.exmp end
.class WATCOM
.system
