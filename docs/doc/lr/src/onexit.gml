.func _onexit onexit
.synop begin
.deprec
#include <stdlib.h>
onexit_t _onexit( _onexit_t func );
.ixfunc2 '&Process' _onexit
onexit_t onexit( onexit_t func );
.ixfunc2 '&Process' onexit
.synop end
.desc begin
The
.id &funcb.
function is passed the address of function
.arg func
to be called when the program terminates normally.
Successive calls to
.id &funcb.
create a list of functions that will be
executed on a "last-in, first-out" basis.
No more than 32 functions can be registered with the
.id &funcb.
function.
.pp
The functions have no parameters and do not return values.
.pp
NOTE: The
.id &funcb.
function is not an ISO C function.
The ISO C standard function
.reffunc atexit
does the same thing.
.np
.deprfunc _onexit atexit
.deprfunc onexit atexit
.desc end
.return begin
The
.id &funcb.
function returns
.arg func
if the registration succeeds,
NULL if it fails.
.return end
.see begin
.seelist abort atexit exit _Exit _exit
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main()
  {
    extern void func1(void), func2(void), func3(void);
.exmp break
    _onexit( func1 );
    _onexit( func2 );
    _onexit( func3 );
    printf( "Do this first.\n" );
  }
.exmp break
void func1(void) { printf( "last.\n" ); }
void func2(void) { printf( "this " ); }
void func3(void) { printf( "Do " ); }
.exmp output
Do this first.
Do this last.
.exmp end
.class WATCOM
.system
