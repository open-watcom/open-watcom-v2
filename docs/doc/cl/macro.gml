.pp
A directive of the form,
.ppix #define
.cillust begin
.mono #define
identifier replacement-list
.cillust end
.pc
defines a
.ix 'macro' 'defining'
.us macro
with the name
.us identifier.
This particular form of macro is called an
.ix 'macro' 'object-like'
.us object-like
macro, because it is used like an object (as opposed to a function).
Any source line that contains a token matching the macro name has
that token replaced by the
.us replacement-list.
The tokens of the replacement-list are then rescanned for more
macro replacements.
.pp
For example, the macro,
.millust #define TABLE_LIMIT 256
defines the macro
.mono TABLE_LIMIT
to be equivalent to the token
.mono 256.
This is sometimes called a
.ix 'manifest constant'
.ix 'constant' 'manifest'
.ix 'constant' '#define'
.ix 'constant' 'enumeration'
.us manifest constant,
because it provides
a descriptive term for a value that makes programs easier to
read.
It is a very good idea to use descriptive names wherever appropriate
to improve the readability of a program.
It may also save time if the same value is used many different places,
and the value must be changed at some point.
.pp
Care must be exercised when using more complicated object-like macros.
Consider the following example:
.millust begin
#define COUNT1 10
#define COUNT2 20
#define TOTAL_COUNT COUNT1+COUNT2
/* ... */
memptr = malloc( TOTAL_COUNT * sizeof( int ) );
.millust end
.pc
If
.kw int
is 2 bytes in size,
this call to
.libfn malloc
will allocate 50 bytes of memory, instead of the expected 60.
This occurs because
.mono TOTAL_COUNT&SYSRB.*&SYSRB.sizeof(&SYSRB.int&SYSRB.)
becomes
.mono 10+20&SYSRB.*&SYSRB.2
after macro replacement, and the precedence rules for expression
evaluation cause the multiply to be done first. To solve this
problem, the macro for
.mono TOTAL_COUNT
should be defined as:
.millust #define TOTAL_COUNT (COUNT1+COUNT2)
.pp
A directive of the form,
.ppix #define
.cillust begin
.kwpp #define
identifier
..ct .mono (
identifier-list
.mono )
replacement-list
.cillust end
.pc
is called a
.ix 'macro' 'function-like'
.us function-like
macro, because it is used like a function call.
No space may appear between
.us identifier
and the left parenthesis in the macro definition.
Any source line(s) that contains what looks like a function call, where
the name of the function matches a function-like macro name, and the
number of parameters matches the number of identifiers in the
.us identifier-list,
has the entire function call replaced by the
.us replacement-list,
substituting the actual arguments of the function call for the
occurrences of the identifiers in the replacement-list.
If the left parenthesis following the macro name was created
as the result of a macro substitution, no further substitution will
take place.
If the macro name appears but is not followed by a left parenthesis,
no further substitution will take place.
.keep begin
:P.
Consider this example:
.millust begin
#define endof( string ) \
    (string + strlen( string ))
.millust end
.keep end
.pc
The
.mono \
causes the two lines to be joined together into one logical
line, making this equivalent to,
.millust #define endof( string )    (string + strlen( string ))
The function-like macro
.mono endof
can be used to find a pointer to the null character terminating
a string. The statement,
.millust begin
endptr = endof( ptr );
.millust end
.pc
will have the macro replaced, so it will then be parsed as,
.millust begin
endptr = (ptr + strlen( ptr ));
.millust end
.pp
Note that, in this case, the argument is evaluated twice. If
.mono StrFn(&SYSRB.ptr&SYSRB.)
was specified instead of
.mono ptr
..ct ,
then the function would get called twice, because the substitution
would yield,
.millust begin
endptr = (StrFn( ptr ) + strlen( StrFn( ptr ) ));
.millust end
.pp
In gathering up the tokens used to identify the arguments,
each sequence of tokens separated by a comma constitutes an
argument, unless that comma happens to be within a matched pair of
left and right parentheses. When a right parenthesis is found that
matches the beginning left parenthesis, and the number of arguments
matches the number of identifiers in the macro definition, then
the gathering of the arguments is complete and the substitution
takes place.
.pp
For example,
.millust begin
#define mymemcpy( dest, src, len ) \
    memcpy( dest, src, len )
/* ... */
mymemcpy( destptr, srcptr, (t=0, t=strlen(srcptr)) );
.millust end
.pc
will, for the parameters
.mono dest
..ct ,
.mono src
and
.mono len
..ct ,
use the arguments
.mono destptr
..ct ,
.mono srcptr
and
.monoon
(t=0, t=strlen(srcptr))
.monooff
respectively.
.pp
This form of macro is also useful for
.ix 'commenting out'
"commenting out" a function
call that is used for debugging the program. For example,
.millust begin
#define alive( where ) printf( "Alive at" where "\n" )
.millust end
.pc
could later be replaced by,
.millust #define alive( where ) /* */
Alternatively, the definition,
.millust #define alive( where )
may be used.
When the module or program is recompiled using this new definition for
.mono alive
..ct ,
all of the calls to
.libfn printf
made as a result of the macro replacement will disappear, without
the necessity of deleting the appropriate lines in each module.
.pp
A directive of the form,
.ppix #undef
.ix 'undefining a macro'
.ix 'macro' 'undefining'
.cillust begin
.kwpp #undef
identifier
.cillust end
.pc
causes the macro definition
for
.us identifier
to be thrown away. No error is reported if no macro definition for
.us identifier
exists.
.*
.section Argument Substitution
.*
.pp
The argument substitution capabilities of
the C preprocessor are very powerful, but
can be tricky. The following sections illustrate the capabilities,
and try to shed light on the problems that might be encountered.
.beglevel
.*
.section Converting An Argument to a String
.*
.pp
In the replacement-string for a function-like macro, each occurrence of
.ix 'preprocessor directive' '#define' '# operator'
.mono #
must be followed by a parameter to the macro. If so, both the
.mono #
and the parameter are replaced by a string created from the characters
of the argument itself, with no
further substitutions performed on the argument.
Each white space within the argument
is converted to a single blank character.
If the argument contains a character constant or string literal, any
occurrences of
.mono "
(double-quote) are replaced by
.mono \"
..ct , and any
occurrences of
.mono \
(backslash) are replaced by
.mono \\
..ct ..li .
.pp
The following table gives a number of examples of the result of the
application of the macro,
.millust #define string( parm ) # parm
as shown in the first column:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 19
.  .boxcol 15
..do end
..el ..do begin
.  .boxcol 23
.  .boxcol 19
..do end
.boxbeg
$Argument             $After Substitution
.boxline
.monoon
$string( abc )        $"abc"
$string( "abc" )      $"\"abc\""
$string( "abc" "def" )$"\"abc\" \"def\""
$string( \'/ )        $"\\'/"
$string( f(x) )       $"f(x)"
.monooff
.boxend
.do end
.el .do begin
.millust begin
Argument                After Substitution
---------------------   ------------------
string( abc )           "abc"
string( "abc" )         "\"abc\""
string( "abc" "def" )   "\"abc\" \"def\""
string( \'/ )           "\\'/"
string( f(x) )          "f(x)"
.millust end
.do end
.*
.section Concatenating Tokens
.*
.pp
In the replacement-list, if a parameter is preceded or followed by
.ix 'preprocessor directive' '#define' '## operator'
.mono ##
..ct ,
then the parameter is replaced by the argument itself, without
examining the argument for any further replacements.
After all such substitutions, each
.mono ##
is removed and the tokens on either side are concatenated together.
The newly formed token is then examined for further macro
replacement.
.pp
.mono ##
may not be either the first or last token in the replacement-list.
.pp
Assuming that the following macros are defined,
.millust begin
#define first     "Piece"
#define last      "of Earth"
#define firstlast "Peace on Earth"
#define first1    "Peas"
.millust end
.pc
the following table gives a number of examples of the result of the
application of the macro,
.millust #define glue( x, y ) x ## y
as shown in the first column.
For the examples that span several lines, each successive line of
the "Result" column indicates successive expansions of the macros.
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 17
.  .boxcol 15
..do end
..el ..do begin
.  .boxcol 21
.  .boxcol 17
..do end
.boxbeg
$Argument           $After Substitution
.boxline
.mono $glue( 12, 34 )     $1234
.boxline
.mono $glue( first, 1 )   $first1
.mono $                   $"Peas"
.boxline
.mono $glue( first, 2 )   $first2
.boxline
.mono $glue( first, last )$firstlast
.mono $                   $"Peace on Earth"
.boxend
.do end
.el .do begin
.millust begin
Argument              After Substitution
-------------------   ------------------
glue( 12, 34 )        1234

glue( first, 1 )      first1
                      "Peas"

glue( first, 2 )      first2

glue( first, last )   firstlast
                      "Peace on Earth"
.millust end
.do end
.*
.section Simple Argument Substitution
.*
.pp
In the absence of either the
.mono #
or
.mono ##
operators, a parameter is replaced by its argument.
Before this happens, however,
the argument is scanned again to see if there
are any further macro substitutions to be made, applying all of
the above rules. The rescanning applies
.us only
to the argument, not to any other tokens that might be adjacent
to the argument when it replaces the parameter. In other words,
if the last token of the argument and the first token following in
the replacement list together form a valid macro, no substitution
of that macro will take place.
.pp
Consider the following examples, with these macro definitions in place:
.millust begin
#define f(a)   a
#define g(x)   (1+x)
#define h(s,t) s t
#define i(y)   2-y
#define xyz    printf
#define rcrs   rcrs+2
.millust end
..sk 1 c
.if &e'&nobox eq 0 .do begin
.cp 18
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 19
.  .boxcol 18
..do end
..el ..do begin
.  .boxcol 22
.  .boxcol 21
..do end
.boxbeg
$Invocation           $After Substitution
.boxline
.mono $f(c)                 $c
.boxline
.mono $f(f(c))              $f(c)
.mono $                     $c
.boxline
.mono $f(g(c))              $f((1+c))
.mono $                     $(1+c)
.boxline
.mono $h("hello",f("there"))$h("hello","there")
.mono $                     $"hello" "there"
.boxline
.mono $f(xyz)("Hello\n")    $f(printf)("Hello\n")
.mono $                     $printf("Hello\n")
.boxend
.do end
.el .do begin
.cp 16
.millust begin
Invocation              After Substitution
---------------------   --------------------
f(c)                    c

f(f(c))                 f(c)
                        c

f(g(c))                 f((1+c))
                        (1+c)

h("hello",f("there"))   h("hello","there")
                        "hello" "there"

f(xyz)("Hello\n")       f(printf)("Hello\n")
                        printf("Hello\n")
.millust end
.do end
.*
.section Variable Argument Macros
.*
.pp
.ix 'variable argument macros'
Macros may be defined to take optional additional parameters.  This is
accomplished using the
.mono ...
(ellipsis) keyword as the last parameter in the macro declaration.
There may be no further parameters past the variable argument, and errors
will be generated if the preprocessor finds anything other than a closing
parenthesis after the ellipsis.  The variable arguments may be referenced
as a whole using the
.kwpp __VA_ARGS__
keyword. Special behavior of pasting this parameter with a comma can
result in the comma being removed (this is an extension to the standard).
The only token to which this applies is a comma. Any other token
which
.kwpp __VA_ARGS__
is pasted with is not removed.
The
.kwpp __VA_ARGS__
parameter may be converted to a string using the
.mono #
operator. Consider the following examples of macros
with variable number of arguments:

.millust begin
#define shuffle1( a, b, ... )  b,__VA_ARGS__##,a
#define shuffle2( a, b, ... )  b,## __VA_ARGS__,a
#define shuffle3( a, b, ... )  b,## __VA_ARGS__##,a
#define showlist( ... )        #__VA_ARGS__
#define args( f, ... )         __VA_ARGS__
.millust end

It is safe to assume that any time a comma is used near
.kwpp __VA_ARGS__
the
.mono ##
operator should be used to paste them together. Both
.mono shuffle1
and
.mono shuffle2
macros are valid examples of pasting
.kwpp __VA_ARGS__
together with a comma; either the leading or trailing comma may be
concatenated, and if
.kwpp __VA_ARGS__
is empty, the comma is removed. The macro
.mono shuffle3
works as well; the sequence of concantenations happens from left
to right, hence first the comma and empty
.kwpp __VA_ARGS__
are concantenated and both are removed, afterwards the trailing comma
is concatentated with
.mono b.
Several example usages of the above macros follow:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.cp 16
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 20
.  .boxcol 19
..do end
..el ..do begin
.  .boxcol 23
.  .boxcol 22
..do end
.boxbeg
$Invocation           $After Substitution
.boxline
.mono $shuffle(x,y,z)           $y,z,x
.boxline
.mono $shuffle(x,y)             $y,x
.boxline
.mono $shuffle(a,b,c,d,e)       $b,c,d,e,a
.boxline
.mono $showlist(x,y,z)          $"x,y,z"
.boxline
.mono $args("%d+%d=%d",a,b,c)   $a,b,c
.boxline
.mono $args("none")             $
.boxend
.do end
.el .do begin
.cp 10
.millust begin
Invocation              After Substitution
---------------------   --------------------
shuffle(x,y,z)          y,z,x
shuffle(x,y)            y,x     // second ',' dissappears
shuffle(a,b,c,d,e)      b,c,d,e,a
showlist(x,y,z)         "x,y,z"
args("%s",charptr)      charptr
args("%d+%d=%d",a,b,c)  a,b,c
args("none")                    // nothing
.millust end
.do end
.*
.section Rescanning for Further Replacement
.*
.pp
After all parameters in the replacement-list have been replaced,
the resulting set of tokens is re-examined for any further replacement.
If, during this scan, an apparent invocation of the macro currently
being replaced is found, it is
.us not
replaced.
Further invocations of
the macro currently being replaced are not eligible
for replacement until a new set of tokens from the source file,
unrelated to the
tokens resulting from the current substitution, are being processed.
.keep begin
.pp
Consider these examples, using the above macro definitions:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 10
.  .boxcol 11
..do end
..el ..do begin
.  .boxcol 13
.  .boxcol 13
..do end
.boxbeg
$Invocation $After Rescanning
.boxline
.mono $f(g)(r)    $g(r)
.mono $           $(1+r)
.boxline
.mono $f(f)(r)    $f(r)
.boxline
.mono $h(f,(b))   $f (b)
.mono $           $b
.boxline
.mono $i(h(i,(b)))$i(i (b))
.mono $           $2-i (b)
.boxline
.mono $i(i (b))   $i(2-b)
.mono $           $2-2-b
.boxline
.mono $rcrs       $rcrs+2
.boxend
.do end
.el .do begin
.millust begin
Invocation    After Rescanning
-----------   ----------------
f(g)(r)       g(r)
              (1+r)

f(f)(r)       f(r)

h(f,(b))      f (b)
              b

i(h(i,(b)))   i(i (b))
              2-i (b)

i(i (b))      i(2-b)
              2-2-b

rcrs          rcrs+2
.millust end
.do end
.keep end
.pc
In other words, if an apparent invocation of a macro appears, and its
name matches the macro currently being replaced, and the apparent
invocation was
.us manufactured
by other replacements, it is
.us not
replaced. If, however, the apparent
invocation comes directly from an argument to the macro replacement,
then it
.us is
replaced.
.pp
After all replacements have been done,
the resulting set of tokens replaces the
invocation of the macro in the source file, and the file is then
rescanned starting at the replacement-list. Any further macro
invocations are then replaced.
However, if as a result of scanning the replacement-list with
following tokens another apparent invocation of the macro just
replaced is found, then that macro name is
.us not
replaced.
An invocation of the macro will again be replaced
only when a new invocation of the macro is found,
unrelated to
the just-replaced macro.
.pp
If the replacement-list of tokens resembles a preprocessor directive,
the preprocessor will not process it.
.pp
A macro definition lasts until it is undefined (with
.kwpp #undef
..ct ) or until the end of the module.
.endlevel
.*
.section More Examples of Macro Replacement
.*
.pp
The following examples are given in the ISO C standard, and are
presented here as a complete guide to the way in which macros are
replaced.
The expansions are shown in stages to better illustrate the process.
.pp
The first set of examples illustrates the rules for creating
string literals (using the
.mono #
operator)
and concatenating tokens (using the
.mono ##
operator).
The following definitions are used:
.code begin
#define str(s)      # s
#define xstr(s)     str(s)
#define debug(s, t) printf("x" # s "= %d, x" # t "= %s", x ## s, x ## t )
#define INCFILE(n)  vers ## n  /* comment */
#define glue(a, b)  a ## b
#define xglue(a, b) glue(a, b)
#define HIGHLOW     "hello"
#define LOW         LOW ", world"
.code end
The following replacements are made. The final result shows adjacent
string literals joined together to form a single string. This step is
not actually part of the preprocessor stage, but is given for clarity.
.code begin
debug( 1, 2 );
   printf( "x" "1" "= %d, x" "2" "= %s", x1, x2 );
   printf( "x1= %d, x2= %s", x1, x2 );
.code end
.code begin
fputs(str(strncmp("abc\0d", "abc", '\4') /* this goes away */
      == 0) str(: @\n), s);
   fputs("strncmp(\"abc\\0d\", \"abc\", '\\4') == 0" ": @\n", s);
   fputs("strncmp(\"abc\\0d\", \"abc\", '\\4') == 0: @\n", s);
.code end
.code begin
#include xstr(INCFILE(2).h)
   #include xstr(vers2.h)
   #include str(vers2.h)
   #include "vers2.h"
   :HP0.(and then the directive is replaced by the file contents):eHP0.
.code end
.code begin
glue(HIGH, LOW)
   HIGHLOW
   "hello"
.code end
.code begin
xglue(HIGH, LOW)
   xglue(HIGH, LOW ", world")
   glue( HIGH, LOW ", world")
   HIGHLOW ", world"
   "hello" ", world"
   "hello, world"
.code end
.pp
The following examples illustrate the rules for redefinition and
re-examination of macros.
The following definitions are used:
.code begin
#define x    3
#define f(a) f(x * (a))
#undef  x
#define x    2
#define g    f
#define z    z[0]
#define h    g(~~
#define m(a) a(w)
#define w    0,1
#define t(a) a
.code end
The following substitutions are made:
.code begin
..li f(y+1) + f(f(z)) % t(t(g)(0) + t)(1)
..li f(x * (y+1)) + ...
..li f(2 * (y+1)) + f(f(z)) % t(t(g)(0) + t)(1)
..li ...          + f(f(x * (z))) % ...
..li ...          + f(f(2 * (z))) % ...
..li ...          + f(x * (f(2 * (z)))) % ...
..li ...          + f(2 * (f(2 * (z)))) % ...
..li ...          + f(2 * (f(2 * (z[0])))) % t(t(g)(0) + t)(1)
..li ...                                   % t(g(0) + t)(1)
..li ...                                   % t(f(0) + t)(1)
..li ...                                   % t(f(x * (0)) + t)(1)
..li ...                                   % t(f(2 * (0)) + t)(1)
..li f(2 * (y+1)) + f(2 * (f(2 * (z[0])))) % f(2 * (0)) + t(1)
.code end
.keep begin
..sk 1 c
Another example:
.code begin
.li g(2+(3,4)-w) | h 5) & m(f)^m(m)
.li f(2+(3,4)-w) | ...
.li f(2+(3,4)-0,1) | ...
.li f(x * (2+(3,4)-0,1)) | ...
.li f(2 * (2+(3,4)-0,1)) | h 5) & ...
.li ...                  | g(~~ 5) & ...
.li ...                  | f(~~ 5) & ...
.li ...                  | f(x * (~~ 5)) & ...
.li ...                  | f(2 * (~~ 5)) & m(f)^...
.li ...                                 & f(w)^...
.li ...                                 & f(0,1)^...
.li ...                                 & f(x * (0,1))^...
.li ...                                 & f(2 * (0,1))^m(m)
.li ...                                               ^m(w)
.li f(2 * (2+(3,4)-0,1)) | f(2 * (~~ 5)) & f(2 * (0,1))^m(0,1)
.code end
.keep end
.*
.section Redefining a Macro
.*
.pp
Once a macro has been defined, its definition remains until it is
explicitly undefined (using the
.kwpp #undef
directive), or until the compilation of the source file is finished.
If a macro is undefined, then it may be redefined in some other (or
the same) way. If, during a macro replacement, the name of a macro
that has been defined, undefined and then defined again is encountered,
the current (most recent) definition of the macro is used,
.us not
the one that was in effect when the macro being replaced was defined.
.pp
Consider this example:
.millust begin
#define MAXVAL 1000
#define g(x)   CheckLimit( x, MAXVAL )

#undef  MAXVAL
#define MAXVAL 200

g( 10 );
.millust end
.pc
This macro invocation expands to,
.millust begin
CheckLimit( 10, 200 );
.millust end
.pp
A macro that has been defined may be redefined (without undefining it
first) only if the new definition
has a replacement-list that is identical to the original definition.
Each preprocessing token in both the original and new replacement lists
must have the same ordering and spelling, and there must be the same
number of tokens.
The number of spaces between tokens does not matter, unless one
definition has no spaces, and the other has spaces.
Comments count as one space.
.pp
The following examples illustrate valid redefinitions of macros:
.millust begin
#define OBJ_LIKE     (1-1)
#define OBJ_LIKE     /******/ (1-1) /****/
#define FN_LIKE(a)   ( a )
#define FN_LIKE( a ) (    /******/ \
                       a  /******* \
                          */ )
.millust end
.pc
The next examples illustrate invalid redefinitions of the same macros:
.millust #define OBJ_LIKE     (0)
.discuss begin
The token sequence is different.
.discuss end
.millust #define OBJ_LIKE     (1 - 1)
.discuss begin
The spacing is different (none versus one).
.discuss end
.keep begin
.millust #define FN_LIKE(b)   ( a )
.discuss begin
The parameter is a different name, and is used differently.
.discuss end
.keep break
.millust #define FN_LIKE(b)   ( b )
.discuss begin
The parameter is a different name.
.discuss end
.keep end
.*
.section Changing the Line Numbering and File Name
.*
.pp
A directive of the form,
.ppix #line
.cillust begin
.kwpp #line
number
.cillust end
.pc
sets the line number that the compiler associates with the current line
in the source file to the
specified number.
.pp
A directive of the form,
.cillust begin
.kwpp #line
number string
.cillust end
.pc
sets the line number as above and also sets the name
that the compiler associates with the source file that is being read
to the name contained in
the string.
.pp
If the directive is not recognized as one of the two forms
described above, then macro substitution is performed (if possible)
on the tokens on the line, and
another attempt is made. If the directive still does not match one
of the two forms, an error is reported.
.*
.section Displaying a Diagnostic Message
.*
.pp
A directive of the form,
.ppix #error
.cillust begin
.kwpp #error
tokens
.cillust end
.pc
causes the compiler to display a diagnostic message containing the
tokens from the directive.
.*
.keep begin
.*
.section Providing Other Information to the Compiler
.*
.pp
A directive of the form,
.ppix #pragma
.cillust begin
.kwpp #pragma
tokens
.cillust end
.keep end
.pc
informs the compiler about some aspect of the compilation,
in an implementation-defined manner.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
See the &pragma for full details of the
.kwpp #pragma
directive.
.shade end
..do end
.*
.************************************************************************
.*
.section Standard Predefined Macros
.*
.ix 'macro' 'predefined'
.ix 'predefined macro'
.pp
The following macro names are reserved by the compiler:
..im defmacro
.pp
Any other macros predefined by the compiler will begin with an
underscore (_) character. None of the predefined macros,
nor the identifier
.mono defined
..ct ,
may be undefined
(with
.kwpp #undef
..ct ) or redefined (with
.kwpp #define
..ct ).
.*
.************************************************************************
.*
.section &wcboth. Predefined Macros
.*
.ix 'macro' 'predefined'
.ix 'predefined macro'
.*
.pp
..im sysmacro
.*
.section The offsetof Macro
.*
.pp
The macro,
.ix 'macro' 'offsetof'
.ix 'offset of member'
.ix 'offsetof'
.cillust begin
.mono offsetof(
type, member
.monoon
);
.monooff
.cillust end
.pc
expands to a constant expression with type
.ix 'size_t'
.kw size_t
..ct ..li .
The value of the expression is the offset in bytes of
.us member
from the start of the structure
.us type
..ct ..li .
.us member
should not be a bit-field.
.pp
To use this macro, include the
.hdr <stddef.h>
header.
.*
.section The NULL Macro
.*
.pp
The
.ix 'NULL macro'
.mkw NULL
macro expands to a
.ix 'null pointer'
.us null pointer constant,
which is a value that indicates a pointer does not currently point to
anything.
.pp
It is recommended that
.mkw NULL
..ct ,
instead of 0, be used for null pointer constants.
.pp
To use this macro, include the
.hdr <stddef.h>
header.
