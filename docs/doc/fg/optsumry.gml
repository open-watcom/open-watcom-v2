Compiler options may be entered in one of two places.
They may be included in the options list of the command line or they
may be included as comments of the form "C$option", "c$option", or
"*$option" in the source input stream.
The compiler recognizes these special comments as compiler directives.
.np
Some options may only be specified in the options list of the command
line.
Unless otherwise stated, an option can appear on the command line
only.
We also indicate what the default is for an option or group of
options.
.np
When specifying options in the source file, it is possible to
specify more than one option on a line.
For example, the following source line tells &cmpname to not issue any
warning or extension messages.
.exam begin
*$nowarn noext
.exam end
.pc
Note that only the first option must contain the "*$", "C$", or "c$"
prefix.
.np
Short forms are indicated by upper case letters.
.begnote $break
:DTHD.Option:
:DDHD.Description:
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note 0
.ixm 'options' '0'
(16-bit only)
&cmpname will make use of only 8088/8086 instructions in the generated
object code.
The resulting code will run on 8086 and all upward compatible
processors.
This is the default option for the 16-bit compiler.
.*
.note 1
.ixm 'options' '1'
(16-bit only)
&cmpname will make use of 188/186 instructions in the generated
object code whenever possible.
The resulting code probably will not run on 8086 compatible processors
but it will run on 186 and all upward compatible processors.
.*
.note 2
.ixm 'options' '2'
(16-bit only)
&cmpname will make use of 286 instructions in the generated object
code whenever possible.
The resulting code probably will not run on 8086 or 186 compatible
processors but it will run on 286 and all upward compatible
processors.
.*
.note 3
.ixm 'options' '3'
&cmpname will assume a 386 processor and will generate instructions
based on 386 instruction timings.
.*
.note 4
.ixm 'options' '4'
&cmpname will assume a 486 processor and will generate
386 instructions based on 486 instruction timings.
The code is optimized for 486 processors rather than 386
processors.
.*
.note 5
.ixm 'options' '5'
&cmpname will assume a Pentium processor and will generate
386 instructions based on Pentium instruction timings.
The code is optimized for Pentium processors rather than 386
processors.
This is the default option for the 32-bit compiler.
.*
.note 6
.ixm 'options' '6'
&cmpname will assume a Pentium Pro processor and will generate
386 instructions based on Pentium Pro instruction timings.
The code is optimized for Pentium Pro processors rather than 386
processors.
.*
.note [NO]ALign
.ixm 'ALIGN option'
.ixm 'options' 'ALIGN'
:cmt. .ixm 'NOALIGN option'
:cmt. .ixm 'options' 'NOALIGN'
The "align" option tells the compiler to allocate all COMMON blocks on
paragraph boundaries (multiples of 16).
If you do not want COMMON blocks to be aligned, specify "noalign".
The default is "align".
.*
.note [NO]AUtomatic
.ixm 'AUTOMATIC option'
.ixm 'options' 'AUTOMATIC'
:cmt. .ixm 'NOAUTOMATIC option'
:cmt. .ixm 'options' 'NOAUTOMATIC'
The "automatic" option tells the compiler to allocate all local
variables, including arrays, on the stack.
This is particularly useful for recursive functions or subroutines that
require a new set of local variables to be allocated for each recursive
invocation.
.ix 'stack size'
Note that the "automatic" option may significantly increase the stack
requirements of your application.
You can increase your stack size by using the "STACK" option when you
link your application.
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]ARraycheck
.ixm 'ARRAYCHECK option'
.ixm 'options' 'ARRAYCHECK'
:cmt. .ixm 'NOARRAYCHECK option'
:cmt. .ixm 'options' 'NOARRAYCHECK'
This option is used to control type checking on array arguments.
It may be placed anywhere in the source input stream or it may be
specified in the options list of the command line.
Specifying the "arraycheck" option turns on the generation of type
checking on array arguments while "noarraycheck" turns it off.
The default is "arraycheck".
.np
Using "noarraycheck" allows the programmer, for example, to pass an array
of type INTEGER to an array of type REAL.
Although this is not FORTRAN 77 standard, it allows the sharing of
array storage for data of different types.
You can selectively choose which argument lists are
.us not
to be checked by enclosing the calling statement with *$NOARRAYCHECK
and *$ARRAYCHECK.
.*
.do end
.*
.note BD
.ixm 'BD option'
.ixm 'options' 'BD'
(32-bit only, OS/2 and Windows NT only)
.ix 'dynamic link library applications'
.ix 'DLL applications'
This option causes the compiler to imbed the appropriate DLL library
name in the object file and to include the appropriate DLL
initialization code sequence when the application is linked.
.*
.note BM
.ixm 'BM option'
.ixm 'options' 'BM'
(32-bit only, OS/2 and Windows NT only)
.ix 'multi-threaded applications'
This option causes the compiler to imbed the appropriate multi-thread
library name in the object file.
.*
.note [NO]BOunds
.ixm 'BOUNDS option'
.ixm 'options' 'BOUNDS'
:cmt. .ixm 'NOBOUNDS option'
:cmt. .ixm 'options' 'NOBOUNDS'
The "bounds" option causes the generation of code that performs array
subscript and character substring bounds checking.
.ix 'debugging' 'bounds check'
Note that this option may significantly reduce the performance of your
application but is an excellent way to eliminate many programming
errors.
The default option is "nobounds".
.*
.note BW
.ixm 'BW option'
.ixm 'options' 'BW'
(OS/2, Windows 3.x, and Windows NT only)
This option causes the compiler to import a special symbol so that the
default windowing library code is linked into your application.
.*
.note [NO]CC
.ixm 'CC option'
.ixm 'options' 'CC'
:cmt. .ixm 'NOCC option'
:cmt. .ixm 'options' 'NOCC'
The "cc" option specifies that the output to devices contains carriage
control information that is to be interpreted appropriately for the
output device (e.g., console device).
ASA carriage control characters are converted to ASCII vertical
spacing control characters.
Note that a blank carriage control character will automatically be
generated for list-directed output and will be interpreted as a
single-line spacing command.
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]CHeck
.ixm 'CHECK option'
.ixm 'options' 'CHECK'
:cmt. .ixm 'NOCHECK option'
:cmt. .ixm 'options' 'NOCHECK'
.ix 'undefined variable'
.ix 'variable' 'undefined'
This option is used to control undefined variable checking by the
compiler.
It may be placed anywhere in the source input stream or it may be
specified in the options list of the command line.
Specifying the "check" option turns on the generation of undefined
variable checking code, while "nocheck" turns it off.
The default is "check".
.np
.ix 'hexadecimal 80'
The hexadecimal (base 16) value used for undefined variable checking
is 80.
This pattern is stored in each byte that represents the variable's
value before the start of program execution.
If a case arises where the legitimate value of a variable happens
to coincide with the undefined pattern, you may wish to place
*$NOCHECK and *$CHECK option lines around the FORTRAN statement
in question.
.np
At execution-time, formatted output of undefined variables
results in "?" characters being written to the output file.
To disable this feature, place *$NOCHECK and *$CHECK option lines
around the FORTRAN
.kw PRINT
or
.kw WRITE
statement in question.
.*
.do end
.*
.note CHInese
.ixm 'CHINESE option'
.ixm 'options' 'CHINESE'
This option is part of the national language support provided by
&cmpname..
It instructs the compiler that the source code contains characters
from the Traditional Chinese character set.
This includes double-byte characters.
This option enables the use of Chinese variable names.
The compiler's run-time system will ensure that character strings are
not split in the middle of a double-byte character when output spans
record boundaries (as can happen in list-directed output).
.if '&cmpclass' eq 'load-n-go' .do begin
The compiler's run-time undefined variable checking routines will ignore
the second byte of a double-byte character (see the description of the
"check" option).
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]COde
.ixm 'CODE option'
.ixm 'options' 'CODE'
:cmt. .ixm 'NOCODE option'
:cmt. .ixm 'options' 'NOCODE'
The "code" option causes the code generator to place character and
numeric constants in code segment.
Data generated for FORMAT statements will also be placed in the code
segment.
The default option is "nocode".
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note COdesize=n
.ixm 'CODESIZE option'
.ixm 'options' 'CODESIZE'
This option is used to control the maximum amount of memory that is to
be used for the object code of the source program.
.us n
must be an unsigned integer constant optionally followed by the letter
"k", in which case
.us n
is multiplied by 1024.
The amount of unused object memory is shown in the listing file
statistics as the "Object bytes free" statistic.
The default is 512k.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note D1
.ixm 'D1 option'
.ixm 'options' 'D1'
Line number information is included in the object file ("type 1
debugging information").
.ix 'debugging' 'd1'
This option provides additional information to &dbgname (at the
expense of larger object files and executable files).
Line numbers are handy when debugging your application with &dbgname..
.*
.note D2
.ixm 'D2 option'
.ixm 'options' 'D2'
In addition to line number information, local symbol and data type
information is included in the object file ("type 2 debugging
information").
Although global symbol information can be made available to &dbgname
through a &lnkname option, local symbol and typing information must be
requested when the source file is compiled.
.ix 'debugging' 'd2'
This option provides additional information to &dbgname (at the
expense of larger object files and executable files).
However, it will make the debugging chore somewhat easier.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
:CMT. .note [NO]DB
:CMT. .ixm 'DB option'
:CMT. .ixm 'options' 'DB'
:CMT. :cmt. .ixm 'NODB option'
:CMT. :cmt. .ixm 'options' 'NODB'
:CMT. The "db" option specifies that browsing information is to be emitted
:CMT. which can be used with the &brname..
:CMT. The browsing information is recorded in a file whose name is
:CMT. constructed from the source file name and the extension "MBR".
:CMT. The default option is "nodb".
.*
.do end
.*
.note [NO]DEBug
.ixm 'DEBUG option'
.ixm 'options' 'DEBUG'
:cmt. .ixm 'NODEBUG option'
:cmt. .ixm 'options' 'NODEBUG'
.if '&cmpclass' eq 'load-n-go' .do begin
This option is used to invoke the interactive debugger at execution time.
For more information on the interactive debugger, see the chapter
entitled "The &product Debugger".
The default is "nodebug".
.do end
.el .do begin
The "debug" option causes the generation of run-time checking code.
.ix 'debugging' 'bounds check'
.ix 'debugging' 'traceback'
This includes subscript and substring bounds checking as well as code
that allows a run-time traceback to be issued when an error occurs.
The default option is "nodebug".
.do end
.*
.note DEFine=<macro>
.ixm 'DEFINE=<macro> option'
.ixm 'options' 'DEFINE=<macro>'
.ix 'compiler directives' 'define'
This option is equivalent to specifying the following "define" compiler
directive.
.millust begin
*$define <macro>
.millust end
.pc
.ix 'macros'
The macro specified by the "define" option or compiler directive
becomes defined.
The definition status of the specified macro can be checked using the "ifdef",
"ifndef", "elseifdef" or "elseifndef" compiler directives.
.ix 'conditional compilation'
This allows source code to be conditionally compiled depending on the definition
status of the macro.
.ix 'macros' 'predefined'
.np
.ix 'predefined macros' '__i86__'
.ix '__i86__'
The macro
.mono __i86__
is a special macro that is defined by the compiler and identifies the
target as a 16-bit Intel 80x86 compatible environment.
.np
.ix 'predefined macros' '__386__'
.ix '__386__'
The macro
.mono __386__
is a special macro that is defined by the compiler and identifies the
target as a 32-bit Intel 386 compatible environment.
.np
.ix 'predefined macros' '__stack_conventions__'
.ix '__stack_conventions__'
The macro
.mono __stack_conventions__
is a special macro that is defined by the 32-bit compiler when the
"sc" compiler option is specified to indicate that stack calling
conventions are to be used for code generation.
.np
.ix 'predefined macros' '__fpi__'
.ix '__fpi__'
The macro
.mono __fpi__
is a special macro that is defined
.if '&cmpclass' eq 'load-n-go' .do begin
when using &c2cmdup (requires a math coprocessor) as opposed to &ccmdup
(no math coprocessor required).
.do end
.el .do begin
by the compiler when one of the following floating-point options is
specified: "fpi" or "fpi87".
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]DEPendency
.ixm 'DEPENDENCY option'
.ixm 'options' 'DEPENDENCY'
:cmt. .ixm 'NODEPENDENCY option'
:cmt. .ixm 'options' 'NODEPENDENCY'
The "dependency" option specifies that file dependencies are to be
included in the object file.
This is the default option.
This option is used by the &wide. to determine if an object file is
up-to-date with respect to the source files used to build it.
You can specify the "nodependency" option if you do not want file
dependencies to be included in the object file.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]DEScriptor
.ixm 'DESCRIPTOR option'
.ixm 'options' 'DESCRIPTOR'
:cmt. .ixm 'NODESCRIPTOR option'
:cmt. .ixm 'options' 'NODESCRIPTOR'
The "descriptor" option specifies that string descriptors are to be
passed for character arguments.
This is the default option.
You can specify the "nodescriptor" option if you do not want string
descriptors to be passed for character arguments.
Instead, the pointer to the actual character data and the length will
be passed as two arguments.
The arguments for the length will be passed as additional arguments
following the normal argument list.
For character functions, the pointer to the data and the length of the
character function will be passed as the first two arguments.
.*
.do end
.*
.note DIsk
.ixm 'DISK option'
.ixm 'options' 'DISK'
When this option is used in conjunction with the "list" option, the
listing file is written to the current directory of the default disk.
The listing file name will be the same as the source file name but the
file extension will be :FNAME.&lst:eFNAME..
By default, listing files are written to disk.
The "disk" option will override any previously specified "type" or
"print" option.
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note DT=<size>
.ixm 'DT=<size> option'
.ixm 'options' 'DT=<size>'
The "data threshold" option is used to set the minimum size for data
objects to be included in the default data segment.
Normally, all data objects smaller than 256 bytes in size are placed
in the default data segment.
When there is a large amount of static data, it is often useful to set
the data threshold size so that all objects of the specified size or
larger are placed into another segment.
For example, the option:
.millust begin
/DT=100
.millust end
.pc
causes all data objects of 100 bytes or more to be placed in a far
data segment.
The "data threshold" only applies to the large and huge memory models
where there can be more than one data segment.
The default data threshold value is 256.
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]EDit
.ixm 'EDIT option'
.ixm 'options' 'EDIT'
:cmt. .ixm 'NOEDIT option'
:cmt. .ixm 'options' 'NOEDIT'
.if '&target' eq 'NEC' .do begin
This option is not supported in the Japanese PC environment.
.do end
.el .do begin
This option is used to control whether the editor interface to &product
is to be used or not.
Specifying the "edit" option causes the editor user interface to be
used.
The default is "noedit" and causes the batch user interface to be used.
.np
The editor is supplied as the program :FNAME.weditf&exe:eFNAME..
When the "edit" option is specified, &product will load and run
the editor provided that it is located in the current directory
or a directory listed in the
.ev PATH
environment string.
.np
This option can only be specified when invoking &product from
the command line and not from the batch prompt (i.e. the
"WATFOR>" prompt).
.do end
.*
.do end
.*
.note [NO]ERrorfile
.ixm 'ERRORFILE option'
.ixm 'options' 'ERRORFILE'
:cmt. .ixm 'NOERRORFILE option'
:cmt. .ixm 'options' 'NOERRORFILE'
This option is used to control whether error messages are output to
a separate error file.
The error file is a disk file of type :FNAME.&err:eFNAME. and
is produced if any diagnostic messages are issued by the compiler.
Specifying "noerrorfile" prevents the creation of an error file.
By default, an error file is created.
.np
If an error file exists before compilation begins, it will be erased.
If no diagnostic messages are produced then an error file will not be
created even though the "errorfile" option is selected.
This option has no effect on the inclusion of diagnostic messages in
the source listing file or the production of diagnostic messages on
the screen.
.*
.note [NO]EXPlicit
.ixm 'EXPLICIT option'
.ixm 'options' 'EXPLICIT'
:cmt. .ixm 'NOEXPLICIT option'
:cmt. .ixm 'options' 'NOEXPLICIT'
The "explicit" option requires the type of all symbols to be
explicitly declared.
An error message will be issued by the compiler if a symbol that does
not appear in a type declaration statement is encountered.
Specifying this option is equivalent to using the
.kw IMPLICIT NONE
statement.
By default, symbols do not have to be explicitly typed.
.*
.note [NO]EXtensions
.ixm 'EXTENSIONS option'
.ixm 'options' 'EXTENSIONS'
:cmt. .ixm 'NOEXTENSIONS option'
:cmt. .ixm 'options' 'NOEXTENSIONS'
This option is used to control the printing of extension messages.
This option may be specified on the command line or it may be placed
anywhere in the source input stream.
In a source file, the option appears as a comment line and takes the
following form.
.millust begin
*$[NO]EXtensions
.millust end
.pc
The "extensions" option enables the printing of extension messages, while
"noextensions" disables the printing of these messages.
By default, extension messages are
.if '&cmpclass' ne 'load-n-go' .do begin
not
.do end
printed.
.*
.note [NO]EZ
.ixm 'EZ option'
.ixm 'options' 'EZ'
:cmt. .ixm 'NOEZ option'
:cmt. .ixm 'options' 'NOEZ'
(32-bit only)
&cmpname will generate an object file in Phar Lap Easy OMF-386 (object
module format) instead of the default Microsoft OMF.
The default option is "noez".
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note FO=<obj_default>
.ixm 'FO=<obj_default> option'
.ixm 'options' 'FO=<obj_default>'
By default, the object file name is constructed from the source file
name.
Using the "fo" option, the default object file drive, path, file name
and extension can be specified.
.exam begin
C>&ccmd32 report /fo=d:\programs\obj\
.exam end
.pc
A trailing "\" must be specified for directory names.
If, for example, the option was specified as "/fo=d:\programs\obj"
then the object file would be called
.fi D:\PROGRAMS\OBJ.OBJ.
.np
A default extension must be preceded by a period (".").
.exam begin
C>&ccmd32 report /fo=d:\programs\obj\.dbo
.exam end
.*
.do end
.*
.note [NO]FORmat
.ixm 'FORMAT option'
.ixm 'options' 'FORMAT'
:cmt. .ixm 'NOFORMAT option'
:cmt. .ixm 'options' 'NOFORMAT'
The "format" option suppresses the run-time checking that ensures that
the type of an input/output list item matches the format edit
descriptor in a format specification.
This allows an input/output list item of type INTEGER to be formatted
using an F, E or D edit descriptor.
It also allows an input/output list item of a floating-point type to
be formatted using an I edit descriptor.
Normally, this generates an error.
The "format" option is particularly useful for applications that use
integer arrays to store integer and floating-point data.
The default option is "noformat".
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]FOVer
.ixm 'FOVER option'
.ixm 'options' 'FOVER'
:cmt. .ixm 'NOFOVER option'
:cmt. .ixm 'options' 'NOFOVER'
The "fover" option causes the compiler to issue an error when a
floating-point overflow occurs during the execution of the program.
Generally, floating-point overflows indicate an error in the program
that should be corrected.
For this reason, the default for this option is "fover" (issue an
error when a floating-point overflow occurs).
If you wish to ignore floating-point overflows, specify the "nofover"
option.
.*
.note [NO]FUNder
.ixm 'FUNDER option'
.ixm 'options' 'FUNDER'
:cmt. .ixm 'NOFUNDER option'
:cmt. .ixm 'options' 'NOFUNDER'
.ix 'debugging' 'traceback'
The "funder" option causes the compiler to issue an error when a
floating-point underflow occurs during the execution of the program.
Generally, floating-point underflows do not indicate an error in the
program.
Typically, underflows occur when an iterative calculation converges to
zero.
For this reason, the default for this option is "nofunder" (ignore
floating-point underflows).
If you wish the compiler to issue an error when a floating-point
underflow occurs, specify the "funder" option.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note FPC
.ixm 'FPC option'
.ixm 'options' 'FPC'
All floating-point arithmetic is done with calls to a floating-point
emulation library.
This option should be used when speed of floating-point emulation is
favoured over code size.
.*
.note FPI
.ixm 'FPI option'
.ixm 'options' 'FPI'
(16-bit only)
&cmpname will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
Depending on which library the code is linked against, these
instructions will be left as is or they will be replaced by special
interrupt instructions.
In the latter case, floating-point will be emulated if an 80x87 is not
present.
This is the default floating-point option if none is specified.
.np
(32-bit only)
&cmpname will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
When any module containing floating-point operations is compiled with
the "fpi" option, coprocessor emulation software will be included in
the application when it is linked.
Thus, an 80x87 coprocessor need not be present at run-time.
This is the default floating-point option if none is specified.
.*
.note FPI87
.ixm 'FPI87 option'
.ixm 'options' 'FPI87'
(16-bit only)
&cmpname will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
An 80x87 coprocessor must be present at run-time.
If the "2" option is used in conjunction with this option,
&cmpname will generate 287/387 compatible instructions;
otherwise &cmpname will generate 8087 compatible instructions.
.np
(32-bit only)
&cmpname will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
When the "fpi87" option is used exclusively, coprocessor emulation
software is not included in the application when it is linked.
A 80x87 coprocessor must be present at run-time.
.ix 'numeric data processor' 'option'
.ix 'math coprocessor' 'option'
.ix 'floating-point' 'option'
.autonote 16-bit Notes:
.note
.ix 'floating-point' 'consistency of options'
When any module in an application is compiled with a particular
"floating-point" option, then all modules must be compiled with
the same option.
.note
Different math libraries are provided for applications which have been
compiled with a particular floating-point option.
See the chapter entitled :HDREF refid='fplibr'..
.endnote
.autonote 32-bit Notes:
.note
.ix 'floating-point' 'consistency of options'
When any module in an application is compiled with the "fpc" option,
then all modules must be compiled with the "fpc" option.
.note
When any module in an application is compiled with the "fpi" or
"fpi87" option, then all modules must be compiled with one of these
two options.
.note
If you wish to have floating-point emulation software included in
the application, you should select the "fpi" option.
A 387 coprocessor need not be present at run-time.
.note
Different math libraries are provided for applications which have been
compiled with a particular floating-point option.
See the chapter entitled :HDREF refid='fplibr'..
.endnote
.*
.note FP2
.ixm 'FP2 option'
.ixm 'options' 'FP2'
&cmpname will generate in-line 80x87 numeric data processor
instructions into the object code for floating-point operations.
For &company compilers generating 16-bit, this is the default.
For 32-bit applications, use this option if you wish to support those
few 386 systems that are equipped with an 80287 numeric data processor
("fp3" is the default for &company compilers generating 32-bit code).
However, for 32-bit applications, the use of this option will reduce
execution performance.
.*
.note FP3
.ixm 'FP3 option'
.ixm 'options' 'FP3'
&cmpname will generate in-line 387-compatible numeric data processor
instructions into the object code for floating-point operations.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
.*
.note FP5
.ixm 'FP5 option'
.ixm 'options' 'FP5'
&cmpname will generate in-line 387-compatible numeric data processor
instructions into the object code for floating-point operations.
The sequence of floating-point instructions will be optimized for
greatest possible performance on the Intel Pentium processor.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
.*
.note FP6
.ixm 'FP6 option'
.ixm 'options' 'FP6'
&cmpname will generate in-line 387-compatible numeric data processor
instructions into the object code for floating-point operations.
The sequence of floating-point instructions will be optimized for
greatest possible performance on the Intel Pentium Pro processor.
For 16-bit applications, the use of this option will limit the range
of systems on which the application will run but there are execution
performance improvements.
.*
.note [NO]FPD
.ixm 'FPD option'
.ixm 'options' 'FPD'
:cmt. .ixm 'NOFPD option'
:cmt. .ixm 'options' 'NOFPD'
.ix 'FDIV bug'
.ix 'Pentium bug'
A subtle problem was detected in the FDIV instruction of Intel's
original Pentium CPU.
In certain rare cases, the result of a floating-point divide could
have less precision than it should.
Contact Intel directly for more information on the issue.
.np
As a result, the run-time system startup code has been modified to
test for a faulty Pentium.
If the FDIV instruction is found to be flawed, the low order bit of
the run-time system variable
.mono __chipbug
will be set.
.np
If the FDIV instruction does not show the problem, the low order bit
will be clear.
If the Pentium FDIV flaw is a concern for your application, there are
two approaches that you could take:
.autonote
.note
You may test the
.mono __chipbug
variable in your code in all floating-point and memory models and take
appropriate action (such as display a warning message or discontinue
the application).
.note
Alternately, you can use the "fpd" option when compiling your code.
This option directs the compiler to generate additional code whenever
an FDIV instruction is generated which tests the low order bit of
.mono __chipbug
and, if on, calls the software workaround code in the math libraries.
If the bit is off, an in-line FDIV instruction will be performed as
before.
.endnote
.np
If you know that your application will never run on a defective
Pentium CPU, or your analysis shows that the FDIV problem will not
affect your results, you need not use the "fpd" option.
.*
.note FPR
.ixm 'FPR option'
.ixm 'options' 'FPR'
Use this option if you want to generate floating-point instructions
that will be compatible with version 9.0 or earlier of the compilers.
For more information on floating-point conventions see the sections
entitled :HDREF refid='prg87'. and :HDREF refid='prg387'..
.*
.note [NO]FSfloats
.ixm 'FSFLOATS option'
.ixm 'options' 'FSFLOATS'
:cmt. .ixm 'NOFSFLOATS option'
:cmt. .ixm 'options' 'NOFSFLOATS'
The "fsfloats" option enables the use of the FS segment register in
the generated code.
This is the default for all but the flat memory model.
In the flat memory model, the default is "nofsfloats" (the FS segment
register is not used in the generated code).
.*
.note [NO]GSfloats
.ixm 'GSFLOATS option'
.ixm 'options' 'GSFLOATS'
:cmt. .ixm 'NOGSFLOATS option'
:cmt. .ixm 'options' 'NOGSFLOATS'
The "gsfloats" option enables the use of the GS segment register in
the generated code.
This is the default.
If you would like to prevent the use of the GS segment register in the
the generated code, specify the "nogsfloats" option.
.*
.note HC
.ixm 'HC option'
.ixm 'options' 'HC'
The type of debugging information that is to be included in the object
file is "Codeview".
The default type of debugging information is "Dwarf" (HD).
If you wish to use the Microsoft Codeview debugger, then choose the
"HC" option.
When linking the application, you must also choose the appropriate
&lnkname DEBUG directive.
See the
.book &lnkname User's Guide
for more information.
.*
.note HD
.ixm 'HD option'
.ixm 'options' 'HD'
The type of debugging information that is to be included in the object
file is "Dwarf".
This is the default type of debugging information.
If you wish to use the Microsoft Codeview debugger, then choose the
"HC" option.
When linking the application, you must also choose the appropriate
&lnkname DEBUG directive.
See the
.book &lnkname User's Guide
for more information.
.note HW
.ixm 'HW option'
.ixm 'options' 'HW'
The type of debugging information that is to be included in the object
file is "&company".
The default type of debugging information is "Dwarf" (HD).
If you wish to use the Microsoft Codeview debugger, then choose the
"HC" option.
When linking the application, you must also choose the appropriate
&lnkname DEBUG directive.
See the
.book &lnkname User's Guide
for more information.
.*
.do end
.*
.note [NO]INCList
.ixm 'INCLIST option'
.ixm 'options' 'INCLIST'
:cmt. .ixm 'NOINCLIST option'
:cmt. .ixm 'options' 'NOINCLIST'
This option is used to control the listing of the contents of INCLUDE
files to the listing file.
The "inclist" option enables the listing of INCLUDE files, while
"noinclist" disables the listing of these files.
The default option is "noinclist".
.*
.note INCPath=[&dr4]path;[&dr4]path...
.ixm 'INCPATH option'
.ixm 'options' 'INCPATH'
This option is used to specify directories that are to be searched for
include files.
Each path is separated from the previous by a semicolon (";").
These directories are searched in the order listed before those in the
.ev FINCLUDE
environment variable.
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]IOVer
.ixm 'IOVER option'
.ixm 'options' 'IOVER'
:cmt. .ixm 'NOIOVER option'
:cmt. .ixm 'options' 'NOIOVER'
The "iover" option causes the compiler to issue an error when an
integer overflow occurs during the execution of the program.
Integer overflows are generated by many random number generators.
These integer overflows do not indicate an error in the program.
For this reason, the default for this option is "noiover" (ignore
integer overflows).
If you wish the compiler to issue an error when an integer overflow
occurs, specify the "iover" option.
.*
.do end
.*
.note [NO]IPromote
.ixm 'IPROMOTE option'
.ixm 'options' 'IPROMOTE'
:cmt. .ixm 'NOIPROMOTE option'
:cmt. .ixm 'options' 'NOIPROMOTE'
The "ipromote" option causes the compiler to promote the INTEGER*1 and
INTEGER*2 arguments of some INTEGER*4 intrinsics without issuing an
error diagnostic.
This allows code such as the following to be compiled without error:
.exam begin
    INTEGER I*1, J*2
    I = 1
    J = 2
    PRINT *, IDIM( I, J )
    END
.exam end
.np
This works for the following intrinsic functions:
ABS(), IABS(), DIM(), IDIM(), SIGN(), ISIGN(), MAX(), AMAX0(), MAX0(),
MIN(), AMIN0(), and MIN0().
When the "ipromote" option is specified, all integer arguments that
are passed to these functions are promoted to INTEGER*4.
.*
.note Japanese
.ixm 'JAPANESE option'
.ixm 'options' 'JAPANESE'
This option is part of the national language support provided by
&cmpname..
It instructs the compiler that the source code contains characters
from the Japanese character set.
This includes double-byte characters.
This option enables the use of Japanese variable names.
The compiler's run-time system will ensure that character strings are
not split in the middle of a double-byte character when output spans
record boundaries (as can happen in list-directed output).
.if '&cmpclass' eq 'load-n-go' .do begin
The compiler's run-time undefined variable checking routines will
ignore the second byte of a double-byte character (see the description
of the "check" option).
.do end
.*
.note KORean
.ixm 'KOREAN option'
.ixm 'options' 'KOREAN'
This option is part of the national language support provided by
&cmpname..
It instructs the compiler that the source code contains characters
from the Korean character set.
This includes double-byte characters.
This option enables the use of Korean variable names.
The compiler's run-time system will ensure that character strings are
not split in the middle of a double-byte character when output spans
record boundaries (as can happen in list-directed output).
.if '&cmpclass' eq 'load-n-go' .do begin
The compiler's run-time undefined variable checking routines will
ignore the second byte of a double-byte character (see the description
of the "check" option).
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]LFwithff
.ixm 'LFWITHFF option'
.ixm 'options' 'LFWITHFF'
:cmt. .ixm 'NOLFWITHFF option'
:cmt. .ixm 'options' 'NOLFWITHFF'
This option is used to control whether a line-feed character
(LF=CHAR(10)) is to be emitted before a form-feed character
(FF=CHAR(12)) is emitted.
This option applies to carriage control handling.
Normally, the run-time system will emit only a form-feed character to
cause a page eject when the ASA control character "1" is found in the
first column of a record.
The "lfwithff" option will cause the run-time system to emit a
line-feed character and then a form-feed character.
.np
The "lfwithff" option will have little effect on printers, but it will
change the appearance of output to the screen by eliminating
overwritten text when form-feed characters are not handled by the
output device.
The default option is "nolfwithff".
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]LIBinfo
.ixm 'LIBINFO option'
.ixm 'options' 'LIBINFO'
:cmt. .ixm 'NOLIBINFO option'
:cmt. .ixm 'options' 'NOLIBINFO'
This option is used to control the inclusion of default library
information in the object file.
The "libinfo" option enables the inclusion of default library
information, while "nolibinfo" disables the inclusion of this
information.
The default option is "libinfo".
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]LInk[=<lnk_file>]
.ixm 'LINK option'
.ixm 'options' 'LINK'
:cmt. .ixm 'NOLINK option'
:cmt. .ixm 'options' 'NOLINK'
This option controls the creation of an executable file.
When the "link" option is specified, the program is not placed into
execution but a file with extension "&exe" is created instead.
The data following the *$DATA compiler directive is ignored but the
source input stream is still terminated at the *$DATA line.
When the "nolink" option is specified, no executable file is created
and the program in memory is executed (provided there were no source
errors).
The default is "nolink".
.np
An optional link file can be specified.
This file contains additional object files and libraries that are to
be used to link your application.
For more information on the use of this option and the format of the
link file, see the chapter entitled "Linking with External
Subprograms".
.*
.do end
.*
.note [NO]LISt
.ixm 'LIST option'
.ixm 'options' 'LIST'
:cmt. .ixm 'NOLIST option'
:cmt. .ixm 'options' 'NOLIST'
This option may be specified on the command line or it may be placed
anywhere in the source input stream.
On the command line, this option is used to control the creation of a
listing file.
The "list" option causes a listing file to be created while "nolist"
requests that no listing file be created.
The default option is "nolist".
.np
In a source file, the option appears as a comment line and takes the
following form.
.millust begin
*$[NO]LISt
.millust end
.pc
Specifying *$LIST causes the source lines that follow this option to
be listed in the source listing file while
*$NOLIST disables the listing of the source lines that follow.
This option cannot appear on the same source line with other options.
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]LOgio
.ixm 'LOGIO option'
.ixm 'options' 'LOGIO'
:cmt. .ixm 'NOLOGIO option'
:cmt. .ixm 'options' 'NOLOGIO'
This option is used to enable or disable logging of all unit 5 and 6
input or output during execution time.
The log will appear in the listing file.
The format of logged input or output is described under the "LOGIO"
command in the chapter entitled "The &product Debugger".
This option may not be used to selectively log unit 5 and 6 input or
output, hence it should only be specified once.
.np
This option is useful for obtaining a record of an interactive,
screen-oriented application.
In an educational environment, the listing file may be turned in
by the student for examination by the teacher.
.*
.do end
.*
.note [NO]MAngle
.ixm 'MANGLE option'
.ixm 'options' 'MANGLE'
:cmt. .ixm 'NOMANGLE option'
:cmt. .ixm 'options' 'NOMANGLE'
This option is used to alter COMMON block segment and class names.
.exam begin
      REAL R, S
      COMMON /BLK/ R, S
      END
.exam end
.np
For a named COMMON block called "BLK", the default convention is to
name the segment "BLK" and the class "BLK".
.code begin
BLK             SEGMENT PARA COMMON USE32 'BLK'
.code end
.np
When you use the "mangle" option, the segment is named "_COMMON_BLK"
and the class is named "_COMMON_BLK_DATA".
.code begin
_COMMON_BLK     SEGMENT PARA COMMON USE32 '_COMMON_BLK_DATA'
.code end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note MC
.ixm 'MC option'
.ixm 'options' 'MC'
(32-bit only)
The "compact" memory model (small code, big data) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.*
.note MF
.ixm 'MF option'
.ixm 'options' 'MF'
.ixm 'default memory model'
(32-bit only)
The "flat" memory model (code and data up to 4 gigabytes) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
This is the default memory model option.
.*
.note MH
.ixm 'MH option'
.ixm 'options' 'MH'
(16-bit only)
The "huge" memory model (big code, huge data) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.*
.note ML
.ixm 'ML option'
.ixm 'options' 'ML'
.ixm 'default memory model'
The "large" memory model (big code, big data) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
This is the default 16-bit memory model option.
.*
.note MM
.ixm 'MM option'
.ixm 'options' 'MM'
The "medium" memory model (big code, small data) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.*
.note MS
.ixm 'MS option'
.ixm 'options' 'MS'
(32-bit only)
The "small" memory model (small code, small data) is selected.
The various models supported by &cmpname are described in the chapters
entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]OBject
.ixm 'OBJECT option'
.ixm 'options' 'OBJECT'
:cmt. .ixm 'NOOBJECT option'
:cmt. .ixm 'options' 'NOOBJECT'
This option is used to control the creation of an object file by the
compiler.
When the "object" option is specified, the program is not placed into
execution and a file of type :FNAME.&obj:eFNAME. is created instead.
When the "noobject" option is specified, no object file is created and
the program in memory is executed (provided there were no source
errors).
The data following the DATA directive is ignored but the source input
stream is still terminated.
The default is "noobject".
.*
.do end
.el .do begin
.*
.note OB
.ixm 'OB option'
.ixm 'options' 'OB'
(32-bit only)
This option allows the use of the ESP register as a base register to
reference local variables and subprogram arguments in the generated
code.
This can reduce the size of the prologue/epilogue sequences generated
by the compiler thus improving overall performance.
Note that when this option is specified, the compiler will abort when
there is not enough memory to optimize the subprogram.
By default, the code generator uses more memory-efficient algorithms
when a low-on-memory condition is detected.
.*
.note OBP
.ixm 'OBP option'
.ixm 'options' 'OBP'
This option causes the code generator to try to order the blocks of
code emitted such that the "expected" execution path (as determined by
a set of simple heuristics) will be straight through, with other cases
being handled by jumps to separate blocks of code "out of line".
This will result in better cache utilization on the Pentium.
If the heuristics do not apply to your code, it could result in a
performance decrease.
.*
.note OC
.ixm 'OC option'
.ixm 'options' 'OC'
This option may be used to disable the optimization where a "CALL"
followed by a "RET" (return) is changed into a "JMP" (jump)
instruction.
This option is required if you wish to link an overlayed program using
the Microsoft DOS Overlay Linker.
The Microsoft DOS Overlay Linker will create overlay calls for a
"CALL" instruction only.
This option is not required when using the &lnkname..
This option is not assumed by default.
.*
.note OD
.ixm 'OD option'
.ixm 'options' 'OD'
Non-optimized code sequences are generated.
The resulting code will be much easier to debug when using &dbgname..
By default, &cmpname will select "od" if "d2" is specified.
.*
.note ODO
.ixm 'ODO option'
.ixm 'options' 'ODO'
Optimized DO-loop iteration code is generated.
Caution should be exercised with the use of this option since the case
of an iterating value overflowing is assumed to never occur.
The following example should not be compiled with this option since
the terminal value of
.id IX
wraps from a positive integer to a negative integer.
.exam begin
    INTEGER*2 IX
    DO IX=32766,32767
        .
        .
        .
    ENDDO
.exam end
.np
The values of
.id IX
are 32766, 32767, -32768, -32767, ... since
.id IX
is INTEGER*2 (a 16-bit signed value) and it never exceeds the terminal
value.
.*
.note OF
.ixm 'OF option'
.ixm 'options' 'OF'
This option selects the generation of traceable stack frames for those
functions that contain calls or require stack frame setup.
To use &company's "Dynamic Overlay Manager" (DOS only), you must
compile all modules using the "of" option.
For near functions, the following function prologue sequence is
generated.
.np
.us 16-bit:
.millust begin
    push BP
    mov  BP,SP
.millust end
.np
.us 32-bit:
.millust begin
    push EBP
    mov  EBP,ESP
.millust end
.pc
For far functions, the following function prologue sequence is
generated.
.np
.us 16-bit:
.millust begin
    inc  BP
    push BP
    mov  BP,SP
.millust end
.np
.us 32-bit:
.millust begin
    inc  EBP
    push EBP
    mov  EBP,ESP
.millust end
.pc
The BP/EBP value on the stack will be even or odd depending on the
code model.
For 16-bit DOS systems, the Dynamic Overlay Manager uses this
information to determine if the return address on the stack is a short
address (16-bit offset) or long address (32-bit segment:offset).
This option is not assumed by default.
.*
.note OH
.ixm 'OH option'
.ixm 'options' 'OH'
This option enables repeated optimizations (which can result in longer
compiles).
.*
.note OI
.ixm 'OI option'
.ixm 'options' 'OI'
This option causes code for statement functions to be generated in-line.
.*
.note OK
.ixm 'OK option'
.ixm 'options' 'OK'
This option enables flowing of register save (from prologue) down into
the subprogram's flow graph.
.*
.note OL
.ixm 'OL option'
.ixm 'options' 'OL'
Loop optimizations are performed.
This includes moving loop-invariant expressions outside the loops.
This option is not assumed by default.
.*
.note OL+
.ixm 'OL+ option'
.ixm 'options' 'OL+'
Loop optimizations are performed including loop unrolling.
This includes moving loop-invariant expressions outside the loops
and can cause loops to be turned into straight-line code.
This option is not assumed by default.
.*
.note OM
.ixm 'OM option'
.ixm 'options' 'OM'
Generate inline 80x87 code for math functions like sin, cos, tan, etc.
If this option is selected, it is the programmer's responsibility to
make sure that arguments to these functions are within the range
accepted by the
.id fsin, fcos,
etc. instructions since no run-time check is made.
.np
If the "ot" option is also specified, the
.id exp
function is generated inline as well.
This option is not assumed by default.
.*
.note ON
.ixm 'ON option'
.ixm 'options' 'ON'
This option allows the compiler to perform certain numerical calculations in
a more efficient manner.
Consider the following example.
.millust begin
Z1 = X1 / Y
Z2 = X2 / Y
.millust end
.np
If the "on" option is specified, the code generator will generate code that
is equivalent to the following.
.millust begin
T = 1 / Y
Z1 = X1 * T
Z2 = X2 * T
.millust end
.np
Since floating-point multiplication is more efficient that division,
the code generator decided to first compute the reciprocal of Y and
then multiply X1 and X2 by the reciprocal of Y.
.np
Note that this optimization may produce less slightly different
results since some, for certain values, precision is lost when
computing the reciprocal.
By using this option, you are indicating that you are willing to
accept the loss in precision for the gain in performance.
.*
.note OP
.ixm 'OP option'
.ixm 'options' 'OP'
By default, floating-point variables may be cached in 80x87
floating-point registers across statements when compiling with the
"fpi" or "fpi87" options.
Floating-point register temporaries use 64 bits of precision in the
mantissa whereas single and double-precision variables use fewer bits
of precision in the mantissa.
The use of this option will force the result to be stored in memory
after each FORTRAN statement is executed.
This will produce less accurate but more predictable floating-point
results.
The code produced will also be less efficient when the "op" option
is used.
.exam begin
XMAX = X + Y / Z
YMAX = XMAX + Q
.exam end
.np
When the "op" option is used in conjunction with the "fpi" or "fpi87"
option, the compiler's code generator will update
.id XMAX
before proceeding with the second statement.
In the second statement, the compiler will reload
.id XMAX
from memory rather than using the result of the previous statement.
The effect of the "op" option on the resulting code can be seen by the
increased code size statistic as well as through the use of the
&disname..
This option is not assumed by default.
.*
.note OR
.ixm 'OR option'
.ixm 'options' 'OR'
This option enables reordering of instructions (instruction
scheduling) to achieve better performance on pipelined architectures
such as the 486.
Selecting this option will make it slightly more difficult to debug
because the assembly language instructions generated for a source
statement may be intermixed with instructions generated for
surrounding statements.
This option is not assumed by default.
.*
.note OS
.ixm 'OS option'
.ixm 'options' 'OS'
Space is favoured over time when generating code (smaller code but
possibly slower execution).
By default, &cmpname selects a balance between "space" and "time".
.*
.note OT
.ixm 'OT option'
.ixm 'options' 'OT'
Time is favoured over space when generating code (faster execution but
possibly larger code).
By default, &cmpname selects a balance between "space" and "time".
.*
.note OX
.ixm 'OX option'
.ixm 'options' 'OX'
Specifying the "ox" option is equivalent to specifying the
"ob" (32-bit only), "obp", "odo", "oi", "ok", "ol", "om", "or", and
"ot" options.
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note Pages=<n>
.ixm 'PAGES=<n> option'
.ixm 'options' 'PAGES=<n>'
This option is used to control the maximum number of pages that will
be produced in the listing file once the program enters the execution
phase.
.us n
must be an unsigned integer constant.
If
.us n
is 0, no limit on the number of pages is imposed.
This is the default.
.*
.note PAGESIze=<n>
.ixm 'PAGESIZE=<n> option'
.ixm 'options' 'PAGESIZE=<n>'
This option is used to specify the number of lines per page that your
printer can produce.
In this way, a listing file can be printed with the appropriate form
feeds.
The default page size is 66.
.*
.do end
.*
.note PRint
.ixm 'PRINT option'
.ixm 'options' 'PRINT'
This option is used to direct the listing file to the printer (device
name "PRN") instead of the disk.
The "print" option will override any previously specified "type" or
"disk" option.
The default is to create a listing file on the disk.
.*
.note [NO]Quiet
.ixm 'QUIET option'
.ixm 'options' 'QUIET'
:cmt. .ixm 'NOQUIET option'
:cmt. .ixm 'options' 'NOQUIET'
The "quiet" option suppresses the banner and summary information
produced by the compiler.
Only diagnostic messages will be displayed.
The default option is "noquiet".
.*
.note [NO]Reference
.ixm 'REFERENCE option'
.ixm 'options' 'REFERENCE'
:cmt. .ixm 'NOREFERENCE option'
:cmt. .ixm 'options' 'NOREFERENCE'
When the "reference" option is specified, warning messages will be
issued for all unreferenced symbols.
In a source file, the option appears as a comment line and takes the
following form.
.millust begin
*$[NO]Reference
.millust end
.pc
This option is most useful when used in an include file that is
included by several subprograms.
Consider an include file that defines many parameter constants and
only a few are referenced by any one subprogram.
If the first line of the include file is
.millust begin
*$noreference
.millust end
.pc
and the last line is
.millust begin
*$reference
.millust end
.pc
warning messages for all unused parameter constants in the include
file would be suppressed.
The default option is "reference".
.*
.note [NO]RESource
.ixm 'RESOURCE option'
.ixm 'options' 'RESOURCE'
:cmt. .ixm 'NORESOURCE option'
:cmt. .ixm 'options' 'NORESOURCE'
The "resource" option specifies that the run-time error messages are
contained as resource information in the executable file.
All messages will be extracted from the resource area of the
executable file when they are required; no messages will be linked
with the application.
The default option is "noresource".
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]RUN
.ixm 'RUN option'
.ixm 'options' 'RUN'
:cmt. .ixm 'NORUN option'
:cmt. .ixm 'options' 'NORUN'
This option is used to control whether the executable file generated by
specifying the "link" option is executed automatically by the compiler.
If the "run" option is specified, the executable file will be executed.
If the "norun" option is specified, the executable file will only be
generated and not executed.
The default option is "norun".
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]SAve
.ixm 'SAVE option'
.ixm 'options' 'SAVE'
:cmt. .ixm 'NOSAVE option'
:cmt. .ixm 'options' 'NOSAVE'
The "save" option is used to instruct &cmpname to "save" all local
variables of subprograms.
All local variables are treated as if they had appeared in FORTRAN 77
.kw SAVE
statements.
By default, local variables are not saved unless named in a
.kw SAVE
statement (i.e., "nosave" is the default option).
.*
.note [NO]SC
.ixm 'SC option'
.ixm 'options' 'SC'
:cmt. .ixm 'NOSC option'
:cmt. .ixm 'options' 'NOSC'
(32-bit only)
If the "sc" option is used, &cmpname will pass all arguments on the
stack.
The resulting code will be larger than that which is generated for the
register method of passing arguments.
The default option is "nosc".
.*
.note [NO]SEpcomma
.ixm 'SEPCOMMA option'
.ixm 'options' 'SEPCOMMA'
:cmt. .ixm 'NOSEPCOMMA option'
:cmt. .ixm 'options' 'NOSEPCOMMA'
The "sepcomma" option allows the comma (",") to be used as field
separator in formatted input.
Thus the following code would work with the input described.
.exam begin
    REAL R, S

    READ(5,21) R, S
    PRINT *, R, S
21  FORMAT(2F11.3)
    END
.exam end
.np
Normally the following input would result in a run-time error message.
.millust begin
0.79,0.21
.millust end
.*
.note [NO]SG
.ix 'stack growing'
.ixm 'SG option'
.ixm 'options' 'SG'
:cmt. .ixm 'NOSG option'
:cmt. .ixm 'options' 'NOSG'
(32-bit only)
.ix 'multi-threaded applications'
The "sg" option is useful for 32-bit OS/2 multi-threaded applications.
It requests the code generator to emit a run-time call at the start of
any function that has more than 4K bytes of automatic variables
(variables located on the stack).
Under 32-bit OS/2, the stack is grown automatically in 4K pages
using the stack "guard page" mechanism.
The stack consists of in-use committed pages topped off with a special
guard page.
A memory reference into the 4K guard page causes OS/2 to grow the
stack by one 4K page and to add a new 4K guard page.
This works fine when there is less than 4K of automatic variables in a
function.
When there is more than 4K of automatic data, the stack must be grown
in an orderly fashion, 4K bytes at a time, until the stack has grown
sufficiently to accommodate all the automatic variable storage
requirements.
.np
.ix 'stack growing' '__GRO'
.ix '__GRO' 'stack growing'
The "stack growth" run-time routine is called
.id __GRO.
.np
The default option is "nosg".
.*
.do end
.*
.note [NO]SHort
.ixm 'SHORT option'
.ixm 'options' 'SHORT'
:cmt. .ixm 'NOSHORT option'
:cmt. .ixm 'options' 'NOSHORT'
The "short" option is used to instruct &cmpname to set the default
INTEGER size to 2 bytes and the default LOGICAL size to 1 bytes.
As required by the FORTRAN 77 language standard, the default INTEGER
size is 4 bytes and the default LOGICAL size is 4 bytes.
The default option is "noshort".
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]SR
.ixm 'SR option'
.ixm 'options' 'SR'
:cmt. .ixm 'NOSR option'
:cmt. .ixm 'options' 'NOSR'
The "sr" option instructs &cmpname to generate subprogram prologue and
epilogue sequences that save and restore any segment registers that
are modified by the subprogram.
Caution should be exercised when using this option.
.ix 'GPF'
.ix 'general protection fault'
If the value of the segment register being restored matches the value
of a segment that was freed within the subprogram, a general
protection fault will occur in protected-mode environments.
The default, "nosr",  does not save and restore segment registers.
.*
.note [NO]SSfloats
.ixm 'SSFLOATS option'
.ixm 'options' 'SSFLOATS'
:cmt. .ixm 'NOSSFLOATS option'
:cmt. .ixm 'options' 'NOSSFLOATS'
(16-bit only)
The "ssfloats" option specifies that the segment register SS does not
necessarily point to the default data segment.
.ix 'multi-threaded applications'
.ix 'dynamic link library applications'
.ix 'DLL applications'
The "ssfloats" option must be specified when compiling a module that
is part of an OS/2 multi-threaded application or dynamic link library.
By default, it is assumed that the SS segment register contains the
segment address of the default data segment (i.e., "nossfloats").
.*
.note [NO]STack
.ixm 'STACK option'
.ixm 'options' 'STACK'
:cmt. .ixm 'NOSTACK option'
:cmt. .ixm 'options' 'NOSTACK'
If "stack" is specified, &cmpname will emit code at the beginning of
every subprogram that will check for the "stack overflow" condition.
By default, stack overflow checking is omitted from the generated code
("nostack").
.*
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note Statements=<n>
.ixm 'STATEMENTS=<n> option'
.ixm 'options' 'STATEMENTS=<n>'
This option is used to control the maximum number of statements that
will be executed once the program enters the execution phase.
.us n
must be an unsigned integer constant.
If
.us n
is 0, no limit on the number statements that will be executed is
imposed.
This is the default.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]SYntax
.ixm 'SYNTAX option'
.ixm 'options' 'SYNTAX'
:cmt. .ixm 'NOSYNTAX option'
:cmt. .ixm 'options' 'NOSYNTAX'
If "syntax" is specified, &cmpname will check the source code only and
omit the generation of object code.
Syntax checking, type checking, and so on are performed as usual.
By default, code is generated if there are no source code errors
(i.e., "nosyntax" is the default).
.*
.do end
.*
.note [NO]TErminal
.ixm 'TERMINAL option'
.ixm 'options' 'TERMINAL'
:cmt. .ixm 'NOTERMINAL option'
:cmt. .ixm 'options' 'NOTERMINAL'
The "noterminal" option may be used to suppress the display of
diagnostic messages to the screen.
By default, diagnostic messages are displayed.
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note Time=<t>
.ixm 'TIME=<t> option'
.ixm 'options' 'TIME=<t>'
This option is used to control the maximum amount of time, in seconds,
that is allotted to a program once it begins executing.
.us t
must be an unsigned integer constant.
If
.us t
is 0, no limit on the execution time is imposed.
This is the default.
.np
Note that execution time is the elapsed time using the time-of-day
clock and includes time for input/output.
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]TRace
.ixm 'TRACE option'
.ixm 'options' 'TRACE'
:cmt. .ixm 'NOTRACE option'
:cmt. .ixm 'options' 'NOTRACE'
The "trace" option causes the generation of code that allows a
traceback to be issued when an error occurs during the execution of
your program.
The default option is "notrace".
.*
.do end
.*
.note TYpe
.ixm 'TYPE option'
.ixm 'options' 'TYPE'
This option is used to direct the listing file to the terminal (device
name "CON") instead of the disk.
The "type" option will override any previously specified "print" or
"disk" option.
The default is to create a listing file on the disk.
.*
.note [NO]WArnings
.ixm 'WARNINGS option'
.ixm 'options' 'WARNINGS'
:cmt. .ixm 'NOWARNINGS option'
:cmt. .ixm 'options' 'NOWARNINGS'
This option is used to control the printing of warning messages.
By default, warning messages are printed.
This option may be specified on the command line or it may be placed
anywhere in the source input stream.
In a source file, the option appears as a comment line and takes the
following form.
.millust begin
*$[NO]WArnings
.millust end
.pc
The "warnings" option enables the printing of warning messages, while
"nowarnings" disables the printing of these messages.
.*
.note [NO]WILd
.ixm 'WILD option'
.ixm 'options' 'WILD'
:cmt. .ixm 'NOWILD option'
:cmt. .ixm 'options' 'NOWILD'
The "wild" option suppresses the compile-time checking that normally
causes an error to be issued when an attempt is made to transfer
control into a block structure from outside the block structure and
vice versa.
For example, this option will allow a transfer of control into an
IF-block from outside the IF-block (which is normally prohibited).
The default option is "nowild".
.np
Extreme caution should be exercised when using this option.
For example, transfer of control into a DO-loop from outside the
DO-loop can cause unpredictable results.
This programming style is not encouraged by this option.
The option has been made available so that existing programs that do
not adhere to the branching restrictions imposed by the FORTRAN 77
standard (i.e. mainframe applications that are being ported to the PC
environment), can be compiled by &product..
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]WIndows
.ixm 'WINDOWS option'
.ixm 'options' 'WINDOWS'
:cmt. .ixm 'NOWINDOWS option'
:cmt. .ixm 'options' 'NOWINDOWS'
(16-bit only)
The "windows" option causes the compiler to generate the
prologue/epilogue code sequences necessary for use in Microsoft
Windows applications.
The default option is "nowindows".
.*
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.note [NO]XFloat
.ixm 'XFLOAT option'
.ixm 'options' 'XFLOAT'
:cmt. .ixm 'NOXFLOAT option'
:cmt. .ixm 'options' 'NOXFLOAT'
.ix 'increased precision'
The "xfloat" option specifies that all REAL variables are treated as
if they had been declared as "DOUBLE PRECISION".
This effectively increases the precision of REAL variables.
Note that the "xfloat" option has implications on the alignment of
variables in common blocks.
The default option is "noxfloat".
.*
.do end
.*
.note [NO]Xline
.ixm 'XLINE option'
.ixm 'options' 'XLINE'
:cmt. .ixm 'NOXLINE option'
:cmt. .ixm 'options' 'NOXLINE'
The "xline" option informs the &cmpname compiler to extend the last
column of the statement portion of a line to column 132.
The default is 72.
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.note [NO]XType
.ixm 'XTYPE option'
.ixm 'options' 'XTYPE'
:cmt. .ixm 'NOXTYPE option'
:cmt. .ixm 'options' 'NOXTYPE'
The "xtype" option is used to indicate that all output to unit 6
should be written to the terminal instead of the listing file.
If "noxtype" is specified, all output to unit 6 will appear following
the source program listing.
The connection of unit 6 to the terminal or listing file using the
"xtype" option may be overridden by a preconnection specification or a
FORTRAN
.kw OPEN
statement.
The default is "xtype".
.*
.do end
.*
.endnote
.*
