.section Preprocessor Directives
.*
.pp
The C preprocessor
recognizes the following directives:
.ix 'include'
.ppix #include
.millust #include
Include the named header, substituting the directive with
the contents of the header.
..sk 1 c
If the header name is of the form
.mono <headername>
..ct ,
then the preprocessor checks a set of implementation-defined locations
for the header. These locations may or may not be actual files.
..sk 1 c
If the header name is of the form
.mono "headername"
..ct ,
then the preprocessor looks for the file named within the quotes.
If it is not found, then the preprocessor treats the header name as if
it had been specified as
.mono <headername>.
.pp
Otherwise,
macro substitutions are performed on the tokens following the
.kwpp #include
..ct ..li .
After substitution,
the directive
must match either the
.ppfont <headername>
or
.ppfont "headername"
forms described above (including < and >, or quotes),
in which case the
.kwpp #include
is processed in the corresponding manner.
.pp
See the &userguide. for details about how the compiler
searches for included files.
.*
.ppix #define
.ix 'macro' 'defining'
.millust #define
Create the macro definition for the name following the directive, using
the tokens following the macro name. The definition ends at the end
of the line, which may be
.ix 'continuation lines'
extended by ending the source line with a
backslash (\) character.
..sk 1 c
In later parts of the source code, invocations of the macro are
replaced using the macro definition.
.*
.ppix #undef
.ix 'macro' 'undefining'
.ix 'undefining a macro'
.millust #undef
The macro named following the directive is discarded.
.*
.ppix #if
.ix 'conditional compilation'
.millust #if
If the condition following the directive evaluates to a non-zero
value, then the source lines up to the next corresponding
.kwpp #elif
..ct ,
.kwpp #else
or
.kwpp #endif
are included and analyzed. If the condition evaluates to zero, then
the lines are not included.
.*
.ppix #elif
.millust #elif
Valid only within the body of a
.kwpp #if
directive. If all previous corresponding
.kwpp #if
and
.kwpp #elif
directive conditions evaluated to zero, then
the condition following the directive is evaluated, and a non-zero
value causes the source lines up to the next corresponding
.kwpp #elif
..ct ,
.kwpp #else
or
.kwpp #endif
to be included and analyzed. If the condition evaluates to zero, then
the lines are not included.
.*
.ppix #else
.millust #else
Valid only within the body of a
.kwpp #if
directive. If all previous corresponding
.kwpp #if
and
.kwpp #elif
directive conditions evaluated to zero, then the source lines following
the
.kwpp #else
directive, up to the corresponding
.kwpp #endif
directive, are included and analyzed. If any of the previous
.kwpp #if
and
.kwpp #elif
conditions evaluated to a non-zero value, then the source lines
following the
.kwpp #else
are not included.
.*
.ppix #endif
.millust #endif
This directive terminates a
.kwpp #if
directive.
.*
.ppix #ifdef
.millust #ifdef
This directive determines if the macro named in the directive is
currently defined. If so, the source lines following the directive, up
to the corresponding
.kwpp #elif
..ct ,
.kwpp #else
or
.kwpp #endif
directive, are included and analyzed. Otherwise, the macro is not
defined and the source lines are not included.
.*
.ppix #ifndef
.millust #ifndef
This directive determines if the macro named in the directive is
currently defined. If not, the source lines following the directive, up
to the next corresponding
.kwpp #elif
..ct ,
.kwpp #else
or
.kwpp #endif
directive, are included and analyzed. Otherwise, the macro is defined
and the source lines are not included.
.*
.ppix #line
.millust #line
The digit sequence following the directive (on the same line)
is used to set a new
current line number, overriding the one that the compiler determines
as it reads lines from the source file(s). If a string literal is
given following the new line number (on the same line),
then the name that the compiler
remembers as being the name of the source file currently being read is
changed to the one contained in the string literal.
.*
.ppix #error
.millust #error
The tokens following the directive (on the same line)
are produced in a diagnostic message
from the compiler.
.*
.ppix #pragma
.millust #pragma
The tokens following the directive (on the same line)
are used by the compiler in an implementation-defined manner.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.pp
See the appropriate user's guide for details on how the compiler
uses the
.kwpp #pragma
directive.
..do end
.*
.************************************************************************
.*
.pp
Any
pragma not recognized by the compiler is ignored.
.*
.ppix #
.ppix null
.millust #
This directive has no effect.
.*
.section Predefined Macro Names
.*
.ix 'macro' 'predefined'
.ix 'predefined macro'
.pp
The following macro names are reserved by the compiler:
..im defmacro
..im sysmacro
