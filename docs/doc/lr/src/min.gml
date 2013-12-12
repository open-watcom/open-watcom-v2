.func min
.synop begin
#include <stdlib.h>
#define min(a,b)  (((a) < (b)) ? (a) : (b))
.synop end
.desc begin
The
.id &func.
macro will evaluate to be the lesser of two values.
It is implemented as follows.
.millust begin
#define min(a,b)  (((a) < (b)) ? (a) : (b))
.millust end
.desc end
.return begin
The
.id &func.
macro will evaluate to the smaller of the two values passed.
.return end
.see begin
.seelist min max
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main()
  {
    int a;
.exmp break
    /*
     * The following line will set the variable "a" to 1
     * since 10 is greater than 1.
     */
    a = min( 1, 10 );
    printf( "The value is: %d\n", a );
  }
.exmp end
.class WATCOM
.system
