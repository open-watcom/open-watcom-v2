.chap Data Representation On x86-based Platforms
.*
.np
This chapter describes the internal or machine representation of the
basic types supported by &cmpname..
The following table summarizes these data types.
.sr c1=&sysin+1
.sr c2=&c1+17
.sr c3=&c2+10
.sr c4=&c3+13
.tb set $
.tb &c1+2 &c2+2 &c3+2
.keep begin
.bx &c1 &c2 &c3 &c4
$Data Type$Size$FORTRAN 77
$$(in bytes)$Standard
.tb &c1+2 &c2+(&c3-&c2)/2 &c3+2
.bx
$LOGICAL$4
$LOGICAL*1$1$(extension)
$LOGICAL*4$4$(extension)
$INTEGER$4
$INTEGER*1$1$(extension)
$INTEGER*2$2$(extension)
$INTEGER*4$4$(extension)
$REAL$4
$REAL*4$4$(extension)
$REAL*8$8$(extension)
$DOUBLE PRECISION$8
$COMPLEX$8
$COMPLEX*8$8$(extension)
$COMPLEX*16$16$(extension)
$DOUBLE COMPLEX$16$(extension)
$CHARACTER$1
$CHARACTER*n$n
.bx off
.keep end
.tb set
.tb
.*
.section LOGICAL*1 Data Type
.*
.np
.ix 'data types' 'LOGICAL*1'
.ix 'LOGICAL*1 data type'
An item of type
.bd LOGICAL*1
occupies 1 byte of storage.
It can only have two values, namely .TRUE. (a value of 1)
and .FALSE. (a value of 0).
.*
.section LOGICAL and LOGICAL*4 Data Types
.*
.np
.ix 'data types' 'LOGICAL'
.ix 'data types' 'LOGICAL*4'
.ix 'LOGICAL data type'
.ix 'LOGICAL*4 data type'
An item of type
.bd LOGICAL
or
.bd LOGICAL*4
occupies 4 bytes of storage.
It can only have two values, namely .TRUE. (a value of 1)
and .FALSE. (a value of 0).
.*
.section INTEGER*1 Data Type
.*
.np
.ix 'data types' 'INTEGER*1'
.ix 'INTEGER*1 data type'
An item of type
.bd INTEGER*1
occupies 1 byte of storage.
Its value is in the following range.
An integer
.sy n
can be represented in 1 byte if
.millust begin
-128 <= n <= 127
.millust end
.*
.section INTEGER*2 Data Type
.*
.np
.ix 'data types' 'INTEGER*2'
.ix 'INTEGER*2 data type'
An item of type
.bd INTEGER*2
occupies 2 bytes of storage.
An integer
.sy n
can be represented in 2 bytes if
.millust begin
-32768 <= n <= 32767
.millust end
.*
.section INTEGER and INTEGER*4 Data Types
.*
.np
.ix 'data types' 'INTEGER'
.ix 'data types' 'INTEGER*4'
.ix 'INTEGER data type'
.ix 'INTEGER*4 data type'
An item of type
.bd INTEGER
or
.bd INTEGER*4
occupies 4 bytes of storage (one numeric storage unit).
An integer
.sy n
can be represented in 4 bytes if
.millust begin
-2147483648 <= n <= 2147483647
.millust end
.*
.section *refid=real REAL and REAL*4 Data Types
.*
.np
.ix 'data types' 'REAL'
.ix 'data types' 'REAL*4'
.ix 'REAL data type'
.ix 'REAL*4 data type'
An item of type
.bd REAL
or
.bd REAL*4
is an approximate representation of a real number and occupies
4 bytes (one numeric storage unit).
If
.id m
is the magnitude of a real number
.id x,
then
.id x
can be approximated if
.millust begin
 -126         128
2     <= m < 2
.millust end
.pc
or in more approximate terms if
.millust begin
1.175494e-38 <= m <= 3.402823e38
.millust end
.np
Items of type
.bd REAL
or
.bd REAL*4
are represented internally as follows.
Note that bytes are stored in memory with the least significant byte
first and the most significant byte last.
.pc
.sr c0=&sysin+1
.sr c1=&c0+3
.sr c2=&c1+14
.sr c3=&c2+22
.tb set $
.tb &c0+1 &c1+3 &c2+6
.bx on &c0 &c1 &c2 &c3
$S$Biased$Significand
$$Exponent
.bx off
$31$30-23$22-0
.tb set
.tb
.begnote
.note S
S = Sign bit (0=positive, 1=negative)
.note Exponent
The exponent bias is 127 (i.e., exponent value 1 represents 2**-126;
exponent value 127 represents 2**0; exponent value 254 represents
2**127; etc.).
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
.section *refid=double DOUBLE PRECISION and REAL*8 Data Types
.*
.np
.ix 'data types' 'DOUBLE PRECISION'
.ix 'data types' 'REAL*8'
.ix 'DOUBLE PRECISION data type'
.ix 'REAL*8 data type'
An item of type
.bd DOUBLE PRECISION
or
.bd REAL*8
is an approximate representation of a real number, occupies 8 bytes
(two numeric storage units) and has precision greater than or equal to
that of an item of type
.bd REAL
or
.bd REAL*4
.ct .li .
If
.id m
is the magnitude of a real number
.id x,
then
.id x
can be approximated if
.millust begin
 -1022         1024
2      <= m < 2
.millust end
.pc
or in more approximate terms if
.millust begin
2.2250738585072e-308 <= m <= 1.79769313486232e308
.millust end
.np
Items of type
.bd DOUBLE PRECISION
or
.bd REAL*8
are represented internally as follows.
Note that bytes are stored in memory with the least significant byte
first and the most significant byte last.
.pc
.sr c0=&sysin+1
.sr c1=&c0+3
.sr c2=&c1+15
.sr c3=&syscl-3
.tb set $
.tb &c0+1 &c1+3 &c2+12
.bx on &c0 &c1 &c2 &c3
$S$Biased$Significand
$$Exponent
.bx off
$63$62-52$51-0
.tb set
.tb
.begnote
.note S
S = Sign bit (0=positive, 1=negative)
.note Exponent
The exponent bias is 1023 (i.e., exponent value 1 represents 2**-1022;
exponent value 1023 represents 2**0; exponent value 2046 represents
2**1023; etc.).
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
.*
.section COMPLEX, COMPLEX*8, and DOUBLE COMPLEX Data Types
.*
.np
.ix 'data types' 'COMPLEX'
.ix 'data types' 'COMPLEX*8'
.ix 'COMPLEX data type'
.ix 'COMPLEX*8 data type'
An item of type
.bd COMPLEX
or
.bd COMPLEX*8
is an approximate representation of a complex number.
The representation is an ordered pair of real numbers, the first
representing the real part of the complex number and the second
representing the imaginary part of the complex number.
Each item of type
.bd COMPLEX
or
.bd COMPLEX*8
occupies 8 bytes (two consecutive numeric storage units), the first
being the real part and the second the imaginary part.
The approximation of the real and imaginary parts of a complex number
is the same degree of approximation used for items of type
.bd REAL
.ct .li .
.*
.section COMPLEX*16 Data Type
.*
.np
.ix 'data types' 'COMPLEX*16'
.ix 'COMPLEX*16 data type'
An item of type
.bd COMPLEX*16
is an approximate representation of a complex number.
The representation is an ordered pair of real numbers, the first
representing the real part of the complex number and the second
representing the imaginary part of the complex number.
Each item of type
.bd COMPLEX*16
occupies 16 bytes (four consecutive numeric storage units), the first
two being the real part and the last two the imaginary part.
The approximation of the real and imaginary parts of a complex number
is the same degree of approximation used for items of type
.bd DOUBLE PRECISION
.ct .li .
.*
.section CHARACTER Data Type
.*
.np
.ix 'data types' 'CHARACTER'
.ix 'CHARACTER data type'
An item of type
.bd CHARACTER
represents a sequence of characters.
Each character occupies 1 byte of storage (1 character storage unit).
The length of an item of type
.bd CHARACTER
is the number of characters it contains.
Each character is assigned an integer that represents its position.
Characters are numbered from 1 to
.sy n
starting from the left,
.sy n
being the number of characters.
.if '&machine' eq '8086' .do begin
The maximum size of an item of type
.bd character
is 65535.
.do end
.np
Items of type
.bd CHARACTER
are represented by a
.us string descriptor.
.ix 'string descriptor'
A string descriptor has the following format.
.pc
.keep begin
.sr oc=&sysin+4
.sr c0=&oc+6
.sr c1=&c0+15
.tb set $
.tb &oc &c0+2
$Offset
.bx &c0 &c1
$0$pointer to data
.bx
$4$length of data
.bx off
.tb set
.tb
.keep end
.pc
.if '&machine' eq '8086' .do begin
The pointer to the actual data is a 32-bit pointer which consists of a
16-bit offset followed by a 16-bit segment.
The length is represented as a 16-bit unsigned integer.
.do end
.el .do begin
The pointer to the actual data is a 32-bit offset in the default data
segment.
The length is represented as a 32-bit unsigned integer.
.do end
.*
.section Storage Organization of Data Types
.*
.np
The following illustrates the relative size of the data types in terms
of bytes.
.bd LOGICAL
is equivalent to
.bd LOGICAL*4
.ct ,
.bd INTEGER
is equivalent to
.bd INTEGER*4
.ct ,
.bd DOUBLE PRECISION
is equivalent to
.bd REAL*8
.ct , and
.bd COMPLEX
is equivalent to
.bd COMPLEX*8
.ct .li .
.ix 'short option'
If the "short" option is used,
.bd LOGICAL
is equivalent to
.bd LOGICAL*1
and
.bd INTEGER
is equivalent to
.bd INTEGER*2
.ct .li .
.pc
.sr c0=&sysin+11
.sr bs=2
.sr c1=&c0+&bs
.sr c2=&c1+&bs
.sr c3=&c2+&bs
.sr c4=&c3+&bs
.sr c5=&c4+&bs
.sr c6=&c5+&bs
.sr c7=&c6+&bs
.sr c8=&c7+&bs
.sr c9=&c8+&bs
.sr c10=&c9+&bs
.sr c11=&c10+&bs
.sr c12=&c11+&bs
.sr c13=&c12+&bs
.sr c14=&c13+&bs
.sr c15=&c14+&bs
.sr c16=&c15+&bs
.tb set $
.tb &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9 &c10 &c11 &c12 &c13 &c14 &c15 &c16
.keep 24
Offset$0$1$2$3$4$5$6$7$8$9$10$11$12$13$14$15
.bx on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9 &c10 &c11 &c12 &c13 &c14 &c15 &c16
in bytes
.bx on &c0 &c1
LOGICAL*1
.bx on &c0 &c4
LOGICAL*4
.bx on &c0 &c1
INTEGER*1
.bx on &c0 &c2
INTEGER*2
.bx on &c0 &c4
INTEGER*4
.bx on &c0 &c4
REAL*4
.bx on &c0 &c8
REAL*8
.tb &c2-1 &c6-2
.bx on &c0 &c4 &c8
COMPLEX*8$real$imaginary
.tb &c4-2 &c12-3
.bx on &c0 &c8 &c16
COMPLEX*16$real part$imaginary part
.bx off
.keep end
.tb set
.tb
.*
.section Floating-point Accuracy On x86-based Platforms
.*
.np
There are a number of issues surrounding floating-point accuracy,
calculations, exceptions, etc. on the x86-based personal computer
platform that we will address in the following sections.
Some result from differences in the behaviour of standard-conforming
FORTRAN 77 compilers.
Other result from idiosyncrasies of the IEEE Standard 754
floating-point that is supported on the x86 platform.
.np
Some FORTRAN 77 compilers extend the precision of single-precision
constants in DATA statement initialization lists when the
corresponding variable is double precision.
This is permitted by the FORTRAN 77 Standard.
&product, however, does not do this.
This is illustrated by the following example.
.exam begin
      double precision pi1, pi2
      data pi1 /3.141592653589793/
      data pi2 /3.141592653589793d0/
      write(unit=*,fmt='(1x,z16,1x,f18.15)') pi1, pi1
      write(unit=*,fmt='(1x,z16,1x,f18.15)') pi2, pi2
      end
.exam end
.np
The output produces two very different results for our pi variables.
The variable
.id PI1
is initialized with a single precision (i.e., REAL) constant.
.millust begin
 400921FB60000000  3.141592741012573
 400921FB54442D18  3.141592653589793
.millust end
.np
A single precision datum has 23 bits in the mantissa; a double
precision datum has 52 bits in the mantissa.
Hence
.id PI1
has 29 fewer bits of accuracy in the mantissa (the difference between
52 and 23) since it is initialized with a single precision constant.
You can verify this by examining the hexadecimal output of the two
pi's.
The bottom 29 bits of the mantissa in
.id PI1
are all zero.
.np
To be on the safe side, the rule is always use double precision
constants (even in DATA statements) if you want as much accuracy as
possible.
.np
This behaviour treats DATA statement initialization as equivalent to
simple assignment as shown in the following example.
.exam begin
      double precision pi1, pi2
      pi1 = 3.141592653589793
      pi2 = 3.141592653589793d0
      write(unit=*,fmt='(1x,z16,1x,f18.15)') pi1, pi1
      write(unit=*,fmt='(1x,z16,1x,f18.15)') pi2, pi2
      end
.exam end
The output follows:
.millust begin
 400921FB60000000  3.141592741012573
 400921FB54442D18  3.141592653589793
.millust end
.np
A second consideration is illustrated by the next example.
On some computer architectures, there is no difference in the exponent
range between single and double precision floating-point
representation.
One such architecture is the IBM mainframe computer (e.g., IBM
System/370).
When a double precision result is assigned to a single precision
(REAL) variable, only precision in the mantissa is lost.
.np
The x86 platform uses the IEEE Standard 754 floating-point
representation.
In this representation, the range of exponent values is greater in
double precision than in single precision.
As described in the section entitled :HDREF refid='real'., the range
for single precision (REAL, REAL*4) numbers is:
.millust begin
1.175494e-38 <= m <= 3.402823e38
.millust end
.np
On the other hand, the range for double precision (DOUBLE PRECISION,
REAL*8) numbers is:
.millust begin
2.2250738585072e-308 <= m <= 1.79769313486232e308
.millust end
.pc
Double precision is described in the section entitled
:HDREF refid='double'..
So you can see that a number like 1.0E234 can easily be represented in
double precision but not in single precision since the maximum
positive exponent value for single precision is 38.
.*
.section Floating-point Exceptions On x86-based Platforms
.*
.np
The following types of exceptions can be enabled/disabled on PC's with
an 80x87 floating-point unit (either a real FPU or a true emulator).
.begnote $setptnt 10
.note DENORMAL
The result has become denormalized.
When the exponent field is all 0 bits and the significand field is
non-zero then the quantity is a special value called a "denormal"
or nonnormal number.
By providing a significand with leading zeros, the range of possible
negative exponents can be extended by the number of bits in the
significand.
Each leading zero is a bit of lost accuracy, so the extended exponent
range is obtained by reducing significance.
.note ZERODIVIDE
A division by zero was attempted.
A real zero quantity occurs when the sign bit, exponent, and
significand are all zero.
.note OVERFLOW
The result has overflowed.
The correct answer is finite, but has a magnitude too great to be
represented in the destination floating-point format.
.note UNDERFLOW
The result has numerically underflowed.
The correct answer is non-zero but has a magnitude too small to be
represented as a normal number in the destination floating-point
format.
IEEE Standard 754 specifies that an attempt be made to represent the
number as a denormal.
This denormalization may result in a loss of significant bits from the
significand.
.note PRECISION
A calculation does not return an exact answer.
This exception is usually masked (disabled) and ignored.
It is used in extremely critical applications, when the user must know
if the results are exact.
The precision exception is called "inexact" in IEEE Standard 754.
.note INVALID
This is the exception condition that covers all cases not covered by
the other exceptions. Included are FPU stack overflow and underflow,
NAN inputs, illegal infinite inputs, out-of-range inputs, and inputs in
unsupported formats.
.endnote
.np
Which exceptions does &product catch and which ones does it ignore by
default?
We can determine the answer to this with the following program.
.code begin
* This program uses the C Library routine "_control87"
* to obtain the math coprocessor exception mask.

      implicit none
      include 'fsignal.fi'

      character*8 status
      integer fp_cw, bits

      fp_cw = _control87( 0, 0 )
      bits = IAND( fp_cw, MCW_EM )
      print '(a,1x,z4)', 'Interrupt exception mask', bits
      print *,'Invalid operation exception ', status(bits, EM_INVALID)
      print *,'Denormalized exception ', status(bits, EM_DENORMAL)
      print *,'Divide by 0 exception ', status(bits, EM_ZERODIVIDE)
      print *,'Overflow exception ', status(bits, EM_OVERFLOW)
      print *,'Underflow exception ', status(bits, EM_UNDERFLOW)
      print *,'Precision exception ', status(bits, EM_PRECISION)
      end

      character*8 function status( bits, mask )
      integer bits, mask

      if( IAND(bits,mask) .eq. 0 ) then
        status = 'enabled'
      else
        status = 'disabled'
      endif
      end
.code end
.np
If you compile and run this program, the following output is produced.
.millust begin
Interrupt exception mask 0032
Invalid operation exception enabled
Denormalized exception disabled
Divide by 0 exception enabled
Overflow exception enabled
Underflow exception disabled
Precision exception disabled
.millust end
.np
So, by default, the &product run-time system will catch "invalid
operation", "divide by 0", and "overflow" exceptions.
It ignores "denormal", "underflow", and "precision" exceptions.
Thus calculations that produce very small results trend towards zero.
Also, calculations that produce inexact results (a very common
occurrence in floating-point calculations) are allowed to continue.
.np
Suppose that you were interested in flagging calculations that result
in denormalized or underflowed results.
To do this, we need to enable both DENORMAL and UNDERFLOW exceptions.
This following program illustrates how to do this.
.code begin
*$ifdef __386__
*$ifdef __stack_conventions__
*$pragma aux _clear87 "!"
*$else
*$pragma aux _clear87 "!_"
*$endif
*$else
*$pragma aux _clear87 "!_"
*$endif

      implicit none
      include 'fsignal.fi'

      character*8 status
      integer fp_cw, fp_mask, bits

*     get rid of any errors so we don't cause an instant exception
      call _clear87

*     fp_mask determines the bits to enable and/or disable
      fp_mask = 0
     1      + EM_DENORMAL
     2      + EM_UNDERFLOW

*     fp_cw determines whether to enable(0) or disable(1)
*     (in this case, nothing is disabled)
      fp_cw = '0000'x

      fp_cw = _control87( fp_cw, fp_mask )

      bits = IAND( fp_cw, MCW_EM )
      print '(a,1x,z4)', 'Interrupt exception mask', bits
      print *,'Invalid operation exception ', status(bits, EM_INVALID)
      print *,'Denormalized exception ', status(bits, EM_DENORMAL)
      print *,'Divide by 0 exception ', status(bits, EM_ZERODIVIDE)
      print *,'Overflow exception ', status(bits, EM_OVERFLOW)
      print *,'Underflow exception ', status(bits, EM_UNDERFLOW)
      print *,'Precision exception ', status(bits, EM_PRECISION)
      end

      character*8 function status( bits, mask )
      integer bits, mask

      if( IAND(bits,mask) .eq. 0 ) then
        status = 'enabled'
      else
        status = 'disabled'
      endif
      end
.code end
.np
If you compile and run this program, the following output is produced.
.millust begin
Interrupt exception mask 0020
Invalid operation exception enabled
Denormalized exception enabled
Divide by 0 exception enabled
Overflow exception enabled
Underflow exception enabled
Precision exception disabled
.millust end
.*
.section Compiler Options Relating to Floating-point
.*
.np
Let us take the program that we developed in the previous section and
test it out.
If you introduce the variable
.id FLT
to the program and calculate the expression "2e-38 x 2e-38", you would
expect to see 0.0 printed when underflow exceptions are disabled and a
run-time diagnostic when underflow exceptions are enabled.
The statements that you would add are show in the following.
.code begin
      real flt

      flt=2e-38
      print *, flt*flt

* code to enable exceptions goes here

      print *, flt*flt

      end
.code end
.np
If you compile the modified program with default options and run it,
the result is as follows.
.millust begin
       0.0000000
Interrupt exception mask 0020
Invalid operation exception enabled
Denormalized exception enabled
Divide by 0 exception enabled
Overflow exception enabled
Underflow exception enabled
Precision exception disabled
       0.0000000
.millust end
.np
This is not what we expected.
Evaluation of the second expression did not produce the run-time
diagnostic that we expected.
The reason this happened is related to the compiler's processing of the
source code.
By default, the compiler optimized the generated code by evaluating
the expression "2e-38 x 2e-38" at compile time producing 0.0 as the
result (due to the underflow).
.millust begin
      flt=2e-38
      print *, flt*flt

reduces to

      print *, 2e-28*2e-38

which further reduces to

      print *, 0.0
.millust end
.np
Recompile the program using the "OP" option and run it.
The result is as follows.
.millust begin
       0.0000000
Interrupt exception mask 0020
Invalid operation exception enabled
Denormalized exception enabled
Divide by 0 exception enabled
Overflow exception enabled
Underflow exception enabled
Precision exception disabled
*ERR* KO-03 floating-point underflow
.millust end
.np
The use of the "OP" option will force the result to be stored in
memory after each FORTRAN statement is executed.
Thus, the source code is not optimized across statements.
Compile-time versus run-time evaluation of expressions can lead to
different results.
It is very instructive to compile and then run your application with a
variety of compile-time options to see the effect of optimizations.
See the chapter entitled :HDREF refid='fpopts'. for more information on
compiler options.
.np
Before we end this section, there is another important aspect of
floating-point exceptions to consider.
A floating-point exception is triggered upon the execution of the next
FPU instruction following the one that caused the exception.
.code begin
      implicit none

      real*4 a
      real*8 b

      b=12.0d123
      a=b*b
      b=1.0
      a=b/2.0
      print *, a, b
      end
.code end
.np
Compile this program with the "OP" and "DEBUG" options and then run
it.
The result is displayed next.
.millust begin
*ERR* KO-02 floating-point overflow
 - Executing line 9 in file pi4.for
.millust end
.np
Line 9 is the line containing the statement
.mono a=b/2.0
which could not possibly be responsible for an overflow.
However, it contains the first floating-point instruction following
the instruction in line 7 where the overflow actually occurred.
To see this, it helps to disassemble the object file.
.code begin
      a=b*b
0029    B8 07 00 00 00      mov       eax,0x00000007
002E    E8 00 00 00 00      call      RT@SetLine
0033    DD 45 F4            fld       qword ptr -0xc[ebp]
0036    D8 C8               fmul      st,st
0038    D9 5D FC            fstp      dword ptr -0x4[ebp]
      b=1.0
003B    B8 09 00 00 00      mov       eax,0x00000009
0040    E8 00 00 00 00      call      RT@SetLine
0045    31 DB               xor       ebx,ebx
0047    89 5D F4            mov       -0xc[ebp],ebx
004A    C7 45 F8 00 00 F0 3F
                            mov       dword ptr -0x8[ebp],0x3ff00000
      a=b/2.0
0051    B8 0A 00 00 00      mov       eax,0x0000000a
0056    E8 00 00 00 00      call      RT@SetLine
005B    DD 45 F4            fld       qword ptr -0xc[ebp]
005E    DC 0D 08 00 00 00
                            fmul      qword ptr L$2
0064    D9 5D FC            fstp      dword ptr -0x4[ebp]
.code end
.np
The overflow occurred when the "fstp" was executed but is signalled
when the subsequent "fld" is executed.
The overflow could also be signalled while executing down in a
run-time routine.
This behaviour of the FPU can be somewhat exasperating.
.*
.section Floating-point Exception Handling
.*
.np
In certain situations, you want to handle floating-point exceptions in
the application itself rather than let the run-time system terminate
your application.
The following example illustrates how to do this by installing a
FORTRAN subroutine as a floating-point exception handler.
.code begin
      implicit none
      include 'fsignal.fi'

      real flt
      external fpehandler
      integer      signal_count, signal_number, signal_type
      common /fpe/ signal_count, signal_number, signal_type

*     begin the signal handling process for floating-point exceptions
      call fsignal( SIGFPE, fpehandler )
*
*   main body of application goes here
*
      flt = 2.0
      print *, 'number of signals', volatile( signal_count )
      print *, flt / 0.0
      print *, 'number of signals', volatile( signal_count )

      end

*$ifdef __386__
*$ifdef __stack_conventions__
*$pragma aux _clear87 "!"
*$else
*$pragma aux _clear87 "!_"
*$endif
*$else
*$pragma aux _clear87 "!_"
*$endif

*$pragma aux fpehandler parm( value, value )

      subroutine fpehandler( sig_num, fpe_type )

      implicit none

*     sig_num and fpe_type are passed by value, not by reference
      integer sig_num, fpe_type

      include 'fsignal.fi'

      integer      signal_count, signal_number, signal_type
      common /fpe/ signal_count, signal_number, signal_type
*     we could add this to our common block
      integer      signal_split( FPE_INVALID:FPE_IOVERFLOW )

      signal_count = signal_count + 1
      signal_number = sig_num
      signal_type = fpe_type

*     floating-point exception types

*     FPE_INVALID         = 129 (0)
*     FPE_DENORMAL        = 130 (1)
*     FPE_ZERODIVIDE      = 131 (2)
*     FPE_OVERFLOW        = 132 (3)
*     FPE_UNDERFLOW       = 133 (4)
*     FPE_INEXACT         = 134 (5)
*     FPE_UNEMULATED      = 135 (6)
*     FPE_SQRTNEG         = 136 (7)
*     undefined           = 138 (8)
*     FPE_STACKOVERFLOW   = 137 (9)
*     FPE_STACKUNDERFLOW  = 138 (10)
*     FPE_EXPLICITGEN     = 139 (11)
*     FPE_IOVERFLOW       = 140 (12)

*     log the type of error for interest only */
      signal_split( fpe_type ) =
     1signal_split( fpe_type ) + 1

*     get rid of any errors
      call _clear87

*     resignal for more exceptions
      call fsignal( SIGFPE, fpehandler )

*     if we don't then a subsequent exception will
*     cause an abnormal program termination

      end
.code end
.np
Note the use of the
.id VOLATILE
intrinsic function to obtain up-to-date contents of the variable
.id SIGNAL_COUNT.
