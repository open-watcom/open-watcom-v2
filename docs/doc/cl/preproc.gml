.ix 'preprocessor'
.pp
The
.us preprocessor,
as its name suggests,
is that part of the C compiler which processes certain directives
embedded in the source file(s) in advance of the actual compilation
of the program. Specifically, the preprocessor allows a source file
to,
.begbull
.bull
include other files
(perhaps referencing externally-defined objects, or
containing the definitions of structures or other types which are
needed by more than one source file),
.bull
compile certain portions of the code depending on
some condition (such as the kind of computer for which the code is
being generated), and,
.bull
replace
.us macros
with other text which is then compiled.
.endbull
.pp
The preprocessing phase occurs after
.ix 'trigraphs'
trigraphs have been converted
and
physical lines ending with
.ix 'continuation lines'
.ix 'line' 'continuation'
.ix 'line' 'physical'
.ix 'line' 'logical'
.mono \
have been concatenated to create
longer logical lines,
but before
.ix 'escape sequences'
escape sequences in character constants have been converted,
or adjacent string literals are concatenated.
.pp
Any line whose first non-blank character is a
.mono #
marks the beginning of a
.us preprocessing directive.
Spaces may appear between the
.mono #
and the identifier for the
directive.
The
.kwpp #include
and
.kwpp #define
directives are each contained on one
line (after concatenation of lines ending with
.mono \
..ct ),
while the
conditional compilation directives span multiple lines.
.pp
A preprocessor directive is not terminated by a semi-colon.
.*
.section The Null Directive
.*
.pp
A preprocessing directive of the form,
.ppix #
.ppix null
.millust #
(with no other tokens on the same line)
has no effect and is discarded.
.*
.section Including Headers and Source Files
.*
.pp
A directive of the form,
.ix 'include'
.ppix #include
.millust #include <name>
will search a sequence of places defined by the implementation for the
.ix 'header'
.ix 'header' 'including'
.us header
identified by
.mono name.
A header
declares a set of
.ix 'library function'
library functions and any necessary types or macros
needed for their use.
Headers are usually provided by the compiler, or by a library
provided for use with the compiler.
.pp
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.mono name
may not contain a
.mono >
character.
..do end
..el ..if '&target' eq 'generic' ..th ..do begin
.mono name
may not contain a >
character, and if any of the characters ' (quote),
" (double-quote) or /* (start comment) occur in
.mono name
..ct ,
the behavior
is undefined.
..do end
.*
.************************************************************************
.*
If the header is found, the entire directive is replaced by the contents
of the header.
If the header is not found, an error will occur.
.pp
A directive of the form,
.millust #include "name"
will search for the source file identified by
.mono name.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.mono name
may not contain a
.mono "
(double-quote) character.
..do end
..el ..if '&target' eq 'generic' ..th ..do begin
The
.mono name
may not contain a " (double-quote) character, and if any
of the characters ' (quote), \ (backslash), or /* (start comment)
occur in
.mono name
..ct ,
the behavior is undefined.
..do end
.*
.************************************************************************
.*
If the source file identified by
.mono name
is found, then the entire directive is replaced by the contents of
the file.
Otherwise, the directive is processed as if the,
.millust #include <name>
form had been used.
.pp
A third form of
.kwpp #include
directive is also supported.
A directive of the form,
.millust #include tokens
causes all macro substitutions (described below) to take place on
.mono tokens.
After substitution,
the directive
must match either the
.mono <name>
or
.mono "name"
forms described above (including
.mono <
and
.mono >
..ct ,
or quotes),
in which case the
.kwpp #include
is processed in the corresponding manner.
.pp
See the &userguide. for details about how the compiler searches
for included files.
.pp
.kwpp #include
directives may be
.ix 'include' 'nested'
.ix 'nesting' 'include'
nested.
Each implementation may allow different depths of nesting, but
all must allow at least 8 levels.
(In other words, a source file
may include another file, which includes another file, and so on, up to
a depth of eight files.)
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The operating system
may further limit the number of files that may be open at one time.
See the appropriate operating system manual for details.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
&wlooc. does
not impose any limit on the level of nesting of include
files.
The operating system
may limit the number of files that may be open at one time.
See the appropriate operating system manual for details.
.shade end
..do end
.*
.************************************************************************
.*
.section Conditionally Including Source Lines
.*
.pp
A directive of the form,
.ix 'conditional compilation'
.ppix #if
.cillust begin
.kwpp #if
constant-expression
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.body of
.kwpp #if
.cbr
.kwpp #endif
.cillust end
.pc
evaluates the
.us constant-expression,
and if it evaluates to a non-zero value, then the body of the
.kwpp #if
is processed by the preprocessor.
Processing of the body ends when a corresponding
.kwpp #elif
..ct ,
.kwpp #else
..ct ,
or the terminating
.kwpp #endif
is encountered.
.pp
The
.kwpp #if
directive allows source and preprocessor lines to be
conditionally processed by the compiler.
.pp
If the
.us constant-expression
evaluates to zero, then the body of the
.kwpp #if
is not processed, and the corresponding
.kwpp #elif
or
.kwpp #else
(if present) is processed.
If neither of these directives are present, then the preprocessor
skips to the
.kwpp #endif
..ct ..li .
Any preprocessing directives within the
body of the
.kwpp #if
are not processed, but they are examined in order to determine
any nested directives, in order to find the matching
.kwpp #elif
..ct ,
.kwpp #else
or
.kwpp #endif
..ct ..li .
.pp
The
.us constant-expression
is of the same form as used in
the
.kw if
statement,
.* actual C source,
except that the
values used must be integer values (including character constants).
No cast or
.kw sizeof
operators or
enumeration constants may be used.
Each identifier that is a macro name is replaced (as described below),
and remaining identifiers
are replaced with
.mono 0L
..ct ..li .
All values are converted to long integers
using the usual arithmetic conversions.
After each item has been converted, the evaluation of the expression
takes place using the arithmetic of the translation environment.
Any character constants are evaluated as members of the
.us source
character set.
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
.shade begin
With &wcboth., character constants have the same value in both the
source and execution character sets.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcall.,
character constants have the same value in the
source and execution character sets.
.shade end
..do end
..el ..if '&target' eq 'generic' ..th ..do begin
For example, consider a C compiler running on an IBM mainframe computer
(with the
.ix 'character set' 'EBCDIC'
.ix 'EBCDIC character set'
EBCDIC character set)
but producing code for a micro-computer that uses the
.ix 'character set' 'ASCII'
.ix 'ASCII character set'
ASCII character set.
The constant expression
.mono 'a'
would evaluate to hexadecimal 81 in a constant expression that is
part of a
.kwpp #if
preprocessor directive, but would evaluate to hexadecimal 61
in an expression that is part of a C statement.
..do end
.*
.************************************************************************
.*
.pp
The unary expression,
.cillust begin
.mono defined
identifier
.cor
.mono defined(
identifier
.mono )
.cillust end
.pc
may be used to determine if an identifier is currently defined as
a macro.
Any macro name that is part of this
unary expression is not expanded.
The above expressions evaluate to 1 if the named identifier
is currently a macro, otherwise they evaluate to 0.
.pp
As discussed above, if the
.us constant-expression
of the
.kwpp #if
evaluates to zero,
the preprocessor looks for a corresponding
.kwpp #elif
..ct ..li .
This directive means "else if", and has a similar form as
.kwpp #if
..ct :
.ppix #elif
.cillust begin
.kwpp #elif
constant-expression
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.body of
.kwpp #elif
.cillust end
.pc
An
.kwpp #elif
may only be placed inside the body of an
.kwpp #if
..ct ..li .
The body of the
.kwpp #elif
is processed only if the
.us constant-expression
evaluates to a non-zero value
and the constant-expressions of the corresponding
.kwpp #if
and (preceding)
.kwpp #elif
statements evaluated to zero.
Otherwise the body is not processed, and the preprocessor
skips to the next corresponding
.kwpp #elif
or
.kwpp #else
..ct ,
or to the
.kwpp #endif
if neither of these directives is present.
.pp
The
.kwpp #else
directive has the form,
.ppix #else
.cillust begin
.kwpp #else
..br
&SYSRB.&SYSRB.&SYSRB.&SYSRB.body&SYSRB.of
.kwpp #else
.cillust end
.pc
The body of the
.kwpp #else
is processed only if the constant expressions of the corresponding
.kwpp #if
and
.kwpp #elif
statements
evaluated to zero.
The body of the
.kwpp #else
is processed until the corresponding
.kwpp #endif
is encountered.
.pp
The form of the
.kwpp #endif
directive is,
.millust #endif
and marks the end of the
.kwpp #if
..ct ..li .
.pp
The following are examples of conditional inclusion of source lines:
.millust begin
#if OPSYS == OS_CMS
    fn_syntax = "filename filetype fm";
#elif OPSYS == OS_MVS
    fn_syntax = "'userid.library.type(membername)'";
#elif OPSYS == OS_DOS  ||  OPSYS == OS_OS2
    fn_syntax = "filename.ext";
#else
    fn_syntax = "filename";
#endif
.millust end
.pc
The object
.mono fn_syntax
is set to the appropriate filename syntax string depending on the
value of the macro
.mono OPSYS.
If
.mono OPSYS
does not match any of the stated values, then
.mono fn_syntax
is set to the default string
.mono "filename".
.millust begin
#if HARDWARE == HW_IBM370
    #if OPSYS == OS_CMS
        escape_cmd = "CMS";
    #elif OPSYS == OS_MVS
        escape_cmd = "TSO";
    #else
        escape_cmd = "SYSTEM";
    #endif
#else
    escape_cmd = "SYSTEM";
#endif
.millust end
.pc
The object
.mono escape_cmd
is set to an appropriate string depending on the values of the macros
.mono HARDWARE
and
.mono OPSYS.
The indentation of the directives clearly illustrates the
flow between various conditions and levels of directives.
.beglevel
.*
.section The #ifdef and #ifndef Directives
.*
.pp
The
.ppix #ifdef
.kwpp #ifdef
directive is used to check if an identifier is currently defined as
a macro. For example, the directive,
.millust #ifdef xyz
processes the body of the
.kwpp #ifdef
only if the identifier
.mono xyz
is currently a macro.
This example is equivalent to,
.millust #if defined xyz
or
.millust #if defined( xyz )
.pp
In a similar manner, the directive,
.ppix #ifndef
.millust #ifndef xyz
is equivalent to,
.millust #if !defined xyz
or
.millust #if !defined( xyz )
.endlevel
