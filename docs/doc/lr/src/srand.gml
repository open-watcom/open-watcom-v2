.func srand
.synop begin
#include <stdlib.h>
void srand( unsigned int seed );
.ixfunc2 'Random Numbers' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses the argument
.arg seed
to start a new sequence of pseudo-random integers to be returned by
subsequent calls to
.kw rand
.ct .li .
A particular sequence of pseudo-random integers can be repeated by
calling
.id &funcb.
with the same
.arg seed
value.
The default sequence of pseudo-random integers is selected with a
.arg seed
value of 1.
.desc end
.return begin
The
.id &funcb.
function returns no value.
.return end
.see begin
.seelist srand rand
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    int i;
.exmp break
    srand( 982 );
    for( i = 1; i < 10; ++i ) {
        printf( "%d\n", rand() );
    }
    srand( 982 );  /* start sequence over again */
    for( i = 1; i < 10; ++i ) {
        printf( "%d\n", rand() );
    }
  }
.exmp end
.class ISO C
.system
