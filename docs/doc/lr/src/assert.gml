.func assert _assert _wassert
#include <assert.h>
void assert( int expression );
.funcend
.desc begin
The &func macro prints a diagnostic message upon the
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
The &func macro is typically used during program development to
identify program logic errors.
The given
.arg expression
should be chosen so that it is true when the program is functioning
as intended.
.if '&machsys' eq 'PP' .do begin
Two variations of the &func macro are available.
When the "za" &company C compiler option is specified, the ANSI C
version of &func is obtained.
When the "za" option is not specified, the non-standard PenPoint
version of &func is obtained.
.np
.us ANSI C:
.do end
After the program has been debugged, the special "no debug" identifier
.kw NDEBUG
can be used to remove &func calls from the program when it is
re-compiled.
If
.kw NDEBUG
is defined (with any value) with a
.id -d
command line option or with a
.id #define
directive, the C preprocessor ignores all &func calls in the program
source.
.if '&machsys' eq 'PP' .do begin
.np
.us PenPoint:
While the program is being debugged, the special "debug" identifier
.kw DEBUG
can be used to include &func calls in the program when it is compiled.
If
.kw DEBUG
is defined (with any value) with a
.id -d
command line option or with a
.id #define
directive, the C preprocessor includes all &func calls in the program
source.
.do end
.desc end
.return begin
The &func macro does not return a value.
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
