.ix 'portable'
.pp
Portable software is software that is written in such a way that it
is relatively easy to get the software running on a new and different
computer.
By choosing the C language, the first step has been taken to reduce
the effort involved in porting, but there are many other things that
must be done.
Some of these things include:
.begbull
.bull
isolating the portions of the code that depend on the hardware or
operating system being used,
.bull
being aware of what features of the C language are implementation-defined
and avoiding them, or taking them into account,
.bull
being aware of the various ranges of values that may be stored in
certain types, and declaring objects appropriately,
.bull
being aware of special features available on some systems that might
be useful.
.endbull
.pp
No programmer can seriously expect to write a large portable program the
first time. The first port of the program will take a significant
period of time, but the final result will be a program which is much
more portable than before.
Generally, each subsequent port will be easier and
take less time.
Of course, if the new target system has a new concept that
was not considered in the original program design (such as a
totally different
user-interface), then porting will necessarily
take longer.
.*
.section Isolating System Dependent Code
.*
.pp
The biggest problem when trying to port a program is to uncover
all the places in the code where an assumption about the underlying
hardware or operating system
was made, and which proves to be incorrect on the new system.
Many of these differences are hidden in library routines, but they can
still cause problems.
.pp
Consider, for example, the issue of distinguishing between alphabetic
and non-alphabetic characters. The library provides the function
.libfn isalpha
which takes a character argument and returns a non-zero value
if the character is
alphabetic, and 0 otherwise.
Suppose a programmer, writing a FORTRAN compiler, wanted to know
if a variable name started with the letters 'I' through 'N', in order
to determine if it should be an integer variable.
The programmer might write,
.millust begin
upletter = toupper( name[0] );
if( upletter >= 'I'  &&  upletter <= 'N' ) {
    /* ... */
}
.millust end
.pc
If the program was being developed on a machine using the
.ix 'ASCII character set'
.ix 'character set' 'ASCII'
ASCII character set, this code would work fine, since the upper case
letters have 26 consecutive values.
However, porting
the program to a machine using the
.ix 'EBCDIC character set'
.ix 'character set' 'EBCDIC'
EBCDIC character set, problems may arise because between
the letters 'I' and 'J' are 7 other characters, including '}'.
Thus, the name "}VAR" might be considered a valid integer variable
name, which it is not.
To solve this problem, the programmer could write,
.millust begin
if( isalpha( name[0] ) ) {
    upletter = toupper( name[0] );
    if( upletter >= 'I'  &&  upletter <= 'N' ) {
        /* ... */
    }
}
.millust end
.pp
In this case, it is not necessary to isolate the code
because a relatively simple coding change
covers both cases.
But there are cases where each system will require a new set of
functions for some aspect of the program.
.pp
Consider the user interface of a program. If the program just
displays lines of output to a scrolling terminal, and accepts
lines of input in the same way, the user interface probably won't
need to change between systems.
But suppose the program has a sophisticated user interface involving
full-screen presentation of data, windows, and menus,
and uses a mouse and the keyboard for input.
In the absence of standards for such interfaces,
it is quite likely that each system will require a customized set of
functions.
Here is where program portability can become an art.
.pp
An approach to this problem is to completely isolate the user
interface code of the program. The processing of
data occurs independently of what appears on the screen. At the
completion of processing, a function is called which updates the
screen. This code may or may not be portable, depending on how many
layers of functions are built between the physical screen and the
generic program.
At a level fairly close to the screen hardware, a
set of functions should be defined which perform the set of actions
that the program needs.
The full set of functions will depend extensively on the
requirements of the program,
but they should be functions that can reasonably be expected to work
on any system to which the program will eventually be ported.
.pp
Other areas that may be system dependent include:
.begbull
.bull
The behavior and capabilities of devices, including printers. Some
printers support multiple fonts, expanded and compressed characters,
underlining, graphics,
and so on. Others support only relatively simple text
output.
.bull
Accessing memory regions outside of normally addressable storage.
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
A good example is the Intel 80x86 family of processors.
With the &wc286. 16-bit compiler, the addressable
storage is 1024 kilobytes, but a 16-bit address can only address
64 kilobytes. Special steps must be taken when compiling in order to
address the full storage space.
Many compilers for the 8086,
including &wcboth.,
introduce new keywords that describe
.ix 'pointer' 'on the 8086'
pointer types beyond the 16-bit pointer.
..do end
.*
.************************************************************************
.*
.bull
Code that has been written in assembly language for speed.
As code generation technology advances,
assembly language code should become less necessary.
.bull
Code that accesses some special feature of the system. As an example,
many systems provide the ability to temporarily
exit to the operating system
level, and later return to the program.
The method of doing this varies between systems, and the
requirements of the program often change as well.
.bull
Handling the command line parameters. While C breaks the list of
parameters down into strings, the interpretation of those strings
may vary between systems. A program probably should attempt to
conform to any conventions of the system on which it is being run.
.bull
Handling other startup requirements.
Allocation of memory, initializing devices, and so on, may be
done at this point.
.endbull
.*
.section Beware of Long External Names
.*
.ix 'external linkage'
.ix 'long names'
.pp
According the C Language standard,
a compiler may limit external names (functions and global objects)
to 6 significant characters.
This limitation is often imposed by the
"linking" stage of the development process.
.pp
In practice,
most systems allow
many more significant characters.
However,
the developer of a portable program should be aware of the potential
for porting the program to a system that has a small limit, and
name external objects accordingly.
.pp
If the developer must port a program with many
names that are not unique within the limitations imposed by the
target development system, the preprocessor may be used to provide
shorter unique names for all objects.
Note that this method may seriously impair any
symbolic debugging facilities provided by the development system.
.*
.section Avoiding Implementation-Defined Behavior
.*
.ix 'implementation-defined behavior'
.pp
Several aspects of the code generated by the C compiler depend on
the behavior of the particular C compiler being used. A portable
program should avoid these where possible, and take them into
consideration where they can't be avoided. It may be possible to
use macros to avoid some of these issues.
.pp
An important behavior that varies between systems is the number of
characters of external objects and functions that the system recognizes.
The standard states that a system must recognize a minimum of 6
characters, although future standards may remove or extend this limit.
Most systems allow more than 6 characters, but several recognize only
8 characters.
For true portability, a function or object that has external linkage
should be kept unique in the first 6 characters.
Sometimes this requires
ingenuity when thinking of names, but developing a system for naming
objects goes a long way towards fitting within this restriction. The
goal, of course, is to still have meaningful object names.
If all systems that will eventually be used have a higher limit, then
the programmer may decide to go past the 6 character limit.
If a port is done to
a system with the 6 character limit, a lot of source
changes may be required.
.pp
To solve this problem,
macros could be used to map the actual function names into more
cryptic names that fit within the 6 character limit.
This technique may have the adverse affect of making debugging
very difficult because many of the function and object names will
not be the same as contained in the source code.
.pp
Another implementation-defined behavior occurs with the type
.ix 'type' 'char'
.kw char
..ct ..li .
The standard does not impose a
.kw signed
or
.kw unsigned
interpretation on the type.
A program that uses an object of type
.kw char
that requires the values to be interpreted as
signed
or
unsigned
should explicitly declare the object with that type.
.*
.section Ranges of Types
.*
.pp
The range of an object of type
.ix 'type' 'int'
.kw int
is not specified by the standard, except to say that the minimum range
is &MINUS.32767 to 32767. If an object is to contain an integer value, then
thought should be given as to whether or not this range of
values is acceptable on all systems. If the object is a counter that
will never go outside the range 0 to 255, then the range will be
adequate. However, if the object is to contain values that may
exceed this range, then a
.kw long int
may be required.
.pp
The same argument applies to objects with type
.ix 'type' 'float'
.kw float
..ct ..li .
It may make more sense to declare them with type
.kw double
..ct ..li .
.pp
When converting floating-point numbers to integers,
the rounding behavior
can also vary between compilers and systems. If it is
important to know how the rounding behaves, then the program should
refer to the macro
.mono FLT_ROUNDS
(defined in the header
.hdr <float.h>
..ct ),
which is a value describing the type of rounding performed.
.*
.section Special Features
.*
.pp
Some systems provide special features that may or may not exist on
other systems. For example, many provide the ability to exit to
the operating system, run some other programs, then return to the
program that was running.
Other systems may not provide this ability.
In an interactive program,
this feature may be very useful.
By isolating the code that deals with this feature, a program
may remain easily portable. On the systems that don't support this
feature, it may be necessary to provide a
.us stub
function which does nothing, or displays a message.
.*
.section Using the Preprocessor to Aid Portability
.*
.pp
The preprocessor is
particularly useful for providing alternate code sequences to deal with
portability issues.
Conditional compilation provided by the
.kwpp #if
directive allows the insertion of differing code sequences depending
on some criteria. Defining a set of macros which describe the various
systems, and another macro that selects a particular system, makes it
easy to add system-dependent code.
.pp
For example, consider the macros,
.millust begin
#define OS_DOS    0
#define OS_CMS    1
#define OS_MVS    2
#define OS_OS2    3
#define OS_QNX    4

#define HW_IBMPC  0
#define HW_IBM370 1

#define PR_i8086  0
#define PR_370    1
.millust end
.pc
They describe a set of operating systems
.mono (OS)
..ct ,
hardware
.mono (HW)
and
processors
.mono (PR)
..ct ,
which together can completely describe a computer and
its operating system.
If the program was being ported to a IBM 370 running the MVS operating
system, then it could include a header defining the macros above, and
declare the macros,
.millust begin
#define OPSYS     OS_MVS
#define HARDWARE  HW_IBM370
#define PROCESSOR PR_370
.millust end
.pc
The following code sequence would include the call
only if the program was being compiled for a 370 running MVS:
.millust begin
#if HARDWARE == HW_IBM370  &&  OPSYS == OS_MVS
    DoMVSStuff( x, y );
#endif
.millust end
.pc
In other cases, code may be conditionally compiled based only on the
hardware regardless of the operating system, or based only on the
operating system
regardless of the hardware or processor.
.pp
This technique may work well if used in moderation.
However, a module that is filled with these directives becomes difficult
to read, and that module becomes a candidate for being rewritten
entirely for each system.
