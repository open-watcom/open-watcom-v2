.chap *refid=videxpr &dbgname Expression Handling
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.ix 'expressions' 'handling of'
.np
The &dbgname is capable of handling a wide variety of expressions.
An expression is a combination of operators and operands selected from
application variables and names, debugger variables, and constants.
Expressions can be used in a large number of debugger commands and dialogs.
For example, the evaluated result of an expression may be displayed
by choosing
.popup New
in the Watches window or by using the print command.
.np
The appropriate syntax of an expression, i.e., the valid sequence of
operators and operands, depends on the grammar of the language that is
currently established.
The &dbgname supports the grammars of the C, C++, and FORTRAN 77
languages.
A grammar is selected automatically by the debugger when tracing the
execution of modules in an application.
For example, part of an application may be written in C, another part
in C++, and another part in FORTRAN 77.
The modules must have been compiled by one of the &company C, C++ or
FORTRAN 77 compilers.
When tracing into a module written in one of these languages, the debugger
will automatically select the appropriate grammar.
In addition to this automatic selection, a particular grammar may be
selected using the debugger
.kw Set LAnguage
command.
The language currently selected can be determined using the
.kw SHow Set LAnguage
command.
.*
.section General Rules of Expression Handling
.*
.np
.ix 'expressions' 'rules'
The debugger handles two types of expressions.
The difference between the two types of expressions is quite subtle.
One is called an "expression" and things operate as you would
normally expect.
This type of expression is used for all "higher" level operations such
as adding items to the Watches window.
The other type is called an "address expression".
It is used whenever the debugger prompts for an address and in
lower level commands such
.kw Examine
and
.kw Modify
.ct .li .
If the notation for a particular command argument is <address>,
it is an address expression.
If it ends in just "expr" then it is a normal expression.
The difference between the two forms lies in how they treat symbol
names.
In a normal expression the value of a symbol is its
.sy rvalue,
or contents.
In an address expression, the value of a symbol is (sometimes) its
.sy lvalue,
or address.
.np
Consider the following case.
You have a symbol
.id sam
at offset 100 and the word at that location contains the value 15.
If you enter
.id sam
into the watches window
you expect the value 15 to be printed and since
the Watches window
takes a normal expression that is what you get.
Now let us try it with the Breakpoint dialog. Enter
.id sam
in the address field. The Breakpoint dialog uses the result of
its expression as the address at which to set a breakpoint. The
Breakpoint dialog takes an address expression, and an implicit unary "&"
operator is placed in front of symbols.
The debugger has a set of heuristics that it applies to determine
whether it should use the rvalue or lvalue of a symbol.
.*
.section Language Independent Variables and Constants
.*
.np
The following sections describe conventions used in the debugger for
identifying modules, variables, line numbers, registers, etc.
.*
.beglevel
.*
.section *refid=exprsym Symbol Names
.*
.np
.ix 'expressions' 'symbol name'
Regardless of the programming language that was used to code the
modules of an application, the names of variables and routines will be
available to the debugger (provided that the appropriate symbolic
debugging information has been included with the application's
execution module).
The debugger does not restrict the way in which names are used in
expressions.
A name could represent a variable but it could also represent the
entry point into a routine.
.np
The syntax of a symbol name reference is quite complicated.
.ix 'expressions' 'module@routine_name'
.ix 'expressions' 'image@module@routine_name'
.ix '@routine_name'
.ix '@@routine_name'
.syntax
[[[image]@][module]@][routine_name.]symbol_name
.esyntax
.pc
Generally, an application will consist of many modules which were
compiled separately.
.ix 'expressions' 'current module'
.ix 'expressions' 'module'
The current image is the one containing the module which is currently
executing.
The current module is the one containing the source lines currently
under examination in the Source or Assembly window.
By default, the Source window's title line contains the current module
name.
.ix 'expressions' 'current routine'
.ix 'expressions' 'routine'
.ix 'expressions' 'function'
.ix 'expressions' 'procedure'
The current routine is the one containing the source line
at which execution is currently paused.
.np
The following are examples of references to symbol names.
.exam begin 7
symbol_name
main
WinMain
FMAIN
printf
LIB$G_OPEN
stdin
.exam end
.np
If the symbol does not exist in the current scope then it must be
qualified with its routine name.
Generally, these are variables that are local to a particular routine.
.exam begin 5
routine_name.symbol_name
main.curr_time
main.tyme
SUB1.X
SUB2.X
.exam end
.np
If the symbol is not externally defined and it does not exist in the
current module then it may be qualified with its module name.
In the C and C++ programming languages, we can define a variable that
is global to a module but known only to that module ("static" storage
class).
.exam begin 1
static char *NarrowTitle = { "Su Mo Tu We Th Fr Sa" };
.exam end
.pc
In the above example, "NarrowTitle" is global to the module
"calendar".
If the current module is not "calendar" then the module name can be
used to qualify the symbol as shown in the following example.
.exam begin 1
calendar@NarrowTitle
.exam end
.np
If the symbol is local to a routine that is not in the current module
then it must be qualified with its module name and routine name.
.exam begin 5
module_name@routine_name.symbol_name
calendar@main.curr_time
calendar@main.tyme
subs@SUB1.X
subs@SUB2.X
.exam end
.np
If the symbol is local to an image that is not in the current executable
then it must be fully qualified with the image name.
.exam begin 7
prog_name@@routine_name
prog_name@module_name@routine_name
prog_name@module_name@routine_name.symbol_name
dll_name@calendar@main.curr_time
dll_name@calendar@main.tyme
program@subs@SUB1.X
program@subs@SUB2.X
.exam end
.np
There is a special case for the primary executable image.  This is the
name of the program you specified when you started the debugger.
You can reference it by omitting the image name. The following examples
all refer to symbols in the primary executable image:
.exam begin 3
@@WinMain
@module@WinMain
@@routine.symbol
.exam end
.np
In the FORTRAN 77 programming language, all variables (arguments,
local variables, COMMON block variables) are available to the
subprogram in which they are defined or referenced.
The same symbol name can be used in more than one subprogram.
If it is a local variable, it represents a different variable in each
subprogram.
If it is an argument, it may represent a different variable in each
subprogram.
If it is a variable in a COMMON block, it represents the same variable
in each subprogram where the COMMON block is defined.
.cp 15
.exam begin 14
SUBROUTINE SUB1( X )
REAL Y
COMMON /BLK/ Z
    .
    .
    .
END
SUBROUTINE SUB2( X )
REAL Y
COMMON /BLK/ Z
    .
    .
    .
END
.exam end
.pc
In the above example, "X" is an argument and need not refer to the
same variable in the calling subprogram.
.exam begin 2
CALL SUB1( A )
CALL SUB2( B )
.exam end
.pc
The variable "Y" is a different variable in each of "SUB1" and "SUB2".
The COMMON block variable "Z" refers to the same variable in each of
"SUB1" and "SUB2" (different names for "Z" could have been used).
To refer to "X", "Y", or "Z" in the subprogram "SUB2", you would
specify "SUB2.X", "SUB2.Y", or "SUB2.Z".
If "SUB2" was in the module "MOD" and it is not the current module,
you would specify "MOD@SUB2.X", "MOD@SUB2.Y", or "MOD@SUB2.Z".
.begnote
.note Note:
Global and local symbol name debugging information is included in an
executable image if you request it of the linker.
However, local symbol information must be present in your object
files.
The &company C, C++ and FORTRAN 77 compilers can include local symbol
debugging information in object files by specifying the appropriate
compiler option.
.refalso vidprep
.endnote
.*
.section Line Numbers
.*
.np
.ix 'expressions' 'line numbers'
Regardless of the programming language that was used to code the
modules of an application, line number information identifying the
start of executable statements will be available to the debugger
(provided that the appropriate symbolic debugging information has
been included with the application's execution module).  The
debugger does not restrict the way in which line number references
are used in expressions.  A line number represents the code
address of an executable statement in a routine.  Not all line
numbers represent executable statements; thus some line numbers
may not be valid in an expression.  For example, source lines
consisting of comments do not represent executable statements.
.np
The general format for a line number reference is:
.syntax
[ [image]@ ] [module_name] @ decimal_digits
.esyntax
.np
The following are examples of references to executable statements.
.exam begin 11
@36
@@45
@51
@125
hello@9
@hello@9
prog@hello@9
otherprg@goodbye@9
puzzle@50
calendar@20
SUB1@30
.exam end
.np
If the line number does not exist in the current module, it must be
qualified with its module name.  If it does not exist in the current
image, it must be qualified with the image name.
Line numbers are not necessarily unique.
For example, an executable statement could occur at line number 20 in
several modules.
The module name can always be used to uniquely identify the line 20
in which we are interested.
In the above examples, we explicitly refer to line 20 in the module
"calendar".
When the module name is omitted, the current module is assumed.
.begnote
.note Note:
Line number debugging information is included in an executable image
if you request it of the linker.
However, line number information must be present in your object files.
The &company C, C++ and FORTRAN 77 compilers can include line number
debugging information in object files by specifying the appropriate
compiler option.
.refalso vidprep
You can request line number debugging information when assembling
assembly language source files using &asmname. The "d1" option must
be specified on the command line.
.endnote
.*
.section Constants
.*
.np
A
.us constant
can be arithmetic or character.  Each constant has a data type
associated with it.  Arithmetic constants consist of those
constants whose data type is one of integer, real, or complex
(FORTRAN only).  C treats character constants like arithmetic
constants so they can be used in arithmetic expressions.  FORTRAN
treats character constants as constants of type CHARACTER so they
cannot be used in arithmetic expressions.
.beglevel
.*
.section Integer Constants
.*
.np
.ix 'expressions' 'integer constant'
An
.us integer constant
is formed by a non-empty string of digits preceded by an optional
radix specifier.
The digits are taken from the set of digits valid for the current
radix.
If the current radix is 10 then the digits are '0' through '9'.
If the current radix is 16 then the digits are '0' through '9' and 'A'
through 'F' or 'a' through 'f'.
.refalso dlgopt
.np
The following are examples of integer constants.
.exam begin 5
123
57DE
1423
345
34565788
.exam end
.np
Radix specifiers may be defined by the user, but two are predefined
by the debugger.
.us 0x
may be defined to be a radix specifier for
hexadecimal (base 16) numbers.
.us 0n
may be defined to be a radix specifier for decimal (base 10)
numbers
.exam begin 7
0x1234      hexadecimal
0n1234      decimal
255         decimal
0xff        hexadecimal
0x1ADB      hexadecimal
0n200       decimal
0x12fc0     hexadecimal
.exam end
.*
.section Real Constants
.*
.np
.ix 'expressions' 'real constant'
We first define a
.us simple real constant
as follows: an optional sign followed by an integer part followed by a
decimal point followed by a fractional part.
The integer and fractional parts are non-empty strings of digits.
The fractional part can be omitted.
.np
.ix 'expressions' 'real constant'
A
.us real constant
has one of the following forms.
.begpoint
.point (1)
A simple real constant.
.point (2)
A simple real constant followed by an
.id E
or
.id e
followed by an optionally signed integer constant.
.endpoint
.pc
The optionally signed integer constant that follows the
.id E
is called the
.us exponent.
The value of a real constant that contains an exponent is the value of
the constant preceding the
.id E
multiplied by the power of ten determined by the exponent.
.np
The following are examples of real constants.
.millust begin
123.764
0.4352344
1423.34E12
+345.E-4
-0.4565788E3
2.E6
1234.
.millust end
.begnote
.note Note:
The accepted forms of floating-point constants are a subset of
that supported by the FORTRAN 77 programming language.  The
debugger does not support floating-point constants that begin with
a decimal point (e.g., .4352344) or have no decimal point (e.g., 2E6).
However, both forms would be acceptable to a FORTRAN
compiler.  Also, the debugger does not support double precision
floating-point constants where "D" is used instead of "E" for the
exponent part (e.g., 2D6, 2.4352344D6).  All floating-point
constants are stored internally by the debugger in double
precision format.
.endnote
.*
.section Complex Constant (FORTRAN Only)
.*
.np
.ix 'expressions' 'complex constant'
A
.us complex constant
consists of a left parenthesis, followed by a real or integer constant
representing the real part of the complex constant, followed by a
comma, followed by a real or integer constant representing the
imaginary part of the complex constant, followed by a right
parenthesis.
.np
The following are examples of complex constants.
.millust begin
( 1423.34E12, 3 )
( +345, 4 )
.millust end
.np
Complex constants will be accepted when the debugger's currently
established language is FORTRAN.
The language currently selected can be determined using the
.kw SHow Set LAnguage
command.
.*
.section Character Constant (C Only)
.*
.np
.ix 'expressions' 'character constant'
In the C and C++ programming languages, a
.us character constant
consists of an apostrophe followed by a single character
followed by an apostrophe.
The apostrophes are not part of the datum.
An apostrophe in a character datum represents one character,
namely the apostrophe.
A character constant must have length 1.
.np
The following are examples of character constants.
.millust begin
'A'
'e'
'''
.millust end
.np
The C/C++ form of a character constant will be accepted when
the debugger's currently established language is C or C++.
The language currently selected can be determined using the
.kw SHow Set LAnguage
command.
.*
.section Character String Constant (FORTRAN Only)
.*
.np
.ix 'expressions' 'character constant'
In the FORTRAN 77 programming language, a
.us character constant
consists of an apostrophe followed by any string of characters
followed by an apostrophe.  The apostrophes are not part of the
datum.  If an apostrophe is to appear as part of the datum it must
be followed immediately by another apostrophe.  Note that blanks
are significant.  The length of the character constant is the
number of characters appearing between the delimiting apostrophes.
Consecutive apostrophes in a character datum represent one
character, namely the apostrophe.  A character constant must not
have length 0.
.np
The following are examples of character constants.
.millust begin
'ABCDEFG1234567'
'There''s always tomorrow'
.millust end
.np
The FORTRAN form of a character constant will be accepted when
the debugger's currently established language is FORTRAN.
.endlevel
.*
.section Memory References
.*
.np
.ix 'expressions' 'memory references'
.ix 'expressions' 'referencing memory'
.ix 'expressions' 'segment'
.ix 'expressions' 'offset'
In addition to referring to memory locations by symbolic name or line
number, you can also refer to them using a combination of constants,
register names, and symbol names.
In the Intel 80x86 architecture, a memory reference requires a segment
and offset specification.
When symbol names are used, these are implicit.
The general form of a memory reference is:
.syntax
[segment:]offset
.esyntax
.np
When an offset is specified alone, the default segment value is taken
from the CS, DS or SS register depending on the circumstances.
.*
.section Predefined Debugger Variables
.*
.np
The debugger defines a number of symbols which have special meaning.
These symbols are used to refer to the computer's registers and other
special variables.
.begnote $break
.note General Purpose Registers
.ix 'expressions' 'registers'
.us eax, ax, al, ah,
.us ebx, bx, bl, bh,
.us ecx, cx, cl, ch,
.us edx, dx, dl, dh
.note Index Registers
.us esi, si,
.us edi, di
.note Base Registers
.us esp, sp,
.us ebp, bp
.note Instruction Pointer
.ix 'expressions' 'instruction pointer'
.us eip, ip
.note Segmentation Registers
.ix 'expressions' 'segment registers'
.us cs, ds, es, fs, gs, ss
.note Flags Registers
.ix 'expressions' 'flags'
.ix 'expressions' 'flags register'
.us fl,
.us fl.o, fl.d, fl.i, fl.s,
.us fl.z, fl.a, fl.p, fl.c,
.us efl,
.us efl.o, efl.d, efl.i, efl.s,
.us efl.z, efl.a, efl.p, efl.c
.note 8087 Registers
.ix 'expressions' 'floating point registers'
.us st0, st1, st2, st3, st4, st5, st6, st7
.note 8087 Control Word
.ix 'expressions' 'control word register'
.us cw,
.us cw.ic, cw.rc, cw.pc, cw.iem, cw.pm,
.us cw.um, cw.om, cw.zm, cw.dm, cw.im
.note 8087 Status Word
.ix 'expressions' 'status word register'
.us sw,
.us sw.b, sw.c3, sw.st, sw.c2, sw.c1, sw.c0,
.us sw.es, sw.sf, sw.pe, sw.ue, sw.oe,
.us sw.ze, sw.de, sw.ie
.note Miscellaneous Variables
.ix 'expressions' 'pre-defined variables'
.us dbg$32, dbg$bottom, dbg$bp, dbg$code,
.us dbg$cpu, dbg$ctid, dbg$data, dbg$etid,
.us dbg$fpu, dbg$ip, dbg$left, dbg$monitor,
.us dbg$ntid, dbg$os, dbg$pid, dbg$psp,
.us dbg$radix, dbg$remote, dbg$right, dbg$sp,
.us dbg$top, dbg$nil, dbg$src, dbg$loaded
.endnote
.np
The debugger permits the manipulation of register contents and special
debugger variables (e.g., dbg$32) using any of the operators described
in this chapter.
By default, these predefined names are accessed just like any other
variables defined by the user or the application.
.ix 'expressions' '_dbg module'
Should the situation ever arise where the application defines a
variable whose name conflicts with that of one of these debugger
variables, the module specifier
.id _dbg
may be used to resolve the ambiguity.
.ix 'expressions' '_dbg@cs'
For example, if the application defines a variable called
.id cs
then
.id _dbg@cs
can be specified to resolve the ambiguity.
The "_dbg@" prefix indicates that we are referring to a debugger
defined symbol rather than an application defined symbol.
.refalso vidsymb
.*
.section Register Aggregates
.*
.np
.ix 'expressions' 'register aggregate'
.ix 'expressions' 'aggregate'
There are times when a value may be stored in more than one register.
For example, a 32-bit "long" integer value may be stored in the
register pair DX:AX.
We require a mechanism for grouping registers to represent a single
quantity for use in expressions.
.np
We define the term "register aggregate" as any grouping of registers
to form a single unit.
An aggregate is specified by placing register names in brackets in
order from most significant to least significant.
Any aggregate may be specified as long as it forms an 8, 16, 32 or
64-bit quantity.
The following are examples of some of the many aggregates that can be
formed.
.exam begin 10
8-bit    [al]
16-bit   [ah al]
16-bit   [bl ah]
16-bit   [ax]
32-bit   [dx ax]
32-bit   [dh dl ax]
32-bit   [dh dl ah al]
32-bit   [ds di]
64-bit   [ax bx cx dx]
64-bit   [edx eax]       (386/486/Pentium only)
.exam end
.np
In some cases, the specified aggregate may be equivalent to a
register.
For example, the aggregates "[ah al]" and "[ax]" are equivalent to
"ax".
.np
.ix 'expressions' 'coercing types'
.ix 'expressions' 'type enforcement'
The default type for 8-bit, 16-bit, and 32-bit aggregates is integer.
The default type for 64-bit aggregates is double-precision
floating-point.
To force the debugger into treating a 32-bit aggregate as
single-precision floating-point, the type coercion operator "[float]"
may be used.
.endlevel
.*
.section *refid=videxpc Operators for the C Grammar
.*
.np
.ix 'expressions' 'C operators'
The debugger supports most C operators and includes an additional set
of operators for convenience.
The
.us &company C Language Reference
manual describes many of these operators.
.np
The syntax for debugger expressions is similar to that of the C
programming language.
Operators are presented in order of precedence, from lowest to
highest.
Operators on the same line have the same priority.
.millust begin
                                        Lowest Priority
Assignment Operators
    =  +=  -=  *=  /=  %=  &=  |=  ^=  <<=  >>=
Logical Operators
    ||
    &&
Bit Operators
    |
    ^
    &
Relational Operators
    ==  !=
    <   <=   <   >=
Shift Operators
    <<  >>
Arithmetic Operators
    +  -
    *  /  %
Unary Operators
    +  -  ~~  !  ++  --  &  *  %
    sizeof unary_expr
    sizeof(type_name)
    (type_name) unary_expr
    [type_name] unary_expr
    ?
Binary Address Operator
    :
                                        Highest Priority
.millust end
.np
Parentheses can be used to order the evaluation of an expression.
.np
In addition to the operators listed above, a number of primary
expression operators are supported.
These operators are used in identifying the object to be operated
upon.
.begnote
.mnote []
subscripting, substringing
.mnote ()
function call
.mnote .
field selection
.mnote ->
field selection using a pointer
.endnote
.np
The following sections describe the operators presented above.
.beglevel
.*
.keep
.section Assignment Operators for the C Grammar
.*
.begnote
.mnote =
Assignment:
The value on the right is assigned to the object on the left.
.mnote +=
Additive assignment:
The value of the object on the left is augmented by the value on the
right.
.mnote -=
Subtractive assignment:
The value of the object on the left is reduced by the value on the
right.
.mnote *=
Multiplicative assignment:
The value of the object on the left is multiplied by the value on the
right.
.mnote /=
Division assignment:
The value of the object on the left is divided by the value on the
right.
.mnote %=
Modulus assignment:
The object on the left is updated with MOD(left,right).
The result is the remainder when the value of the object on the left
is divided by the value on the right.
.mnote &=
Bit-wise AND:
The bits in the object on the left are ANDed with the bits of the
value on the right.
.mnote |=
Bit-wise inclusive OR:
The bits in the object on the left are ORed with the bits of the
value on the right.
.mnote ^=
Bit-wise exclusive OR:
The bits in the object on the left are exclusively ORed with the bits
of the value on the right.
.mnote <<=
Left shift:
The bits in the object on the left are shifted to the left by the
amount of the value on the right.
.mnote >>=
Right shift:
The bits in the object on the left are shifted to the right by the
amount of the value on the right.
If the object on the left is described as unsigned, the vacated
high-order bits are zeroed.
If the object on the left is described as signed, the sign bit is
propagated through the vacated high-order bits.
The debugger treats registers as unsigned items.
.endnote
.*
.section Logical Operators for the C Grammar
.*
.begnote
.mnote &&
Logical conjunction:
The logical AND of the value on the left and the value on the right is
produced.
If either of the values on the left or right is equal to 0 then the
result is 0; otherwise the result is 1.
.mnote ||
Logical inclusive disjunction:
The logical OR of the value on the left and the value on the right is
produced.
If either of the values on the left or right is not equal to 0 then
the result is 1; otherwise the result is 0.
If the value on the left is not equal to 0 then the expression on the
right is not evaluated (this is known as short-circuit expression
evaluation).
.endnote
.*
.section Bit Operators for the C Grammar
.*
.begnote
.mnote &
Bit-wise AND:
The bits of the value on the left and the value on the right are
ANDed.
.mnote |
Bit-wise OR:
The bits of the value on the left and the value on the right are ORed.
.mnote ^
Bit-wise exclusive OR:
The bits of the value on the left and the value on the right are
exclusively ORed.
.endnote
.*
.section Relational Operators for the C Grammar
.*
.begnote
.mnote ==
Equal:
If the value on the left is equal to the value on the right then the
result is 1; otherwise the result is 0.
.mnote !=
Not equal:
If the value on the left is not equal to the value on the right then
the result is 1; otherwise the result is 0.
.mnote <
Less than:
If the value on the left is less than the value on the right then the
result is 1; otherwise the result is 0.
.mnote <=
Less than or equal:
If the value on the left is less than or equal to the value on the
right then the result is 1; otherwise the result is 0.
.mnote >
Greater than:
If the value on the left is greater than the value on the right then
the result is 1; otherwise the result is 0.
.mnote >=
Greater than or equal:
If the value on the left is greater than or equal to the value on the
right then the result is 1; otherwise the result is 0.
.endnote
.*
.section Arithmetic/Logical Shift Operators for the C Grammar
.*
.begnote
.mnote <<
Left shift:
The bits of the value on the left are shifted to the left by the
amount described by the value on the right.
.mnote >>
Right shift:
The bits of the value on the left are shifted to the right by the
amount described by the value on the right.
If the object on the left is described as unsigned, the vacated
high-order bits are zeroed.
If the object on the left is described as signed, the sign bit is
propagated through the vacated high-order bits.
The debugger treats registers as unsigned items.
.endnote
.*
.section Binary Arithmetic Operators for the C Grammar
.*
.begnote
.mnote +
Addition:
The value on the right is added to the value on the left.
.mnote ~_
Subtraction:
The value on the right is subtracted from the value on the left.
.mnote *
Multiplication:
The value on the left is multiplied by the value on the right.
.mnote /
Division:
The value on the left is divided by the value on the right.
.mnote %
Modulus:
The modulus of the value on the left with respect to the value on the
right is produced.
The result is the remainder when the value on the left is divided by
the value on the right.
.endnote
.*
.keep
.section Unary Arithmetic Operators for the C Grammar
.*
.begnote
.mnote +
Plus:
The result is the value on the right.
.mnote ~_
Minus:
The result is the negation of the value on the right.
.mnote ~~
Bit-wise complement:
The result is the bit-wise complement of the value on the right.
.mnote !
Logical complement:
If the value on the right is equal to 0 then the result is 1;
otherwise it is 0.
.mnote ++
Increment:
Both prefix and postfix operators are supported.
If the object is on the right, it is pre-incremented by 1 (e.g., ++x).
If the object is on the left, it is post-incremented by 1 (e.g., x++).
.mnote ~_ ~_
Decrement:
Both prefix and postfix operators are supported.
If the object is on the right, it is pre-decremented by 1 (e.g., --x).
If the object is on the left, it is post-decremented by 1 (e.g., x--).
.mnote &
Address of:
The result is the address (segment:offset) of the object on the right
(e.g., &main).
.mnote *
Points:
The result is the value stored at the location addressed by the value
on the right (e.g., *(ds:100), *string.loc).
In the absence of typing information, a near pointer is produced.
If the operand does not have a segment specified, the default data segment
(DGROUP) is
assumed.
.code begin
      (SS:00FE) = FFFF
var:  (SS:0100) = 0152
      (SS:0102) = 1240
      (SS:0104) = EEEE
.code end
.mnote %
Value at address:
The result is the value stored at the location addressed by the value
on the right (e.g., %(ds:100), %string.loc).
In the absence of typing information, a far pointer is produced.
If the operand does not have a segment specified, the
default data segment (DGROUP) is assumed.
.code begin
      (SS:00FE) = FFFF
var:  (SS:0100) = 0152
      (SS:0102) = 1240
      (SS:0104) = EEEE
.code end
.np
Note that this operator is not found in the C or C++ programming
languages.
.endnote
.*
.section Special Unary Operators for the C Grammar
.*
.begnote
.mnote sizeof unary_expression
.exam begin 2
sizeof tyme
sizeof (*tyme)
.exam end
.mnote sizeof(type_name)
.exam begin 1
sizeof( struct tm )
.exam end
.mnote (type_name) unary_expression
The type conversion operator
.sy (type_name)
is used to convert an item from one type to another.
The following describes the syntax of "type_name".
.syntax
type_name ::= type_spec { [ "near" | "far" | "huge" ] "*" }
type_spec ::= typedef_name
                |   "struct" structure_tag
                |   "union"  union_tag
                |   "enum"   enum_tag
                |   scalar_type { scalar_type }
scalar_type ::= "char" | "int" | "float" | "double"
                |   "short" | "long" | "signed" | "unsigned"
.esyntax
.exam begin 2
(float) 4
(int) 3.1415926
.exam end
.mnote [type_name] unary_expression
You can force the debugger to treat a memory reference as a particular
type of value by using a type coercion operator.
A type specification is placed inside brackets as shown above.
.ix 'expressions' 'coercing types'
.ix 'expressions' 'type enforcement'
The basic types are
.kw char
(character, 8 bits),
.kw short
(short integer, 16 bits),
.kw long
(long integer, 32 bits),
.kw float
(single-precision floating-point, 32 bits),
and
.kw double
(double-precision floating-point, 64 bits).
Unless qualified by the
.kw short
or
.kw long
keyword, the
.kw int
type will be 16 bits in 16-bit applications and 32 bits in 32-bit
applications (386, 486 and Pentium systems).
The character, short integer and long integer types may be treated
as
.kw signed
or
.kw unsigned
items.
The default for the character type is unsigned.
The default for the integer types is signed.
.exam begin 12
[char]                  (default unsigned)
[signed char]
[unsigned char]
[int]                   (default is signed)
[short]                 (default is signed)
[short int]             (default is signed)
[signed short int]
[long]                  (default is signed)
[long int]              (default is signed)
[signed long]
[unsigned long int]
[float]
[double]
.exam end
.pc
Note that it is unnecessary to specify the
.kw int
keyword when
.kw short
or
.kw long
are specified.
.mnote ?
Existence test:
The "?" unary operator may be used to test for the existence of
a symbol.
.exam begin 1
?id
.exam end
.pc
The result of this expression is 1 if "id" is a symbol known to
the debugger and 0 otherwise.
If the symbol does not exist in the current scope then it must
be qualified with its module name.
Automatic symbols exist only in the current function.
.endnote
.*
.section Binary Address Operator for the C Grammar
.*
.begnote
.mnote :
Memory locations can be referenced by using the binary ":" operator
and a combination of constants, register names, and symbol names.
In the Intel 80x86 architecture, a memory reference requires a segment
and offset specification.
A memory reference using the ":" operator takes the following form:
.syntax
segment:offset
.esyntax
.pc
The elements
.sy segment
and
.sy offset
can be expressions.
.exam begin 2
(ES):(DI+100)
(SS):(SP-20)
.exam end
.endnote
.*
.section Primary Expression Operators for the C Grammar
.*
.begnote
.mnote []
Elements of an array can be identified using subscript expressions.
Consider the following 3-dimensional array defined in the "C"
language.
.exam begin 10
char *ProcessorType[2][4][2] =
    { { { "Intel 8086",   "Intel 8088"  },
        { "Intel 80186",  "Intel 80188" },
        { "Intel 80286",  "unknown" },
        { "Intel 80386",  "unknown" } },

      { { "NEC V30",      "NEC V20" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" } } };
.exam end
.pc
This array can be viewed as two layers of rectangular matrices of 4
rows by 2 columns.
The array elements are all pointers to string values.
.np
By using a subscript expression, specific slices of an array can be
displayed.
To see only the values of the first layer, the following expression can
be issued.
.exam begin 1
processortype[0]
.exam end
.np
To see only the first row of the first layer, the following expression
can be issued.
.exam begin 1
processortype[0][0]
.exam end
.pc
To see the second row of the first layer, the following command can be
issued.
.exam begin 1
processortype[0][1]
.exam end
.np
To see the value of a specific entry in a matrix, all the indices can
be specified.
.exam begin 3
processortype[0][0][0]
processortype[0][0][1]
processortype[0][1][0]
.exam end
.mnote ()
The function call operators appear to the right of a symbol name and
identify a function call in an expression.
The parentheses can contain arguments.
.exam begin 3
ClearScreen()
PosCursor( 10, 20 )
Line( 15, 1, 30, '-', '+', '-' )
.exam end
.mnote .
The "." operator indicates field selection in a structure.
In the following example,
.id tyme2
is a structure and
.id tm_year
is a field in the structure.
.exam begin 1
tyme2.tm_year
.exam end
.mnote ->
The "->" operator indicates field selection when using a pointer
to a structure.
In the following example,
.id tyme
is the pointer and
.id tm_year
is a field in the structure to which it points.
.exam begin 1
tyme->tm_year
.exam end
.endnote
.endlevel
.*
.section Operators for the C++ Grammar
.*
.np
.ix 'expressions' 'C++ operators'
Debugger support for the C++ grammar includes all of the C operators
described in the previous section entitled :HDREF refid='videxpc'..
In addition to this, the debugger supports a variety of C++ operators
which are described in the
.us C++ Programming Language
manual.
.np
Perhaps the best way to illustrate the additional capabilities of
the debugger's support for the C++ grammar is by way of an example.
The following C++ program encompasses the features of C++ that we will
use in our debugging example.
.exam begin
// DBG_EXAM.C: C++ debugging example program

struct BASE {
    int a;
    BASE() : a(0) {}
    ~~BASE(){}
    BASE & operator =( BASE const &s )
    {
        a = s.a;
        return *this;
    }
    virtual void foo()
    {
        a = 1;
    }
};
.exam break

struct DERIVED : BASE {
    int b;
    DERIVED() : b(0) {}
    ~~DERIVED() {}
    DERIVED & operator =( DERIVED const &s )
    {
        a = s.a;
        b = s.b;
        return *this;
    }
    virtual void foo()
    {
        a = 2;
        b = 3;
    }
    virtual void foo( int )
    {
    }
};
.exam break

void use( BASE *p )
{
    p->foo();
}
.exam break

void main()
{
    DERIVED x;
    DERIVED y;

    use( &x );
    y = x;
}
.exam end
.np
Compile and link this program so that the most comprehensive debugging
information is included in the executable file.
.*
.beglevel
.*
.section Ambiguity Resolution in the C++ Grammar
.*
.np
Continuing with the example of the previous section, we can step into
the call to
.id use
and up to the
.id p->foo()
function call. Try to set a breakpoint at foo.
.np
You will be presented with a window containing a list of "foo"
functions to choose from since the reference to
.id foo
at this point is ambiguous.
Select the one in which you are interested.
.np
You may also have observed that, in this instance,
.id p
is really a pointer to the variable
.id x
which is a
.id DERIVED
type.
To display all the fields of
.id x,
you can type cast it as follows.
.exam begin 1
*(DERIVED *)p
.exam end
.*
.section The "this" Operator for the C++ Grammar
.*
.np
Continuing with the example of the previous sections, we can step into
the call to
.id f->foo()
and up to the
.id b=3&semi.
statement.
You can use the "this" operator as illustrated in the following
example.
.exam begin 2
this->a
*this
.exam end
.*
.section "operator" Functions in the C++ Grammar
.*
.np
Continuing with the example of the previous sections, we can set
breakpoints at C++ operators using expressions similar to the
following:
.exam begin 1
operator =
.exam end
.millust begin
    DERIVED & operator =( DERIVED const &s )
    {
        a = s.a;
        b = s.b;
        return *this;
    }
.millust end
.*
.section Scope Operator "::" for the C++ Grammar
.*
.np
We can use the scope operator "::" to identify what it is that we wish
to examine.
Continuing with the example of the previous sections, we can enter an
address like:
.millust begin
base::foo
.millust end
.np
In some cases, this also helps to resolve any ambiguity.
The example above permits us to set a breakpoint at the source code for the
function
.id foo
in the class
.id BASE.
.millust begin
    virtual void foo()
    {
        a = 1;
    }
.millust end
.np
Here are some more interesting examples:
.millust begin
derived::foo
derived::operator =
.millust end
.np
The first of these two examples contains an ambiguous reference so a
prompt window is displayed to resolve the ambiguity.
.*
.section Constructor/Destructor Functions in the C++ Grammar
.*
.np
We can also examine the constructor/destructor functions of
an object or class.
Continuing with the example of the previous sections, we can enter
expressions like:
.exam begin 2
base::base
base::~~base
.exam end
.np
The examples above permit us to reference the source code for the
constructor and destructor functions in the class
.id BASE.
.*
.endlevel
.*
.section Operators for the FORTRAN Grammar
.*
.np
.ix 'expressions' 'FORTRAN operators'
The debugger supports most FORTRAN 77 operators and includes an
additional set of operators for convenience.
The additional operators are patterned after those available in the C
programming language.
.np
The grammar that the debugger supports is close to that of the FORTRAN 77
language but there are a few instances where space characters must be
used to clear up any ambiguities.
For example, the expression
.millust begin
1.eq.x
.millust end
.pc
will result in an error since the debugger will form a floating-point
constant from the "1." leaving the string "eq.x".
If we introduce a space character after the "1" then we clear up the
ambiguity.
.millust begin
1 .eq.x
.millust end
.np
Unlike FORTRAN, the parser in the debugger treats spaces as significant
characters.
Thus spaces must not be introduced in the middle of symbol names,
constants, multi-character operators like .EQ. or //, etc.
.np
Operators are presented in order of precedence, from lowest to
highest.
Operators on the same line have the same priority.
.millust begin
                                        Lowest Priority
Assignment Operators
    =  +=  -=  *=  /=  %=  &=  |=  ^=  <<=  >>=
Logical Operators
    .EQV.  .NEQV.
    .OR.
    .AND.
    .NOT.
Bit Operators
    |
    ^
    &
Relational Operators
    .EQ.  .NE.  .LT.  .LE.  .GT.  .GE.
Shift and Concatenation Operators
    <<  >>  //
Arithmetic Operators
    +  -
    *  /  %
    ** (unsupported)
Unary Operators
    +  -
    ~~  ++  --  &   *   %
    [type_name] unary_expr
    ?
Binary Address Operator
    :
                                        Highest Priority
.millust end
.np
Parentheses can be used to order the evaluation of an expression.
.np
In addition to the operators listed above, a number of primary
expression operators are supported.
These operators are used in identifying the object to be operated
upon.
.begnote
.mnote ()
subscripting, substringing, or function call
.mnote .
field selection
.mnote ->
field selection using a pointer
.endnote
.np
The following built-in functions may be used to convert the specified
argument to a particular type.
.millust begin
INT( )      conversion to integer
REAL( )     conversion to real
DBLE( )     conversion to double-precision
CMPLX( )    conversion to complex
DCMPLX( )   conversion to double-precision complex
.millust end
.np
The following sections describe the operators presented above.
.beglevel
.*
.section Assignment Operators for the FORTRAN Grammar
.*
.begnote
.mnote =
Assignment:
The value on the right is assigned to the object on the left.
.mnote +=
Additive assignment:
The object on the left is augmented by the value on the right.
.mnote -=
Subtractive assignment:
The object on the left is reduced by the value on the right.
.mnote *=
Multiplicative assignment:
The object on the left is multiplied by the value on the right.
.mnote /=
Division assignment:
The object on the left is divided by the value on the right.
.mnote %=
Modulus assignment:
The object on the left is updated with MOD(left,right).
The result is the remainder when the value of the object on the left
is divided by the value on the right.
.mnote &=
Bit-wise AND:
The bits in the object on the left are ANDed with the bits of the
value on the right.
.mnote |=
Bit-wise inclusive OR:
The bits in the object on the left are ORed with the bits of the
value on the right.
.mnote ^=
Bit-wise exclusive OR:
The bits in the object on the left are exclusively ORed with the bits
of the value on the right.
.mnote <<=
Left shift:
The bits in the object on the left are shifted to the left by the
amount of the value on the right.
.mnote >>=
Right shift:
The bits in the object on the left are shifted to the right by the
amount of the value on the right.
If the object on the left is described as unsigned, the vacated
high-order bits are zeroed.
If the object on the left is described as signed, the sign bit is
propagated through the vacated high-order bits.
The debugger treats registers as unsigned items.
.endnote
.*
.section Logical Operators for the FORTRAN Grammar
.*
.begnote
.mnote .EQV.
Logical equivalence:
The logical equivalence of the value on the left and the value on the
right is produced.
.mnote .NEQV.
Logical non-equivalence:
The logical non-equivalence of the value on the left and the value on
the right is produced.
.mnote .OR.
Logical inclusive disjunction:
The logical OR of the value on the left and the value on the right is
produced.
.mnote .AND.
Logical conjunction:
The logical AND of the value on the left and the value on the right is
produced.
.mnote .NOT.
Logical negation:
The logical complement of the value on the right is produced.
.endnote
.*
.section Bit Operators for the FORTRAN Grammar
.*
.begnote
.mnote |
Bit-wise OR:
The bits of the value on the left and the value on the right are ORed.
.mnote ^
Bit-wise exclusive OR:
The bits of the value on the left and the value on the right are
exclusively ORed.
.mnote &
Bit-wise AND:
The bits of the value on the left and the value on the right are
ANDed.
.endnote
.*
.section Relational Operators for the FORTRAN Grammar
.*
.begnote
.mnote .EQ.
Equal:
If the value on the left is equal to the value on the right then the
result is 1; otherwise the result is 0.
.mnote .NE.
Not equal:
If the value on the left is not equal to the value on the right then
the result is 1; otherwise the result is 0.
.mnote .LT.
Less than:
If the value on the left is less than the value on the right then the
result is 1; otherwise the result is 0.
.mnote .LE.
Less than or equal:
If the value on the left is less than or equal to the value on the
right then the result is 1; otherwise the result is 0.
.mnote .GT.
Greater than:
If the value on the left is greater than the value on the right then
the result is 1; otherwise the result is 0.
.mnote .GE.
Greater than or equal:
If the value on the left is greater than or equal to the value on the
right then the result is 1; otherwise the result is 0.
.endnote
.*
.section Arithmetic/Logical Shift Operators for the FORTRAN Grammar
.*
.begnote
.mnote <<
Left shift:
The bits of the value on the left are shifted to the left by the
amount described by the value on the right.
.mnote >>
Right shift:
The bits of the value on the left are shifted to the right by the
amount described by the value on the right.
If the object on the left is described as unsigned, the vacated
high-order bits are zeroed.
If the object on the left is described as signed, the sign bit is
propagated through the vacated high-order bits.
The debugger treats registers as unsigned items.
.endnote
.*
.section Concatenation Operator for the FORTRAN Grammar
.*
.begnote
.mnote //
String concatenation:
The concatenation of the character string value on the left and right
is formed.
.endnote
.*
.section Binary Arithmetic Operators for the FORTRAN Grammar
.*
.begnote
.mnote +
Addition:
The value on the right is added to the value on the left.
.mnote ~_
Subtraction:
The value on the right is subtracted from the value on the left.
.mnote *
Multiplication:
The value on the left is multiplied by the value on the right.
.mnote /
Division:
The value on the left is divided by the value on the right.
.mnote %
Modulus:
The modulus of the value on the left with respect to the value on the
right is produced.
The result is the remainder when the value on the left is divided by
the value on the right.
.mnote **
Exponentiation:
This operation is not supported by the debugger.
.endnote
.*
.section Unary Arithmetic Operators for the FORTRAN Grammar
.*
.begnote
.mnote +
Plus:
The result is the value on the right.
.mnote ~_
Minus:
The result is the negation of the value on the right.
.mnote ~~
Bit-wise complement:
The result is the bit-wise complement of the value on the right.
.mnote ++
Increment:
Both prefix and postfix operators are supported.
If the object is on the right, it is pre-incremented by 1 (e.g., ++x).
If the object is on the left, it is post-incremented by 1 (e.g., x++).
.mnote ~_ ~_
Decrement:
Both prefix and postfix operators are supported.
If the object is on the right, it is pre-decremented by 1 (e.g., --x).
If the object is on the left, it is post-decremented by 1 (e.g., x--).
.mnote &
Address of:
The result is the address (segment:offset) of the object on the right
(e.g., &main).
.mnote *
Points:
The result is the value stored at the location addressed by the value
on the right (e.g., *(ds:100), *string.loc).
In the absence of typing information, the value on the right is
treated as a pointer into the default data segment (DGROUP)
and a near pointer is
produced.
.code begin
      (SS:00FE) = FFFF
var:  (SS:0100) = 0152
      (SS:0102) = 1240
      (SS:0104) = EEEE
.code end
.mnote %
Value at address:
The result is the value stored at the location addressed by the value
on the right (e.g., %(ds:100), %string.loc).
In the absence of typing information, the value on the right is
treated as a pointer into the default data segment (DGROUP)
and a far pointer is produced.
.code begin
      (SS:00FE) = FFFF
var:  (SS:0100) = 0152
      (SS:0102) = 1240
      (SS:0104) = EEEE
.code end
.np
Note that this operator is not found in the FORTRAN 77 programming
language.
.endnote
.*
.section Special Unary Operators for the FORTRAN Grammar
.*
.begnote
.mnote ?
Existence test:
The "?" unary operator may be used to test for the existence of
a symbol.
.millust begin
?id
.millust end
.pc
The result of this expression is 1 if "id" is a symbol known to
the debugger and 0 otherwise.
If the symbol does not exist in the current scope then it must
be qualified with its module name.
Automatic symbols exist only in the current subprogram.
.endnote
.*
.section Binary Address Operator for the FORTRAN Grammar
.*
.begnote
.mnote :
Memory locations can be referenced by using the binary ":" operator
and a combination of constants, register names, and symbol names.
In the Intel 80x86 architecture, a memory reference requires a segment
and offset specification.
A memory reference using the ":" operator takes the following form:
.syntax
segment:offset
.esyntax
.pc
The elements
.sy segment
and
.sy offset
can be expressions.
.exam begin 2
(ES):(DI+100)
(SS):(SP-20)
.exam end
.endnote
.*
.section Primary Expression Operators for the FORTRAN Grammar
.*
.begnote
.mnote ()
Elements of an array can be identified using subscript expressions.
.mnote .
The "." operator indicates field selection in a structure.
This operator is useful in mixed language applications where part of
the application is written in the C or C++ programming language.
In the following example,
.id tyme2
is a structure and
.id tm_year
is a field in the structure.
.millust begin
tyme2.tm_year
.millust end
.mnote ->
The "->" operator indicates field selection when using a pointer
to a structure.
This operator is useful in mixed language applications where part of
the application is written in the C or C++ programming language.
In the following example,
.id tyme
is the pointer and
.id tm_year
is a field in the structure to which it points.
.millust begin
tyme->tm_year
.millust end
.endnote
.endlevel
