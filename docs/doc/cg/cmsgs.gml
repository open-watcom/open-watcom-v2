.chap *refid=cmsgs &cmpcname Diagnostic Messages
.*
.np
.ix 'warning messages'
.ix 'error messages'
The following is a list of all warning and error messages produced by
the &cmpcname compilers.
Diagnostic messages are issued during compilation and execution.
.np
The messages listed in the following sections contain references to
.id %s,
.id %d
and
.id %u.
They represent strings that are substituted by the &cmpcname.
compilers to make the error message more exact.
.id %d
and
.id %u
represent a string of digits;
.id %s
a string, usually a symbolic name.
.np
Consider the following program, named
.fi err.&langsuff.
.ct , which contains errors.
.exam begin
#include <stdio.h>

void main()
  {
    int i;
    float i;

    i = 383;
    x = 13143.0;
    printf( "Integer value is %d\n", i );
    printf( "Floating-point value is %f\n", x );
  }
.exam end
.pc
If we compile the above program, the following messages will appear on
the screen.
.millust begin
err.c(6): Error! E1034: Symbol 'i' already defined
err.c(9): Error! E1011: Symbol 'x' has not been declared
err.c: 12 lines, included 191, 0 warnings, 2 errors
.millust end
.pc
The diagnostic messages consist of the following information:
.autopoint
.point
the name of the file being compiled,
.point
the line number of the line containing the error (in parentheses),
.point
a message number, and
.point
text explaining the nature of the error.
.endpoint
.pc
In the above example, the first error occurred on line 6 of the file
.fi err.&langsuff..
Error number 1034 (with the appropriate substitutions) was
diagnosed.
The second error occurred on line 9 of the file
.fi err.&langsuff..
Error number 1011 (with the appropriate substitutions) was
diagnosed.
.np
.ix 'run-time' 'error messages'
The following sections contain a complete list of the messages.
Run-time messages (messages displayed during execution) do not have
message numbers associated with them.
:INCLUDE file='CERRS'.

