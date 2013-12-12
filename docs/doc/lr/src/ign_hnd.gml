.func ignore_handler_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
void ignore_handler_s(
        const char * restrict msg,
        void * restrict ptr, 
        errno_t error );
.ixfunc2 '&Process' &func
.synop end
.*
.desc begin
A pointer to the
.id &func.
function may be passed as an argument to the
.mono set_constraint_handler_s
function.
The
.id &func.
function simply returns to its caller.
.desc end
.*
.return begin
The
.id &func.
function does not returns no value.
.return end
.*
.see begin
.seelist ignore_handler_s abort_handler_s set_constraint_handler_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
#include <stdio.h>
.exmp break
void main( void )
{
    constraint_handler_t    old_handler;
.exmp break
    old_handler = 
        set_constraint_handler_s( ignore_handler_s );
    if( getenv_s( NULL, NULL, 0, NULL ) ) {
        printf( "getenv_s failed\n" );
    }
    set_constraint_handler_s( old_handler );
}
.exmp output
getenv_s failed
.exmp end
.*
.class TR 24731
.system
