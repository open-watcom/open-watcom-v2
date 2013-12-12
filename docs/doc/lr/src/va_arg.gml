.func va_arg
.synop begin
#include <stdarg.h>
type va_arg( va_list param, type );
.ixfunc2 'variable arguments'  &func
.synop end
.desc begin
&func is a macro that can be used to obtain the next argument in a
list of variable arguments.
It must be used with the associated macros
.kw va_start
and
.kw va_end
.ct .li .
A sequence such as
.millust begin
void example( char *dst, ... )
{
    va_list curr_arg;
    int next_arg;

    va_start( curr_arg, dst );
    next_arg = va_arg( curr_arg, int );
    .
    .
    .
.millust end
.pc
causes
.id next_arg
to be assigned the value of the next variable argument.
The argument
.arg type
(which is
.kw int
in the example) is the type of the argument originally passed
to the function.
.np
The macro
.kw va_start
must be executed first in order to properly initialize the variable
.id curr_arg
and the macro
.kw va_end
should be executed after all arguments have been obtained.
.np
The data item
.id curr_arg
is of type
.kw va_list
which contains the information to permit successive acquisitions
of the arguments.
.desc end
.return begin
The macro returns the value of the next variable argument, according to
type passed as the second parameter.
.return end
.see begin
.im seevarg va_arg
.see end
.exmp begin
#include <stdio.h>
#include <stdarg.h>
.exmp break
static void test_fn(
  const char *msg,   /* message to be printed    */
  const char *types, /* parameter types (i,s)    */
  ... )              /* variable arguments       */
{
    va_list     argument;
    int         arg_int;
    char        *arg_string;
    const char  *types_ptr;
.exmp break
    types_ptr = types;
    printf( "\n%s -- %s\n", msg, types );
    va_start( argument, types );
    while( *types_ptr != '\0' ) {
        if (*types_ptr == 'i') {
            arg_int = va_arg( argument, int );
            printf( "integer: %d\n", arg_int );
        } else if (*types_ptr == 's') {
            arg_string = va_arg( argument, char * );
            printf( "string:  %s\n", arg_string );
        }
        ++types_ptr;
    }
    va_end( argument );
}
.exmp break
void main( void )
{
    printf( "VA...TEST\n" );
    test_fn( "PARAMETERS: 1, \"abc\", 546",
             "isi", 1, "abc", 546 );
    test_fn( "PARAMETERS: \"def\", 789",
             "si", "def", 789 );
}
.exmp output
VA...TEST

PARAMETERS: 1, "abc", 546 -- isi
integer: 1
string:  abc
integer: 546

PARAMETERS: "def", 789 -- si
string:  def
integer: 789
.exmp end
.class ISO C90
.system
