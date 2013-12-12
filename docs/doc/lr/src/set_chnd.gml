.func set_constraint_handler_s
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
constraint_handler_t set_constraint_handler_s(
        constraint_handler_t handler );
.ixfunc2 '&Process' &func
.synop end
.*
.desc begin
The &func function sets the runtime-constraint handler to be
.arg handler
.ct .li .
The runtime-constraint handler is the function called when a library function
detect a runtime-constraint violation. Only the most recent handler registered
with &func is called when a runtime-constraint violation occurs.
.np
When the handler is called, it is passed the following arguments:
.autonote
.note
A pointer to a character string describing the runtime-constraint violation.
.note
A null pointer or a pointer to an implementation defined object. This
implementation passes a null pointer.
.note
If the function calling the handler has a return type declared as
.kw errno_t
.ct , the return value of the function is passed. Otherwise, a positive value of type
.kw errno_t
is passed.
.endnote
If no calls to the &func function have been made, a default constraint handler
is used. This handler will display an error message and abort the program.
.np
If the
.arg handler
argument to &func is a null pointer, the default handler becomes the current
constraint handler.
.desc end
.*
.return begin
The &func function returns a pointer to the previously registered handler.
.return end
.*
.see begin
.seelist set_constraint_handler_s abort_handler_s ignore_handler_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
#include <stdio.h>
.exmp break
void my_handler( const char *msg, void *ptr, errno_t error )
{
    fprintf( stderr, "rt-constraint violation caught :" );
    fprintf( stderr, msg );
    fprintf( stderr, "\n" );
}
.exmp break    
void main( void )
{
    constraint_handler_t    old_handler;
.exmp break
    old_handler = set_constraint_handler_s( my_handler );
    if( getenv_s( NULL, NULL, 0, NULL ) ) {
        printf( "getenv_s failed\n" );
    }
    set_constraint_handler_s( old_handler );
}
.exmp output
rt-constraint violation caught: getenv_s, name == NULL.
getenv_s failed
.exmp end
.*
.class TR 24731
.system
