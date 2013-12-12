.func max
.synop begin
#include <stdlib.h>
#define max(a,b)  (((a) > (b)) ? (a) : (b))
.synop end
.desc begin
The
.id &func.
macro will evaluate to be the greater of two values.
It is implemented as follows.
.millust begin
#define max(a,b)  (((a) > (b)) ? (a) : (b))
.millust end
.desc end
.return begin
The
.id &func.
macro will evaluate to the larger of the two values passed.
.return end
.see begin
.seelist max min
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
     * The following line will set the variable "a" to 10
     * since 10 is greater than 1.
     */
    a = max( 1, 10 );
    printf( "The value is: %d\n", a );
  }
.exmp end
.class WATCOM
.system
