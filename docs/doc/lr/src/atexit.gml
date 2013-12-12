.func atexit
#include <stdlib.h>
int atexit( void (*func)(void) );
.ixfunc2 '&Process' &func
.synop end
.desc begin
The &func  function is passed the address of function
.arg func
to be called when the program terminates normally.
Successive calls to &func
create a list of functions that will be executed on a
"last-in, first-out" basis.
No more than 32 functions can be registered with the &func function.
.pp
The functions have no parameters and do not return values.
.desc end
.return begin
The &func function returns zero if the registration succeeds,
non-zero if it fails.
.return end
.see begin
.seelist atexit abort _exit exit
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main()
  {
    extern void func1(void), func2(void), func3(void);
.exmp break
    atexit( func1 );
    atexit( func2 );
    atexit( func3 );
    printf( "Do this first.\n" );
  }
.exmp break
void func1(void) { printf( "last.\n" ); }
.exmp break
void func2(void) { printf( "this " ); }
.exmp break
void func3(void) { printf( "Do " ); }
.exmp output
Do this first.
Do this last.
.exmp end
.class ANSI
.system
