.func rand
.synop begin
#include <stdlib.h>
int rand( void );
.ixfunc2 'Random Numbers' &funcb
.synop end
.desc begin
The
.id &funcb.
function computes a sequence of pseudo-random integers in
the range 0 to
.kw RAND_MAX
(32767).
The sequence can be started at different values by calling the
.kw srand
function.
.desc end
.return begin
The
.id &funcb.
function returns a pseudo-random integer.
.return end
.see begin
.seelist rand srand
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    int i;
.exmp break
    for( i=1; i < 10; ++i ) {
      printf( "%d\n", rand() );
    }
  }
.exmp end
.class ISO C
.system
