.chap *refid=wjcmsgs Sybase Java Compiler Diagnostic Messages
.*
.np
The following is a list of all warning and error messages produced by
the Java compiler.
Diagnostic messages are issued during compilation and execution.
.np
The messages listed in the following sections contain references to
.id %C,
.id %D,
.id %L,
.id %P,
.id %R,
.id %S,
.id %T,
.id %c,
.id %d,
.id %f,
.id %i,
.id %s
and
.id %u.
They represent strings that are substituted by the Java compiler to
make the error message more exact.
.id %c
represents a Unicode character;
.id %d
and
.id %u
represent a string of digits;
.id %f
represents a filename;
.id %i
represents a Java identifier (\uxxxx is used for Unicode chars);
.id %s
represents a string;
.id %C
represents a short time interval (seconds or milliseconds);
.id %D
represents a decorated symbol (including notes for location
information);
.id %L
represents a token location (source file, line number, and column);
.id %P
represents a symbol name (no location information is included);
.id %R
represents a list of types (<type1>, <type2>, etc.);
.id %S
represents a symbol name (including notes for type and location
information);
and
.id %T
represents a Java type.
.np
Consider the following code fragment with diagnostics (both error and
informational) appearing before the source line.
.exam begin
x cannot access member java.awt.Component.x (default access).
x number of arguments do not match those in prototype(s).
- argument types: 'traverse(int,int,int,boolean,boolean)'.
- rejected candidate: 'boolean powersoft.powerj.ui.grid.
                       Grid.traverse(int,int,boolean,boolean)'.
- symbol defined in: powersoft/powerj/ui/grid/Grid.class.
        grid_1.traverse( x, i/4, i%4, false, false );
.exam end
.np
The diagnostics include the use of the following messages.
.autonote
.note
cannot access member %P (default access)
.note
number of arguments do not match those in prototype(s)
.note
argument types: '%i%R'
.note
rejected candidate: '%D'
.note
symbol defined %L
.endnote
.np
Note how information has been substituted for the %? place holders.
.np
The following sections contain a complete list of the messages.
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
