.chap Commonly Asked Questions and Answers
.*
.np
.ix 'common questions'
.ix 'questions'
.ix 'answers to general problems'
As with any sophisticated piece of software, there are topics that are
not directly addressed by the descriptive portions of the manuals.
The purpose of this chapter is to anticipate common questions
concerning &cmpname..
It is difficult to predict what topics will prove to be useful but
with that in mind, we hope that this chapter will help our customers
make full use of &cmpname..
.np
A number of example programs are presented throughout.
The source text for these files can be found in the
.fi &pathnamup.\SAMPLES\GOODIES
directory.
.np
The purpose of this
.if '&target' eq 'QNX' .do begin
appendix
.do end
.el .do begin
chapter
.do end
is to present some of the more commonly asked questions from our users
and the answers to these questions.
The following topics are discussed:
.begbull $compact
.bull
How do I determine my current patch level?
.bull
How do I convert to &cmpname?
.bull
What should I know about optimization?
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
How do I read a stream of binary data from a file?
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
How do I redefine math error handling with &cmpname?
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
Why can't the compiler find my include files?
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Why can't the compiler find "stdio.h"?
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
Why does the linker report a "stack segment not found" error?
.do end
.bull
How do I resolve an "Undefined Reference" linker error?
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
Why aren't local variable values maintained between subprogram calls?
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Why aren't my variables set to zero?
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
What does "size of DGROUP exceeds 64K" mean for 16-bit applications?
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
What does "NULL assignment detected" mean in 16-bit applications?
.do end
.bull
What does "Stack Overflow!" mean?
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
What are the probable causes of a General Protection Fault in 32-bit applications?
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
Which floating-point compiler option should I use for my application?
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Why do I get redefinition errors from &lnkcmdup?
.do end
.if '&target' ne 'QNX' .do begin
.bull
How can I open more than 20 files at a time?
.do end
.bull
How can I see my source files in the debugger?
.bull
What is the difference between the "d1" and "d2" compiler options?
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
What is the difference between the "debug" and "d2" compiler options?
.do end
:CMT. .bull
:CMT. How do I trace back from a program crash in VIDEO?
.endbull
.*
.if '&target' ne 'QNX' .do begin
.*
.section Determining my current patch level
.*
.np
.ix 'patch level'
.ix 'patches'
.ix 'TECHINFO'
In an effort to immediately correct any problems discovered in the
originally shipped product, &company provides patches as a continued
service to its customers.  To determine the current patch level of your
&company software, a TECHINFO utility program has been provided.
This program will display your current environment variables, the patch
level of various &company software programs, and other pertinent
information, such as your
.fi AUTOEXEC.BAT
and
.fi CONFIG.SYS
files.
This information proves to be
very useful when reporting a problem to the Technical Support team.
.np
To run TECHINFO, you must ensure the &company environment variable has
been set to the directory where your &company software has been installed.
TECHINFO will pause after each screenful of information.
The output is also placed in the file
.fi TECHINFO.OUT.
.np
Below is an example of some partial output produced by running the TECHINFO
utility:
.tinyexam begin
WATCOM's Techinfo Utility, Version 1.4
Current Time: Thu Oct 27 15:58:34 1994

WATCOM                          Phone: (519) 884-0702
415 Phillip St.                 Fax: (519) 747-4971
Waterloo, Ontario
CANADA    N2L 3X2

-------------WATCOM C Environment Variables -------------
WATCOM=<c:\watcom>
EDPATH=<c:\watcom\eddat>
INCLUDE=<c:\watcom\h;c:\watcom\h\os2>
FINCLUDE=<c:\watcom\src\fortran;c:\watcom\src\fortran\win>
LIBOS2=<c:\watcom\lib286\os2;c:\watcom\lib286>
PATH=<c:\dos;c:\windows;c:\watcom\binw>
TMP=<h:\temp>
File 'c:\watcom\binw\wcc386.exe' has been patched to level '.d'
.li ...etc...
.tinyexam end
.np
In this example, the software has been patched to level "d".
In most cases, all tools will share a common patch level.
However, there are instances where certain tools have been patched
to one level while others are patched to a different level.
For example, the compiler may be patched to level "d" while the
debugger is only patched to level "c".
Basically, this means that there were no debugger changes in
the D-level patches.
.np
If you run the TECHINFO utility, and determine that you are not at the
current patch level, it is recommended that you update your software.
.ix 'BBS'
.ix 'bulletin board'
.ix 'FTP site'
.ix 'CompuServe'
Patches are available on &company's bulletin board, &company's FTP
site and CompuServe.
They are available 24 hours a day.
Patches are also available on the current release CD-ROM.
Each patch will include a batch file that allows you to apply the
patches to your existing software.
Note that patches must be applied in sequential order, as each patch
depends on the previous one.
.do end
.*
.section Converting to &cmpname
.*
.np
.ix 'converting to &cmpname'
.ix 'converting to &cmpname' 'common problems'
.ix 'converting to &cmpname' 'what you need to know'
.ix '&cmpname' 'converting to'
.if '&lang' eq 'FORTRAN 77' .do begin
.ix 'FORTRAN 77' 'Extensions'
.ix 'SAVE'
Applications written in ANSI standard FORTRAN 77 code usually only need
to be recompiled with the &cmpname compiler.
In addition to the ANSI standard, many compilers support specific
extensions.
If you are porting code from a UNIX platform or other DOS compilers,
check
.us Appendix A - Extensions to Standard FORTRAN 77
of the
.book &company &lang Language Reference,
to determine which FORTRAN 77 extensions are supported.
.np
By default, most &lang compilers preserve the values of local
variables in a subprogram between calls.
With &cmpname, local variables are kept on the stack and their values
are popped from the stack when exiting a subprogram.
To preserve local variables, use the &lang SAVE statement for
variables that you wish to preserve, or the "save" compiler option to
preserve all local variables.
Note that the use of the "save" compiler option causes an overall
performance degradation.
.np
&cmpname uses register-based parameter passing as a default, however,
the compiler is flexible enough to use different calling conventions
on a per function basis.
Auxiliary pragmas can be used to specify the calling convention that
is to be used to interface with assembler code.
This enables you to explicitly state how parameters are to be passed
to the assembler code.
This topic is described in the "Pragmas" chapter
.if '&target' ne 'QNX' .do begin
of the
.book &cmpname User's Guide
.do end
under "Describing Argument Information".
See also the chapter entitled :HDREF refid='cmixf'..
:cmt. .np
:cmt. For example, the statement "C$PRAGMA AUX foo PARM ROUTINE []" can be used
:cmt. to indicate that the parameters for a procedure named "foo" will be managed
:cmt. by the called procedure, "foo" and none of the registers will be used.
:cmt. If the application interfaces with assembler code that assumes parameters
:cmt. are passed on the stack, then you must either convert the assembler code
:cmt. or use "C$PRAGMA" statements to define the calling convention for the
:cmt. assembler routines.
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
There are some common steps involved in converting C programs written
for other compilers.
Conversion from UNIX and other IBM-compatible PC compilers will be
covered in detail later.
There are six major problems with most programs that are ported to
&cmpname..
The assumptions that most foreign programs make that may be invalid
when using &cmpname are:
.autonote
.note
sizeof( pointer ) == sizeof( int )
.np
(true for 16-bit systems except "far" pointers,
true for 32-bit systems except "far" pointers)
.note
sizeof( long ) == sizeof( int )
.np
(not true for 16-bit systems)
.note
sizeof( short ) == sizeof( int )
.np
(not true for 32-bit systems)
.note
arguments are always passed on the stack
.note
dereferencing the
.id NULL
pointer
.note
"char" is either signed or unsigned
.endnote
.np
These assumptions are very easy to make when developing programs for
only one system.
The first point becomes important when you move a program to 80x86
systems.
.ix 'integer/pointer equivalence'
.ix 'invalid conversion'
.ix 'memory models' 'what you need to know'
Depending on the memory model, the size of an integer might not equal
the size of a pointer.
You might ask how this assumption is made in programs.
The C language will assume that a function returns an integer unless
told otherwise.
If a programmer does not declare a function as returning a pointer,
the compiler will generate code which would convert an integer to a
pointer.
On other systems, where the size of an integer is equal to the size of
a pointer this would amount to nothing because no conversion was
necessary (to change size).
The older C compilers did not worry about warning the programmer about
this condition and as such this error is imbedded in a lot of older C
code.
As C was moved to other machines, it became apparent that this
assumption was no longer valid for all machines.
The 80x86 architecture can have 16-bit integers and 32-bit pointers
(in the compact, large, and huge memory models), which means that
more care must be taken when working with declarations (converting
an int to a 32-bit pointer will result in a segment value of 0x0000
or 0xffff).
Similarly, the 386 architecture can have 32-bit integers and 48-bit
pointers.
.np
The &cmpname compiler will complain about incorrect pointer and
integer mixing thus making programs compiled with &cmpname much more
portable.
For instance, if the &cmpname compiler complains about your usage of
the "malloc" memory allocation function then you probably forgot to
include "<stdlib.h>" which contains the prototype of the "malloc"
function.
.exam begin
extern void *malloc( unsigned );
.exam end
.pc
The &cmpname compiler was complaining about you trying to assign an
integer (the value returned by "malloc") to a pointer.
By including the header file with the correct prototype, the &cmpname
compiler can validate that you are in fact assigning a pointer value
to a pointer.
.np
.ix 'arguments' 'what you need to know'
.ix 'pushing arguments' 'what you need to know'
.ix 'parameters' 'what you need to know'
.ix 'calling convention' 'what you need to know'
.ix '&cmpname' 'calling convention'
.ix '&cmpname' 'unique aspects'
.ix 'ISO/ANSI standard' 'variable number of arguments'
Passing arguments on the stack has been the method used by most older
compilers because it allowed the C library function "printf" to work
with a variable number of arguments.
Older C compilers catered to a few functions by forcing all the
argument handling to be handled by the caller of the function.
With the advent of the ANSI (and later ISO) standard, which forced all
functions expecting a variable number of arguments to be declared properly,
compilers can generate smaller code for routines that did not require
a variable number of arguments.
.exam begin
/* function accepting two arguments */
extern FILE *fopen( char *, char * );
/* function accepting a variable number of arguments */
extern int printf( char *, ... );
.exam end
.pc
The &cmpname compiler takes advantage of this part of the ISO/ANSI
standard by passing arguments in registers (for the first few
arguments).
.ix 'registers' 'calling convention'
If there are not enough registers for all of the arguments, the rest
of the arguments are passed on the stack but the routine being called
is responsible for removing them from the stack.
By default,
the &cmpname compiler uses this calling convention because it results
in faster procedure calls and smaller code.
The &cmpname calling convention carries with it a responsibility to
ensure that all functions are prototyped correctly before they are
used.
For instance, if a procedure is called with too few arguments, the
assumptions that the code generator made (while generating the code)
will be invalidated.
The code generator assumes that AX (EAX for the 32-bit compiler)
and any other registers used to pass arguments will be modified by the
called function.
The code generator also assumes that the exact amount of arguments
pushed on the stack will be removed by the function that is called.
It is important to recognize this aspect of the &cmpname compiler
because the program will simply not work unless the caller and the
function being called strictly agree on the number and types of the
arguments being passed.
See the "Assembly Language Considerations" chapter
.if '&target' ne 'QNX' .do begin
in the
.book &cmpname User's Guide
.do end
for more details.
.np
.ix 'NULL pointer'
.ix 'portability' 'NULL pointer'
Some compilers allow the
.id NULL
pointer to be dereferenced and return
.id NULL
(we have never understood the rationale behind this, nor why some
compilers continue to support this type of code).
Leaving the aesthetics of this type of code behind, using the
.id NULL
dereferencing assumption in a program will ensure that the program
will not be portable.
Source code which contains the
.id NULL
dereferencing assumption must be corrected before it will work with
&cmpname..
.np
.ix 'signed char'
.ix 'portability' 'signed char'
Programs that assume that the "char" type is "signed" should use the
&cmpname compiler "j" option.
The "j" option will indicate to the &cmpname compiler that the "char"
type is "signed" rather than the default "unsigned".
.*
.beglevel
.*
.section Conversion from UNIX compilers
.*
.np
.ix 'converting to &cmpname' 'from UNIX'
.ix 'UNIX'
The ISO/ANSI standard for C (which &cmpname adheres to) is very similar to
UNIX C.
.if '&target' ne 'QNX' .do begin
Most of the effort in converting UNIX C programs will involve
replacing references to library functions (such as the CURSES
library).
There are many third-party libraries which are implementations of UNIX
libraries on IBM-compatible Personal Computers.
.do end
There is a common problem which many older UNIX programs exhibit,
namely, functions that accept a variable number of arguments are coded
in many different ways.
Functions accepting a variable number of arguments must be coded
according to the ISO standard if they are to work with &cmpname..
We will code an example of a function which will return the maximum of
a list of positive integers.
.ix 'example' 'variable number of arguments'
.ix 'variable number of arguments'
.millust begin
/*
  variable number of arguments example
*/
#include <stdarg.h>

int MaxList( int how_many, ... )
{
    va_list args;
    int max;

    max = 0;
    va_start( args, how_many );
    while( how_many > 0 ) {
        value = va_arg( args, int );
        if( value > max ) {
            max = value;
        }
    }
    va_end( args );

    return( max );
}
.millust end
.pc
Notice that the standard header file
.fi STDARG.H
must be included in any source file which defines a function that
handles a variable number of arguments.
The function "MaxList" must be prototyped correctly in other source
files external to the source file containing the definition of
"MaxList".
.millust begin
extern int MaxList( int how_many, ... );
.millust end
.pc
See the
.book &company C Library Reference
manual description of "va_arg" for a more complete description of
variable number of arguments handling.
.*
.section Conversion from IBM-compatible PC compilers
.*
.np
.ix 'converting to &cmpname' 'from IBM-compatible PC compilers'
.ix 'IBM-compatible PC compilers'
Most of the compilers available for IBM-compatible PCs
have been following the ISO/ANSI standard and, as such,
the majority of programs will not require extensive source changes.
There are problems with programs that use compiler-specific library
functions.
The use of compiler-specific library functions can be dealt with in
two different ways:
.autopoint
.point
use equivalent &cmpname library functions
.point
write your own library functions
.endpoint
.pc
:cmt. A good example is the set of library functions that deal with wild
:cmt. card file specifications.
:cmt. The &cmpname library contains the POSIX conforming functions
:cmt. "opendir", "readdir", and "closedir" but other IBM PC and PS/2
:cmt. compilers use the functions "findfirst" and "findnext".
:cmt. Any program that uses the "findfirst" and "findnext" functions could
:cmt. be changed (in a simple way) to use the functions present in the
:cmt. &cmpname library.
.ix 'predefined macros' '__WATCOMC__'
.ix 'macros' '__WATCOMC__'
.ix '__WATCOMC__'
If portability must be maintained with the other compiler, the
predefined macro
.id "__WATCOMC__"
can be used to conditionally compile the correct code for the &cmpname
compiler.
.np
The default calling convention for the &cmpname compiler is different
from the calling convention used by other compilers for Intel-based
personal computers.
The &cmpname calling convention is different because it will pass some
arguments in registers (thus reducing the overhead of a function call)
rather than pushing all of the arguments on the stack.
The &cmpname compiler is flexible enough to use different calling
conventions on a per function basis.
Converting code from other compilers usually involves recompiling the
C source files and setting up prototypes (to use the older calling
convention) for functions written in assembly language.
For instance, if you have the functions "video_init", "video_put", and
"video_get" written in assembly language, you can use the following
prototypes in any source file which uses these functions.
.ix 'calling conventions' 'cdecl'
.ix 'cdecl' 'calling convention'
.code begin
#include <stddef.h>

extern int  cdecl video_init( void );
extern void cdecl video_put( int row,int col,char ch,int attr );
extern char cdecl video_get( int row,int col );
.code end
.pc
The inclusion of the
.fi STDDEF.H
header file defines the "cdecl" calling convention.
The &cmpname compiler will ensure that any calls to these three
functions will adhere to the "cdecl" calling conventions.
The &cmpname compiler will put a trailing underscore "_" character (as
opposed to the beginning of the name for the "cdecl" convention) on
any function names to ensure that the program will not link register
calling convention calls to "cdecl" convention functions (or vice
versa).
If the linker indicates that functions defined in assembler files
cannot be resolved, it could be a result of not prototyping the
functions properly as "cdecl" functions.
.cp 25
.hint
(16-bit applications only)
Most 16-bit C compilers (including &cmpname) have a "large" memory
model which means that four byte pointers are used for both code and
data references.
A subtle point to watch out for involves differences between memory
model definitions of different compilers.
The "cdecl" calling convention allows functions to assume that the DS
segment register points to the group "DGROUP".
The &cmpname large memory model has what is called a "floating DS".
Any function used for the large memory model cannot assume that the DS
segment register points to the group "DGROUP".
There are a few possible recourses.
.autonote
.note
The assembly code could save and restore the DS segment register and
set DS to DGROUP in order to conform to the &cmpname convention.
If there are only a few accesses to DGROUP data, it is advisable to
use the SS segment register which points to DGROUP in the large memory
model.
.ix 'auxiliary pragma' 'loadds'
.ix 'loadds pragma option'
.ix 'pragma' 'loadds option'
.note
The assembly function could be described using a pragma that states
that DS should point to "DGROUP" before calling the function.
.ix 'DS segment register'
.ix 'segment registers' 'DS'
.np
.us #pragma aux _Setcolor parm loadds
.np
In the above example,
.us _Setcolor
is the sample function being described.
.note
.ix 'compile options' 'zdp'
The final alternative would be the use of the "zdp" compiler option.
The "zdp" option informs the code generator that the DS register must
always point to "DGROUP".
This is the default in the small, medium and flat memory models.
Note that "flat" is a 32-bit memory model only.
.endnote
.ehint
.endlevel
.*
.do end
.*
.section What you should know about optimization
.*
.np
.ix 'optimization' 'what you should know'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The &lang language contains features which allow simpler compilers to
generate code of reasonable quality.
Register declarations and imbedding assignments in expressions are two
of the ways that C allows the programmer to "help" the compiler
generate good quality code.
An important point about the &cmpname compiler is that it is not as
important (as it is with other compilers) to "help" the compiler.
In order to make good decisions about code generation, the &cmpname
compiler uses modern optimization techniques.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
The
.book &cmpname User's Guide
contains a detailed description for each of the optimization options
supported by the compiler.
These options allow you to customize the type of code optimizations
that are performed.
For instance, the "OS" option can be used to reduce the size of your
code, but this may affect the execution speed.
To ensure that the speed of the code is optimized, possibly at the
cost of code size, use the "OT" option.
The "OX" option, intended for the maximum number of optimizations,
generates code that is a combination of "OM" (inline math functions),
"OL" (loop), "OT" (time) and the "OR" (instruction scheduling)
optimization options.
Note that when you are using the "OM" option to generate inline math
functions no argument validation will be done for the intrinsic math
functions such as "sin" or "cos".
Consider the needs of your application and select the optimization
options that best meet your requirements.
.do end
.ix 'optimization' 'suggested reading'
.ix 'dragon book'
.cp 12
.hint
The definitive reference on compiler design is the "dragon" book
"Compilers - Principles, Techniques, and Tools", Alfred V. Aho, Ravi
Sethi, and Jeffrey D. Ullman, published by Addison-Wesley, Reading,
Massachusetts, 1986.
The authors of the "dragon" book advocate a conservative approach to
code generation where optimizations must preserve the semantics of the
original program.
The conservative approach is used throughout the &cmpname compiler to
ensure that programmers can use the compiler without worrying about
the semantics of their program being changed.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.* there is no "oa" option with FORTRAN
The programmer can request that potentially unsafe optimizations be
performed.
With regard to the "oa" (ignore aliasing) option provided by the
&cmpname compiler, the compiler only ignores aliasing of global
variables rather than ignore aliasing totally like other compilers.
.do end
.ehint
.np
There are certain pieces of information which the compiler cannot
derive from the source code.
.ix 'pragma'
.if '&lang' eq 'FORTRAN 77' .do begin
The "*$pragma" compiler directive is used to provide extra information
to the compiler.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The "#pragma" compiler directive is used to provide extra information
to the compiler.
.do end
It is necessary to have a complete understanding of both &lang and the
machine architecture (i.e., 80x86) before using the powerful pragma
compiler directives.
See the "Pragmas" chapter
.if '&target' ne 'QNX' .do begin
in the
.book &cmpname User's Guide
.do end
for more details.
.np
.ix 'debugging' 'optimized programs'
Debugging optimized programs is difficult because variables can be
assigned to different locations (i.e., memory or registers) in
different parts of the function.
.ix '&cmpname options' 'd1'
.ix '&cmpname options' 'd2'
The "d2" compiler option will restrict the amount of optimization so
that variables occupy one location and can be easily displayed.
It follows that the "d2" option is useful for initial development but
production programs should be compiled with only the "d1" option for
the best code quality.
.ix '&stripname'
Before you distribute your application to others, you may wish to use
the &stripname (&stripcmdup) to remove debugging information from the
executable image on disk thereby reducing disk space requirements.
.ix 'symbolic information'
.ix 'line number information'
.cp 7
.hint
.ix 'debugging'
.ix 'd1'
.ix 'd2'
The "d2" compiler option will generate symbolic
information (for every local variable) and line number information for
the source file.
The "d1" compiler option will only generate line number information
for the source file.
The use of these options determines what kind of information will be
available for the particular module during the debugging session.
.ehint
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
Incorrect programs can sometimes work when compiled with the "d2"
option and not work when compiled with the "d1" option.
One way this sort of problem arises involves local arrays.
.ix 'problems' 'with d2 and d1 options'
.ix 'array subscript errors' 'how they hurt'
.millust begin
void example( void )
{
    int i;
    int a[10];

    for( i = 0; i <= 10; ++i )
        a[i] = i;
    do_something( a );
}
.millust end
.pc
The "for" loop initializes one too many array elements but the version
compiled with the "d2" option will overwrite the variable "i" without
causing any problems.
The same function compiled with the "d1" option would have the
variable "i" in a register.
The erroneous access of "a[10]" would modify a value that is used to
restore a register when the function returns.
The register would be "restored" with an incorrect value and this
would affect the execution of the function that called this function.
The above example shows how a program can work when compiled with the
"d2" option and stop working when compiled with the "d1" option.
You should always test your program fully with all the modules
compiled with the "d1" option to protect yourself from any surprises.
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section Reading a stream of binary data from a file
.*
.np
.ix 'binary data'
.ix 'sequential'
.ix 'formatted'
.ix 'fixed record type'
The &cmpname allows for three types of binary data file formats:
.begbull
.bull
Variable length, unformatted sequential access binary records,
.bull
Fixed length, unformatted direct access binary records, and
.bull
Unformatted, sequential, binary data with a fixed record type.
.endbull
.np
Variable length binary records are preceded by a four byte
descriptor that indicates
the length of the record in bytes.
The end of the binary record is
also marked by another descriptor tag specifying the length.
Binary records that are of a fixed length are kept in a direct access,
unformatted file.
Refer to the
.book &cmpname User's Guide
section on File Handling for more information on file formats.
.np
Binary data files that have no structure or record length information
may be read if you open the file as a sequential, unformatted file
with a fixed record type.
This allows you to read files that contain a stream of binary data
without any inherent record format.
If you know the type of data that is contained in the binary file, you
may then read the binary data directly into variables.
The following program provides an example of reading binary stream
data.
.millust begin
* BINDATA.FOR - This program demonstrates how to read a
* binary data file that does not have any defined records.

      program bindata

      integer BinArray(20)
      integer i

      open( unit=1, file='bindata.fil',
     +      access='sequential',
     +      form='unformatted',
     +      recordtype='fixed' )

*  Read 20 integers from the binary data file
      do i = 1, 20
         read( 1 ) BinArray( i )
      end do

*  Write the extracted values to standard output
      do i = 1, 20
         write( *, * ) BinArray( i )
      end do
      end
.millust end
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section Redefining math error handling with &cmpname
.*
.np
.ix 'math errors'
If you wish to customize math error handling for your application, you
can create your own math error handling procedure.
The following illustrates the procedures for trapping errors by way of
an example.
See the
.book &cmpname User's Guide
for a description of the
.kw fsignal
subroutine and math library error handling.
.np
The main program example "MATHDEMO" is a FORTRAN program that contains
a floating-point divide by zero error,
a floating-point overflow error,
a floating-point underflow error, and
an invalid argument to a math library function.
.code begin
      program mathdemo

* MATHDEMO.FOR - This program forms part of a collection of FORTRAN
*                code that demonstrates how to take over control of
*                math error handling from the run-time system.

* Compile: wfl[386] mathdemo cw87 _matherr

.code break
* Notes:
* (1) We call "cw87" to enable underflow exceptions which are
*     masked (ignored) by default.
* (2) The signal handler must be re-installed after each signal
*     (it can also be re-installed even when there is no signal).
* (3) To prevent compile-time constant folding in expressions,
*     we add log(1.0) which is 0.  We do this for the sake of
*     demonstrating exception handling.

      implicit none

      double precision x, y, z

      call cw87         ! init 80x87 control word

.code break
      call resetFPE     ! install signal handler
      print *, ' '
      print *, 'Divide by zero will be attempted'
      x = 1.0d0 + DLOG( 1.0d0 )
      y = 0.0d0
      z = x / y
      call chkFPE       ! check for exception
      print *, z

.code break
      call resetFPE     ! install signal handler
      print *, ' '
      print *, 'Overflow will be attempted'
      x = 1.2d300 + DLOG( 1.0d0 )
      y = 1.2d300
      z = x * y
      call chkFPE       ! check for exception
      print *, z

.code break
      call resetFPE     ! install signal handler
      print *, ' '
      print *, 'Underflow will be attempted'
      x = 1.14d-300 + DLOG( 1.0d0 )
      y = 2.24d-308
      z = x * y
      call chkFPE       ! check for exception
      print *, z

.code break
      call resetFPE     ! install signal handler
      print *, ' '
      print *, 'Math error will be attempted'
      x = -12.0
      ! an exception will not be generated since the intrinsic function
      ! will validate the argument - if you compile with /om, the "fsqrt"
      ! 80x87 instruction will be generated in-line and an exception
      ! will occur
      y = SQRT( x )
      call chkFPE       ! check for exception
      print *, x, y
      end

.code break
      subroutine resetFPE
      include 'fsignal.fi'
      external fpe_handler
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
      fpe_flag = .false.
      call fsignal( SIGFPE, fpe_handler )
      end

.code break
*$pragma aux fpe_handler parm( value )

      subroutine fpe_handler( sig, fpe )
      integer sig, fpe
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
      fpe_flag = .true.
      fpe_sig = sig
      fpe_fpe = fpe
      end

.code break
*$pragma aux fwait = "fwait"

      subroutine chkFPE
      include 'fsignal.fi'
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
* Notes:
* (1) An fwait is required to make sure that the last
*     floating-point instruction has completed.
* (2) "volatile" is not needed here but would be
*     needed in main program if it references "fpe_flag"
      call fwait()
.code break
      if( volatile( fpe_flag ) ) then
        print *, '*ERROR* exception occurred',
     &           fpe_sig, fpe_fpe
        if( fpe_fpe .eq. FPE_INVALID )then
          print *, 'Invalid'
        else if( fpe_fpe .eq. FPE_DENORMAL )then
          print *, 'Denormalized operand error'
        else if( fpe_fpe .eq. FPE_ZERODIVIDE )then
          print *, 'Divide by zero error'
        else if( fpe_fpe .eq. FPE_OVERFLOW )then
          print *, 'Overflow error'
        else if( fpe_fpe .eq. FPE_UNDERFLOW )then
          print *, 'Underflow error'
        else if( fpe_fpe .eq. FPE_INEXACT )then
          print *, 'Inexact result (precision)then error'
        else if( fpe_fpe .eq. FPE_UNEMULATED )then
          print *, 'Unemulated instruction error'
        else if( fpe_fpe .eq. FPE_SQRTNEG )then
          print *, 'Square root of a negative number error'
        else if( fpe_fpe .eq. FPE_STACKOVERFLOW )then
          print *, 'NDP stack overflow error'
        else if( fpe_fpe .eq. FPE_STACKUNDERFLOW )then
          print *, 'NDP stack underflow error'
        else if( fpe_fpe .eq. FPE_EXPLICITGEN )then
          print *, 'SIGFPE signal raised (software)'
        else if( fpe_fpe .eq. FPE_IOVERFLOW )then
          print *, 'Integer overflow error'
        endif
      else
        print *, '*OK* no exception occurred'
      endif
      end
.code end
.np
The following subroutine illustrates how to enable or disable
particular types of floating-point exceptions.
.code begin
      subroutine cw87

* CW87.FOR
* This subroutine uses the C Library routine "_control87"
* to modify the math coprocessor exception mask.

.code break
* Compile: wfc[386] cw87

      include 'fsignal.fi'

      character*9 status(0:1)/' disabled',' enabled'/
      integer fp_cw, fp_mask, bits, i

.code break
* Enable floating-point underflow since default is disabled.
* The mask defines which bits we want to change (1 means change,
* 0 means do not change).  The corresponding bit in the control
* word (fp_cw) is set to 0 to enable the exception or 1 to disable
* the exception.  In this example, we change only the underflow
* bit and leave the others unchanged.

      fp_mask = EM_UNDERFLOW    ! mask for the bits to set/reset
      fp_cw = '0000'x           ! new bit settings (0=enable/1=disable)
      fp_cw = _control87( fp_cw, fp_mask )

.code break
* Now get up-to-date setting

      fp_cw = _control87( 0, 0 )

      bits = IAND( fp_cw, MCW_EM )
      print '(a,1x,z4)', 'Interrupt Exception Mask', bits
.code break
      i = 0
      if( IAND(fp_cw, EM_INVALID) .eq. 0 ) i = 1
      print *, '  Invalid Operation exception', status(i)
.code break
      i = 0
      if( IAND(fp_cw, EM_DENORMAL) .eq. 0 ) i = 1
      print *, '  Denormalized exception', status(i)
.code break
      i = 0
      if( IAND(fp_cw, EM_ZERODIVIDE) .eq. 0 ) i = 1
      print *, '  Divide-By-Zero exception', status(i)
.code break
      i = 0
      if( IAND(fp_cw, EM_OVERFLOW) .eq. 0 ) i = 1
      print *, '  Overflow exception', status(i)
.code break
      i = 0
      if( IAND(fp_cw, EM_UNDERFLOW) .eq. 0 ) i = 1
      print *, '  Underflow exception', status(i)
.code break
      i = 0
      if( IAND(fp_cw, EM_PRECISION) .eq. 0 ) i = 1
      print *, '  Precision exception', status(i)
      end
.code end
.np
The following subroutine illustrates how to replace the run-time
system's math error handler.
Source code similar to this example is provided with the software
(look for the file
.fi _matherr.for
.ct ).
.code begin
*
* _MATHERR.FOR  : math error handler
*
* Compile: wfc[386] _matherr

*$pragma aux __imath2err "*_" parm( value, reference, reference )
*$pragma aux __amath1err "*_" parm( value, reference )
*$pragma aux __amath2err "*_" parm( value, reference, reference )
*$pragma aux __math1err "*_" parm( value, reference )
*$pragma aux __math2err "*_" parm( value, reference, reference )
*$pragma aux __zmath2err "*_" parm( value, reference, reference )
*$pragma aux __qmath2err "*_" parm( value, reference, reference )


.code break
      integer function __imath2err( err_info, arg1, arg2 )
      integer err_info
      integer arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_POW )
              print *, 'arg2 cannot be <= 0'
          case( FUNC_MOD )
              print *, 'arg2 cannot be 0'
          end select
      end if
      __imath2err = 0
      end


.code break
      real function __amath1err( err_info, arg1 )
      integer err_info
      real arg1
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_COTAN )
              print *, 'overflow'
          end select
      end if
      __amath1err = 0.0
      end


.code break
      real function __amath2err( err_info, arg1, arg2 )
      integer err_info
      real arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_MOD )
              print *, 'arg2 cannot be 0'
          end select
      end if
      __amath2err = 0.0
      end


.code break
      double precision function __math1err( err_info, arg1 )
      integer err_info
      double precision arg1, __math2err
      __math1err = __math2err( err_info, arg1, arg1 )
      end


.code break
      double precision function __math2err( err_info, arg1, arg2 )
      integer err_info
      double precision arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
.code break
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_SQRT )
              print *, 'argument cannot be negative'
          case( FUNC_ASIN, FUNC_ACOS )
              print *, 'argument must be less than or equal to one'
          case( FUNC_ATAN2 )
              print *, 'both arguments must not be zero'
          case( FUNC_POW )
              if( arg1 .eq. 0.0 )then
                  print *, 'a zero base cannot be raised to a ',
     &                    'negative power'
              else ! base < 0 and non-integer power
                  print *, 'a negative base cannot be raised to a ',
     &                    'non-integral power'
              endif
          case( FUNC_LOG, FUNC_LOG10 )
              print *, 'argument must not be negative'
          end select
.code break
      else if( ( err_info .and. M_SING ) .ne. 0 )then
          if( ( ( err_info .and. FUNC_MASK ) .eq. FUNC_LOG ) .or.
     &        ( ( err_info .and. FUNC_MASK ) .eq. FUNC_LOG10 ) )then
              print *, 'argument must not be zero'
          endif
.code break
      else if( ( err_info .and. M_OVERFLOW ) .ne. 0 )then
          print *, 'value of argument will cause overflow condition'
.code break
      else if( ( err_info .and. M_UNDERFLOW ) .ne. 0 )then
          print *, 'value of argument will cause underflow ',
     &              'condition - return zero'
      end if
      __math2err = 0
      end


.code break
      complex function __zmath2err( err_info, arg1, arg2 )
      integer err_info
      complex arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
.code break
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_POW )
              ! arg1 is (0,0)
              if( imag( arg2 ) .ne. 0 )then
                  print *, 'a zero base cannot be raised to a',
     &            ' complex power with non-zero imaginary part'
              else
                  print *, 'a zero base cannot be raised to a',
     &            ' complex power with non-positive real part'
              endif
          end select
      end if
      __zmath2err = (0,0)
      end


.code break
      double complex function __qmath2err( err_info, arg1, arg2 )
      integer err_info
      double complex arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
          select( err_info .and. FUNC_MASK )
          case( FUNC_POW )
              ! arg1 is (0,0)
              if( imag( arg2 ) .ne. 0 )then
                  print *, 'a zero base cannot be raised to a',
     &             ' complex power with non-zero imaginary part'
              else
                  print *, 'a zero base cannot be raised to a',
     &              ' complex power with non-positive real part'
              endif
          end select
      end if
      __qmath2err = (0,0)
      end
.code end
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section The compiler cannot find my include files
.*
.np
.ix 'message' 'unable to find files'
.ix 'unable to find files'
.ix 'message' 'include files'
.ix 'files' 'unable to find'
In order to locate your INCLUDE files, the compiler first searches
your current directory, then each directory listed in the
.ev &incvarup
environment variable (in the order that they are specified).
If the compiler reports that it is unable to find one of your include
files, change the
.ev &incvarup
environment variable by adding the directory path to your include
files.
For more information on setting the environment variable, refer to the
"Compiling an Application"
.if '&target' eq 'QNX' .do begin
chapter.
.do end
.el .do begin
chapter of the
.book &cmpname User's Guide.
.do end
.*
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section The compiler cannot find "stdio.h"
.*
.np
.ix 'message' 'unable to find files'
.ix 'unable to find files'
.ix 'message' 'header files'
.ix 'header files'
.ix 'files' 'unable to find'
The standard header files are usually located in the sub-directory
that the &cmpname compiler is installed in.
Suppose that the header files are located in the sub-directory
.fi &dr3.&pathnam.&hdrdir..
If the compiler indicates (through an error message) that it is unable
to locate the file
.fi STDIO.H
.ct , we have forgotten something.
There are two ways to indicate to the &cmpname compiler the location
of the standard header files.
.autopoint
.point
use the
.ev &incvarup
environment variable
.if '&target' eq 'QNX' .do begin
.point
use the "i" option (&cmpname)
.point
use the "I" option (&wclname)
.do end
.el .do begin
.point
use the "i" option (&cmpname, &wclname)
.do end
.endpoint
.np
.ix '&cmpname options' 'i'
The use of the environment variable is the simplest way to ensure that
the include files will be found.
.if '&target' eq 'QNX' .do begin
.ix 'user initialization file'
For instance, if you include the following line in your user
initialization file,
.do end
.el .do begin
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT' 'system initialization file'
For instance, if you include the following line in your system
initialization file,
.fi AUTOEXEC.BAT,
.do end
.millust begin
&setcmd &incvar=&dr3.&pathnam.&hdrdir
.millust end
.pc
the &cmpname compiler will be able to find the standard include files.
The use of the "i" option is another way to give the directory name of
the standard include files.
.exam begin
&prompt.&ccmd16 myfile.c -i&dr3.&pathnam.&hdrdir
    or
&prompt.&ccmd32 myfile.c -i&dr3.&pathnam.&hdrdir
    or
&prompt.&pcmd16 myfile.cpp -i&dr3.&pathnam.&hdrdir
    or
&prompt.&pcmd32 myfile.cpp -i&dr3.&pathnam.&hdrdir
.exam end
.np
The usual manner that these methods are combined is as follows.
The
.ev &incvarup
environment variable is used to give the location of the standard C
library header files.
Any directories of header files local to a specific programming
project are often candidates for the "i" option method.
.if '&target' eq 'QNX' .do begin
See the "#include File Processing" section of the appropriate chapter.
.do end
.el .do begin
See the "&product #include File Processing" section of the chapter
entitled "The &product Compilers" in the
.book &cmpname User's Guide
for more details.
.do end
.if '&target' eq 'QNX' .do begin
Note that
.kw &wclcmd
automatically directs the compiler to include
.fi &dr3.&pathnam.&hdrdir
in its search path.
.do end
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section The linker reports a "stack segment not found" error
.*
.np
.ix 'message' 'stack segment not found'
.ix 'stack segment not found'
.ix 'message' 'no starting address found'
.ix 'no starting address found'
The linker usually reports the error "1014: stack segment not found"
when it is unable to find the run-time libraries required to link
your application.
To ensure you are linking with the correct run-time libraries,
check to see that your link command contains the correct "SYSTEM"
directive.
As well, the
.ev &pathvarup.
environment variable should be pointing to the directory containing
the &cmpname software.
For a &cmpname application, if this error is followed by the error
"1023: no starting address found", it may indicate that you are
attempting to link code that does not have a main program
procedure.
Ensure that you include your main program object module with your
linker directives.
.np
With &lang, "STACK" is a reserved word.
If you use "STACK" as the name of a common block, this may also result
in the "Stack Segment Not Found" error.
Check the names of your common blocks and rename them if necessary.
.*
.do end
.*
.section Resolving an "Undefined Reference" linker error
.*
.np
.ix 'linker' 'undefined references'
.ix 'message' 'undefined references'
.ix 'undefined references'
The &lnkname builds an executable file by a process of resolving
references to functions or data items
that are declared in other source files.
Certain conditions arise that cause the linker to generate an
"Undefined Reference" error message.
An "Undefined Reference" error message will be displayed by the linker
when it cannot find a function or data item that was referenced in
the program.
Verify that you have included all the required object modules in the
linker command and that you are linking with the correct libraries.
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.np
.ix 'system'
.ix 'library'
The "SYSTEM" linker directive should be used to indicate the target
environment for the executable.
This directive specifies the format of the executable and the
libraries for the target environment.
Verify that the
.ev &pathvarup.
environment variable is set to the directory that &cmpname was
installed in since it is used to complete the library path in the
"SYSTEM" directive.
You may also explicitly include a library using the "LIBRARY" linker
directive.
.np
.ix 'undefined references' '_cstart_'
.ix '_cstart_'
If the linker reports an unresolved reference for "_cstart_", this
indicates that the linker could not find the &lang run-time libraries.
.*
In 16-bit applications, the &lang run-time libraries for the medium
memory model (/mm) and the floating-point calls floating-point model
(/fpc) would be
.fi flibm.lib.
In 32-bit applications, the &lang run-time libraries for the flat
memory model would be
.fi flib.lib.
Verify that the "LIB" environment variable has been set to point to
the correct
.ev &pathvarup.
library directories and that the library corresponds to the memory and
floating-point model that you selected.
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.ix '_cstart_'
There are a couple of "undefined references" that require some
explanation.
.begnote
.mnote _cstart_
The unresolved reference for
.ix 'undefined references' '_cstart_'
.ix '_cstart_'
.id _cstart_
indicates that the linker cannot find the &lang run-time libraries.
.*
The 16-bit C run-time libraries for the small memory
model are
.fi clibs.lib
and, either
.fi maths.lib,
or
.fi math87s.lib.
The 32-bit C run-time libraries for the flat memory model
compiled for the register-based argument passing model are
.fi clib3r.lib
and, either
.fi math3r.lib,
or
.fi math387r.lib.
Ensure that the
.ev &pathvarup.
environment variable is set to the directory that &cmpname
was installed in.
.ix 'undefined references' '_fltused_'
.ix '_fltused_'
.mnote _fltused_
The
.id _fltused_
undefined reference indicates that floating-point arithmetic has
been used in the modules that exhibit this error.
The remedy is to ensure that the linker can find the appropriate math
library.
For the 16-bit small memory model, it is either
.fi maths.lib,
or
.fi math87s.lib
For the 32-bit register-based argument passing model, it is either
.fi math3r.lib,
or
.fi math387r.lib
depending on which floating-point option is used.
Ensure that the
.ev &pathvarup.
environment variable is set to the directory that &cmpname
was installed in.
.ix 'undefined references' '_small_code_'
.ix '_small_code_'
.mnote _small_code_
If this undefined reference occurs when you are trying to create a
16-bit application, we have saved you many hours of debugging!
The reason for this undefined reference is that the "main" entry
point has been compiled for a big code model (in any one of medium,
large, or huge memory models).
Any of the modules that have this undefined reference have been
compiled for a small code model (in any one of small or compact memory
models) and as such do not have the correct return instructions.
You should recompile the modules so that all the modules are
compiled for the same memory model.
Combining source modules compiled for different memory models is very
difficult and often leads to strange bugs.
If your program has special considerations and this reference causes
you problems, there is a "work-around".
You could resolve the reference with a PUBLIC declaration in an
assembler file or code the following in &cmpname..
.millust begin
/* rest of your module */

void _small_code( void )
{}
.millust end
.pc
The code generator will generate a single RET instruction
with the public symbol
.id _small_code_
attached to it.
The common epilogue optimizations will probably combine this
function with another function's RET instruction and you will not
even pay the small penalty of one byte of extra code.
.np
There may be another cause of this problem, the "main" function must
be entered in lower case letters ("Main" or "MAIN" are not
identified as being the same as "main" by the compiler).
The compiler will identify the module that contains the definition
of the function "main" by creating the public definition of either
.id _small_code_
or
.id _big_code_
depending on the memory model it was compiled in.
.ix 'undefined references' '_big_code_'
.ix '_big_code_'
.mnote _big_code_
Your module that contains the "main" entry point has been compiled
with a 16-bit small code model (small or compact).
The modules that have this undefined reference have been compiled in
16-bit big code models (medium, large, or huge).
You should recompile the modules so that all the modules are
compiled in the same memory model.
See the explanation for
.id _small_code_
for more details.
.ix 'undefined references' 'main_'
.ix 'main_'
.mnote main_
All C programs
.if '&target' ne 'QNX' .do begin
.ct , except applications developed specifically for Microsoft Windows,
.do end
must have a function called "main".
The name "main" must be in lower case for the compiler to generate
the appropriate information in the "main" module.
.*
.if '&target' ne 'QNX' .do begin
.mnote WINMAIN
.ix 'undefined references' 'WinMain'
.ix 'WinMain'
All Windows programs must have a function called "WinMain".
The function "WinMain" must be declared "pascal" in order that the
compiler generate the appropriate name in the "WinMain" module.
.do end
.endnote
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section Why local variable values are not maintained between subprogram calls
.*
.np
.ix 'variables' 'set to zero'
.ix 'SAVE'
.ix 'initializing' 'variables'
By default, the local variables for a subprogram are stored on the
stack and are not initialized.
When the subprogram returns, the variables are popped off the stack
and their values are lost.
If you want to preserve the value of a local variable, after the
execution of a RETURN or END statement in a subprogram, the FORTRAN 77
SAVE statement or the "save" compiler option can be used.
.np
Using the &lang SAVE statement in your program allows you to
explicitly select which values you wish to preserve.
The SAVE statement ensures that space is allocated for a local
variable from static memory and not the stack.
Include a SAVE statement in your &lang code for each local variable
that you wish to preserve.
.np
To automatically preserve all local variables, you can use the "save"
compiler option.
This option adds code to initialize and allocate space for each local
variable in the program.
This is equivalent to specifying a SAVE statement.
The "save" option makes it easier to ensure that all the variables are
preserved during program execution, but it increases the size of the
code that is generated.
You may wish to use this option during debugging to help diagnose bugs
caused by corrupted local values.
Usually, it is more efficient to use SAVE statements rather than the
general "save" compiler option.
You should selectively use the SAVE statement for each subprogram
variable that you want to preserve until the next call.
This leads to smaller code than the "save" option and avoids the
overhead of allocating space and initializing values unnecessarily.
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Why my variables are not set to zero
.*
.np
.ix 'initialized global data'
.ix 'variables' 'set to zero'
.ix 'clearing' 'variables'
The linker is the program that handles the organization of code and
data and builds the executable file.
C guarantees that all global and static uninitialized data
will contain zeros.
The "BSS" region contains all uninitialized global and static data for
C programs (the name "BSS" is a remnant of the early UNIX C
compilers).
Most C compilers take advantage of this situation by not explicitly
storing all the zeros to achieve smaller executable file sizes.
.if '&target' ne 'QNX' .do begin
In order for the program to work correctly, there must be some code
(that will be executed before "main") that will clear the "BSS"
region.
The code that is executed before "main" is called "startup" code.
.ix 'BSS segment'
The linker must indicate to the startup code where the "BSS" region is
located.
In order to do this, the &lnkname (&lnkcmdup) treats the "BSS"
segment (region) in a special manner.
.do end
.if '&target' eq 'QNX' .do begin
The &lnkname (&lnkcmdup) treats the "BSS" segment (region) in a
special manner.
.do end
The special variables '_edata' and '_end' are constructed by the
&lnkname so that the startup code knows the beginning and end of
the "BSS" region.
.if '&target' ne 'QNX' .do begin
.np
Some users may prefer to use the linker provided by another compiler
vendor for development.
In order to have the program execute correctly, some extra care must
be taken with other linkers.
.ix 'Microsoft' 'LINK'
.ix 'Microsoft' 'LINK386'
.ix 'LINK'
.ix 'LINK386'
For instance, with the Microsoft linker (LINK) you must ensure that
the '/DOSSEG' command line option is used.
.ix 'Phar Lap' '386LINK'
.ix '386LINK'
With the Phar Lap Linker, you must use the "-DOSORDER" command line
option.
In general, if you must use other linkers, extract the module that
contains
.id _cstart
from
.fi clib?.lib
(? will change depending on the memory model) and specify the object
file containing
.id _cstart
as the first object file to be processed by the linker.
The object file will contain the information necessary for the linker
to build the executable file correctly.
.do end
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section What does "size of DGROUP exceeds 64K" mean for 16-bit applications?
.*
.np
.ix 'DGROUP size exceeds 64K'
.ix 'size of DGROUP exceeds 64K'
This question applies to 16-bit applications.
There are two types of segments in which data is stored.
The two types of segments are classified as "near" and "far".
There is only one "near" segment while there may be many "far" segments.
The single "near" segment is provided for quick access to data but is
limited to less than 64K in size.
Conversely, the "far" segments can hold more than 64K of data but
suffer from a slight execution time penalty for accessing the data.
The "near" segment is linked by arranging for the different parts of
the "near" segment to fall into a group called DGROUP.
See the section entitled "Memory Layout" in
.if '&target' eq 'QNX' .do begin
this guide
.do end
.el .do begin
the
.book &lnkname User's Guide
.do end
for more details.
.np
The 8086 architecture cannot support segments larger than 64K.
As a result, if the size of DGROUP exceeds 64K, the program cannot
execute correctly.
The basic idea behind solving this problem is to move data out of the
single "near" segment into one or more "far" segments.
Of course, this solution does not come without any penalties.
The penalty is paid in decreased execution speed as a result of
accessing "far" data items.
The magnitude of this execution speed penalty depends on the behavior
of the program and, as such, cannot be predicted (i.e., we cannot say
that the program will take precisely 5% longer to execute).
The specific solution to this problem depends on the memory model
being used in the compilation of the program.
.np
If you are compiling with the tiny, small, or medium memory models
then there are two possible solutions.
The first solution involves changing the program source code so that
any large data items are declared as "far" data items and accessed with
"far" pointers.
The addition of the "far" keyword into the source code makes the source
code non-portable but this might be an acceptable tradeoff.
See the "Advanced Types" chapter in the
.book &company C Language Reference
manual for details on the use of the "near" and "far" keywords.
The second solution is to change memory models and use the large or
compact memory model.
The use of the large or compact memory model allows the compiler to
use "far" segments to store data items that are larger than 32K.
.np
The large and compact memory models will only allocate data items into
"far" segments if the size of the data item exceeds 32K.
If the size of DGROUP exceeds 64K then a good solution is to reduce
the size threshold so that smaller data items will be stored into "far"
segments.
The relevant compiler option to accomplish this task is "zt<num>".
The "zt" option sets a data size threshold which, if exceeded, will
allocate the data item in "far" segments.
For instance, if the option "zt100" is used, any data item larger than
100 bytes will be allocated in "far" segments.
A good starting value for the data threshold is 32 bytes (i.e.,
"zt32").
The number of compilations necessary to reduce the size of DGROUP for
a successful link with &lnkcmdup depends on the program.
Minimally, any files which allocate a lot of data items should be
recompiled.
The "zt<num>" option should be used for all subsequent compiles, but
the recompilation of all the source files in the program is not
necessary.
If the "DGROUP exceeds 64K" &lnkcmdup error persists, the threshold
used in the "zt<num>" option should be reduced and all of the source
files should be recompiled.
.*
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section What does "NULL assignment detected" mean in 16-bit applications?
.*
.np
.ix 'NULL assignment detected'
.ix 'NULL assignment detected' 'debugging'
.ix 'debugging' 'NULL assignment detected'
.ix 'debugging' 'memory bugs'
.ix 'debugging' 'techniques'
.ix 'ISO/ANSI standard' 'NULL'
This question applies to 16-bit applications.
The C language makes use of the concept of a
.id NULL
pointer.
The
.id NULL
pointer cannot be dereferenced according to the ISO standard.
The &cmpname compiler cannot signal the programmer when the
.id NULL
address has been written to or read from because the Intel-based
personal computers do not have the necessary hardware support.
The best that the run-time system can do is help programmers find
these sorts of errors through indirect means.
The lower 32 bytes of "near" memory have been seeded with 32 bytes of
the value 0x01.
The C run-time function "_exit" checks these 32 bytes to ensure that
they have not been written over.
Any modification of these 32 bytes results in the "NULL assignment
error" being printed before the program terminates.
.np
Here is an overview of a good debugging technique for this sort of
error:
.autopoint
.point
use the &dbgname to debug the program
.point
let the program execute
.point
find out what memory has been incorrectly modified
.point
set a watchpoint on the modified memory address
.point
restart the program with the watchpoint active
.point
let the program execute, for a second time
.point
when the memory location is modified, execution will be suspended
.endpoint
.np
We will go through the commands that are executed for this debugging
session.
First of all, we invoke the &dbgname from the command line as
follows:
.millust begin
&prompt.&dbgcmd myprog
.millust end
.pc
Once we are in the debugger type:
.millust begin
DBG>go
.millust end
.pc
The program will now execute to completion.
At this point we can look at the output screen with the debugger
command, "FLIP".
.millust begin
DBG>flip
.millust end
.pc
We would see that the program had the run-time error "NULL assignment
detected".
At this point, all we have to do is find out what memory locations
were modified by the program.
.np
The following command will display the lower 16 bytes of "near" memory.
.millust begin
DBG>examine __nullarea
.millust end
.pc
The command should display 16 bytes of value 0x01.
Press the space bar to display the next 16 bytes of memory.
This should also display 16 bytes of value 0x01.
Notice that the following data has two bytes which have been
erroneously modified by the program.
.code begin
__nullarea     01 01 56 12 01 01 01 01-01 01 01 01 01 01 01 01
__nullarea+16  01 01 01 01 01 01 01 01-01 01 01 01 01 01 01 01
.code end
.np
The idea behind this debugging technique is to set a watchpoint on the
modified memory so that execution of the program will be suspended
when it modifies the memory.
The following command will "watch" the memory for you.
.millust begin
DBG>watch __nullarea+2
.millust end
.pc
There has to be a way to restart the program without leaving the
&dbgname so that the watchpoint is active during a subsequent
execution of the program.
The &dbgname command "NEW" will reload the program and prepare for a
new invocation of the program.
.millust begin
DBG>new
DBG>go
.millust end
.pc
The &dbgname command "GO" will start execution of the program.
You may notice that the program executes much slower than usual but
eventually the debugger will show the part of the program that
modified the two bytes.
At this point, you might want to clear the watchpoint and proceed to
debug why the memory was modified.
The command to clear the watchpoint is:
.millust begin
DBG>watch/clear 1
.millust end
.pc
The "1" indicates that you want watchpoint number 1 to be cleared.
Typing "WATCH" by itself will print out all active watchpoints.
The above technique is generally useful for any type of memory
overwrite error provided you know which memory location has been
overwritten.
.hint
The &dbgname allows many commands to have short forms.
For instance, the "EXAMINE" command can be shortened to an "E".
We used the full commands in the examples for clarity.
.ehint
.*
.do end
.*
.section What "Stack Overflow!" means
.*
.np
.ix 'stack overflow'
.ix 'debugging' 'stack overflow'
.ix 'debugging' 'techniques'
The memory used for local variables is allocated from the function
call stack although the &company compilers will often use registers
for local variables.
The size of the function call stack is limited at link-time
and it is possible to exceed the amount of stack space during execution.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The &company run-time library will perform checks whenever a large
amount of stack space is required by a function but it is up to the
user to check stack requirements before calling a &company run-time
function.
Compiling programs with stack checking will ensure that there is
enough stack space to call a &company run-time function.
.do end
.np
There are various ways of protecting against stack overflow errors.
First, one should minimize the number of recursive functions used
in an application program.
This can be done by recoding recursive functions to use loops.
.if '&lang' eq 'FORTRAN 77' .do begin
.pc
The user may also optionally force the compiler to use static storage
for all local variables (&cmpname "save" option).
This would eliminate most stack problems for FORTRAN programs.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Keep the amount of stack used in functions to a minimum by using and
reusing static arrays whenever possible.
.do end
These techniques will reduce the amount of stack space required
but there still may be times where the default amount of stack space
is insufficient.
The &lnkname (&lnkcmdup) allows the user to set the amount of
stack space at link-time through the directive
"OPTION STACK=size" where size may be specified in bytes with an optional
"k" suffix for kilobytes (1024 bytes).
.exam begin
option stack=9k
.exam end
.if '&lang' eq 'FORTRAN 77' .do begin
.np
Note that with the &cmpname run-time system, the I/O routines require 4k
of stack space.
If your application requires 5K of stack space, set aside 9K to allow
for 4K of I/O stack space in addition to the stack space required by
the application.
.do end
.np
Debugging a program that reports a stack overflow error can be
accomplished with the following sequence.
.autonote
.note
Load your application into the debugger
.note
Set a breakpoint at
.id __STKOVERFLOW
.note
Run the application until the breakpoint at
.id __STKOVERFLOW
is triggered
.note
Issue the debugger "show calls" command.
This will display a stack traceback giving you the path of calls
that led up to the stack overflow situation.
.endnote
.np
The solution to the stack overflow problem at this point depends on the
programmer.
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section What are the probable causes of a General Protection Fault in 32-bit applications?
.*
.np
.ix 'Unexpected Interrupt'
.ix 'DEBUG option'
.ix 'STACK option'
If you are running a 32-bit application using DOS/4GW, a program crash
may report an "Unexpected Interrupt 0D" general protection fault
error.
The Phar Lap DOS extender would report an "Abnormal Program
Termination" general protection fault error.
This often indicates that something in your program has tried to
access an invalid memory location.
In a &cmpname application, the most likely causes of a general
protection fault are:
.*
.begbull
.bull
Attempting to access an array out of bounds.
.bull
Running out of stack space.
.bull
Passing incorrect parameter types to a function.
.endbull
.*
.np
To help locate the cause of the protection fault, compile your program
with the "debug" and "stack" options.
With these options, code will be added to your application to help
identify these problems and generate run-time error messages when they
are encountered.
In addition, the "stack" option checks for stack overflow conditions by
including code at the beginning of each subprogram.
.np
If you still encounter general protection faults after compiling with
"debug" and "stack", then debug the program using the debugger.
This will help to identify the location of the crash and the state of
your parameters and variables at the time of the crash.
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section Which floating-point compiler option should I use for my application?
.*
.np
.ix 'fpi option'
.ix 'fpi87 option'
.ix 'fpc option'
.ix 'floating-point options'
The answer to this question depends on the expected target machines
for your application.
If you know that a co-processor will be available, use the "fpi87"
compiler option to optimize the run-time performance of the
application.
.np
When you are running a &lang application on a machine with or without
a co-processor and you want to favour the use of emulation libraries
over code size, use the "fpc" option.
The "fpc" option will also take advantage of an 80x87 co-processor if
it is available.
If your application needs to be flexible enough to run either with or
without a co-processor, the "fpc" option is recommended.
.np
The "fpi" option is the default floating-point option and can be used
with or without a co-processor.
On machines that do not have a co-processor, you may notice that
programs compiled using "fpc" run faster than those compiled with "fpi".
This occurs because the "fpc" option uses the floating-point libraries
directly whereas the "fpi" option interfaces with an emulator interrupt
handler.
Although the "fpi" option is slower than "fpc" without a co-processor,
the code that it generates is smaller.
.np
When you are running an application that has been compiled with "fpi",
the startup code checks to determine whether a math co-processor is
present.
If it is not present, the emulator hook is installed at the INT 7h
interrupt to manage the co-processor requests and convert them to the
emulation library calls.
Each time a floating-point operation is requested, the processor
issues an INT 7h.
.np
For 16-bit applications, the interrupt handler overhead accounts for
the performance discrepancy between the "fpc" and "fpi" options.
.np
For 32-bit applications, the manner in which this interrupt is handled
depends on the DOS extender.
Depending on the DOS extender, there are two methods of managing
floating-point instructions through the interrupt handler.
The DOS extender will either pass the interrupt directly to the INT 7h
handler or it will perform some intermediary steps.
Similarly, there is a delay after the interrupt as control is passed
back through the DOS extender.
Passing floating-point handling from the DOS extender to the interrupt
handler results in the performance degradation.
This performance degradation may vary across DOS extenders.
It is the overhead of transferring the call through an interrupt that
leads to the speed difference between "fpi" and "fpc".
If you need to run an application on machines without math
co-processors, and you want to ensure that your performance is
optimal, build your application using the "fpc" option rather than
"fpi".
.np
In a Windows environment, both the "fpi87" and the "fpi" options will
use floating-point emulation if a co-processor is not available.
Windows floating-point emulation is provided through &company's
"WEMU387.386".
"WEMU387.386" is royalty free and may be redistributed with your
application.
For machines that do not have a math co-processor, install
"WEMU387.386" as a device in the [386Enh] section of the Windows
SYSTEM.INI file to handle the floating-point operations.
Note that the speed of code using "WEMU387.386" on machines without a
co-processor will be much slower than code compiled with the "fpc"
option that always uses floating-point libraries.
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Why redefinition errors are issued from &lnkcmdup
.*
.np
.ix 'porting' 'from Microsoft C'
.ix 'porting' 'from UNIX'
This question comes up often in discussions about porting old UNIX or
Microsoft C programs.
The problem stems from the forgiving nature of early UNIX linkers.
In early C code, it was common to define header files like this:
.exam begin
/* define global variables */
int line_count;
int word_count;
int char_count;
.exam end
.pc
The header file would then be included in many different modules.
The C compiler would generate a definition of each variable in each
module and leave it to the linker to pick one and resolve all
references to one variable.
The development of the ANSI C standard made this practice
non-conforming.
The &company C compiler is an ISO/ANSI C compiler and as such, is not
required to support this obsolete behavior.
The effect is that &lnkcmdup will report redefinition errors.
The header file must be coded in such a way that the variables are
defined in one module.
One way to do this is as follows:
.exam begin
#ifdef DEFINE_HERE
#define GLOBAL
#else
#define GLOBAL  extern
#endif
/* define global variables */
GLOBAL int line_count;
GLOBAL int word_count;
GLOBAL int char_count;
.exam end
.pc
In most modules, the macro "DEFINE_HERE" will not be defined so the
file will be equivalent to:
.exam begin
/* define global variables */
extern int line_count;
extern int word_count;
extern int char_count;
.exam end
.pc
In one module, the macro "DEFINE_HERE" must be defined before the
header file is included.
This can be done by defining the macro on the command line or by
coding like this:
.exam begin
#define DEFINE_HERE
#include "globals.h"
.exam end
.*
.do end
.*
.if '&target' ne 'QNX' .do begin
.*
.section How more than 20 files at a time can be opened
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="growhndl"      value="_grow_handles".
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="growhndl"      value="GROWHANDLES".
.do end
.np
.ix 'opening more than 20 files'
.ix 'files' 'more than 20'
The number of file handles allowed by &cmpname is initialized to 20 in
.fi stdio.h,
but this can be changed by the application developer.
To change the number of file handles allowed with &cmpname, follow the
steps outlined below.
.autonote
.note
Let
.id n
represent the number of files the application developer wishes to have
open.
Ensure that the
.us stdin,
.us stdout,
.us stderr,
.us stdaux,
and
.us stdprn
files are included in the count.
.note
Change the
.fi CONFIG.SYS
file to include "files=n" where "n" is the number of file handles
required by the application plus an additional 5 handles for the
standard files (this applies to DOS 5.0).
The number "n" may vary depending on your operating system and
version.
If you are running a network such as Novell's NetWare, this will also
affect the number of available file handles.
In this case, you may have to increase the number specified in the
"files=n" statement.
.note
Add a call to
.id &growhndl
.ix '&growhndl'
in your application.
.endnote
.np
The following example illustrates the use of
.id &growhndl..
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin
/*
 *  HANDLES.C
 *  This C program grows the number of file handles so
 *  more than 16 files can be opened. This program
 *  illustrates the interaction between _grow_handles and
 *  the DOS 5.0 file system. If you are running a network
 *  such as Novell's NetWare, this will also affect the
 *  number of available file handles. In the actual trial,
 *  FILES=40 was specified in CONFIG.SYS.
 */
.exam break

#include <stdio.h>

void main()
{
    int    i, j, maxh, maxo;
    FILE  *temp_files[50];

    for( i = 25; i < 40; i++ ) {
        /* count 5 for stdin,stdout,stderr,stdaux,stdprn */
        printf( "Trying for %2.2d handles...", 5 + i );
        maxh = _grow_handles( 5 + i );
        maxo = 0;
        for( j = 0; j < maxh; j++ ) {
            temp_files[j] = tmpfile();
            if( temp_files[j] == NULL )break;
            maxo++;
        }
        printf( " %d/%d temp files opened\n", maxo, maxh );
        for( j = 0; j < maxo; j++ ) {
            fclose( temp_files[j] );
        }
    }
}
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
*  FHANDLES.FOR
*
*  This FORTRAN program grows the number of file handles so
*  more than 16 files can be opened. This program
*  illustrates the interaction between GROWHANDLES and
*  the DOS 5.0 file system. If you are running a network
*  such as Novell's NetWare, this will also affect the
*  number of available file handles. In the actual trial,
*  FILES=40 was specified in CONFIG.SYS.

.exam break
*  Compile/Link: set finclude=\watcom\src\fortran
*                wfl[386] fhandles

*     Get proper typing information from include file
      include 'fsublib.fi'

      integer i, j, maxh, maxo
      integer tmpfile
      integer units(7:57)

.exam break
      do i = 25, 40
*       Count 5 for stdin, stdout, stderr, stdaux,
*       and stdprn
        print 100, 5 + i
        maxh = growhandles( 5 + i )
        print *, 'Growhandles=',maxh
        maxo = 0
.exam break
        do j = 7, 7 + maxh
            print *, 'Attempting file', j
            units(j) = tmpfile(j)
            if( units(j) .eq. 0 )goto 10
            maxo = maxo + 1
        enddo
.exam break
10      print 101, maxo, maxh
        do j = 7, 7 + maxo
            close( units(j) )
        enddo
      enddo
.exam break
100   format('Trying for ',I2,' handles... ',$)
101   format(I2,'/',I2,' temp files opened')
      end

.exam break
      integer function tmpfile( un )
      integer un, ios
      open( unit=un, status='SCRATCH', iostat=ios )
      if( ios .eq. 0 )then
        write( unit=un, fmt='(I2)', err=20 ) un
        tmpfile = un
        return
      endif
20    tmpfile = 0
      end
.exam end
.do end
.*
.do end
.*
.section How source files can be seen in the debugger
.*
.np
.ix 'debugging information' 'source file'
.ix 'debugging information' 'line numbering'
.ix 'debugging information' 'types'
.ix 'debugging information' 'local variables'
.ix 'debugging information' 'global variables'
The selection and use of debugging information is important for
getting the most out of the &dbgname..
If you are not able to see your source code in the &dbgname source
window, there are three areas where things may have gone wrong,
namely:
.autopoint
.point
using the correct option for the &cmpname..
.point
using the correct directives for the &lnkname..
.point
using the right commands in the &dbgname..
.endpoint
.np
.ix 'debugging information' '&cmpname'
The &cmpname compiler takes &lang source and creates an object file
containing the generated code.
By default, no debugging information is included in the object file.
The compiler will output debugging information into the object file if
you specify a debugging option during the compile.
There are two levels of debugging information that the compiler can
generate:
.ix '&cmpname options' 'd1'
.ix '&cmpname options' 'd2'
.autopoint
.point
Line numbers and local variables ("d2" option)
.point
Line numbers ("d1" option)
.endpoint
.np
The options are used to determine how much debugging information
will be visible when you are debugging a particular module.
If you use the "d2" option, you will be able to see your source file
and display your local variables.
The "d1" option will display the source but will not give you access
to local variable information.
.np
.ix 'debugging information' '&lnkcmdup'
The &lnkname (&lnkcmdup) is the tool that puts together a
complete program and sets up the debugging information for all the
modules in the executable file.
There is a linker directive that indicates to the linker when it
should include debugging information from the modules.
There are five levels of debugging information that can be collected
during the link.
These are:
.autopoint
.point
global names (DEBUG)
.point
global names, line numbers (DEBUG LINE)
.point
global names, types (DEBUG TYPES)
.point
global names, local variables (DEBUG LOCALS)
.point
all of the above (DEBUG ALL)
.endpoint
.pc
Notice that global names will always be included in any request for
debugging information.
The debugging options can be combined
.millust begin
DEBUG LINE, TYPES
.millust end
.pc
with the above directive resulting in full line number and typing
information being available during debugging.
The directives are position dependent so you must precede any object
files and libraries with the debugging directive.
For instance, if the file
.fi mylink.lnk
contained:
.millust begin
#
# invoke with: &lnkcmd @mylink
#
file main
debug line
file input, output
debug all
file process
.millust end
.pc
then the modules
.fi input
and
.fi output
will have global names and source line information available during
debugging.
All debugging information in the module
.fi process
will be available during debugging.
.hint
A subtle point to debugging information is that all the modules will
have global names available if any debugging directive is used.
In the above example, the module
.fi main
will have global name information even though it does not have a DEBUG
directive preceding it.
.ehint
.np
It is preferable to have one DEBUG directive before any FILE and
LIBRARY directives.
You might wonder if this increases the size of the executable file so
that it will occupy too much memory during debugging.
The debugging information is loaded "on demand" by the debugger during
the debugging session.
A small amount of memory (40k default, selectable with the &dbgname
"dynamic" command line option) is used to hold the most recently used
module debugging information.
In practice, this approach saves a lot of memory because most
debugging information is never used.
The overhead of accessing the disk for debugging information is
negligible compared to accessing the source file information.
In other words, you can have as much debugging information as you want
included in the executable file without sacrificing memory required by
the program.
See the section entitled "The DEBUG Directive" in
.if '&target' eq 'QNX' .do begin
this guide
.do end
.el .do begin
the
.book &lnkname User's Guide
.do end
for more details.
.np
.ix 'debugging information' '&dbgname'
If the previous steps have been followed, you should be well on your
way to debugging your programs with source line information.
There are instances where the &dbgname cannot find the appropriate
source file even though it knows all the line numbers.
The problem that has surfaced involves how the source file is
associated with the debugging information of the module.
The original location of the source file is included in the
debugging information for a module.
The name that is included in the debugging information is the
original name that was on the &cmpname command line.
If the original filename is no longer valid (i.e., you have moved the
executable to another directory), the &dbgname must be told where to
find the source files.
The &dbgname "Source Path" menu item (under "File") can be used to
supply new directories to search for source files.
If your source files are located in two directories, the following
paths can be added in the &dbgname:
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
&dr3.&pc.program&pc.fortran&pc.*.for
&dr3.&pc.program&pc.new&pc.fortran&pc.*.for
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
&dr3.&pc.program&pc.c&pc.*.c
&dr3.&pc.program&pc.new&pc.c&pc.*.c
.millust end
.do end
.pc
The "*" character indicates where the module name will be inserted
while the &dbgname is searching for the source file.
See the description of the "Source Path" menu item in the
.book &dbgname User's Guide
for more details.
.*
.section The difference between the "d1" and "d2" compiler options
.*
.np
.ix 'd1 versus d2'
The reason that there are two levels of debugging information
available is that the code optimizer can perform many more
optimizations and still maintain "d1" (line) information.
The "d2" option forces the code optimizer to ensure that any local
variable can be displayed at any time in the function.
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
To illustrate why this results in less optimum code being generated
for a function, let us look at a simple array initialization.
.ix 'debugging' 'optimized programs'
.ix '&cmpname options' 'd1'
.ix '&cmpname options' 'd2'
.millust begin
extern int a[100];

void init_a( void )
{
    int i;

    for( i = 0; i < 100; ++i ) {
        a[i] = 3*i;
    }
}
.millust end
.np
The code optimizer will ensure that you can print the value of the
variable "i" at any time during the execution of the loop.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.np
.do end
The "d2" option will always generate code and debugging information
so that you can print the value of any variable during the execution
of the function.
In order to get the best code possible and still see your source file
while debugging, the "d1" option only generates line number
information into the object file.
With line number information, much better code can be generated.
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Here is the C equivalent of the code generated for the array
initialization example.
.millust begin
extern int a[100];

void init_a( void )
{
    int *t1;
    int t2;

    /* for( i = 0; i < 100; ++i ) { */
    t1 = a;
    t2 = 0;
    do {
        /* a[i] = 3*i; */
        *t1 = t2;
        ++t1;
        t2 += 3;
    /* } */
    } while( t1 != a + 100 );
}
.millust end
.pc
The above code executes very quickly but notice that the variable "i"
has been split into two different variables.
One of the variables handles the use of "i" as an array index and the
other handles the calculation of "3*i".
.do end
.*
The debugging of programs that have undergone extensive optimization
can be difficult, but with the source line information it is much
easier.
To summarize, use the "d2" compiler option if you are developing a
module and you would like to be able to display each local variable.
The "d1" compiler option will give you line number information and the
best generated code possible.
There is absolutely no reason not to specify the "d1" option because
the code quality will be identical to code generated without the "d1"
option.
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section The difference between the "debug" and "d2" compiler options
.*
.np
.ix '&cmpname debugging' 'd2'
.ix '&cmpname debugging' 'debug'
.ix 'debugger option' 'd1'
.ix 'debugger option' 'd2'
.ix 'd2 versus debug'
.ix 'debug versus d2'
The "d2" (and "d1") compiler options are used to add debugging
information to your executable.
The "d2" option makes line numbering, local symbol and typing
information available to the debugger whereas "d1" only provides line
number debugging information to the debugger.
This information is used during a debugging session to examine
the state of variables and to provide the source code display.
.np
The "debug" option provides run-time error messages that are
independent of the &dbgname..
The "debug" option causes the generation of run-time error checking
code.
This includes subscript and substring bounds checking as well as code
that allows a run-time traceback to be issued when an error occurs.
During the execution of the application, if an error occurs, the code
added with the "debug" option will halt the program and provide an
informative error message.
.*
.do end
:CMT. .*
:CMT. .section Tracing back from a program crash in the debugger
:CMT. .*
:CMT. .np
:CMT. .ix '&cmpname debugging' 'Trace Back'
:CMT. The &cmpname compiler has been designed to produce the fastest,
:CMT. smallest code.
:CMT. As a result, our libraries do not contain stack frame information for
:CMT. performing tracebacks (it would degrade performance and increase
:CMT. executable code size).
:CMT. .np
:CMT. The debugger has been made smarter, so that it can display traceback
:CMT. information despite the lack of stack frame information.
:CMT. All you need to do is issue the
:CMT. .millust begin
:CMT. show calls
:CMT. .millust end
:CMT. command when a program crashes during a debugging session.
:cmt. trace back through the procedure calls to determine which line of code
:cmt. caused the crash.
:cmt. The active assembler instruction will be highlighted in the assembler
:cmt. window.
:cmt. Rather than attempting to continue execution of the assembler code,
:cmt. you can selectively execute just the "POP" or "LEAVE" assembler
:cmt. instructions to step back to the calling procedures.
:cmt. The following steps outline the procedure that is used to recover from
:cmt. a crash within VIDEO:
:cmt. .autonote
:cmt. .note
:cmt. Compile and link your application with at least "d1" debugging
:cmt. information and load the program into VIDEO.
:cmt. .note
:cmt. Before running the program, type "MIX" in VIDEO at the DBG> command
:cmt. line to display the source, assembler code and register windows.
:cmt. .note
:cmt. When the program crashes, make a note of the assembler module and line
:cmt. where the program stopped.
:cmt. .note
:cmt. Using the mouse or cursor in the assembler window, scroll ahead in the
:cmt. assembler code to the next occurrence of the "POP" or "LEAVE"
:cmt. instruction.
:cmt. Select this line to ensure that it is highlighted as the current
:cmt. instruction.
:cmt. .note
:cmt. Return to the DBG> command line and type "/eip=." to set the
:cmt. instruction pointer to the currently highlighted assembler
:cmt. instruction.
:cmt. .note
:cmt. Type "t/a" to activate tracing at the assembler level.
:cmt. .note
:cmt. Use the space bar to execute the "POP" statements which return you to
:cmt. the calling procedure.
:cmt. .note
:cmt. Repeat steps 4 through 7 to trace back through the calling sequence
:cmt. until you reach the source code instruction that lead to the crash.
:cmt. .endnote
:cmt. .np
:cmt. This procedure should help to determine the location of a program
:cmt. crash.
:cmt. You may want to set a breakpoint at this line of code and restart the
:cmt. program to examine the state of the program just before the crash.
