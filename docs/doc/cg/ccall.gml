.if '&machine' eq '8086' .do begin
:set symbol="calref" value="cal86".
:set symbol="calttl" value="16-bit Assembly Language Considerations".
:set symbol="machint" value="short int".
:set symbol="intsize" value="2".
:set symbol="maxint" value="32767".
:set symbol="umaxint" value="65535".
:set symbol="minint" value="-32768".
:set symbol="ax" value="ax".
:set symbol="axup" value="AX".
:set symbol="bx" value="bx".
:set symbol="bxup" value="BX".
:set symbol="cx" value="cx".
:set symbol="cxup" value="CX".
:set symbol="dx" value="dx".
:set symbol="dxup" value="DX".
:set symbol="di" value="di".
:set symbol="diup" value="DI".
:set symbol="si" value="si".
:set symbol="siup" value="SI".
:set symbol="bp" value="bp".
:set symbol="bpup" value="BP".
:set symbol="sp" value="sp".
:set symbol="spup" value="SP".
:set symbol="reg4" value="DX AX".
:set symbol="reg8" value="AX BX CX DX".
:set symbol="arg_2_regs" value="long int".
:set symbol="typ_2_regs" value="long int".
:set symbol="nearsize" value="1 word (16 bits)".
:set symbol="farsize" value="2 words (32 bits)".
:set symbol="bitmode" value="16-bit mode".
:set symbol="nptrsz" value="2".
:set symbol="nargsz" value="2".
:set symbol="fptrsz" value="4".
:set symbol="fargsz" value="4".
.do end
.el .do begin
:set symbol="calref" value="cal386".
:set symbol="calttl" value="32-bit Assembly Language Considerations".
:set symbol="machint" value="long int".
:set symbol="intsize" value="4".
:set symbol="maxint" value="2147483647".
:set symbol="umaxint" value="4294967295".
:set symbol="minint" value="-2147483648".
:set symbol="ax" value="eax".
:set symbol="axup" value="EAX".
:set symbol="bx" value="ebx".
:set symbol="bxup" value="EBX".
:set symbol="cx" value="ecx".
:set symbol="cxup" value="ECX".
:set symbol="dx" value="edx".
:set symbol="dxup" value="EDX".
:set symbol="di" value="edi".
:set symbol="diup" value="EDI".
:set symbol="si" value="esi".
:set symbol="siup" value="ESI".
:set symbol="bp" value="ebp".
:set symbol="bpup" value="EBP".
:set symbol="sp" value="esp".
:set symbol="spup" value="ESP".
:set symbol="reg4" value="EAX".
:set symbol="reg8" value="EDX EAX".
:set symbol="arg_2_regs" value="double  ".
:set symbol="typ_2_regs" value="double".
:set symbol="nearsize" value="1 double word (32 bits)".
:set symbol="farsize" value="2 double words (64 bits)".
:set symbol="bitmode" value="32-bit mode".
:set symbol="nptrsz" value="4".
:set symbol="nargsz" value="4".
:set symbol="fptrsz" value="6".
:set symbol="fargsz" value="8".
.do end
.*
.chap *refid=&calref. &calttl.
.*
.ix 'calling conventions'
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.el .do begin
.   .if '&machine' eq '8086' .do begin
.   .   .helppref 16-bit:
.   .do end
.   .el .do begin
.   .   .helppref 32-bit:
.   .do end
.do end
.*
.np
This chapter will deal with the following topics.
.autonote
.note
The data representation of the basic types supported by &cmpname..
.note
The memory layout of a &cmpname program.
.note
The method for passing arguments and returning values.
.note
The two methods for passing floating-point arguments and returning
floating-point values.
.np
One method is used when one of the &cmpname
"fpi" or "fpi87"
options is specified for the generation of in-line 80x87 instructions.
When the "fpi" option is specified, an 80x87 emulator is included from
a math library if the application includes floating-point operations.
When the "fpi87"
option is used exclusively, the 80x87 emulator will not be included.
.np
The other method is used when the &cmpname "fpc" option is specified.
In this case, the compiler generates calls to floating-point support
routines in the alternate math libraries.
.endnote
.pc
An understanding of the Intel 80x86 architecture is assumed.
.*
.section Data Representation
.*
.np
.ix 'data types'
.ix 'data representation'
This section describes the internal or machine representation of the
basic types supported by &cmpname..
.*
.beglevel
.*
.section Type "char"
.*
.np
.ix 'types' 'char'
.ix 'char type'
An item of type "char" occupies 1 byte of storage.
Its value is in the following range.
.millust begin
0 <= n <= 255
.millust end
.pc
Note that "char" is, by default, unsigned.
The &cmpname compiler option "j" can be used to change the default
from unsigned to signed.
If "char" is signed, an item of type "char" is in the following range.
.millust begin
-128 <= n <= 127
.millust end
.pc
You can force an item of type "char" to be unsigned or signed regardless
of the default by defining them to be of type "unsigned char" or
"signed char" respectively.
.*
.section Type "short int"
.*
.np
.ix 'types' 'short int'
.ix 'short int type'
An item of type "short int" occupies 2 bytes of storage.
Its value is in the following range.
.millust begin
-32768 <= n <= 32767
.millust end
.pc
Note that "short int" is signed and hence "short int" and
"signed short int" are equivalent.
If an item of type "short int" is to be unsigned, it must be defined as
"unsigned short int".
In this case, its value is in the following range.
.millust begin
0 <= n <= 65535
.millust end
.*
.*
.section Type "long int"
.*
.np
.ix 'types' 'long int'
.ix 'long int type'
An item of type "long int" occupies 4 bytes of storage.
Its value is in the following range.
.millust begin
-2147483648 <= n <= 2147483647
.millust end
.pc
Note that "long int" is signed and hence "long int" and
"signed long int" are equivalent.
If an item of type "long int" is to be unsigned, it must be defined as
"unsigned long int".
In this case, its value is in the following range.
.millust begin
0 <= n <= 4294967295
.millust end
.*
.section Type "int"
.*
.np
.ix 'types' 'int'
.ix 'int type'
An item of type "int" occupies &intsize bytes of storage.
Its value is in the following range.
.millust begin
&minint <= n <= &maxint
.millust end
.pc
Note that "int" is signed and hence "int" and "signed int" are
equivalent.
If an item of type "int" is to be unsigned, it must be defined as
"unsigned int".
In this case its value is in the following range.
.millust begin
0 <= n <= &umaxint
.millust end
.np
If you are generating code that executes in &bitmode, "&machint" and
"int" are equivalent, "unsigned &machint" and "unsigned int" are
equivalent, and "signed &machint" and "signed int" are equivalent.
This may not be the case in other environments where "int" and
.if '&machine' eq '8086' .do begin
"long int" are 4 bytes.
.do end
.el .do begin
"short int" are 2 bytes.
.do end
.*
.section Type "float"
.*
.np
.ix 'types' 'float'
.ix 'float type'
A datum of type "float" is an approximate representation of a real
number.
Each datum of type "float" occupies 4 bytes.
If
.id m
is the magnitude of
.id x
(an item of type "float") then
.id x
can be approximated if
.millust begin
.if &e'&dohelp eq 0 .do begin
2&S'-126. <= m < 2&S'128.
.do end
.el .do begin
 -126           128
2      <= m <  2
.do end
.millust end
.pc
or in more approximate terms if
.millust begin
1.175494e-38 <= m <= 3.402823e38
.millust end
.np
Data of type "float" are represented internally as follows.
Note that bytes are stored in memory with the least significant byte
first and the most significant byte last.
.millust begin
+---+---------+---------------------+
| S | Biased  |      Significand    |
|   | Exponent|                     |
+---+---------+---------------------+
 31    30-23           22-0
.millust end
.begnote
.notehd1 Notes
.notehd2 ~b
.note S
S = Sign bit (0=positive, 1=negative)
.note Exponent
.if &e'&dohelp eq 0 .do begin
.sr n1=2&S'-126.
.sr n2=2&S'0.
.sr n3=2&S'127.
.do end
.el .do begin
.sr n1=2**-126
.sr n2=2**0
.sr n3=2**127
.do end
The exponent bias is 127 (i.e., exponent value 1 represents &n1.;
exponent value 127 represents &n2.; exponent value 254 represents
&n3.; etc.).
The exponent field is 8 bits long.
.note Significand
The leading bit of the significand is always 1, hence it is
not stored in the significand field. Thus the significand is
always "normalized".
The significand field is 23 bits long.
.note Zero
A real zero quantity occurs when the sign bit, exponent, and
significand are all zero.
.note Infinity
When the exponent field is all 1 bits and the significand field is
all zero bits then the quantity represents positive or negative
infinity, depending on the sign bit.
.note Not Numbers
When the exponent field is all 1 bits and the significand field is
non-zero then the quantity is a special value called a NAN
(Not-A-Number).
.np
When the exponent field is all 0 bits and the significand field is
non-zero then the quantity is a special value called a "denormal"
or nonnormal number.
.endnote
.*
.section Type "double"
.*
.np
.ix 'types' 'double'
.ix 'double type'
A datum of type "double" is an approximate representation of
a real number.
The precision of a datum of type "double" is greater than or equal to
one of type "float".
Each datum of type "double" occupies 8 bytes.
If
.id m
is the magnitude of
.id x
(an item of type "double") then
.id x
can be approximated if
.millust begin
.if &e'&dohelp eq 0 .do begin
2&S'-1022. <= m < 2&S'1024.
.do end
.el .do begin
 -1022          1024
2      <= m <  2
.do end
.millust end
.pc
or in more approximate terms if
.millust begin
2.2250738585072e-308 <= m <= 1.79769313486232e308
.millust end
.np
Data of type "double" are represented internally as follows.
Note that bytes are stored in memory with the least significant byte
first and the most significant byte last.
.millust begin
+---+---------+--------------------------------------+
| S | Biased  |              Significand             |
|   | Exponent|                                      |
+---+---------+--------------------------------------+
 63    62-52                   51-0
.millust end
.begnote
.notehd1 Notes:
.notehd2 ~b
.note S
S = Sign bit (0=positive, 1=negative)
.note Exponent
.if &e'&dohelp eq 0 .do begin
.sr n1=2&S'-1022.
.sr n2=2&S'0.
.sr n3=2&S'1023.
.do end
.el .do begin
.sr n1=2**-1022
.sr n2=2**0
.sr n3=2**1023
.do end
The exponent bias is 1023 (i.e., exponent value 1 represents &n1.;
exponent value 1023 represents &n2.; exponent value 2046 represents
&n3.; etc.).
The exponent field is 11 bits long.
.note Significand
The leading bit of the significand is always 1, hence it is
not stored in the significand field. Thus the significand is
always "normalized".
The significand field is 52 bits long.
.note Zero
A double precision zero quantity occurs when the sign bit,
exponent, and significand are all zero.
.note Infinity
When the exponent field is all 1 bits and the significand field is
all zero bits then the quantity represents positive or negative
infinity, depending on the sign bit.
.note Not Numbers
When the exponent field is all 1 bits and the significand field is
non-zero then the quantity is a special value called a NAN
(Not-A-Number).
.np
When the exponent field is all 0 bits and the significand field is
non-zero then the quantity is a special value called a "denormal"
or nonnormal number.
.endnote
.endlevel
.*
.im wmemlay
.*
.section Calling Conventions for Non-80x87 Applications
.*
.np
.ix 'conventions' 'non-80x87'
The following sections describe the calling convention used when compiling
with the "fpc" compiler option.
.*
.beglevel
.*
.section Passing Arguments Using Register-Based Calling Conventions
.*
.np
.ix 'passing arguments'
How arguments are passed to a function with register-based calling
conventions is determined by the
size (in bytes) of the argument and where in the argument list the
argument appears.
Depending on the size, arguments are either passed in registers or on
the stack.
Arguments such as structures are almost always passed on the stack since
they are generally too large to fit in registers.
Since arguments are processed from left to right, the first few
arguments are likely to be passed in registers (if they can fit) and,
if the argument list contains many arguments,
the last few arguments are likely to be passed on the stack.
.np
.ix 'passing arguments' 'in registers'
The registers used to pass arguments to a function are &axup, &bxup, &cxup
and &dxup..
The following algorithm describes how arguments are passed to
functions.
.np
Initially, we have the following registers available for passing
arguments: &axup, &dxup, &bxup and &cxup..
Note that registers are selected from this list in the order they
appear.
That is, the first register selected is &axup and the last is &cxup..
For each argument
.us Ai,
starting with the left most argument, perform the following steps.
.autonote
.note
.ix 'passing arguments' '1 byte'
.ix 'passing arguments' '2 bytes'
If the size of
.us Ai
is 1 byte
.if '&machine' eq '80386' .do begin
or 2 bytes
.do end
.ct , convert it to &intsize bytes and proceed to the next step.
If
.us Ai
is of type "unsigned char"
.if '&machine' eq '80386' .do begin
or "unsigned short int"
.do end
.ct , it is converted to an "unsigned int".
If
.us Ai
is of type "signed char"
.if '&machine' eq '80386' .do begin
or "signed short int"
.do end
.ct , it is converted to a "signed int".
If
.us Ai
.if '&machine' eq '8086' .do begin
is a 1-byte structure,
.do end
.el .do begin
is a 1-byte or 2-byte structure,
.do end
the padding is determined by the compiler.
.note
If an argument has already been assigned a position on the stack,
.us Ai
will also be assigned a position on the stack.
Otherwise, proceed to the next step.
.note
.ix 'passing arguments' '&intsize bytes'
If the size of
.us Ai
is &intsize bytes, select a register from the list of available registers.
If a register is available,
.us Ai
is assigned that register.
The register is then removed from the list of available registers.
If no registers are available,
.us Ai
will be assigned a position on the stack.
.note
.ix 'passing arguments' 'far pointers'
.if '&machine' eq '8086' .do begin
If the size of
.us Ai
is 4 bytes,
.do end
.el .do begin
If the type of
.us Ai
is "far pointer",
.do end
select a register pair from the following list of combinations:
[&dxup &axup] or [&cxup &bxup].
The first available register pair is assigned to
.us Ai
and removed from the list of available pairs.
.if '&machine' eq '8086' .do begin
The high-order 16 bits of the argument are assigned to the first
register in the pair; the low-order 16 bits are assigned to the second
register in the pair.
.do end
.el .do begin
The segment value will actually be passed in register DX or CX and the
offset in register &axup or &bxup..
.do end
If none of the above register pairs is available,
.us Ai
will be assigned a position on the stack.
.if '&machine' eq '80386' .do begin
Note that 8 bytes will be pushed on the stack even though the size of
an item of type "far pointer" is 6 bytes.
.do end
.note
.ix 'passing arguments' '8 bytes'
.ix 'passing arguments' 'of type double'
If the type of
.us Ai
is "double" or "float" (in the absence of a function prototype),
.if '&machine' eq '8086' .do begin
select [AX BX CX DX] from the list of available registers.
All four registers are removed from the list of available registers.
The high-order 16 bits of the argument are assigned to the first
register and the low-order 16 bits are assigned to the fourth
register.
If any of the four registers is not available,
.do end
.el .do begin
select a register pair from the following list of combinations:
[EDX EAX] or [ECX EBX].
The first available register pair is assigned to
.us Ai
and removed from the list of available pairs.
The high-order 32 bits of the argument are assigned to the first
register in the pair; the low-order 32 bits are assigned to the second
register in the pair.
If none of the above register pairs is available,
.do end
.us Ai
will be assigned a position on the stack.
.note
All other arguments will be assigned a position on the stack.
.endnote
.autonote Notes:
.note
Arguments that are assigned a position on the stack are padded to a
multiple of &intsize bytes.
That is, if a 3-byte structure is assigned a position on the stack,
4 bytes will be pushed on the stack.
.note
Arguments that are assigned a position on the stack are pushed onto
the stack starting with the rightmost argument.
.endnote
.*
.section *refid=&calref.s Sizes of Predefined Types
.*
.np
The following table lists the predefined types, their size as returned
by the "sizeof" function, the size of an argument of that type and the
registers used to pass that argument if it was the only argument in
the argument list.
.ix 'size of' 'predefined types'
.ix 'predefined types' 'size of'
.ix 'size of' 'char'
.ix 'char' 'size of'
.ix 'size of' 'signed char'
.ix 'signed char' 'size of'
.ix 'size of' 'unsigned char'
.ix 'unsigned char' 'size of'
.ix 'size of' 'int'
.ix 'int' 'size of'
.ix 'size of' 'unsigned int'
.ix 'unsigned int' 'size of'
.ix 'size of' 'signed int'
.ix 'signed int' 'size of'
.ix 'size of' 'short int'
.ix 'short int' 'size of'
.ix 'size of' 'signed short int'
.ix 'signed short int' 'size of'
.ix 'size of' 'unsigned short int'
.ix 'unsigned short int' 'size of'
.ix 'size of' 'long int'
.ix 'long int' 'size of'
.ix 'size of' 'signed long int'
.ix 'signed long int' 'size of'
.ix 'size of' 'unsigned long int'
.ix 'unsigned long int' 'size of'
.ix 'size of' 'float'
.ix 'float' 'size of'
.ix 'size of' 'long float'
.ix 'long float' 'size of'
.ix 'size of' 'double'
.ix 'double' 'size of'
.ix 'size of' 'long double'
.ix 'long double' 'size of'
.ix 'size of' 'near pointer'
.ix 'near pointer' 'size of'
.ix 'size of' 'far pointer'
.ix 'far pointer' 'size of'
.np
.tb set ^
.tb &sysin.+5 &sysin.+15c &sysin.+23c &sysin.+31
.ul
^Basic Type^"sizeof"^Argument^Registers
.ul
^^^Size^Used
.tb &sysin.+5 &sysin.+15 &sysin.+23 &sysin.+31
^char^1^&intsize^[&axup]
^short int^2^&intsize^[&axup]
^int^&intsize.^&intsize^[&axup]
^long int^4^4^[&reg4]
^float^4^8^[&reg8]
^double^8^8^[&reg8]
^near pointer^&nptrsz.^&nargsz.^[&axup]
^far pointer^&fptrsz.^&fargsz.^[&dxup &axup]
.if '&machine' eq '8086' .do begin
^huge pointer^&fptrsz.^&fargsz.^[&dxup &axup]
.do end
.tb set
.tb
.np
.us Note that the size of the argument listed in the table assumes
.us that no function prototypes are specified.
Function prototypes affect the way arguments are passed.
This will be discussed in the section entitled
"Effect of Function Prototypes on Arguments".
.autonote Notes:
.note
Provided no function prototypes exist, an argument will be converted
to a default type as described in the following table.
.begnote $compact $setptnt 15
.notehd1 Argument Type
.notehd2 Passed As
.note char
unsigned int
.note signed char
signed int
.note unsigned char
unsigned int
.if '&machine' eq '80386' .do begin
.note short
unsigned int
.note signed short
signed int
.note unsigned short
unsigned int
.do end
.note float
double
.endnote
.endnote
.*
.section Size of Enumerated Types
.*
.np
.ix 'size of' 'enumerated types'
.ix 'enumerated types' 'size of'
The integral type of an enumerated type is determined by the values of the
enumeration constants.
In strict ISO/ANSI C mode, all enumerated constants are of type
.id int.
In the extensions mode, the compiler will use the smallest integral
type possible (excluding
.id long
ints) that can represent all values of the enumerated type.
For instance, if the minimum and maximum values of the enumeration
constants are in the range &minus.128 and 127, the enumerated type will be
equivalent to a
.id signed char
(size = 1 byte).
All references to enumerated constants in the previous instance will have type
.id signed char.
An enumerated constant is always promoted to an
.id int
when passed as an argument.
.*
.section Effect of Function Prototypes on Arguments
.*
.np
.ix 'function prototypes' 'effect on arguments'
Function prototypes define the types of the formal parameters of a
function.
Their appearance affects the way in which arguments are passed.
An argument will be converted to the type of the corresponding formal
parameter in the function prototype.
Consider the following example.
.millust begin
void prototype( float x, int i );

void main()
{
  float x;
  int   i;

  x = 3.14;
  i = 314;
  prototype( x, i );
  rtn( x, i );
}
.millust end
.pc
The function prototype for
.id prototype
specifies that the first argument is to be passed as a "float"
and the second argument is to be passed as an "int".
This results in the first argument being passed in
.if '&machine' eq '8086' .do begin
registers DX and AX
.do end
.el .do begin
register EAX
.do end
and the second argument being passed in
.if '&machine' eq '8086' .do begin
register BX.
.do end
.el .do begin
register EDX.
.do end
.np
If no function prototype is given, as is the case for the function
.id rtn,
the first argument will be passed as a "double" and the
second argument would be passed as an "int".
This results in the first argument being passed in registers
.if '&machine' eq '8086' .do begin
AX, BX, CX and DX
.do end
.el .do begin
EDX and EAX
.do end
and the second argument being passed
.if '&machine' eq '8086' .do begin
on the stack.
.do end
.el .do begin
in register EBX.
.do end
.np
Note that even though both
.id prototype
and
.id rtn
were called with identical argument lists, the way in which the arguments
were passed was completely different simply because a function prototype
for
.id prototype
was specified.
Function prototyping is an excellent way to guarantee that arguments
will be passed as expected to your assembly language function.
.*
.section Interfacing to Assembly Language Functions
.*
.np
Consider the following example.
.exam begin
void main()
{
    &arg_2_regs x;
    int      i;
    &arg_2_regs y;

    x = 7;
    i = 77;
    y = 777;
    myrtn( x, i, y );
}
.exam end
.pc
.id myrtn
is an assembly language function that requires three arguments.
The first argument is of type "&typ_2_regs", the second argument is
of type "int" and the third argument is again of type
"&typ_2_regs".
Using the rules for register-based calling conventions,
these arguments will be passed to
.id myrtn
in the following way:
.autonote
.note
The first argument will be passed in registers &dxup and &axup leaving
&bxup and &cxup as available registers for other arguments.
.note
The second argument will be passed in register &bxup leaving &cxup as an
available register for other arguments.
.note
The third argument will not fit in register &cxup
.if '&machine' eq '8086' .do begin
(its size is 4 bytes)
.do end
.el .do begin
(its size is 8 bytes)
.do end
and hence will be pushed on the stack.
.endnote
.np
Let us look at the stack upon entry to
.id myrtn.
.if '&machine' eq '8086' .do begin
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- SP points here
        +----------------+
  2     | argument #3    |
        |                |
        +----------------+
  6     |                |
.monooff
.millust end
.do end
.el .do begin
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- ESP points here
        +----------------+
  4     | argument #3    |
        |                |
        +----------------+
 12     |                |
.monooff
.millust end
.do end
.if '&machine' eq '8086' .do begin
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- SP points here
        |                |
        +----------------+
  4     | argument #3    |
        |                |
        +----------------+
  8     |                |
.monooff
.millust end
.do end
.el .do begin
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- ESP points here
        |                |
        +----------------+
  8     | argument #3    |
        |                |
        +----------------+
 16     |                |
.monooff
.millust end
.do end
.autonote Notes:
.note
The return address is the top element on the stack.
In a small code model, the return address is
&nearsize; in a big code model, the return address is &farsize..
.endnote
.if '&machine' eq '8086' .do begin
:set symbol="s_o1" value=" 2".
:set symbol="s_o2" value=" 4".
:set symbol="s_o3" value=" 8".
:set symbol="b_o1" value=" 2".
:set symbol="b_o2" value=" 6".
:set symbol="b_o3" value="10".
:set symbol="fill" value="  ".
.do end
.el .do begin
:set symbol="s_o1" value=" 4".
:set symbol="s_o2" value=" 8".
:set symbol="s_o3" value="16".
:set symbol="b_o1" value=" 4".
:set symbol="b_o2" value="12".
:set symbol="b_o3" value="20".
:set symbol="fill" value=" ".
.do end
.np
.if '&machine' eq '8086' .do begin
Register SP cannot be used as a base register to address the third
argument on the stack.
.do end
.ix 'addressing arguments'
Register &bpup is normally used to address arguments on the stack.
Upon entry to the function, register &bpup is set to point to the
stack but before doing so we must save its contents.
The following two instructions achieve this.
.millust begin
push    &bpup           ; save current value of &bpup
mov     &bpup,&spup      &fill; get access to arguments
.millust end
.np
After executing these instructions, the stack looks like this.
.if '&machine' eq '8086' .do begin
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | saved BP       | <- BP and SP point here
        +----------------+
  2     | return address |
        +----------------+
  4     | argument #3    |
        |                |
        +----------------+
  8     |                |
.monooff
.millust end
.do end
.el .do begin
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | saved EBP      | <- EBP and ESP point here
        +----------------+
  4     | return address |
        +----------------+
  8     | argument #3    |
        |                |
        +----------------+
 16     |                |
.monooff
.millust end
.do end
.if '&machine' eq '8086' .do begin
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | saved BP       | <- BP and SP point here
        +----------------+
  2     | return address |
        |                |
        +----------------+
  6     | argument #3    |
        |                |
        +----------------+
 10     |                |
.monooff
.millust end
.do end
.el .do begin
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | saved EBP      | <- EBP and ESP point here
        +----------------+
  4     | return address |
        |                |
        +----------------+
 12     | argument #3    |
        |                |
        +----------------+
 20     |                |
.monooff
.millust end
.do end
.pc
As the above diagrams show, the third argument is at offset &s_o2 from
register &bpup in a small code model and offset &b_o2 in a big code model.
.np
Upon exit from
.id myrtn,
we must restore the value of &bpup..
The following two instructions achieve this.
.millust begin
mov     &spup,&bpup      &fill; restore stack pointer
pop     &bpup           ; restore &bpup
.millust end
.pc
The following is a sample assembly language function which
implements
.id myrtn.
.if '&machine' eq '8086' .do begin
:set symbol="argsize" value="4".
.do end
.el .do begin
:set symbol="argsize" value="8".
.do end
.tb set $
.tb 20 28 38
.millust begin
.us Small Memory Model (small code, small data)
.monoon
DGROUP  $group  $_DATA, _BSS
_TEXT   $segment byte public 'CODE'
        $assume $CS:_TEXT
        $assume $DS:DGROUP
        $public $myrtn_
myrtn_  $proc   $near
        $push   $&bpup        $; save &bpup
        $mov    $&bpup,&spup  $; get access to arguments
;
; body of function
;
        $mov    $&spup,&bpup  $; restore &spup
        $pop    $&bpup        $; restore &bpup
        $ret    $&argsize     $; return and pop last arg
myrtn_  $endp
_TEXT   $ends
:cmt. ;
:cmt. ; Data Definitions (initialized).
:cmt. ;
:cmt.         $public $_GblData
:cmt. _DATA   $segment byte public 'DATA'
:cmt. LocData $dd     $0
:cmt. _GblData $dd    $0
:cmt. _DATA   $ends
:cmt. ;
:cmt. ; Data Definitions (uninitialized)
:cmt. ;
:cmt. _BSS    $segment byte public 'BSS'
:cmt. XData   $dd     $?
:cmt. _BSS    $ends
:cmt.         $end
.monooff
.millust end
.millust begin
.us Large Memory Model (big code, big data)
.monoon
DGROUP  $group $_DATA, _BSS
MYRTN_TEXT segment byte public 'CODE'
        $assume $CS:MYRTN_TEXT
        $public $myrtn_
myrtn_  $proc   $far
        $push   $&bpup        $; save &bpup
        $mov    $&bpup,&spup  $; get access to arguments
;
; body of function
;
        $mov    $&spup,&bpup  $; restore &spup
        $pop    $&bpup        $; restore &bpup
        $ret    $&argsize     $; return and pop last arg
myrtn_  $endp
MYRTN_TEXT ends
:cmt. ;
:cmt. ; Data Definitions
:cmt. ;
:cmt.         $public $_GblData
:cmt. _DATA   $segment byte public 'DATA'
:cmt. LocData $dd     $0
:cmt. _GblData $dd    $0
:cmt. _DATA   $ends
:cmt. ;
:cmt. ; Data Definitions (uninitialized)
:cmt. ;
:cmt. _BSS    $segment byte public 'BSS'
:cmt. XData   $dd     $?
:cmt. _BSS    $ends
:cmt.         $end
.monooff
.millust end
.tb set
.tb
.autonote Notes:
.note
Global function names must be followed with an underscore.
Global variable names must be preceded with an underscore.
.note
All used 80x86 registers must be saved on entry and restored on exit
except those used to pass arguments and return values, and AX, 
which is considered a stratch register.
.ix 'options' 'r'
Note that segment registers only have to saved and restored if you are
compiling your application with the "r" option.
.note
The direction flag must be clear before returning to the caller.
.note
In a small code model,
any segment containing executable code must belong to the segment
"_TEXT" and the class "CODE".
The segment "_TEXT" must have a "combine" type of "PUBLIC".
On entry, CS contains the segment address of the segment "_TEXT".
In a big code model there is no restriction on the naming of segments
which contain executable code.
.note
In a small data model, segment register DS contains the segment address
of the group "DGROUP".
This is not the case in a big data model.
.note
When writing assembly language functions for the small code model, you
must declare them as "near".
If you wish to write assembly language functions for the big code model,
you must declare them as "far".
.note
In general, when naming segments for your code or data,
you should follow the conventions described in the section entitled
"Memory Layout" in this chapter.
.note
If any of the arguments was pushed onto the stack,
the called routine must pop those arguments off the stack in the "ret"
instruction.
.endnote
.*
.if '&machine' eq '80386' .do begin
.section Using Stack-Based Calling Conventions
.*
.np
.ix 'stack-based calling convention'
Let us now consider the example in the previous section except this time
we will use the stack-based calling convention.
The most significant difference between the stack-based calling convention
and the register-based calling convention is the way the arguments are
passed.
When using the stack-based calling conventions, no registers are used to
pass arguments.
Instead, all arguments are passed on the stack.
.np
Let us look at the stack upon entry to
.id myrtn.
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- ESP points here
        +----------------+
  4     | argument #1    |
        |                |
        +----------------+
 12     | argument #2    |
        |                |
        +----------------+
 16     | argument #3    |
        |                |
        +----------------+
 24     |                |
.monooff
.millust end
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | return address | <- ESP points here
        |                |
        +----------------+
  8     | argument #1    |
        |                |
        +----------------+
 16     | argument #2    |
        |                |
        +----------------+
 20     | argument #3    |
        |                |
        +----------------+
 28     |                |
.monooff
.millust end
.autonote Notes:
.note
The return address is the top element on the stack.
In a small code model, the return address is
&nearsize; in a big code model, the return address is &farsize..
.endnote
:set symbol="s_o1" value=" 4".
:set symbol="s_o2" value=" 8".
:set symbol="s_o3" value="16".
:set symbol="b_o1" value=" 4".
:set symbol="b_o2" value="12".
:set symbol="b_o3" value="20".
:set symbol="fill" value=" ".
.np
.ix 'addressing arguments'
Register &bpup is normally used to address arguments on the stack.
Upon entry to the function, register &bpup is set to point to the stack
but before doing so we must save its contents.
The following two instructions achieve this.
.millust begin
push    &bpup           ; save current value of &bpup
mov     &bpup,&spup      &fill; get access to arguments
.millust end
.np
After executing these instructions, the stack looks like this.
.millust begin
.us Small Code Model
.monoon
Offset
        +----------------+
  0     | saved EBP      | <- EBP and ESP point here
        +----------------+
  4     | return address |
        +----------------+
  8     | argument #1    |
        |                |
        +----------------+
 16     | argument #2    |
        |                |
        +----------------+
 20     | argument #3    |
        |                |
        +----------------+
 28     |                |
.monooff
.millust end
.millust begin
.us Big Code Model
.monoon
Offset
        +----------------+
  0     | saved EBP      | <- EBP and ESP point here
        +----------------+
  4     | return address |
        |                |
        +----------------+
 12     | argument #1    |
        |                |
        +----------------+
 20     | argument #2    |
        |                |
        +----------------+
 24     | argument #3    |
        |                |
        +----------------+
 32     |                |
.monooff
.millust end
.pc
As the above diagrams show, the argument are all on the stack and are
referenced by specifying an offset from register &bpup..
.np
Upon exit from
.id myrtn,
we must restore the value of &bpup..
The following two instructions achieve this.
.millust begin
mov     &spup,&bpup      &fill; restore stack pointer
pop     &bpup           ; restore &bpup
.millust end
.pc
The following is a sample assembly language function which
implements
.id myrtn.
.tb set $
.tb 20 28 38
.millust begin
.us Small Memory Model (small code, small data)
.monoon
DGROUP  $group  $_DATA, _BSS
_TEXT   $segment byte public 'CODE'
        $assume $CS:_TEXT
        $assume $DS:DGROUP
        $public $myrtn
myrtn   $proc   $near
        $push   $&bpup        $; save &bpup
        $mov    $&bpup,&spup  $; get access to arguments
;
; body of function
;
        $mov    $&spup,&bpup  $; restore &spup
        $pop    $&bpup        $; restore &bpup
        $ret    $             $; return
myrtn   $endp
_TEXT   $ends
:cmt. ;
:cmt. ; Data Definitions (initialized).
:cmt. ;
:cmt.        $public $_GblData
:cmt. _DATA  $segment byte public 'DATA'
:cmt. LocData  $dd   $0
:cmt. _GblData $dd   $0
:cmt. _DATA  $ends
:cmt. ;
:cmt. ; Data Definitions (uninitialized)
:cmt. ;
:cmt. _BSS   $segment byte public 'BSS'
:cmt. XData  $dd     $?
:cmt. _BSS   $ends
:cmt.        $end
.monooff
.millust end
.millust begin
.us Large Memory Model (big code, big data)
.monoon
DGROUP $group   $_DATA, _BSS
MYRTN_TEXT segment byte public 'CODE'
       $assume  $CS:MYRTN_TEXT
       $public  $myrtn
myrtn  $proc    $far
       $push    $&bpup       $; save &bpup
       $mov     $&bpup,&spup $; get access to arguments
;
; body of function
;
       $mov     $&spup,&bpup  $; restore &spup
       $pop     $&bpup        $; restore &bpup
       $ret     $             $; return
myrtn  $endp
MYRTN_TEXT ends
:cmt. ;
:cmt. ; Data Definitions
:cmt. ;
:cmt.        $public  $_GblData
:cmt. _DATA  $segment byte public 'DATA'
:cmt. LocData $dd     $0
:cmt. _GblData $dd    $0
:cmt. _DATA  $ends
:cmt. ;
:cmt. ; Data Definitions (uninitialized)
:cmt. ;
:cmt. _BSS   $segment byte public 'BSS'
:cmt. XData  $dd      $?
:cmt. _BSS   $ends
:cmt.        $end
.monooff
.millust end
.tb set
.tb
.autonote Notes:
.note
Global function names must not be followed with an underscore as was the
case with the register-based calling convention.
Global variable names must not be preceded with an underscore as was the
case with the register-based calling convention.
.note
All used 80x86 registers except registers EAX, ECX and EDX must be saved on
entry and restored on exit.
Segment registers DS and ES must also be saved on entry and restored on
exit.
Segment register ES does not have to be saved and restored when using a
memory model that is not a small data model.
.ix 'options' 'r'
Note that segment registers only have to be saved and restored if you are
compiling your application with the "r" option.
.note
The direction flag must be clear before returning to the caller.
.note
In a small code model,
any segment containing executable code must belong to the segment "_TEXT"
and the class "CODE".
The segment "_TEXT" must have a "combine" type of "PUBLIC".
On entry, CS contains the segment address of the segment "_TEXT".
In a big code model there is no restriction on the naming of segments
which contain executable code.
.note
In a small data model, segment register DS contains the segment address
of the group "DGROUP".
This is not the case in a big data model.
.note
When writing assembly language functions for the small code model, you
must declare them as "near".
If you wish to write assembly language functions for the big code model,
you must declare them as "far".
.note
In general, when naming segments for your code or data,
you should follow the conventions described in the section entitled
"Memory Layout" in this chapter.
.note
The caller is responsible for removing arguments from the stack.
.endnote
.do end
.*
.section Functions with Variable Number of Arguments
.*
.np
.ix 'variable argument lists'
A function prototype with a parameter list that ends with ",..."
has a variable number of arguments.
In this case, all arguments are passed on the stack.
Since no prototyping information exists for arguments represented by
",...", those arguments are passed as described in the section
"Passing Arguments".
.*
.section Returning Values from Functions
.*
.np
The way in which function values are returned depends on the size of
the return value.
The following examples describe how function values are to be returned.
They are coded for a small code model.
.ix 'functions' 'returning values'
.ix 'returning values from functions'
.autonote
.note
1-byte values are to be returned in register AL.
.exam begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret1_
Ret1_   proc    near   ; char Ret1()
        mov     AL,'G'
        ret
Ret1_   endp
_TEXT   ends
        end
.exam end
.note
2-byte values are to be returned in register AX.
.exam begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret2_
Ret2_   proc    near   ; short int Ret2()
        mov     AX,77
        ret
Ret2_   endp
_TEXT   ends
        end
.exam end
.if '&machine' eq '8086' .do begin
.note
4-byte values are to be returned in registers DX and AX with the most
significant word in register DX.
.exam begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret4_
Ret4_   proc    near   ; long int Ret4()
        mov     AX,word ptr CS:Val4+0
        mov     DX,word ptr CS:Val4+2
        ret
Val4    dd      7777777
Ret4_   endp
_TEXT   ends
        end
.exam end
.note
8-byte values, except structures, are to be returned in registers AX, BX,
CX and DX with the most significant word in register AX.
.exam begin
        .8087
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret8_
Ret8_   proc    near   ; double Ret8()
        mov     DX,word ptr CS:Val8+0
        mov     CX,word ptr CS:Val8+2
        mov     BX,word ptr CS:Val8+4
        mov     AX,word ptr CS:Val8+6
        ret
Val8:   dq      7.7
Ret8_   endp
_TEXT   ends
        end
.exam end
.np
The ".8087" pseudo-op must be specified so that all floating-point
constants are generated in 8087 format.
When using the "fpc" (floating-point calls) option, "float" and "double"
are returned in registers.
See section "Returning Values in 80x87-based Applications" when using
the "fpi" or "fpi87" options.
.do end
.el .do begin
.note
4-byte values are to be returned in register EAX.
.exam begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret4_
Ret4_   proc    near   ; int Ret4()
        mov     EAX,7777777
        ret
Ret4_   endp
_TEXT   ends
        end
.exam end
.note
8-byte values, except structures, are to be returned in registers
EDX and EAX.
When using the "fpc" (floating-point calls) option, "float" and "double"
are returned in registers.
See section "Returning Values in 80x87-based Applications" when using
the "fpi" or "fpi87" options.
.exam begin
        .8087
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  Ret8_
Ret8_   proc    near   ; double Ret8()
        mov     EDX,dword ptr CS:Val8+4
        mov     EAX,dword ptr CS:Val8
        ret
Val8:   dq      7.7
Ret8_   endp
_TEXT   ends
        end
.exam end
.np
The ".8087" pseudo-op must be specified so that all floating-point
constants are generated in 8087 format.
.do end
.note
Otherwise, the caller allocates space on the stack for the return value
and sets register &siup to point to this area.
In a big data model, register &siup contains an offset relative
to the segment value in segment register SS.
.if '&machine' eq '8086' .do begin
:set symbol="word" value="word".
.do end
.el .do begin
:set symbol="word" value="dword".
.do end
.cp 19
.exam begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  RetX_
;
; struct int_values {
;     int value1, value2, value3, value4, value5;
;                   };
;
RetX_   proc    near ; struct int_values RetX()
        mov     &word ptr SS:0[&siup],71
        mov     &word ptr SS:4[&siup],72
        mov     &word ptr SS:8[&siup],73
        mov     &word ptr SS:12[&siup],74
        mov     &word ptr SS:16[&siup],75
        ret
RetX_   endp
_TEXT   ends
        end
.exam end
.np
When returning values on the stack, remember to use a segment override
to the stack segment (SS).
.endnote
.pc
The following is an example of a &cmpname program calling the above
assembly language subprograms.
.keep 12
.millust begin
#include <stdio.h>

struct int_values {
    int value1;
    int value2;
    int value3;
    int value4;
    int value5;
};

.millust break
extern  char              Ret1(void);
extern  short int         Ret2(void);
extern  long int          Ret4(void);
extern  double            Ret8(void);
extern  struct int_values RetX(void);

.millust break
void main()
{
    struct int_values x;

    printf( "Ret1 = %c\n", Ret1() );
    printf( "Ret2 = %d\n", Ret2() );
    printf( "Ret4 = %ld\n", Ret4() );
    printf( "Ret8 = %f\n", Ret8() );
    x = RetX();
    printf( "RetX1 = %d\n", x.value1 );
    printf( "RetX2 = %d\n", x.value2 );
    printf( "RetX3 = %d\n", x.value3 );
    printf( "RetX4 = %d\n", x.value4 );
    printf( "RetX5 = %d\n", x.value5 );
}
.millust end
.pc
The above function should be compiled for a small code model (use the
.if '&machine' eq '8086' .do begin
"ms" or "mc" compiler option).
.do end
.el .do begin
"mf", "ms" or "mc" compiler option).
.do end
.if '&machine' eq '80386' .do begin
.remark
.ix 'stack-based calling convention' 'returning values from functions'
.ix 'options' 'fpc'
Returning values from functions in the stack-based calling convention
is the same as returning values from functions in the register-based
calling convention when using the "fpc" option.
.eremark
.do end
.endlevel
.*
.section Calling Conventions for 80x87-based Applications
.*
.np
.ix 'passing arguments' 'in 80x87-based applications'
When a source file is compiled by &cmpname with one of the
"fpi" or "fpi87"
options, all floating-point arguments are passed on the 80x86 stack.
The rules for passing arguments are as follows.
.autonote
.note
If the argument is not floating-point, use the procedure described
earlier in this chapter.
.note
If the argument is floating-point, it is assigned a position on the
80x86 stack.
.endnote
.if '&machine' eq '80386' .do begin
.remark
.ix 'stack-based calling convention' '80x87 considerations'
When compiling using the "fpi" or "fpi87" options, the method used for
passing floating-point arguments in the stack-based calling convention
is identical to the method used in the register-based calling
convention.
However, when compiling using the "fpi" or "fpi87" options, the method
used for returning floating-point values in the stack-based calling
convention is different from the method used in the register-based
calling convention.
The register-based calling convention returns floating-point values in
ST(0), whereas the stack-based calling convention returns
floating-point values in EDX and EAX.
.eremark
.do end
.*
.beglevel
.*
.section Passing Values in 80x87-based Applications
.*
.np
.ix 'conventions' '80x87'
Consider the following example.
.cp 15
.exam begin
extern  void    myrtn(int,float,double,long int);

void main()
{
    float    x;
    double   y;
    int      i;
    long int j;

    x = 7.7;
    i = 7;
    y = 77.77
    j = 77;
    myrtn( i, x, y, j );
}
.exam end
.pc
.id myrtn
is an assembly language function that requires four arguments.
The first argument is of type "int" ( &intsize bytes),
the second argument is of type "float" (4 bytes),
the third argument is of type "double" (8 bytes)
and the fourth argument is of type "long int" (4 bytes).
.if '&machine' eq '8086' .do begin
These arguments will be passed to
.do end
.el .do begin
.np
When using the stack-based calling conventions, all of the arguments
will be passed on the stack.
When using the register-based calling conventions, the above arguments
will be passed to
.do end
.id myrtn
in the following way:
.autonote
.note
The first argument will be passed in register &axup leaving &bxup,
&cxup and &dxup as available registers for other arguments.
.note
The second argument will be passed on the 80x86 stack since it is a
floating-point argument.
.note
The third argument will also be passed on the 80x86 stack
since it is a floating-point argument.
.note
The fourth argument will be passed on the 80x86 stack since a previous
argument has been assigned a position on the 80x86 stack.
.endnote
.pc
Remember, arguments are pushed on the stack from right to
left.
That is, the rightmost argument is pushed first.
.np
Any assembly language function must obey the following rule.
.autonote
.note
All arguments passed on the stack must be removed by the called function.
.endnote
.np
The following is a sample assembly language function which
implements
.id myrtn.
.cp 13
.exam begin
        .8087
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  myrtn_
myrtn_  proc    near
;
; body of function
;
        ret 16           ; return and pop arguments
myrtn_  endp
_TEXT   ends
        end
.exam end
.autonote Notes:
.note
Function names must be followed by an underscore.
.note
All used 80x86 registers must be saved on entry and restored on exit
except those used to pass arguments and return values, and EAX, 
which is considered a stratch register.
.ix 'options' 'r'
Note that segment registers only have to saved and restored if you are
compiling your application with the "r" option.
In this example, &axup does not have to be saved as it was
used to pass the first argument.
Floating-point registers can be modified without saving their contents.
.note
The direction flag must be clear before returning to the caller.
.note
This function has been written for a small code model.
Any segment containing executable code must belong to the class "CODE"
and the segment "_TEXT".
On entry, CS contains the segment address of the segment "_TEXT".
The above restrictions do not apply in a big code memory model.
.note
When writing assembly language functions for a small code model, you
must declare them as "near".
If you wish to write assembly language functions for a big code model,
you must declare them as "far".
.endnote
.*
.section Returning Values in 80x87-based Applications
.*
.np
.ix 'conventions' '80x87'
.if '&machine' eq '8086' .do begin
Floating-point values are returned in ST(0) when using the "fpi" or
"fpi87" options.
.do end
.el .do begin
When using the stack-based calling conventions with "fpi" or "fpi87",
floating-point values are returned in registers.
Single precision values are returned in EAX, and double precision
values are returned in EDX:EAX.
.np
When using the register-based calling conventions with "fpi" or
"fpi87", floating-point values are returned in ST(0).
.do end
All other values are returned in the manner described earlier in this
chapter.
.endlevel
.*
.if &e'&dohelp eq 1 .do begin
.   .if '&machine' eq '8086' .do begin
.   .   .helppref
.   .do end
.   .el .do begin
.   .   .helppref
.   .do end
.do end
