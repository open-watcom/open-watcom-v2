.chap *refid=fcond &product Compiler Directives
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'conditional compilation'
A number of compiler directives are available that allow, for example,
conditional compilation of source code and the inclusion of source code
from other files.
A compiler directive is specified by placing
a comment character ('c', 'C', or '*') in column one followed by a dollar
sign ('$') immediately followed by the compiler directive.
The following lists all of the compiler directives available with
&cmpname..
.autopoint
.if '&cmpclass' eq 'load-n-go' .do begin
.point
DATA
.do end
.point
EJECT
.point
INCLUDE
.point
PRAGMA
.point
DEFINE
.point
UNDEFINE
.point
IFDEF
.point
IFNDEF
.point
ENDIF
.point
ELSE
.point
ELIFDEF
.point
ELIFNDEF
.endpoint
.np
These compiler directives will be described in the following sections.
.np
In addition to the above compiler directives, it is also possible to specify
certain compiler options in the same way.
The following lists these options.
.autopoint
.point
[NO]EXTENSIONS
.point
[NO]LIST
.point
[NO]REFERENCE
.point
[NO]WARNINGS
.if '&cmpclass' eq 'load-n-go' .do begin
.point
[NO]ARRAYCHECK
.point
[NO]CHECK
.point
PAGES=
.point
STATEMENTS=
.point
TIME=
.do end
.endpoint
.np
For more information on these options, see the the chapter
entitled :HDREF refid='fpopts'..
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.section The DATA Compiler Directive
.*
.np
.ix 'compiler directives' 'data'
.ix 'DATA compiler directive'
This compiler directive is used to indicate the end of FORTRAN source input
and the start of data records to be read by the FORTRAN program.
Unit 5 becomes preconnected to the source
input stream following the line containing the DATA directive.
Connection of unit 5 to the source input stream using the DATA
directive may be overridden by a preconnection specification or a
FORTRAN
.kw OPEN
statement.
.np
This compiler directive allows data for the program to be included along with
the source code.
It must not be specified:
.autopoint
.point
in a file which has been included using the INCLUDE compiler directive.
.point
in a file which has been included from a source library.
.point
in an editor buffer.
.endpoint
.pc
No FORTRAN source or compiler directives may follow the DATA compiler
directive.
.np
The following example illustrates the use of the DATA compiler directive.
.exam begin
      INTEGER I, VALUE(5)
      READ *, VALUE
      DO I = 1, 5
          PRINT *, VALUE(I), VALUE(I)**2
      END DO
      END
*$DATA
3, 45, 23, 12, 4
.exam end
.*
.do end
.*
.section The EJECT Compiler Directive
.*
.np
.ix 'compiler directives' 'eject'
.ix 'EJECT compiler directive'
This compiler directive causes a form-feed to be generated in the listing
file.
The listing file is a carriage-control file that is created by the compiler
when the "list" compiler option is specified.
In the following example, a form-feed character will be generated
immediately before the source for subroutine
.id sub2
and immediately before the source for subroutine
.id sub3.
.exam begin
    subroutine sub1
    ! source code
    end
*$eject
    subroutine sub2
    ! source code
    end
*$eject
    subroutine sub3
    ! source code
    end
.exam end
.*
.section *refid=fcondi The INCLUDE Compiler Directive
.*
.np
.ix 'compiler directives' 'include'
.ix 'INCLUDE compiler directive'
.im include
.*
.section The PRAGMA Compiler Directive
.*
.np
.ix 'compiler directives' 'pragma'
.ix 'PRAGMA compiler directive'
This compiler directive is described in the chapters
entitled :HDREF refid='prg86'. and :HDREF refid='prg386'..
.*
.section The DEFINE Compiler Directive
.*
.np
.ix 'compiler directives' 'define'
.ix 'DEFINE compiler directive'
The DEFINE compiler directive sets the definition status of a
.us macro
to defined.
If a macro does not appear in a DEFINE directive, its definition status
is undefined.
.exam begin
*$define debug
.exam end
.np
In the above example, the macro
.id debug
is defined.
.np
The DEFINE compiler option can also be used to define a macro.
.ix 'options' 'DEFine=<macro>'
.exam begin
&prompt.&ccmd16 /define=debug test
&prompt.&ccmd32 /define=debug test
.exam end
.*
.section The UNDEFINE Compiler Directive
.*
.np
.ix 'compiler directives' 'undefine'
.ix 'UNDEFINE compiler directive'
The UNDEFINE compiler directive sets the definition status of a
.us macro
to undefined.
.exam begin
*$undefine debug
.exam end
.np
In the above example, the definition status of the macro
.id debug
is set to undefined.
.*
.section The IFDEF, IFNDEF and ENDIF Compiler Directive
.*
.np
.ix 'compiler directives' 'ifdef'
.ix 'compiler directives' 'ifndef'
.ix 'compiler directives' 'endif'
.ix 'IFDEF compiler directive'
.ix 'IFNDEF compiler directive'
.ix 'ENDIF compiler directive'
The IFDEF and IFNDEF compiler directives check the definition status of a
macro.
If the macro appearing in an IFDEF directive is defined or the macro appearing
in an IFNDEF directive is not defined, then all source code up to
the corresponding ENDIF compiler directive will be compiled.
Otherwise, it will be ignored.
.np
In the following example, the FORTRAN 77 statements represented by
.id <debugging_statements>
will be compiled.
.exam begin
*$define debug
    ...
*$ifdef debug
    <debugging_statements>
*$endif
.exam end
.np
In the following example, the FORTRAN 77 statements represented by
.id <debugging_statements>
will not be compiled.
.exam begin
*$undefine debug
    ...
*$ifdef debug
    <debugging_statements>
*$endif
.exam end
.np
In the following example, the FORTRAN 77 statements represented by
.id <debugging_statements>
will be compiled.
.exam begin
*$undefine debug
    ...
*$ifndef debug
    <debugging statements>
*$endif
.exam end
.*
.section The ELSE Compiler Directive
.*
.np
.ix 'compiler directives' 'else'
.ix 'ELSE compiler directive'
The ELSE compiler directive must be preceded by an
IFDEF, IFNDEF, ELSEIFDEF or ELSEIFNDEF compiler directive.
If the condition of the preceding compiler directive was satisfied,
then all source code between the ELSE compiler directive
and the corresponding ENDIF compiler directive will be ignored.
If the condition of the preceding compiler directive was not
satisfied, then all source code between the ELSE compiler directive
and the corresponding ENDIF compiler directive will be compiled.
.np
In the following example, the FORTRAN 77 statements represented by
.id <debugging_level_2_statements>
will be compiled.
.exam begin
*$undefine debug_level_1
    ...
*$ifdef debug_level_1
    <debugging_level_1_statements>
*$else
    <debugging_level_2_statements>
*$endif
.exam end
.*
.section The ELSEIFDEF and ELSEIFNDEF Compiler Directive
.*
.np
.ix 'compiler directives' 'elseifdef'
.ix 'compiler directives' 'elseifndef'
.ix 'ELSEIFDEF compiler directive'
.ix 'ELSEIFNDEF compiler directive'
The ELSEIFDEF and ELSEIFNDEF compiler directives must be preceded by an
IFDEF, IFNDEF, ELSEIFDEF or ELSEIFNDEF compiler directive.
If the condition of the preceding compiler directive was satisfied,
then all source code between the ELSEIFDEF or ELSEIFNDEF compiler directive
and the corresponding ENDIF compiler directive will be ignored.
If the condition of the preceding compiler directive was not
satisfied, then the definition status of the macro specified in the ELSEIFDEF
or ELSEIFNDEF compiler directive is checked.
If the macro appearing in the ELSEIFDEF compiler directive is defined, or the
macro appearing in the ELSEIFNDEF compiler directive is not defined, then all
source up to the next ELSEIFDEF, ELSEIFNDEF, ELSE or ENDIF compiler
directive will be compiled.
.np
In the following example, the FORTRAN 77 statements represented by
.id <debugging_level_2_statements>
will be compiled.
.exam begin
*$define debug_level_2
    ...
*$ifdef debug_level_1
    <debugging_level_1_statements>
*$elseifdef debug_level_2
    <debugging_level_2_statements>
*$endif
.exam end
.*
.section Debugging statements ("D" in Column 1)
.*
.np
.ix 'debugging statements'
.ix 'D in column 1'
.ix 'debugging macro' '__debug__'
If the character "D" or "d" appears in column 1, that line will be
conditionally compiled depending on the definition status of the macro
.mono __debug__.
Statements that contain a "D" or "d" in column 1 are called debugging
statements.
If the
.mono __debug__
macro is defined, the line will be compiled; otherwise it will be
ignored.
The
.mono __debug__
macro can be defined by using the DEFINE compiler directive or the
"define" option.
In the following example, the "define" option is used to force compilation
of debugging statements.
.exam begin
&prompt.&ccmd16 /def=__debug__ test
&prompt.&ccmd32 /def=__debug__ test
.exam end
.*
.section General Notes About Compiler Directives
.*
.autonote
.note
Compiler directives must not contain embedded blanks.
The following is not a valid ENDIF compiler directive.
.exam begin
*$end if
.exam end
.pc
.note
Nesting is allowed up to a maximum of 16 levels.
.exam begin
*$ifdef sym1
    <statements>
*$ifdef sym2
    <statements>
*$endif
*$endif
.exam end
.pc
.note
.ix 'predefined macros' '__i86__'
The macro
.mono __i86__
is a special macro that is defined by the compiler and identifies the
target as a 16-bit Intel 80x86 compatible environment.
.note
.ix 'predefined macros' '__386__'
The macro
.mono __386__
is a special macro that is defined by the compiler and identifies the
target as a 32-bit Intel 80386 compatible environment.
.note
The macro
.ix 'predefined macros' '__stack_conventions__'
.mono __stack_conventions__
is a special macro that is defined by the 32-bit compiler when stack
conventions are used for code generation.
Stack conventions are used when the "sc" or "3s" compiler options are
specified.
.note
The macro
.ix 'predefined macros' '__fpi__'
.mono __fpi__
is a special macro that is defined
.if '&cmpclass' = 'load-n-go' .do begin
when using &c2cmdup16. or &c2cmdup32..
.do end
.el .do begin
by the compiler when one of the following floating-point options is
specified: "fpi" or "fpi87".
.do end
.note
.ix 'debugging macro' '__debug__'
The macro
.mono __debug__
is a special macro that can be used to conditionally compile debugging
statements.
A debugging statement is one that contains the character "D" or "d" in
column one.
.endnote
