.np
.ix 'benchmarking'
The &product compiler contains many options for controlling the code to
be produced.
It is impossible to have a certain set of compiler options that will
produce the absolute fastest execution times for all possible
applications.
With that said, we will list the compiler options that we think will
give the best execution times for most applications.
You may have to experiment with different options to see which
combination of options generates the fastest code for your particular
application.
.np
The recommended options for generating the fastest 16-bit Intel code
are:
.ix 'fastest 16-bit code'
.begnote
.note Pentium Pro
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.6 &sw.fpi87 &sw.fp6
.note Pentium
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.5 &sw.fpi87 &sw.fp5
.note 486
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.4 &sw.fpi87 &sw.fp3
.note 386
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.3 &sw.fpi87 &sw.fp3
.note 286
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.2 &sw.fpi87 &sw.fp2
.note 186
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.1 &sw.fpi87
.note 8086
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.0 &sw.fpi87
.endnote
.np
The recommended options for generating the fastest 32-bit Intel code
are:
.ix 'fastest 32-bit code'
.begnote
.note Pentium Pro
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.6 &sw.fp6
.note Pentium
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.5 &sw.fp5
.note 486
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.4 &sw.fp3
.note 386
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.3 &sw.fp3
.endnote
.np
The "oi+" option is for C++ only.
Under some circumstances, the "ob" and "ol+" optimizations may also
give better performance with 32-bit Intel code.
.if '&alpha' eq 'AXP' .do begin
.np
The recommended options for generating the fastest DEC Alpha AXP code
are:
.ix 'fastest AXP code'
.begnote
.note Alpha AXP
&sw.onatx &sw.zp4 &sw.zps &sw.as
.endnote
.do end
.*
.np
Option "on" causes the compiler to replace floating-point divisions
with multiplications by the reciprocal.
This generates faster code (multiplication is faster than division),
but the result may not be the same because the reciprocal may not be
exactly representable.
.np
Option "oe" causes small user written functions to be expanded in-line
rather than generating a call to the function.
Expanding functions in-line can further expose other optimizations that
couldn't otherwise be detected if a call was generated to the
function.
.np
Option "oa" causes the compiler to relax alias checking.
.np
Option "ot" must be specified to cause the code generator to select
code sequences which are faster without any regard to the size of
the code.
The default is to select code sequences which strike a balance between
size and speed.
.np
Option "ox" is equivalent to "obmiler" and "s" which causes the
compiler/code generator to
do branch prediction ("ob"),
generate 387 instructions in-line for math functions such as sin, cos,
sqrt ("om"),
expand intrinsic functions in-line ("oi"),
perform loop optimizations ("ol"),
expand small user functions in-line ("oe"),
reorder instructions to avoid pipeline stalls ("or"),
and to not generate any stack overflow checking ("s").
Option "or" is very important for generating fast code for the Pentium
and Pentium Pro processors.
.np
Option "oh" causes the compiler to attempt repeated optimizations
(which can result in longer compiles but more optimal code).
.np
Option "oi+" causes the C++ compiler to expand intrinsic functions
in-line (just like "oi") but also sets the
.us inline_depth
to its maximum (255).
By default,
.us inline_depth
is 3.
The
.us inline_depth
can also be changed by using the C++
.mono inline_depth
pragma.
.np
Option "ei" causes the compiler to allocate at least an "int" for all
enumerated types.
.np
Option "zp8" causes all data to be aligned on 8 byte boundaries.
The default is "zp2" for the 16-bit compiler and "zp8" for 32-bit
compiler.
If, for example, "zp1" packing was specified then this would pack all
data which would reduce the amount of data memory required but would
require extra clock cycles to access data that is not on an
appropriate boundary.
.np
Options "0", "1", "2", "3", "4", "5" and "6" emit Intel code sequences
optimized for processor-specific instruction set features and timings.
For 16-bit Intel applications, the use of these options may limit the
range of systems on which the application will run but there are
execution performance improvements.
.np
Options "fp2", "fp3", "fp5" and "fp6" emit Intel floating-point
operations targetted at specific features of the math coprocessor in
the Intel series.
For 16-bit Intel applications, the use of these options may limit the
range of systems on which the application will run but there are
execution performance improvements.
.np
Option "fpi87" causes in-line Intel 80x87 numeric data processor
instructions to be generated into the object code for floating-point
operations.
Floating-point instruction emulation is not included so as to obtain
the best floating-point performance in 16-bit Intel applications.
.np
For 32-bit Intel applications, the use of the "fp5" option will give
good performance on the Intel Pentium but less than optimal
performance on the 386 and 486.
The use of the "5" option will give good performance on the Pentium
and minimal, if any, impact on the 386 and 486.
Thus, the following set of options gives good overall performance
for the 386, 486 and Pentium processors.
.illust begin
&sw.onatx &sw.oh &sw.oi+ &sw.ei &sw.zp8 &sw.5 &sw.fp3
.illust end
.if '&alpha' eq 'AXP' .do begin
.np
Option "zps" (Alpha AXP only) causes the compiler to align all
structures on qword boundaries.
.np
Option "as" (Alpha AXP only) causes the compiler to emit a shorter
sequence of instructions to load short integers.
You must ensure that your source code aligns all short integers to
even boundaries.
.do end
