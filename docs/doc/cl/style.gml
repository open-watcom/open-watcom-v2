.ix 'style'
.ix 'programming style'
.pp
Programming style is as individual as a person's
preference in clothing.
Unfortunately, just as some programmers wouldn't
win a fashion contest, some code has poor style. This code is
usually easy to spot, because it is difficult to understand.
.pp
Good programming style can make the difference between programs
that are easy to debug and modify, and those that you just want to
avoid.
.pp
There are a number of aspects to programming style. There is no
perfect style that is altogether superior to all others. Each programmer
must find a style that makes him or her comfortable.
The intention is to write code that is easy to read and understand,
not to try to stump the next person who has to fix a problem in
the code.
.pp
Good programming style will also lead to less time spent writing a
program, and certainly less time spent debugging or modifying it.
.pp
The following sections discuss various aspects of programming style.
They reflect the author's own biases, but they are biases based
on years of hacking his way through
code, mostly good and some bad, and much of it his own!
.*
.section Consistency
.*
.pp
Perhaps
the most important aspect of style is
.ix 'style' 'consistency'
.us consistency
..ct ..li .
Try, as much as possible, to use the same rules throughout the entire
program.
Having a mixed bag of styles within one program will confuse even the
best of programmers trying to decipher the code.
.pp
If more than one programmer is involved in the project, it may be
appropriate, before the first line of code is written, to discuss
general rules of style. Some rules are more important than others.
Make sure everyone understands the rules, and are encouraged to
follow them.
.*
.section Case Rules for Object and Function Names
.*
.pp
When examining a piece of code, the scope of an object is sometimes
difficult to determine. One needs to examine the declarations of
objects within the function, then those declared outside of any
functions, then those declared included from other source files.
If no strict rules of naming objects are followed, each place will
need to be laboriously searched each time.
.pp
.ix 'name' 'mixed case'
.ix 'style' 'case rules'
Using mixed case object names, with strict rules, can make the job
much easier.
It does not matter what rules are established, as long as the rules
are consistently applied throughout the program.
.pp
Consider the following sample set of rules, used throughout this book:
.autonote
.note
objects declared within a function
with
.ix 'automatic storage duration'
.ix 'storage duration' 'automatic'
automatic storage duration
are entirely in lower case,
.millust begin
int        x, counter, limit;
float      save_global;
struct s * sptr;
.millust end
.note
objects with
.ix 'static storage duration'
.ix 'storage duration' 'static'
static storage duration (global objects)
start with an upper
case letter, and words or word fragments also start with
upper case,
.millust begin
static int      TotalCount;
extern float    GlobalAverage;
static struct s SepStruct;
.millust end
.note
function names start with an upper case letter, and words or word
fragments also start with upper case, (distinguishable from global objects
by the left parenthesis),
.millust begin
extern int     TrimLength( char * ptr, int len );
static field * CreateField( char * name );
.millust end
.note
all
.ix 'manifest constant'
.ix 'constant' 'manifest'
.ix 'constant' '#define'
.ix 'constant' 'enumeration'
constants are entirely in upper case.
.millust begin
#define FIELD_LIMIT 500
#define BUFSIZE     32

enum { INVALID, HELP, ADD, DELETE, REPLACE };
.millust end
.note
all
.ix 'type definition'
.kw typedef
tags are in upper case.
.millust begin
typedef struct {
    float real;
    float imaginary;
} COMPLEX;
.millust end
.endnote
.pc
Thus, the storage duration and scope of each identifier can be
determined without regard to context. Consider this program fragment:
.millust begin
    chr = ReadChar();
    if( chr != EOF ) {
        GlbChr = chr;
    }
.millust end
.pc
Using the above rules,
.autonote
.note
.mono ReadChar
is a function,
.note
.mono chr
is an object
with automatic storage duration
defined within the current function,
.note
.mono EOF
is a constant,
.note
.mono GlbChr
is an object with static storage duration.
.endnote
.pp
Note: the
.ix 'library function'
library functions do not use mixed case names.
Also, the function
.ix 'function' 'main'
.mono main
does not begin with an upper case
.mono M.
Using the above coding style, library functions would stand out from
other functions because of the letter-case difference.
.*
.section Choose Appropriate Names
.*
.pp
.ix 'style' 'object names'
The naming of objects can be critical to the ease with which
bugs can be found, or changes can be made. Using object names such
as
.mono linecount, columns
and
.mono rownumber
will make the program more readable. Of course, short forms
will creep into the code (few programmers
like to type more than is really
necessary), but they should be used judiciously.
.pp
Consistency of naming also helps
to make the code more readable. If a structure is used throughout the
program, and many different routines need a pointer to that structure,
then the name of each object that points to it could be made the same.
Using the example of a symbol table, the object name
.mono symptr
might be used everywhere to mean "pointer to a symbol structure".
A programmer seeing that object will automatically know what it is
declared to be.
.pp
Appropriate function names are also very important. Names such as
.mono DoIt
..ct ,
while saving the original programmer from trying to think of a good
name, make it more difficult for the next programmer to figure out
what is going on.
.*
.section Indent to Emphasize Structure
.*
.ix 'style' 'indenting'
.pp
The following is a valid function:
.millust begin
static void BubbleSort( int list[], int n )
/**********************************/ { int index1
= 0; int index2; int temp; if( n < 2 )return; do {
index2 = index1 + 1; do { if( list[ index1 ] >
list[ index2 ] ) { temp = list[ index1 ]; list[
index1 ] = list[ index2 ]; list[ index2 ] = temp;
} } while( ++index2 < n ); } while( ++index1 < n-1
); }
.millust end
.pc
(The compiler will know that it's valid, but the programmer would
find it difficult to validate.)
Here is the same function, but using indenting to clearly illustrate
the function structure:
.millust begin
static void BubbleSort( int list[], int n )
/*****************************************/
  {
    int index1 = 0;
    int index2;
    int temp;

    if( n < 2 )return;
    do {
        index2 = index1 + 1;
        do {
            if( list[ index1 ] > list[ index2 ] ) {
                temp = list[ index1 ];
                list[ index1 ] = list[ index2 ];
                list[ index2 ] = temp;
            }
        } while( ++index2 < n );
    } while( ++index1 < n-1 );
  }
.millust end
.pp
Generally, it is good practice to indent each level of code by a
consistent amount, for example 4 spaces. Thus, the subject of
an
.kw if
statement is always indented 4 spaces inside the
.kw if
..ct ..li .
In this manner, all loop and selection statements will stand out,
making it easier to determine when the statements end.
.pp
The following are some recommended patterns to use when indenting
statements.
These patterns have been used throughout the book.
.millust begin
int Fn( void )
/************/
{
    /* indent 4 */
}
.millust break
..sk 1 c
if( condition ) {
    /* indent 4 */
} else {
    /* indent 4 */
}
.millust break
..sk 1 c
if( condition ) {
    /* indent 4 */
} else if( condition ) {
    /* indent 4 from first if */
    if( condition ) {
        /* indent 4 from nearest if */
    }
} else {
    /* indent 4 from first if */
}
.millust break
..sk 1 c
switch( condition ) {
  case VALUE:
    /* indent 4 from switch */
  case VALUE:
  default:
}
.millust break
..sk 1 c
do {
    /* indent 4 */
while( condition );
.millust break
..sk 1 c
while( condition ) {
    /* indent 4 */
}
.millust break
..sk 1 c
for( a; b; c ) {
    /* indent 4 */
}
.millust end
.keep begin
.pc
Two other popular indenting styles are,
.millust begin
if( condition )
  {
    :ITAL.statement:eITAL.
  }
.millust end
.keep break
.pc
and,
.millust begin
if( condition )
{
    :ITAL.statements:eITAL.
}
.millust end
.keep end
.pc
It is not important which style is used. However, a consistent style
is an asset.
.*
.section Visually Align Object Declarations
.*
.ix 'style' 'aligning declarations'
.pp
A lengthy series of object declarations can be difficult to read if
care is not taken to improve the readability. Consider the
declarations,
.millust begin
struct flentry *flptr;
struct fldsym *sptr;
char *bufptr,*wsbuff;
int length;
.millust end
.pc
Now, consider the same declarations, but with some
.ix 'visually aligning object declarations'
visual alignment done:
.millust begin
struct flentry * flptr;
struct fldsym  * sptr;
char           * bufptr;
char           * wsbuff;
int              length;
.millust end
.pc
It is easier to scan a list of objects when their names
all begin in
the same column.
.*
.section Keep Functions Small
.*
.ix 'style' 'small functions'
.pp
A function that is several hundred lines long can be difficult
to comprehend, especially if it is being looked at on a terminal,
which might only have 25 lines.
Large functions also tend to have a lot of nesting of program
structures, making it difficult to follow the logic.
.pp
A function that fits entirely within the terminal display can be
studied and understood more easily. Program constructs don't
get as complicated. Large functions often can be broken up into
smaller functions which are easier to
maintain.
..if 0 = 1 ..th ..do begin
.*
.section Use Extra Spacing for Emphasis
.*
.ix 'style' 'spacing'
.pp
In complicated expressions and conditions, use spacing to
emphasize the order of operations. Consider these examples:
.millust begin
if( count > 17  &&  flag == 0 ) {
    result = count*3 + 2;
    flag = oldflag << 3  |  1;
}
.millust end
.pc
The extra spaces used within the expressions emphasize
that the subexpressions with fewer spaces are performed first.
.*
.section Underline Function Declarations
.*
.ix 'style' 'underlining function declarations'
.pp
A large module with many functions is a common occurrence.
.ix 'underlining function declarations'
Using underlining to emphasize function declarations makes
it easier to spot the start and end of the functions.
Consider the following example:
.millust begin
extern int TrimLength( char * string )
/************************************/
{
/* ... */
}
.millust end
..do end
.*
.section Use static for Most Functions
.*
.ix 'style' 'static objects'
.pp
Most functions do not need to be called from routines outside of the
current module. Yet, if the keyword
.kw static
is not used in the function declaration, then the function is
automatically given
.ix 'external linkage'
.ix 'linkage' 'external'
.us external linkage
..ct ..li .
This can lead to a proliferation of external symbols, which
may cause naming conflicts.
Also, some linking programs may impose limitations.
.pp
Only those functions that must have external linkage
should be made external. All other definitions
of functions should start with the keyword
.kw static
..ct ..li .
.pp
It also is a good idea to start definitions for external
functions with the keyword
.kw extern
..ct ,
even though it is the default case.
.*
.section Group Static Objects Together
.*
.pp
Static objects that are declared outside of any function definition,
and are used throughout the module,
generally should be declared together,
for example
before the definition of the first
function.
Placing the declarations of these objects near the beginning
of the module makes them easier to find.
..if 0 = 1 ..th ..do begin
.*
.section Declare Automatic Objects Before Function Statements
.*
.pp
Automatic objects should be declared at the beginning of the
function definition. Specifically, declaring objects at the beginning
of a block of code that is nested within the function definition
should be avoided. Most modern compilers will produce the same code,
regardless of the placement of the declaration.
Collecting the definitions together under the function declaration
will make them easier to find.
..do end
.*
.section Do Not Reuse the Names of Static Objects
.*
.ix 'style' 'reusing names'
.pp
If an object with
.ix 'static storage duration'
.ix 'storage duration' 'static'
static storage duration exists in one module, but
has
.ix 'internal linkage'
.ix 'linkage' 'internal'
.us internal linkage
..ct ,
then another object with the same name should not be declared in another
module.
The programmer may confuse them.
.pp
Even more importantly, if an object exists with
.ix 'external linkage'
.ix 'linkage' 'external'
.us external linkage
..ct ,
a module should not declare another object with the same name with
.us internal linkage
..ct ..li .
This second object will overshadow the first within the module, but
the next programmer to look at the code will likely be confused.
.*
.section Use Included Files to Organize Structures
.*
.pp
.ix 'included file'
.ix 'style' 'included files'
Included source files
can be used to organize data structures and related information.
They should be used when the same structure is needed in
different modules.
They should even be considered when the
structure is used only in one place.
.pp
Generally, each included source file should contain structures and
related information for one aspect of the program. For example,
a file that describes a symbol table might contain the
actual structures or other types that are required, along with any
.ix 'manifest constant'
.ix 'constant' 'manifest'
.ix 'constant' '#define'
.ix 'constant' 'enumeration'
manifest constants that are useful.
.*
.section Use Function Prototypes
.*
.ix 'style' 'function prototypes'
.*
.pp
Function prototypes are very useful for eliminating common errors when
calling functions.
If every function in a program is prototyped (and the prototypes are used),
then it is difficult to pass the wrong number or types of arguments,
or to misinterpret the return value.
.pp
Using the symbol table example, the included source file that describes the
symbol table structure and any related global objects or constant values
could also contain the function prototypes for the functions used to
access the table.
Another approach is to have separate source files containing the
function prototypes, possibly using a different naming convention for the file.
For example,
.millust begin
#include "symbols.h"
#include "symbols.fn"
.millust end
.pc
would include the structures and related values from
.mono symbols.h
..ct ,
and the function prototypes from
.mono symbols.fn
..ct ..li .
.*
.section Do Not Do Too Much In One Statement
.*
.ix 'style' 'complicated statements'
.pp
In the same manner that a big function that does too much can be confusing,
so too can a long statement.
Historically, a programmer might combine many operations into a single
statement in order to get the compiler to produce better code.
With current compilers, splitting the statement into two or more simpler
statements will produce equivalent code, and will make the program easier
to understand.
.keep begin
.pp
A common example of a statement that
can be split is,
.millust begin
    if( (c = getchar()) != EOF ) {
.millust end
.keep end
.pc
Historically, this statement might have allowed the
compiler to avoid storing the
value of
.mono c
and then reloading it again to compare with
.mono EOF.
However, the equivalent,
.millust begin
    c = getchar();
    if( c != EOF ) {
.millust end
.pc
is more readable, and most compilers will produce the same code.
.*
.section Do Not Use goto Too Much
.*
.pp
.ix 'style' 'goto'
The
.ix 'goto statement'
.ix 'statement' 'goto'
.kw goto
statement is a very powerful tool, but it is very easy to misuse.
Here are some general rules for the use of
.kw goto
..ct 's:
.begbull
.bull
don't use them!
.endbull
.pp
If that rule is not satisfactory, then these should be followed:
.begbull
.bull
Never
.kw goto
a label that is above. That is the beginning of
.ix 'spaghetti code'
.us spaghetti code
..ct ..li .
Loop statements can always be used.
.bull
Never
.kw goto
the middle of a block (compound-statement).
A block should always be entered by passing over the opening brace.
.bull
Use
.kw goto
to jump out of nested blocks, where the
.ix 'break statement'
.ix 'statement' 'break'
.kw break
statement is not appropriate.
.endbull
.pp
Above all, keep the use of
.kw goto
..ct 's
to a minimum.
.*
.section Use Comments
.*
.pp
.ix 'style' 'comments'
.ix 'comment'
Comments are crucial to good programming style. Regardless of how
well the program is written,
some code will be difficult to understand. Comments make it possible
to give a full explanation for what the code is trying to do.
.pp
Each function definition should begin with a short comment describing
what the function does.
.pp
Each module should begin with comments describing the purpose of the
module. It is also a good idea to type in
who wrote it, when it was written, who modified it and why,
and when it was modified.
This last collection of information is commonly called an
.ix 'audit trail'
.us audit trail
..ct ,
as it leaves a trail allowing a programmer to see the evolution of
the module, along with who has been changing it.
.pp
The following audit trail is from
one module in an actual product:
.code begin
/* Modified: By:            Reason:
 * ========  ==             ======
 * 84/04/23  Dave McClurkin Initial implementation
 * 84/11/08  Jim Graham     Implemented TOTAL non-combinable;
 *                            added MAXIMUM,MINIMUM,AVERAGE
 * 84/12/12  Steve McDowell Added call to CheckBreak
 * 85/01/12  ...            Fixed overflow problems
 * 85/01/29  Alex Kachura   Saves value of TYP_ field
 * 86/01/31  Steve McDowell Switched to use of numeric accumulator
 * 86/12/10  ...            Removed some commented code
 * 87/02/24  ...            Made all commands combinable
 */
.code end
