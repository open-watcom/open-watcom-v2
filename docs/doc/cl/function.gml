.ix 'function' 'definition'
.pp
There are two forms for defining a function. The first form is,
.cillust begin
storage-class return-type identifier
.mono (
parameter-type-list
.mono )
.cbr
.mono {
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.declaration-list
.csk
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement-list
.cbr
.mono }
.cillust end
.pc
The
.us storage-class
may be one of
.kw extern
or
.kw static
..ct ..li .
If
.us storage-class
is omitted,
.kw extern
is assumed.
.pp
The
.us return-type
may be any valid type except an
.us array
..ct ..li .
If
.us return-type
is omitted,
.kw int
is assumed.
.pp
The
.us identifier
is the name of the function.
.pp
The
.us parameter-type-list
is either
.kw void
or empty,
meaning the function takes no parameters, or a
comma-separated list of declarations of the objects, including both
type and parameter name (identifier).
If multiple arguments of the same type are specified, the type of
each argument must be given individually. The form,
.uillust type id1, id2
is not permitted within the parameter list.
.pp
If the
.us parameter-type-list
ends with
.mono ,...
then the function will accept a variable number of
arguments.
.pp
Any parameter declared as "array of
.us type
..ct "
is changed to
"pointer to
.us type
..ct ".
Any parameter declared as "
.us function
..ct "
is changed to
"pointer to
.us function
..ct ".
.keep begin
.pp
The following examples illustrate several function definitions:
.millust int F( void )
.discuss begin
The function
.mono F
has no parameters, and returns an integer.
.discuss end
.keep break
.millust void G( int x )
.discuss begin
The function
.mono G
has one parameter, an integer object named
.mono x
..ct ,
and does not return a value.
.discuss end
.keep break
.millust void * H( long int len, long int wid )
.discuss begin
The function
.mono H
has two parameters, long integer objects named
.mono len
and
.mono wid
..ct ,
and returns a pointer which does not point to any particular type
of object.
.discuss end
.keep break
.millust void I( char * format, ... )
.discuss begin
The function
.mono I
has one known parameter, an object named
.mono format
that is a pointer to a character (string).
The function also accepts a variable
number of parameters following
.mono format
..ct ..li .
The function does not return a result.
.discuss end
.keep end
.pp
This form of function definition also serves as a prototype declaration
for any calls to the function that occur later in the same
module. With the function prototype in scope at the time of a call to the
function, the arguments are converted to the type of the corresponding
parameter prior to the value being assigned. If a call to the function
is to be made prior to its definition, or from another module, a
function prototype should be specified for it in order to ensure proper
conversion of argument types.
Failure to do this will result in the
.ix 'default argument promotion'
default argument promotions being
performed, with undefined behavior if the function parameter types
do not match the promoted argument types.
.keep begin
.pp
The second form of function definition is,
.cillust begin
storage-class return-type identifier
.mono (
identifier-list
.mono )
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.declaration-list
.cbr
.mono {
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.declaration-list
.csk
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement-list
.cbr
.mono }
.cillust end
.keep end
.pc
The
.us storage-class
..ct ,
.us return-type
and
.us identifier
parts are all the same as for the first form of definition.
In this form, the
.us identifier-list
is a (possibly empty) comma-separated list of identifiers (object
names) without any type information. Following the closing parenthesis,
and before the opening brace of the body of the function,
the declarations for the objects are given, using the normal rules.
Any object of type
.kw int
need not be explicitly declared.
.pp
In the declarations of the parameter identifiers,
.kw register
is the only storage-class specifier that may be used.
.pp
A function prototype is created from the definition
after the
.ix 'default argument promotion'
default argument promotions have been performed on each
parameter.
All arguments to a function declared in this manner will have the
default argument promotions performed on them.
The resulting types
must match the types of the declared parameters, after promotion.
Otherwise,
the behavior is undefined.
.pp
Note that it is impossible to pass an object of type
.kw float
to a function declared in this manner. The argument of type
.kw float
will automatically be promoted to
.kw double
..ct ,
and the parameter will also be promoted to
.kw double
(assuming that it was declared as
.kw float
..ct ..li ).
For similar reasons, it is not possible to pass an object of type
.kw char
or
.kw short int
without promotion taking place.
.pp
According to the ISO standard for the C language,
.bd this form of function definition is obsolete
and should not be used. It is provided for historical reasons,
in particular, for compatibility with older C compilers.
Using the first form of function definition often allows the compiler
to generate better code.
.keep begin
.pp
The following examples are the same as those
given with the first form above,
with the appropriate modifications:
.millust int F()
.discuss begin
The function
.mono F
has no parameters, and returns an integer.
.discuss end
.keep break
.millust void G( x )
.discuss begin
The function
.mono G
has one parameter, an integer object named
.mono x
..ct ,
and does not return a value.
This example could have also been written as,
.millust begin
void G( x )
    int x;
.millust end
.keep break
.pc
which explicitly declares
.mono x
to be an integer.
.discuss end
.keep break
.millust begin
void * H( len, wid )
    long int len;
    long int wid;
.millust end
.discuss begin
The function
.mono H
has two parameters, both integer objects named
.mono len
and
.mono wid
..ct ,
and returns a pointer which does not point to any particular type
of object.
Any call to this function must ensure that the arguments are long
integers, either by using an object so declared, or by explicitly
casting the object to the type.
.discuss end
.keep end
.pp
The last example using the ellipsis (
..ct .mono ,...
..ct )
notation is not
directly representable using the second form of function definition.
With most compilers it is possible to handle variable argument lists
in this form,
but knowledge of the mechanism used to pass arguments to functions
is required, and this mechanism may vary between different compilers.
.*
.section The Body of the Function
.*
.pp
Following the declaration of the function
and the opening brace
is the
.us body
of the function.
It consists of two portions, both of which are optional.
.pp
The first portion is the declaration list for any objects needed within
the function. These objects may have any type and any storage class.
Objects with storage class
.kw register
or
.kw auto
have
.ix 'automatic storage duration'
.ix 'storage duration' 'automatic'
.us automatic storage duration
..ct ,
meaning they are created when the function is called, and destroyed
when the function returns to the caller.
(The value of the object is not preserved between calls
to the function.)
Objects with storage class
.kw extern
or
.kw static
have
.ix 'static storage duration'
.ix 'storage duration' 'static'
.us static storage duration
..ct ,
meaning they are created once, before the function is ever called, and
destroyed only when the program terminates. Any value placed in such
an object will remain even after the function has returned, so that
the next time the function is called the value will still be present
(unless some other action is taken to change it, such as using
another object containing a pointer to the static object
to modify the value).
.pp
Unless an explicit
.kw return
statement is executed, the function
will not return to the caller until the brace at the end of the
function definition is encountered.
The return will be as if a
.kw return
statement with no expression was executed.
If the function is declared as returning a value, and the caller
attempts to use the value returned in this manner, the behavior
is undefined.
The value used will be arbitrary.
.pp
A function may call itself
.ix 'recursion'
.ix 'function' 'recursion'
.us :HP0.(:eHP0.recursion:HP0.):eHP0.
directly, or it may call another function or functions which in turn
call it. Any objects declared with
.us automatic storage duration
are created as a new instance of the object upon each recursion, while
objects declared with
.us static storage duration
only have one instance shared between the recursive instances of the
function.
.*
.section Function Prototypes
.*
.ix 'function' 'prototype'
.ix 'prototype' 'function'
.pp
A function prototype is like a definition of a function, but without
the body.
A semi-colon is specified immediately following the closing
right parenthesis of the function's declaration.
The prototype describes the name of the function, the
types of parameters it expects (names are optional)
and the type of the return value.
This information can be used by the C compiler to do proper argument
type checking and
conversion for calls to the function, and to properly handle the
return value.
.pp
If no function prototype has been found by the time a call to a function
is made, all arguments have the default argument promotions performed
on them, and the return type is assumed to be
.kw int
..ct ..li .
If the actual definition of the function does not have parameters that
match the promoted types, the behavior is undefined.
If the return type is not
.kw int
and a return value is required, the behavior is undefined.
.pp
The prototype for a function must match the function definition.
Each parameter type and the type of the return value must be the same,
otherwise the behavior is undefined.
.pp
All library functions have prototypes in one of several
.ix 'header'
header files. That header file should be included whenever a function
described therein is used.
Refer to the &libref for details.
.beglevel
.*
.section Variable Argument Lists
.*
.ix 'variable argument list'
.pp
If the prototype (and definition)
for a function has a parameter list that ends with
.mono ,...
then the function has a
.us variable argument list
or
.us variable parameter list
meaning that the number of arguments to the function can vary.
(The
library function
.libfn printf
is an example.)
At least one argument must be provided before the variable portion.
This argument usually describes, in some fashion, how many other
arguments to expect.
It may be a simple count, or may involve (as with
.libfn printf
..ct )
an encoding of the number and types of arguments.
.pp
All arguments that correspond to a variable argument list have the
default argument promotions performed on them, since it is not possible
to determine, at compilation time, what types will be required by
the function.
.pp
Since the parameters represented by the
.mono ,...
don't have names, special handling is required.
The C language provides a special type and three macros for
handling variable argument lists.
To be able to use these, the header
.hdr <stdarg.h>
must be included.
.pp
The type
.ix 'type' 'va_list'
.ix 'va_list type'
.kw va_list
is an
implementation-specific
type used to store information about the variable list.
Within the function, an object must be declared with type
.kw va_list
..ct ..li .
This object is used by the macros and functions for processing the list.
.* .pp
.* In the following examples, it is assumed that the declaration
.* .millust begin
.* va_list parminfo;
.* .millust end
.* .pc
.* has been made in the function processing its variable parameter list.
.pp
The macro
.ix 'macro' 'variable argument' 'va_start'
.ix 'va_start'
.mkw va_start
has the form,
.millust begin
void va_start( va_list :ITAL.parminfo:eITAL., :ITAL.lastparm:eITAL. );
.millust end
.pc
The
object
:ITAL.parminfo:eITAL.
is set up by the macro with information describing the variable
list. The argument
:ITAL.lastparm:eITAL.
is the name (identifier) of the last parameter before the
.mono ,...
and must not have been declared with the storage class
.ix 'storage class' 'register'
.ix 'register'
.kw register
..ct ..li .
.pp
The macro
.mkw va_start
must be executed before any processing of the variable portion of the
parameter list is performed.
.pp
.mkw va_start
may be executed more than once, but only if an intervening
.mkw va_end
is executed.
.pp
The macro
.ix 'macro' 'variable argument' 'va_arg'
.ix 'va_arg'
.mkw va_arg
has the form,
.millust begin
:ITAL.type:eITAL. va_arg( va_list :ITAL.parminfo:eITAL., :ITAL.type:eITAL. );
.millust end
.pp
.us parminfo
is the same object named in the call to
.mkw va_start
..ct ..li .
.us type
is the type of argument expected.
The types expected should only be those that result from the default
argument promotions (
..ct .kw int
..ct ,
.kw long int
and
.kw long long int
and their unsigned varieties,
.kw double
and
.kw long double
..ct ),
and those that are not subject to promotion
(pointers, structures and unions).
The type must be determined
by the program.
The
.mkw va_arg
macro expands to an expression that has the type and value of the
next parameter in the variable list.
.pp
In the case of
.libfn printf
..ct ,
the parameter type expected is determined by the "conversion
specifications" such as
.mono %s
..ct ,
.mono %d
and so on.
.pp
The first invocation of the
.mkw va_arg
macro (after executing a
.mkw va_start
..ct ) returns the value of the parameter following
.us lastparm
(as specified in
.mkw va_start
..ct ). Each subsequent invocation of
.mkw va_arg
returns the next parameter in the list.
At each invocation, the value of
.us parminfo
is modified (in some implementation-specific manner)
to reflect the processing of the parameter list.
.pp
If the type of the next parameter does not match
.us type
..ct ,
or if no parameter was specified, the behavior is undefined.
.pp
The macro
.ix 'va_end'
.ix 'macro' 'variable argument' 'va_end'
.mkw va_end
has the form,
.millust begin
void va_end( va_list :ITAL.parminfo:eITAL. );
.millust end
.pp
.us parminfo
is the same object named in the corresponding call to
.mkw va_start
..ct ..li .
The function
.mkw va_end
closes off processing of the variable argument list, which must be
done prior to returning from the function. If
.mkw va_end
is not called before returning, the behavior is undefined.
.pp
If
.mkw va_end
is called without a corresponding call to
.mkw va_start
having been done, the behavior is undefined.
.pp
After calling
.mkw va_end
and prior to returning, it is possible to call
.mkw va_start
again and reprocess the variable list. It will be necessary to
call
.mkw va_end
again before returning.
.pp
The following function takes an arbitrary number of floating-point
numbers as parameters along with a count, and returns the
average of the numbers:
.millust begin
#include <stdarg.h>

extern double Average( int count, ... )
/*************************************/
{
    double  sum = 0;
    int     i;
    va_list parminfo;

    if( count == 0 ) {
        return( 0.0 );
    }
    va_start( parminfo, count );
    for( i = 0; i < count; i++ ) {
        sum += va_arg( parminfo, double );
    }
    va_end( parminfo );
    return( sum / count );
}
.millust end
.endlevel
.*
.section The Parameters to the Function main
.*
.pp
The function
.ix 'function' 'main'
.ix 'main'
.ix 'entry point'
.ix 'main' 'parameters to'
.mono main
has a special meaning in C.
It is the function that receives control
when a program is started.
The function
.mono main
has the following definition:
.millust begin
extern int main( int argc, char * argv[] )
{
    :ITAL.statements:eITAL.
}
.millust end
.pc
The objects
.ix 'argc'
.ix 'parameter' 'to main' 'argc'
.mono argc
and
.ix 'argv'
.ix 'parameter' 'to main' 'argv'
.mono argv
have the following properties:
.begbull
.bull
.mono argc
is the "argument count", or the number of parameters
(including program name)
supplied to the program, and its value is greater than zero,
.bull
.mono argv
is an array of pointers to strings containing the parameters,
.bull
.mono argv[0]
is the program name, if available, otherwise it is a pointer to
a string containing only the null character,
.bull
.mono argv[argc]
is a null pointer,
representing the end of the argument list,
.bull
.mono argv[1]
through
.mono argv[argc-1]
are pointers to strings representing the arguments to the program.
These strings are modifiable by the program, and exist throughout
the execution of the program.
The strings will generally be in mixed (upper and lower)
case, although a system that
cannot provide mixed case argument strings will provide them in
lower case.
.endbull
.pc
The translation of the arguments to the program, as
provided by the operating system (often from the command-line used
to invoke the program),
into the strings contained in
.mono argv
..ct ,
is implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
each unquoted, blank-separated token
on the command line is made into a string that is an element of
.mono argv.
Quoted strings are maintained as one element without the quotes.
.pp
For example, the command line,
.millust pgm 2+ 1 tokens "one token"
will result in
.mono argc
having the value 5, and the elements of
.mono argv
being the strings
.mono "pgm"
..ct ,
.mono "2+"
..ct ,
.mono "1"
..ct ,
.mono "tokens"
and
.mono "one token".
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc., there are different ways in which command-line
parameters may be passed to the function
.mono main.
See the &userguide. for full details.
.shade end
..do end
.*
.************************************************************************
.*
.pp
The function
.mono main
may also be declared without any parameters, as,
.millust begin
extern int main( void )
{
    statements
}
.millust end
.pp
The return value of
.mono main
is an integer, usually representing a
.ix 'termination status'
.ix 'main' 'return value'
termination
status.
If no return value is specified (by using a
.kw return
statement with no expression or encountering the closing brace in the
function),
then the value returned is undefined.
.pp
The
.libfn exit
library function may be used to terminate the program at any point.
The value of the argument to
.libfn exit
is returned as if
.mono main
had returned the value.
