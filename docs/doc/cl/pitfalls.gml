.pp
Even though a C program is much easier to write than the corresponding
assembly language program, there are a few areas where most programmers
make mistakes, and spend a great deal of time staring at the code
trying to figure out why the program doesn't work.
.pp
The bugs that are the most
difficult to find often occur
when the compiler doesn't
give an error or warning, but the code generated is not what the
programmer expected.
After a great deal of looking, the programmer spots the error and
realizes that the compiler generated the correct code, but it wasn't
the code that was wanted.
.pp
Some compilers,
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
including &wcboth.,
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
including &wcall.,
..do end
..el ..if '&target' eq '370' ..th ..do begin
including &wlooc.,
..do end
.*
.************************************************************************
.*
have optional checking for common errors
built into them, providing warnings when these conditions
arise.
It is probably better to eliminate the code that causes the
warning than to turn off the checking done by the compiler.
.pp
The following sections illustrate several common pitfalls, and discuss
how to avoid them.
.*
.section Assignment Instead of Comparison
.*
.ix 'common error' '= instead of =='
.ix 'pitfall' '= instead of =='
.pp
The code fragment,
.millust begin
chr = getc();
if( chr = 'a' ) {
    printf( "letter is 'a'\n" );
} else {
    printf( "letter is not 'a'\n" );
}
.millust end
.pc
will never print the message
.mono letter is not 'a'
..ct ,
regardless of the value of
.mono chr.
.keep begin
.pp
The problem occurs in the second line of the example. The statement,
.millust begin
if( chr = 'a' ) {
.millust end
.pc
assigns the character constant
.mono 'a'
to the object
.mono chr.
If the value of
.mono chr
is not zero,
then the statement that is the subject of the
.kw if
is executed.
.keep end
.pp
The value of the constant
.mono 'a'
is never zero, so the first part of the
.kw if
will always be executed. The second part might as well not even be
there!
.pp
Of course, the correct way to code the second line is,
.millust begin
if( chr == 'a' ) {
.millust end
.pc
changing the
.mono =
to
.mono ==
..ct ..li .
This statement says to compare the value of
.mono chr
against the constant
.mono 'a'
and to execute the subject of the
.kw if
only if the values are the same.
.pp
Using one equal sign (assignment) instead of two (comparison for
equality) is a common errors made by programmers, often
by those who are familiar with languages such as Pascal,
where the single
.mono =
means "comparison for equality".
.*
.section Unexpected Operator Precedence
.*
.ix 'common error' 'mixing operator precedence'
.ix 'pitfall' 'mixing operator precedence'
.pp
The code fragment,
.millust begin
if( chr = getc() != EOF ) {
    printf( "The value of chr is %d\n", chr );
}
.millust end
.pc
will always print
.mono 1
..ct ,
as long as end-of-file is not detected in
.libfn getc
..ct ..li .
The intention was to assign the value from
.libfn getc
to
.mono chr
..ct ,
then to test the value against
.mono EOF
..ct ..li .
.pp
The problem occurs in the first line, which
says to call the
library function
.libfn getc
..ct ..li .
The return value from
.libfn getc
(an integer value representing a character, or
.mono EOF
if end-of-file
is detected),
is compared against
.mono EOF
..ct ,
and if they are not equal (it's not end-of-file),
then 1 is assigned to the object
.mono chr.
Otherwise, they are equal and 0 is assigned to
.mono chr.
The value of
.mono chr
is, therefore, always 0 or 1.
.pp
The correct way to write this code fragment is,
.millust begin
if( (chr = getc()) != EOF ) {
    printf( "The value of chr is %d\n", chr );
}
.millust end
.pc
The extra parentheses force the assignment to occur first, and then
the comparison for equality is done.
.pp
Note: doing assignment inside the
.ix 'controlling expression'
controlling expression of
loop or selection
statements is
not a good
programming practice. These expressions tend
to be difficult to read, and problems such as using
.mono =
instead of
.mono ==
are more difficult to detect when, in some cases,
.mono =
is
desired.
.*
.section Delayed Error From Included File
.*
.ix 'common error' 'delayed error from included file'
.ix 'pitfall' 'delayed error from included file'
.pp
Suppose the source file
.mono mytypes.h
contained the line,
.millust typedef int COUNTER
and the main source file being compiled started with,
.millust begin
#include "mytypes.h"

extern int main( void )
/*********************/
{
    COUNTER x;
/* ... */
}
.millust end
.pc
Attempting to compile the
main source file would report a message such as,
.millust begin
Error! Expecting ';' but found 'extern' on line 3
.millust end
.pp
Examining the main source file does not show any problem. The problem
actually occurs in the included source file, since the
.kw typedef
statement does not end with a semi-colon.
It is this semi-colon that
the compiler is expecting to find. The next token found is the
.mono extern
keyword, so the error is reported in the main source file.
.pp
When an error occurs shortly after an
.kwpp #include
directive, and the error is not readily apparent, the error may actually
be caused by something in the included file.
.*
.keep begin
.section Extra Semi-colon in Macros
.*
.pp
The next code fragment illustrates a common error when using the
preprocessor to define constants:
:IH1.common error
:I2.; in #define
:IH1.pitfall
:I2.; in #define
.millust begin
#define MAXVAL 10;

/* ... */

if( value >= MAXVAL ) break;
.millust end
.keep end
.pc
The compiler will report an error message like,
.millust begin
Error! Expecting ')' but found ';' on line 372
.millust end
.pp
The problem is easily spotted when the macro substitution is
performed on line 372. Using the definition for
.mono MAXVAL
..ct ,
the substituted version of line 372 reads,
.millust begin
if( value >= 10; ) break;
.millust end
.pc
The semi-colon
(:MSEMI.)
in the definition was not treated as an
end-of-statement indicator as expected, but was included in the
definition of the macro
.ix 'manifest constant'
.ix 'constant' 'manifest'
.ix 'constant' '#define'
.ix 'constant' 'enumeration'
(:ITAL.manifest constant:eITAL.)
.mono MAXVAL.
The substitution then results in a semi-colon being placed in
the middle of the controlling expression, which yields the syntax
error.
.*
.section The Dangling else
.*
.ix 'common error' 'dangling else'
.ix 'pitfall' 'dangling else'
.pp
In the code fragment,
.millust begin
if( value1 > 0 )
    if( value2 > 0 )
        printf( "Both values greater than zero\n" );
else
    printf( "value1 is not greater than zero\n" );
.millust end
.pc
suppose
.mono value1
has the value 3, while
.mono value2
has the value &MINUS.7.
This code fragment will cause the message,
.millust value1 is not greater than zero
to be displayed.
.pp
The problem occurs because of the
.kw else
..ct ..li .
The program is indented incorrectly according to the syntax that the
compiler will determine from the statements. The correct indentation
should clearly show where the error lies:
.millust begin
if( value1 > 0 )
    if( value2 > 0 )
        printf( "Both values greater than zero\n" );
    else
        printf( "value1 is not greater than zero\n" );
.millust end
.pc
The
.kw else
belongs to the
.us second
.kw if
..ct ,
not the first. Whenever there is more than one
.kw if
statement without braces and without an
.kw else
statement, the next
.kw else
will be matched to the most recent
.kw if
statement.
.pp
This code fragment clearly illustrates the usefulness of using
braces to state program structure. The above example would be
(correctly) written as,
.millust begin
if( value1 > 0 ) {
    if( value2 > 0 ) {
        printf( "Both values greater than zero\n" );
    }
} else {
    printf( "value1 is not greater than zero\n" );
}
.millust end
.*
.section Missing break in switch Statement
.*
.ix 'pitfall' 'missing break in switch'
.ix 'common error' 'missing break in switch'
.pp
In the code fragment,
.millust begin
switch( value ) {
  case 1:
    printf( "value is 1\n" );
  default:
    printf( "value is not 1\n" );
}
.millust end
.pp
if
.mono value
is 1, the following output will appear:
.millust begin
value is 1
value is not 1
.millust end
.pc
This unexpected behavior occurs because, when
.mono value
is 1, the
.kw switch
causes control to be passed to the
.mono case&SYSRB.1:
label, where the first
.libfn printf
occurs. Then the
.mono default
label is encountered. Labels are ignored in execution, so the next
statement executed is the second
.libfn printf
..ct ..li .
.pp
To correct this example, it should be changed to,
.millust begin
switch( value ) {
  case 1:
    printf( "value is 1\n" );
    break;
  default:
    printf( "value is not 1\n" );
}
.millust end
.pc
The
.kw break
statement causes control to be passed to the statement following the
closing brace of the
.kw switch
statement.
.*
.section Side-effects in Macros
.*
.ix 'pitfall' 'side-effects in macros'
.ix 'common error' 'side-effects in macros'
.pp
In the code fragment,
.millust begin
#define endof( ptr ) ptr + strlen( ptr )
/* ... */
endptr = endof( ptr++ );
.millust end
.pc
the statement gets expanded to,
.millust begin
endptr = ptr++ + strlen( ptr++ );
.millust end
.pc
The parameter
.mono ptr
gets incremented twice, rather than once as expected.
.pp
The only way to avoid this pitfall is to be aware of what macros
are being used, and to be careful when using them. Several
.ix 'library function'
library functions may be implemented as macros on some systems. These
functions include,
.millust begin
getc       putc
getchar    putchar
.millust end
.pc
The ISO standard requires that documentation states which
library functions evaluate their arguments more than once.
