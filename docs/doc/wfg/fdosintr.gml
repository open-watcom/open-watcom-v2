:set symbol="newauto"   value="newauto.bat".
.if '&machine' eq '80386' .do begin
:set symbol="newos2"    value="newconf.os2".
.do end
.*
.chap The &product Compiler
.*
.section Introduction to &product
.*
.np
&product is the newest member of the WATFOR family of debugging
FORTRAN compilers.
.ix '&product'
.ix '&cmpname'
.ix 'WATFOR'
.ix 'WATFIV'
Like its predecessors, WATFOR and WATFIV, the compiler shortens the
time spent to develop FORTRAN programs.
A program is compiled directly into the computer's memory and then
executed.
A program starts executing much sooner by eliminating the usual
separate steps for compilation, linking and execution.
An interactive debugging system provides additional tools to detect
program bugs.
.np
The
.us &product User's Guide
describes how to use &product on systems running DOS or OS/2 2.0.
Versions of DOS 3.0 or later are supported.
System specific topics such as preconnection of files, record formats,
and FORTRAN subprogram library support, are also discussed.
.np
The
.us WATCOM FORTRAN 77 Language Reference
describes the FORTRAN 77 language and extensions to it which
are supported by &cmpname..
.*
.section &product Arithmetic Support
.*
.np
Two versions of &product are available.
The versions differ in the way integer and floating-point arithmetic
is handled.
.ix '&cmpcmdup.&exc'
One version, :FNAME.&cmpcmd.&exe:eFNAME., does
.us not
require the presence of the Intel 80x87 numeric data processor
(commonly called the "math coprocessor").
If the math coprocessor is present, it will be used to perform floating-point
computations.
If the math coprocessor is not present, all computations are performed using
the instruction set of the Intel 80x86.
.ix 'floating-point' 'support'
.ix 'floating-point' 'simulation'
.ix 'Intel 8088'
.ix 'Intel 8086'
Performing floating-point operations such as addition and
multiplication without the presence of computer instructions
specifically designed for this purpose is called floating-point
simulation.
.np
.ix '&cmp2cmdup.&exc'
.ix '80x87 support'
.ix 'math coprocessor'
.ix 'numeric data processor'
.ix 'Intel 80x87'
The other version, :FNAME.&cmp2cmd.&exe:eFNAME., uses the 80x87 math
coprocessor extensively for floating-point arithmetic during both the
compilation and execution phases and therefore requires the presence
of a math coprocessor.
.np
If a math coprocessor is present, either version of &product may be
used.
.np
If a math coprocessor is not present, the
:FNAME.&cmp2cmd.&exe:eFNAME.
version of WATFOR-77 will display the message
.millust begin
*ERR* math coprocessor is not present
.millust end
.pc
and then exit.
.if '&machine' = '80386' .do begin
.np
If you are running the OS/2-hosted version of &product, the OS/2 2.x
operating system supports 80x87 emulation which allows you to use the
:FNAME.&cmp2cmd.&exe:eFNAME.
version of WATFOR-77 even if your machine is not equipped with a math
coprocessor.
.do end
.np
There are some advantages to using the version of &cmpname which uses
the math coprocessor.
.autonote
.note
The compiler is smaller in size since simulation code is not
required.
Thus more memory is available to the FORTRAN program.
.note
Floating-point calculations will execute much faster than on a
computer where simulation is required.
.endnote
.*
.section The &cmpname User Interfaces
.*
.np
&cmpname provides two distinct ways of using the compiler.
One way of using the compiler is command-line oriented.
You prepare your FORTRAN program using some text editor
and then invoke the compiler to process your program.
The edit phase and the compile/execute phase are two distinctly
separate tasks and you usually alternate between the two.
We describe this mode of using the compiler as "batch" oriented
since, as we shall see, the compiler can compile and execute one or
more programs at a time.
The chapter entitled "Using &product", describes the batch oriented
compiler.
.np
A second way of using &cmpname is with an integrated editor.
.if '&target' ne 'NEC' .do begin
When invoked through &cmpname, the editor can be used
to create, modify, and run FORTRAN programs.
The editor and compiler are completely memory resident.
.do end
The editor can be used to retrieve and store FORTRAN source
code on the disk, make changes, etc.
Using the editor's "RUN" command, a FORTRAN program in the
editor's workspace can be compiled and executed.
When execution completes, the compiler returns control to the
editor.
Thus, a program can be modified and then tested without needing to
store the source code on disk each time you wish to compile and
execute it.
While this mode of operation uses additional memory, it provides a
complete program development environment.
.if '&target' ne 'NEC' .do begin
The chapter entitled "Using the &product Editor", describes the editor
user interface.
.do end
.if '&machine' = '80386' .do begin
.remark
The editor user interface is only available if you are using the DOS-hosted
versions of &cmpname..
.eremark
.do end
.*
.section Extensions to FORTRAN 77
.*
.np
.ix 'extension'
.ix 'WATFOR-77 extensions'
&cmpname provides many extensions to the FORTRAN 77 language
standard.
The
.us WATCOM FORTRAN 77 Language Reference
manual points out these extensions.
In general, use of an extension is diagnosed by the compiler.
There are some exceptions.
For example, the use of any function or subroutine
which is provided with the compiler but that is not described in
the language standard is not diagnosed.
.np
.ix 'message extension'
Extension messages are issued by the compiler when you select the
"extensions" option (see the chapter entitled "Compiler
Options").
.np
The user is advised that use of a language extension may preclude
portability of the FORTRAN source programs to other compiler
systems.
The user must weigh the benefits of a particular extension before
using it.
.*
.im envsetup
