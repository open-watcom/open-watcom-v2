.chap *refid=cppmsgs &cmppname Diagnostic Messages
.*
.np
The following is a list of all warning and error messages produced by
the &cmppname compilers.
Diagnostic messages are issued during compilation and execution.
.np
The messages listed in the following sections contain references to
.id %N,
.id %S,
.id %T,
.id %s,
.id %d
and
.id %u.
They represent strings that are substituted by the &cmppname.
compilers to make the error message more exact.
.id %d
and
.id %u
represent a string of digits;
.id %N,
.id %S,
.id %T
and
.id %s
a string, usually a symbolic name.
.np
Consider the following program, named
.fi err.cpp
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
File: err.cpp
(6,12): Error! E042: symbol 'i' already defined
  'i' declared at: (5,9)
(9,5): Error! E029: symbol 'x' has not been declared
err.cpp: 12 lines, included 174, no warnings, 2 errors
.millust end
.pc
The diagnostic messages consist of the following information:
.autopoint
.point
the name of the file being compiled,
.point
the line number and column of the line containing the error (in parentheses),
.point
a message number, and
.point
text explaining the nature of the error.
.endpoint
.pc
In the above example, the first error occurred on line 6 of the file
.fi err.cpp.
Error number 042 (with the appropriate substitutions) was diagnosed.
The second error occurred on line 9 of the file
.fi err.cpp.
Error number 029 (with the appropriate substitutions) was diagnosed.
.np
.ix 'run-time' 'error messages'
The following sections contain a complete list of the messages.
Run-time messages (messages displayed during execution) do not have
message numbers associated with them.
.np
A number of messages contain a reference to the ARM.
This is the "Annotated C++ Reference Manual" written by
Margaret A. Ellis and Bjarne Stroustrup and published by
Addison-Wesley (ISBN 0-201-51459-1).
.*
.if &e'&dohelp eq 0 .do begin
.section Diagnostic Messages
.begnote
.do end
.ti set `
:INCLUDE file='MESSAGES'.
.ti set ~
.if &e'&dohelp eq 0 .do begin
.endnote
.do end
