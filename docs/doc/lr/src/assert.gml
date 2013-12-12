.func assert _assert _wassert
.synop begin
#include <assert.h>
void assert( int expression );
.synop end
.desc begin
The
.id &func.
macro prints a diagnostic message upon the
.kw stderr
stream and terminates the program if
.arg expression
is false (0).
The diagnostic message has the form
.np
.mono Assertion failed:
.arg expression
.ct ,
.mono file
.arg filename
.ct ,
.mono line
.arg linenumber
.np
.pc
where
.arg filename
is the name of the source file and
.arg linenumber
is the line number of the assertion that failed in the source file.
.arg Filename
and
.arg linenumber
are the values of the preprocessing macros
.kw __FILE__
and
.kw __LINE__
respectively.
No action is taken if
.arg expression
is true (non-zero).
.np
The
.id &func.
macro is typically used during program development to
identify program logic errors.
The given
.arg expression
should be chosen so that it is true when the program is functioning
as intended.
After the program has been debugged, the special "no debug" identifier
.kw NDEBUG
can be used to remove
.id &func.
calls from the program when it is
re-compiled.
If
.kw NDEBUG
is defined (with any value) with a
.id -d
command line option or with a
.id #define
directive, the C preprocessor ignores all
.id &func.
calls in the program
source.
.desc end
.return begin
The
.id &func.
macro does not return a value.
:cmt. Because the function uses the
:cmt. .kw fprintf
:cmt. function to display errors, the
:cmt. .kw errno
:cmt. global variable can be set when an output error occurs.
.return end
.exmp begin
#include <stdio.h>
#include <assert.h>

void process_string( char *string )
  {
    /* use assert to check argument */
    assert( string != NULL );
    assert( *string != '\0' );
    /* rest of code follows here */
  }
.exmp break
void main()
  {
    process_string( "hello" );
    process_string( "" );
  }
.exmp end
.class ANSI
.system
