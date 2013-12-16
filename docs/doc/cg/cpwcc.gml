.chap *refid=cpwcc The &product Compilers
.*
.np
.ix 'compiler' 'features'
This chapter covers the following topics.
.begbull
.bull
Command line syntax
(see :HDREF refid='wccclf'.)
.bull
Environment variables used by the compilers
(see :HDREF refid='wccenv'.)
.bull
Examples of command line syntax
(see :HDREF refid='wcccle'.)
.bull
Interpreting diagnostic messages
(see :HDREF refid='wccdiag'.)
.bull
#include file handling
(see :HDREF refid='wccinc'.)
.bull
Using the preprocessor built into the compilers
(see :HDREF refid='wccprep'.)
.bull
System-dependent macros predefined by the compilers
(see :HDREF refid='wccmacs'.)
.bull
Additional keywords supported by the compilers
(see :HDREF refid='wccext'.)
.bull
Based pointer support in the compilers
(see :HDREF refid='wccbase'.)
.bull
Notes about the Code Generator
(see :HDREF refid='wccgen'.)
.endbull
.*
.section *refid=wccclf &product Command Line Format
.*
.np
.ix 'command line format'
.ix 'compiling' 'command line format'
The formal &product command line syntax is shown below.
.ix 'invoking &product'
.mbigbox
compiler_name [options] [file_spec] [options] [@extra_opts]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.note compiler_name
is one of the &product compiler command names.
.ix 'command name' 'compiler'
.if '&target' eq 'QNX' .do begin
.begnote $compact
.note &ccmd16
is the &cmpcname compiler for 16-bit Intel platforms.
.note &pcmd16
is the &cmppname compiler for 16-bit Intel platforms.
.note &ccmd32
is the &cmpcname compiler for 32-bit Intel platforms.
.note &pcmd32
is the &cmppname compiler for 32-bit Intel platforms.
.if '&alpha' eq 'AXP' .do begin
.note &ccmdAX
is the &cmpcname compiler for DEC Alpha AXP platforms.
.note &pcmdAX
is the &cmppname compiler for DEC Alpha AXP platforms.
.do end
.endnote
.do end
.el .do begin
.begnote $compact
.note &ccmdup16
is the &cmpcname compiler for 16-bit Intel platforms.
.note &pcmdup16
is the &cmppname compiler for 16-bit Intel platforms.
.note &ccmdup32
is the &cmpcname compiler for 32-bit Intel platforms.
.note &pcmdup32
is the &cmppname compiler for 32-bit Intel platforms.
.if '&alpha' eq 'AXP' .do begin
.note &ccmdupAX
is the &cmpcname compiler for DEC Alpha AXP platforms.
.note &pcmdupAX
is the &cmppname compiler for DEC Alpha AXP platforms.
.do end
.endnote
.do end
.note file_spec
is the file name specification of
one or more files to be compiled.
If file_spec is specified as the single character ".", an input file
is read from standard input and the output file name defaults to
stdin.obj.
.if '&target' ne 'QNX' .do begin
.np
If no drive is specified, the default drive is assumed.
.do end
.np
If no path is specified, the current working directory is assumed.
.np
.ix 'directories' 'C'
.ix 'C directory'
.ix 'options' 'xx'
If the "xx" option was not specified and the file is not in the current
directory then an adjacent "C" directory (i.e.,
.fi ~..&pc.c
.ct ) is searched if it exists.
.np
.ix 'filename extension'
.ix 'default filename extension'
.ix 'extension' 'default'
If no file extension is specified, the compiler will check for a file
with one of the following extensions in the order listed:
.if '&target' eq 'QNX' .do begin
.begnote $compact
.note ~.cpp
(C++ only)
.note ~.cc
(C++ only)
.note ~.c
(C/C++)
.endnote
.np
A QNX filename extension consists of that portion of a filename
containing the last "." and any characters which follow it.
.exam begin 5
File Specification              Extension
/home/john.doe/foo              (none)
/home/john.doe/foo.             .
/home/john.doe/foo.bar          .bar
/home/john.doe/foo.goo.bar      .bar
.exam end
.do end
.el .do begin
.begnote $compact
.note ~.CPP
(C++ only)
.note ~.CC
(C++ only)
.note ~.C
(C/C++)
.endnote
.do end
.np
If a period "." is specified but not the extension, the file is
assumed to have no filename extension.
.np
If only the compiler name is specified then the compiler will display
a list of available options.
.note options
.ix 'command line options' 'compiler'
is a list of valid compiler options, each preceded by
.if '&target' ne 'QNX' .do begin
a slash ("/") or
.do end
a dash ("&minus.").
Options may be specified in any order.
.note extra_opts
.ix 'command line options' 'environment variable'
.ix 'environment variable' 'command line options'
.ix 'command line options' 'options file'
.ix 'options file' 'command line options'
is the name of an environment variable or file which contains
additional command line options to be processed.
If the specified environment variable does not exist, a search is made
for a file with the specified name.
.ix 'occ file extension'
If no file extension is included in the specified name, the default
file extension is ".occ".
A search of the current directory is made.
.ix 'directories' 'OCC'
.ix 'OCC directory'
If not successful, an adjacent "OCC" directory (i.e.,
.fi ~..&pc.occ
.ct ) is searched if it exists.
.endnote
.*
.if '&target' ne 'QNX' .do begin
.*
.section *refid=wccdll &product DLL-based Compilers
.*
.np
.ix 'DLL compilers'
.ix 'compiling' 'using DLL compilers'
The compilers are also available in Dynamic Link Library (DLL) form.
.begnote $compact
.note WCCD
is the DLL version of the &cmpcname compiler for 16-bit Intel platforms.
.note WPPDI86
is the DLL version of the &cmppname compiler for 16-bit Intel platforms.
.note WCCD386
is the DLL version of the &cmpcname compiler for 32-bit Intel platforms.
.note WPPD386
is the DLL version of the &cmppname compiler for 32-bit Intel platforms.
.if '&alpha' eq 'AXP' .do begin
.note WCCDAXP
is the DLL version of the &cmpcname compiler for DEC Alpha AXP platforms.
.note WPPDAXP
is the DLL version of the &cmppname compiler for DEC Alpha AXP platforms.
.do end
.endnote
.np
The DLL versions of the compilers can be loaded from the &wide (&vip)
and &makname..
.*
.do end
.*
.section *refid=wccenv Environment Variables
.*
.np
.ix 'environment variables'
Environment variables can be used to specify commonly used compiler
options.
There is one environment variable for each compiler (the name of the
environment variable is the same as the compiler name).
The &product environment variable names are:
.ix '&setcmdup'
.*
.dm ixev begin
.ix 'environment variables' '&*'
.ix '&* environment variable'
.dm ixev end
.*
.begnote
.note &cvarup16
.ixev &cvarup16
used with the &cmpcname compiler for 16-bit Intel platforms
.exam begin 1
&prompt.&setcmd &setdelim.&cvar16=&sw.d1 &sw.ot&setdelim
.exam end
.note &pvarup16
.ixev &pvarup16
used with the &cmppname compiler for 16-bit Intel platforms
.exam begin 1
&prompt.&setcmd &setdelim.&pvar16=&sw.d1 &sw.ot&setdelim
.exam end
.note &cvarup32
.ixev &cvarup32
used with the &cmpcname compiler for 32-bit Intel platforms
.exam begin 1
&prompt.&setcmd &setdelim.&cvar32=&sw.d1 &sw.ot&setdelim
.exam end
.note &pvarup32
.ixev &pvarup32
used with the &cmppname compiler for 32-bit Intel platforms
.exam begin 1
&prompt.&setcmd &setdelim.&pvar32=&sw.d1 &sw.ot&setdelim
.exam end
.if '&alpha' eq 'AXP' .do begin
.note &cvarupAX
.ixev &cvarupAX
used with the &cmpcname compiler for DEC Alpha AXP platforms
.exam begin 1
&prompt.&setcmd &setdelim.&cvarAX=&sw.d1 &sw.ot&setdelim
.exam end
.note &pvarupAX
.ixev &pvarupAX
used with the &cmppname compiler for DEC Alpha AXP platforms
.exam begin 1
&prompt.&setcmd &setdelim.&pvarAX=&sw.d1 &sw.ot&setdelim
.exam end
.do end
.endnote
.np
The options specified in environment variables are processed before
options specified on the command line.
The above examples define the default options to be "d1" (include line
number debugging information in the object file), and "ot" (favour
time optimizations over size optimizations).
.if '&target' ne 'QNX' .do begin
.np
.ix 'environment string' '= substitute'
.ix 'environment string' '#'
Whenever you wish to specify an option that requires the use of an
"=" character, you can use the "#" character in its place.
This is required by the syntax of the "&setcmdup" command.
.do end
.np
Once a particular environment variable has been defined, those options
listed become the default each time the associated compiler is used.
The compiler command line can be used to override any options
specified in the environment string.
.*
.if '&target' ne 'QNX' .do begin
.np
These environment variables are not examined by the &wclname utilities.
Since the &wclname utilities pass the relevant options found in their
associated environment variables to the compiler command line, their
environment variable options take precedence over the options
specified in the environment variables associated with the compilers.
.do end
.keep 18
.hint
.if '&target' eq 'QNX' .do begin
.ix 'user initialization file'
If you use the same compiler options all the time, you may find it
handy to define the environment variable in your user initialization
file.
.do end
.el .do begin
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT'
If you are running DOS and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
DOS system initialization file,
.fi AUTOEXEC.BAT.
.np
.ix 'system initialization' 'Windows NT'
.ix 'Windows NT' 'system initialization'
If you are running Windows NT, use the "System" icon in the
.bd Control Panel
to define environment variables.
.np
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS'
If you are running OS/2 and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
OS/2 system initialization file,
.fi CONFIG.SYS.
.do end
.ehint
.*
.section *refid=wcccle &product Command Line Examples
.*
.np
The following are some examples of using &product to compile &lang
source programs.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. report &sw.d1 &sw.s
.exam end
.pc
The compiler processes
.fi report.c(pp)
producing an object file which contains source line number
information.
Stack overflow checking is omitted from the object code.
.ix 'memory model'
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. &sw.mm &sw.fpc calc
.exam end
.pc
The compiler compiles
.fi calc.c(pp)
for the Intel "medium" memory model and generates calls to
floating-point library emulation routines for all floating-point
operations.
Memory models are described in the
chapter entitled :HDREF refid='mdl86'..
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. kwikdraw &sw.2 &sw.fpi87 &sw.oaxt
.exam end
.pc
The compiler processes
.fi kwikdraw.c(pp)
producing 16-bit object code for an Intel 286 system equipped with an
Intel 287 numeric data processor (or any upward compatible 386/387,
486DX, or Pentium system).
While the choice of these options narrows the number of microcomputer
systems where this code will execute, the resulting code will be
highly optimized for this type of system.
.ix 'memory model'
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. &sw.mf &sw.3s calc
.exam end
.pc
The compiler compiles
.fi calc.c(pp)
for the Intel 32-bit "flat" memory model.
The compiler will generate 386 instructions based on 386 instruction
timings using the stack-based argument passing convention.
The resulting code will be optimized for Intel 386 systems.
Memory models are described in the
chapter entitled :HDREF refid='mdl386'..
Argument passing conventions are described in the
chapter entitled :HDREF refid='cal386'..
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. kwikdraw &sw.4r &sw.fpi87 &sw.oaimxt
.exam end
.pc
The compiler processes
.fi kwikdraw.c(pp)
producing 32-bit object code for an Intel 386-compatible system
equipped with a 387 numeric data processor.
The compiler will generate 386 instructions based on 486 instruction
timings using the register-based argument passing convention.
The resulting code will be highly optimized for Intel 486 systems.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. ..&pc.source&pc.modabs &sw.d2
.exam end
.pc
The compiler processes
.fi ~..&pc.source&pc.modabs.c(pp)
(a file in a directory which is adjacent to the current one).
The object file is placed in the current directory.
Included with the object code and data is information on local
symbols and data types.
The code generated is straight-forward, unoptimized code which can
be readily debugged with the &dbgname..
.exam begin 2
.if '&target' eq 'QNX' .do begin
&prompt.&setcmd &setdelim.:SF font=1.compiler_name:eSF.=&sw.i=&pc.includes &sw.mc&setdelim.
.do end
.el .do begin
&prompt.&setcmd &setdelim.:SF font=1.compiler_name:eSF.=&sw.i#&pc.includes &sw.mc&setdelim.
.do end
&prompt.:SF font=1.compiler_name:eSF. &pc.cprogs&pc.grep.tst &sw.fi=iomods.c
.exam end
.pc
The compiler processes the program contained in the file
.fi &pc.cprogs&pc.grep.tst.
The file
.fi iomods.c
is included as if it formed part of the source input stream.
The include search path and memory model options are defaults each
time the compiler is invoked.
The memory model option could be overridden on the command line.
After looking for an "include" file in the current directory, the
compiler will search each directory listed in the "i" path.
See the section entitled :HDREF refid='wccinc'. for more information.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. grep &sw.fo=..&pc.obj&pc.
.exam end
.pc
The compiler processes the program contained in the file
.fi grep.c(pp)
which is located in the current directory.
The object file is placed in the directory
.fi ~..&pc.obj
under the name
.fi grep&obj..
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. &sw.dDBG=1 grep &sw.fo=..&pc.obj&pc..dbo
.exam end
.pc
The compiler processes the program contained in the file
.fi grep.c(pp)
which is located in the current directory.
The macro "DBG" is defined so that conditional debugging statements
that have been placed in the source are compiled.
The object file is placed in the directory
.fi ~..&pc.obj
and its filename extension will be ".dbo" (instead of "&obj").
Selection of a different filename extension permits easy
identification of object files that have been compiled with debugging
statements.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. &sw.g=GKS &sw.s &pc.gks&pc.gopks
.exam end
.pc
The compiler generates code for
.fi gopks.c(pp)
and places it into the "GKS" group.
If the "g" option had not been specified, the code would not have
been placed in any group.
Assume that this file contains the definition of the routine
.id gopengks
as follows:
.millust begin
void far gopengks( int workstation, long int h )
{
    .
    .
    .
}
.millust end
.pc
For a small code model, the routine
.id gopengks
must be defined in this file as
.kwm far
since it is placed in another group.
The "s" option is also specified to prevent a run-time call to the
stack overflow check routine which will be placed in a different
code segment at link time.
The
.id gopengks
routine must be prototyped by C routines in other groups as
.millust begin
void far gopengks( int workstation, long int h );
.millust end
.pc
since it will appear in a different code segment.
.*
.section *refid=wccfast Benchmarking Hints
.*
:INCLUDE file='CPBENCH'
.*
.section *refid=wccdiag Compiler Diagnostics
.*
.np
.ix 'diagnostics' '&product'
.ix 'error file' '&err'
If the compiler prints diagnostic messages to the screen, it will also
place a copy of these messages in a file in your current directory.
The file will have the same file name as the source file and an
extension of "&err".
The compiler issues two types of diagnostic messages, namely
warnings or errors.
A warning message does not prevent the production of an object
file.
However, error messages indicate that a problem is severe enough
that it must be corrected before the compiler will produce an
object file.
The error file is a handy reference when you wish to correct the
errors in the source file.
.np
Just to illustrate the diagnostic features of &product, we will modify
the "hello" program in such a way as to introduce some errors.
.exam begin
#include <stdio.h>

int main()
  {
     int x;
     printf( "Hello world\n" );
     return( y );
  }
.exam end
.np
The equivalent C++ program follows:
.exam begin
#include <iostream.h>
#include <iomanip.h>

int main()
{
    int x;
    cout << "Hello world" << endl;
    return( y );
}
.exam end
.np
In this example, we have added the lines:
.millust begin
int x;
.millust end
.pc
and
.millust begin
return( y );
.millust end
.pc
and changed the keyword
.kwm void
to
.kwm int
.ct .li .
.np
We compile the program with the "warning" option.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. hello &sw.w3
.exam end
.np
For the C program, the following output appears on the screen.
.code begin
hello.c(7): Error! E1011: Symbol 'y' has not been declared
hello.c(5): Warning! W202: Symbol 'x' has been defined, but not
                           referenced
hello.c: 8 lines, included 174, 1 warnings, 1 errors
.code end
.np
For the C++ program, the following output appears on the screen.
.code begin
hello.cpp(8): Error! E029: (col 13) symbol 'y' has not been declared
hello.cpp(9): Warning! W014: (col 1) no reference to symbol 'x'
hello.cpp(9): Note! N392: (col 1) 'int x' in 'int main( void )'
              defined in: hello.cpp(6) (col 9)
hello.cpp: 9 lines, included 1628, 1 warning, 1 error
.code end
.np
.ix 'diagnostics' 'error'
.ix 'diagnostics' 'warning'
Here we see an example of both types of messages.
An error and a warning message have been issued.
As indicated by the error message, we require a declarative
statement for the identifier
.id y.
The warning message indicates that, while it is not a violation of
the rules of &lang. to define a variable without ever using it, we
probably did not intend to do so.
Upon examining the program, we find that:
.autopoint
.point
the variable
.id x
should have been assigned a value, and
.point
the variable
.id y
has probably been incorrectly typed and should have been entered as
.id x.
.endpoint
.if &e'&dohelp eq 0 .do begin
.np
The complete list of &product diagnostic messages is presented
in an appendix of this guide.
.do end
.*
.section *refid=wccinc &product #include File Processing
.*
.np
.ix 'include' 'directive'
.ix 'include' 'header file'
.ix 'include' 'source file'
.ix 'header file' 'including'
.ix 'source file' 'including'
.ix '#include'
When using the
.id #include
preprocessor directive, a header is identified by a sequence of
characters placed between the "<" and ">" delimiters (e.g., <file>)
and a source file is identified by a sequence of characters enclosed
by quotation marks (e.g., "file").
&product makes a distinction between the use of "<>" or quotation
marks to surround the name of the file to be included.
The search techniques for header files and source files are slightly
different.
Consider the following example.
.exam begin 2
#include <stdio.h>  /* a system header file */
#include "stdio.h"  /* your own header or source file */
.exam end
.pc
You should use "<" and ">" when referring to standard or system header
files and quotation marks when referring to your own header and source
files.
.np
The character sequence placed between the delimiters in an
.id #include
directive represents the name of the file to be included.
The file name may include &drive, path, and extension.
.np
It is not necessary to include the &drive and path specifiers in the
file specification when the file resides on a different &drive or in a
different directory.
&product provides a mechanism for looking up include files which may
be located in various directories and disks of the computer system.
&product searches directories for header and source files in the
following order (the search stops once the file has been located):
.ix 'include file' 'searching'
.ix 'header file' 'searching'
.ix 'source file' 'searching'
.autonote
.note
If the file specification enclosed in quotation marks ("file-spec") or
angle brackets (<file-spec>) contains the complete &drive and path
specification, that file is included (provided it exists).
No other searching is performed.
The &drive need not be specified in which case the current &drive is
assumed.
.note
.ix 'options' 'xx'
Next, if the "xx" option was not specified and the file specification is
enclosed in quotation marks then the current directory is searched.
.note
Next, if the file specification is enclosed in quotation marks, the
directory of the file containing the
.id #include
directive is searched.
.note
Next, if the "xx" option was not specified and the current file is also an
.id #include
file then the directory of the parent file is searched next.
This search continues recursively through all the nested
.id #include
files until the original source file's directory is searched.
.note
.ix 'options' 'i'
Next, if the file specification enclosed in quotation marks
("file-spec") or in angle brackets (<file-spec>), each directory
listed in the "i" path is searched (in the order that they were
specified).
.note
.ix 'options' 'bt'
.ix 'options' 'x'
Next, if the "x" option was not specified, each directory listed in the
.ev <os>_INCLUDE
environment variable is searched (in the order that they were
specified).
The environment variable name is constructed from the current build
target name.
The default build targets are:
.begpoint
.point DOS
when the host operating system is DOS,
.point OS2
when the host operating system is OS/2,
.point NT
when the host operating system is Windows NT/95, or
.point QNX
when the host operating system is QNX.
.point LINUX
when the host operating system is Linux.
.endpoint
.np
For example, the environment variable
.ev OS2_INCLUDE
will be searched if the build target is "OS2".
The build target would be OS/2 if:
.autopoint
.point
the host operating system is OS/2 and the "bt" option was not
specified, or
.point
the "bt=OS2" option was explicitly specified.
.endpoint
.note
Next, if the "x" option was not specified, each directory listed in the
.ev &incvarup
environment variable is searched (in the order that they were
specified).
.note
.ix '&hdrdirup directory'
Finally, if the "xx" option was not specified and the file specification
is enclosed in quotation marks then an adjacent "H" directory (i.e.,
.fi ~..&pc.h
.ct ) is searched if it exists.
.endnote
.np
In the above example,
.id <stdio.h>
and
.id "stdio.h"
could refer to two different files if there is a
.fi stdio.h
in the current directory and one in the &product include file
directory
.fi (&pathnamup&hdrdirup)
and the current directory is not listed in an "i" path or the
.ev &incvarup
environment variable.
.np
The compiler will search the directories listed in "i" paths (see
description of the "i" option) and the
.ev &incvarup
environment variable in a manner analogous to that which the operating
system shell will use when searching for programs by using the
.ev PATH
environment variable.
.np
The "&setcmdup" command is used to define an
.ev &incvarup
environment variable that contains a list of directories.
.ix '&setcmdup' '&incvarup environment variable'
A command of the form
.if '&target' eq 'QNX' .do begin
.millust begin
&setcmd &incvarup=path&ps.path...
.millust end
.do end
.el .do begin
.millust begin
&setcmdup &incvarup=[d:]path;[d:]path...
.millust end
.do end
.pc
is issued before running &product the first time.
.if '&target' ne 'QNX' .do begin
The brackets indicate that the drive "d:" is optional and the ellipsis
indicates that any number of paths may be specified.
For Windows NT, use the "System" icon in the Control Panel to define
environment variables.
.do end
.np
We illustrate the use of the
.id #include
directive in the following example.
.exam begin
#include <stdio.h>
#include <time.h>
#include <dos.h>
.exam break
#include "common.c"

int main()
  {
    initialize();
    update_files();
    create_report();
    finalize();
  }
.exam break
#include "part1.c"
#include "part2.c"
.exam end
.pc
If the above text is stored in the source file
.fi report.c
in the current directory then we might issue the following commands to
compile the application.
.exam begin 5
.if '&target' ne 'QNX' .do begin
&prompt.rem -- Two places to look for include files
.do end
&prompt.&setcmd &incvar=&dr3.&pathnam.&hdrdir.&ps.&dr2.&pc.headers
.if '&target' ne 'QNX' .do begin
&prompt.rem -- Now compile application specifying a
&prompt.rem    third location for include files
.do end
&prompt.:SF font=1.compiler_name:eSF. report &sw.fo=..&pc.obj&pc &sw.i=..&pc.source
.exam end
.pc
.ix '&setcmdup' '&incvarup environment variable'
In the above example, the "&setcmdup" command is used to define the
.ev &incvarup
environment variable.
It specifies that the
.fi &pathnam&hdrdir
directory
.if '&target' eq 'QNX' .do begin
(of the current node)
.do end
.el .do begin
(of the "C" disk)
.do end
and the
.fi &pc.headers
directory
.if '&target' eq 'QNX' .do begin
(a directory on node 1)
.do end
.el .do begin
(a directory of the "B" disk)
.do end
are to be searched.
.np
.ix 'options' 'i'
The &product "i" option defines a third place to search for
include files.
The advantage of the
.ev &incvarup
environment variable is that it need not be specified each time the
compiler is run.
.*
.section *refid=wccprep &product Preprocessor
.*
.np
.ix 'preprocessor'
The &product preprocessor forms an integral part of &product..
When any form of the "p" option is specified, only the preprocessor is
invoked.
No code is generated and no object file is produced.
The output of the preprocessor is written to the standard output file,
although it can also be redirected to a file using the "fo" option.
Suppose the following &lang. program is contained in the file
.fi msgid.c.
.keep 20
.exam begin
#define _IBMPC 0
#define _IBMPS2 1

#if _TARGET == _IBMPS2
char *SysId = { "IBM PS/2" };
#else
char *SysId = { "IBM PC" };
#endif

/* Return pointer to System Identification */

char *GetSysId()
  {
    return( SysId );
  }
.exam end
.pc
We can use the &product preprocessor to generate the &lang. code that
would actually be compiled by the compiler by issuing the following
command.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. msgid &sw.plc &sw.fo &sw.d_TARGET=_IBMPS2
.exam end
.pc
The file
.fi msgid&pxt
will be created and will contain the following &lang. code.
.millust begin
#line 1 "msgid.c"




char *SysId = { "IBM PS/2" };
#line 9 "msgid.c"

/* Return pointer to System Identification */

char *GetSysId()
  {
    return( SysId );
  }
.millust end
.pc
Note that the file
.fi msgid&pxt
can be used as input to the compiler.
.exam begin 1
&prompt.:SF font=1.compiler_name:eSF. msgid&pxt
.exam end
.pc
Since
.id #line
directives are present in the file, the compiler can issue error
messages in terms of the original source file line numbers.
.*
.section *refid=wccmacs &product Predefined Macros
.*
.np
In addition to the standard ISO-defined macros supported by the
&product compilers, several additional system-dependent macros are
also defined.
These are described in this section.
See the
.book &company C Language Reference
manual for a description of the standard macros.
.np
The &product compilers run on various host operating systems
including DOS, OS/2, Windows NT, Windows 95 and QNX.
Any of the supported host operating systems can be used to develop
applications for a number of target systems.
By default, the target operating system for the application is the
same as the host operating system unless some option or combination of
options is specified.
For example, DOS applications are built on DOS by default, OS/2
applications are built on OS/2 by default, and so on.
But the flexibility is there to build applications for other operating
systems/environments.
.np
The macros described below may be used to identify the target system
for which the application is being compiled.
(Note: In several places in the following text, a pair of underscore
characters appears as __ which resembles a single, elongated
underscore.)
.np
The &product compilers support both 16-bit and 32-bit application
development.
The following macros are defined for 16-bit and 32-bit target systems.
.ix 'macros' '__X86__'
.ix '__X86__'
.ix 'macros' '__I86__'
.ix '__I86__'
.ix 'macros' '__386__'
.ix '__386__'
.ix 'macros' 'M_I86'
.ix 'M_I86'
.ix 'macros' 'M_I386'
.ix 'M_I386'
.ix 'macros' '_M_I86'
.ix '_M_I86'
.ix 'macros' '_M_I386'
.ix '_M_I386'
.ix 'macros' '_M_IX86'
.ix '_M_IX86'
.millust begin
 16-bit      32-bit
========    ========
__X86__     __X86__
__I86__     __386__
M_I86       M_I386
_M_I86      _M_I386
_M_IX86     _M_IX86
.millust end
.autonote Notes:
.note
The
.kwm __X86__
identifies the target as an Intel environment.
.note
The
.kwm __I86__
.ct ,
.kwm M_I86
and
.kwm _M_I86
macros identify the target as a 16-bit Intel environment.
.note
The
.kwm __386__
.ct ,
.kwm M_I386
and
.kwm _M_I386
macros identify the target as a 32-bit Intel environment.
.note
The
.kwm _M_IX86
macro is identically equal to 100 times the architecture compiler
option value (&sw.0, &sw.1, &sw.2, &sw.3, &sw.4, &sw.5, etc.).
If "&sw.5" (Pentium instruction timings) was specified as a compiler
option, then the value of
.kwm _M_IX86
would be 500.
.endnote
.np
The &product compilers support application development for a
variety of operating systems.
The following macros are defined for particular target operating
systems.
.ix 'macros' '_DOS'
.ix '_DOS'
.ix 'macros' '__DOS__'
.ix '__DOS__'
.ix 'macros' 'MSDOS'
.ix 'MSDOS'
.ix 'macros' '__OS2__'
.ix '__OS2__'
.ix 'macros' '__QNX__'
.ix '__QNX__'
.ix 'macros' '__NETWARE__'
.ix '__NETWARE__'
.ix 'macros' '__NETWARE_386__'
.ix '__NETWARE_386__'
.ix 'macros' '__NT__'
.ix '__NT__'
.ix 'macros' '_WINDOWS'
.ix '_WINDOWS'
.ix 'macros' '__WINDOWS__'
.ix '__WINDOWS__'
.ix 'macros' '__WINDOWS_386__'
.ix '__WINDOWS_386__'
.ix 'macros' '__UNIX__'
.ix '__UNIX__'
.ix 'macros' '__LINUX__'
.ix '__LINUX__'
.millust begin
Target      Macros
======      ======================================
DOS         __DOS__, _DOS, MSDOS
OS/2        __OS2__
QNX         __QNX__, __UNIX__
Netware     __NETWARE__, __NETWARE_386__
NT          __NT__
Windows     __WINDOWS__, _WINDOWS, __WINDOWS_386__
Linux       __LINUX__, __UNIX__
.millust end
.autonote Notes:
.note
The
.kwm __DOS__
.ct ,
.kwm _DOS
and
.kwm MSDOS
macros are defined when the build target is "DOS" (16-bit DOS or
32-bit extended DOS).
.note
The
.kwm __OS2__
macro is defined when the build target is "OS2" (16-bit or 32-bit
OS/2).
.note
The
.kwm __QNX__
and
.kwm __UNIX__
macros are defined when the build target is "QNX" (16-bit or 32-bit
QNX).
.note
The
.kwm __NETWARE__
and
.kwm __NETWARE_386__
macros are defined when the build target is "NETWARE" (Novell NetWare).
.note
The
.kwm __NT__
macro is defined when the build target is "NT" (Windows NT and Windows
95).
.note
The
.kwm __WINDOWS__
macro is defined when the build target is "WINDOWS" or one of the
"zw", "zW", "zWs" options is specified (identifies the target
operating system as 16-bit Windows or 32-bit extended Windows but not
Windows NT or Windows 95).
.note
The
.kwm _WINDOWS
macro is defined when the build target is "WINDOWS" or one of the
"zw", "zW", "zWs" options is specified and you are using a 16-bit
compiler (identifies the target operating system as 16-bit Windows).
.note
The
.kwm __WINDOWS_386__
macro is defined when the build target is "WINDOWS" or the "zw" option
is specified and you are using a 32-bit compiler (identifies the
target operating system as 32-bit extended Windows).
.note
The
.kwm __LINUX__
and
.kwm __UNIX__
macros are defined when the build target is "LINUX" (32-bit Linux).
.endnote
.np
The following macros are defined for the indicated options.
.ix 'macros' '__FPI__'
.ix '__FPI__'
.ix 'macros' '__CHAR_SIGNED__'
.ix '__CHAR_SIGNED__'
.ix 'macros' '__INLINE_FUNCTIONS__'
.ix '__INLINE_FUNCTIONS__'
.ix 'macros' 'NO_EXT_KEYS'
.ix 'NO_EXT_KEYS'
.millust begin
Option    Macro
====== ==================
bm     _MT
br     _DLL
fpi    __FPI__
fpi87  __FPI__
j      __CHAR_SIGNED__
oi     __INLINE_FUNCTIONS__
xr     _CPPRTTI (C++ only)
xs     _CPPUNWIND (C++ only)
xss    _CPPUNWIND (C++ only)
xst    _CPPUNWIND (C++ only)
za     NO_EXT_KEYS
zw     __WINDOWS__
zW     __WINDOWS__
zWs    __WINDOWS__
.millust end
.np
The following memory model macros are defined for the indicated memory
model options.
.ix 'macros' '__FLAT__'
.ix 'macros' '__SMALL__'
.ix 'macros' '__MEDIUM__'
.ix 'macros' '__COMPACT__'
.ix 'macros' '__LARGE__'
.ix 'macros' '__HUGE__'
.ix 'macros' 'M_I86SM'
.ix 'macros' 'M_I86MM'
.ix 'macros' 'M_I86CM'
.ix 'macros' 'M_I86LM'
.ix 'macros' 'M_I86HM'
.ix 'macros' '_M_I86SM'
.ix 'macros' '_M_I86MM'
.ix 'macros' '_M_I86CM'
.ix 'macros' '_M_I86LM'
.ix 'macros' '_M_I86HM'
.ix 'macros' 'M_386FM'
.ix 'macros' 'M_386SM'
.ix 'macros' 'M_386MM'
.ix 'macros' 'M_386CM'
.ix 'macros' 'M_386LM'
.ix 'macros' '_M_386FM'
.ix 'macros' '_M_386SM'
.ix 'macros' '_M_386MM'
.ix 'macros' '_M_386CM'
.ix 'macros' '_M_386LM'
.millust begin
Option     All        16-bit only          32-bit only
====== =========== =================    =================
mf     __FLAT__                         _M_386FM  M_386FM
ms     __SMALL__   _M_I86SM  M_I86SM    _M_386SM  M_386SM
mm     __MEDIUM__  _M_I86MM  M_I86MM    _M_386MM  M_386MM
mc     __COMPACT__ _M_I86CM  M_I86CM    _M_386CM  M_386CM
ml     __LARGE__   _M_I86LM  M_I86LM    _M_386LM  M_386LM
mh     __HUGE__    _M_I86HM  M_I86HM
.millust end
.np
The following macros indicate which compiler is compiling the C/C++
source code.
.begnote $break
.*
.note __cplusplus
.ix 'macros' '__cplusplus'
.ix '__cplusplus'
&cmppname predefines the macro
.kwm __cplusplus
to identify the compiler as a C++ compiler.
.*
.note __WATCOMC__
.ix 'macros' '__WATCOMC__'
.ix '__WATCOMC__'
&product predefines the macro
.kwm __WATCOMC__
to identify the compiler as one of the &product compilers.
.np
The value of the macro depends on the version number of the compiler.
The value is 100 times the version number (version 8.5 yields 850,
version 9.0 yields 900, etc.). Note that for Open Watcom 1.0, the
value of this macro is 1200, for Open Watcom 1.1 it is 1210 etc.
.*
.note __WATCOM_CPLUSPLUS__
.ix 'macros' '__WATCOM_CPLUSPLUS__'
.ix '__WATCOM_CPLUSPLUS__'
&cmppname predefines the macro
.kwm __WATCOM_CPLUSPLUS__
to identify the compiler as one of the &cmppname compilers.
.np
The value of the macro depends on the version number of the compiler.
The value is 100 times the version number (version 10.0 yields 1000,
version 10.5 yields 1050, etc.). Note that for Open Watcom 1.0, the
value of this macro is 1200, for Open Watcom 1.1 it is 1210 etc.
.*
.endnote
.np
The following macros are defined for compatibility with Microsoft.
.begnote $break
.*
.note __CPPRTTI
.ix 'macros' '__CPPRTTI'
.ix '__CPPRTTI'
&cmppname predefines the
.kwm __CPPRTTI
macro to indicate that C++ Run-Time Type Information (RTTI) is in
force.
This macro is predefined if the &cmppname "xr" compile option is
specified and is not defined otherwise.
.*
.note __CPPUNWIND
.ix 'macros' '__CPPUNWIND'
.ix '__CPPUNWIND'
&cmppname predefines the
.kwm __CPPUNWIND
macro to indicate that C++ exceptions supported.
This macro is predefined if any of the &cmppname "xs", "xss" or "xst"
compile options are specified and is not defined otherwise.
.*
.note _INTEGRAL_MAX_BITS
.ix 'macros' '_INTEGRAL_MAX_BITS'
.ix '_INTEGRAL_MAX_BITS'
.ix 'keywords' '__int64'
&product predefines the
.kwm _INTEGRAL_MAX_BITS
macro to indicate that maximum number of bits supported in an integral
type (see the description of the "__int64" keyword in the next section).
Its value is 64 currently.
.*
.note _PUSHPOP_SUPPORTED
.ix 'macros' '_PUSHPOP_SUPPORTED'
.ix '_PUSHPOP_SUPPORTED'
&product predefines the
.kwm _PUSHPOP_SUPPORTED
macro to indicate that
.mono #pragma pack(push)
and
.mono #pragma pack(pop)
are supported.
.*
.note _STDCALL_SUPPORTED
.ix 'macros' '_STDCALL_SUPPORTED'
.ix '_STDCALL_SUPPORTED'
&product predefines the
.kwm _STDCALL_SUPPORTED
macro to indicate that the standard 32-bit Win32 calling convention is
supported.
.endnote
.np
The following table summarizes the predefined macros supported by the
compilers and the values that the respective compilers assign to them.
A "yes" under the column means that the compiler supports the macro
with the indicated value.
Note that the C and C++ compilers sometime support the same macro but
with different values (including no value which means the symbol is
defined without a value).
.code begin
Predefined Macro                Supported by Compiler
 and Setting                wcc     wcc386  wpp     wpp386  note
--------------------------- ------  ------  ------  ------ ---------
__386__=1                             Yes             Yes
__3R__=1                                              Yes
_based=__based                Yes     Yes     Yes     Yes  extension
_cdecl=__cdecl                Yes     Yes     Yes     Yes  extension
cdecl=__cdecl                 Yes     Yes     Yes     Yes  extension
__cplusplus=1                                 Yes     Yes
_CPPRTTI=1                                    Yes     Yes
_CPPUNWIND=1                                  Yes     Yes
_export=__export              Yes     Yes     Yes     Yes  extension
_far16=__far16                Yes     Yes     Yes     Yes  extension
_far=__far                    Yes     Yes     Yes     Yes  extension
far=__far                     Yes     Yes     Yes     Yes  extension
_fastcall=__fastcall          Yes     Yes     Yes     Yes  extension
__FLAT__=1                            Yes             Yes
_fortran=__fortran            Yes     Yes     Yes     Yes  extension
fortran=__fortran             Yes     Yes     Yes     Yes  extension
__FPI__=1                     Yes     Yes     Yes     Yes
_huge=__huge                  Yes     Yes     Yes     Yes  extension
huge=__huge                   Yes     Yes     Yes     Yes  extension
__I86__=1                     Yes             Yes
_inline=__inline              Yes     Yes     Yes     Yes  extension
_INTEGRAL_MAX_BITS=64         Yes     Yes     Yes     Yes
_interrupt=__interrupt        Yes     Yes     Yes     Yes  extension
interrupt=__interrupt         Yes     Yes     Yes     Yes  extension
_loadds=__loadds              Yes     Yes     Yes     Yes  extension
_M_386FM=1                            Yes             Yes
M_386FM=1                             Yes             Yes  extension
_M_I386=1                             Yes             Yes
M_I386=1                              Yes             Yes  extension
_M_I86=1                      Yes             Yes
M_I86=1                       Yes             Yes          extension
_M_I86SM=1                    Yes             Yes
M_I86SM=1                     Yes             Yes          extension
_M_IX86=0                     Yes             Yes
_M_IX86=500                           Yes             Yes
_near=__near                  Yes     Yes     Yes     Yes  extension
near=__near                   Yes     Yes     Yes     Yes  extension
__NT__=1 (on Win32 platform)  Yes     Yes     Yes     Yes
_pascal=__pascal              Yes     Yes     Yes     Yes  extension
pascal=__pascal               Yes     Yes     Yes     Yes  extension
_saveregs=__saveregs          Yes     Yes     Yes     Yes  extension
_segment=__segment            Yes     Yes     Yes     Yes  extension
_segname=__segname            Yes     Yes     Yes     Yes  extension
_self=__self                  Yes     Yes     Yes     Yes  extension
__SMALL__=1                   Yes             Yes
SOMDLINK=__far                Yes                          extension
SOMDLINK=_Syscall                     Yes             Yes  extension
SOMLINK=__cdecl               Yes                          extension
SOMLINK=_Syscall                      Yes             Yes  extension
_STDCALL_SUPPORTED=1                  Yes             Yes
__SW_0=1                      Yes             Yes
__SW_3R=1                             Yes             Yes
__SW_5=1                              Yes             Yes
__SW_FP287=1                                  Yes
__SW_FP2=1                    Yes
__SW_FP387=1                                          Yes
__SW_FP3=1                            Yes
__SW_FPI=1                    Yes     Yes     Yes     Yes
__SW_MF=1                             Yes             Yes
__SW_MS=1                     Yes             Yes
__SW_ZDP=1                    Yes     Yes     Yes     Yes
__SW_ZFP=1                    Yes     Yes     Yes     Yes
__SW_ZGF=1                            Yes             Yes
__SW_ZGP=1                    Yes             Yes
_stdcall=__stdcall            Yes     Yes     Yes     Yes  extension
_syscall=__syscall            Yes     Yes     Yes     Yes  extension
__WATCOM_CPLUSPLUS__=&vermacro                     Yes     Yes
__WATCOMC__=&vermacro              Yes     Yes     Yes     Yes
__X86__=1                     Yes     Yes     Yes     Yes
.code end
.*
Note: "extension" mean it is old extension macros (non-ISO compliant names).
Thay are suppressed by following options: -zam, -za, -zA
.*
.section *refid=wccext &product Extended Keywords
.*
.np
&product supports the use of some special keywords to describe system
dependent attributes of functions and other object names.
These attributes are inspired by the Intel processor architecture and
the plethora of function calling conventions in use by compilers for
this architecture.
In keeping with the ISO C and C++ language standards, &product
uses the double underscore (i.e., "__") or single underscore followed
by uppercase letter (e.g., "_S") prefix with these keywords.
To support compatibility with other C/C++ compilers, alternate forms
of these keywords are also supported through predefined macros.
.*
.begnote $break
.*
.note __near &optdag.
.ix 'keywords' '__near'
&product supports the
.kwm __near
keyword to describe functions and other object names that are in near
memory and pointers to near objects.
.np
&product predefines the macros
.kwm near
and
.kwm _near
to be equivalent to the
.kwm __near
keyword.
.*
.note __far &optdag.
.ix 'keywords' '__far'
&product supports the
.kwm __far
keyword to describe functions and other object names that are in far
memory and pointers to far objects.
.np
&product predefines the macros
.kwm far
.ct ,
.kwm _far
and
.kwm SOMDLINK
(16-bit only)
to be equivalent to the
.kwm __far
keyword.
.*
.note __huge &optdag.
.ix 'keywords' '__huge'
&product supports the
.kwm __huge
keyword to describe functions and other object names that are in huge
memory and pointers to huge objects.
The 32-bit compilers treat these as equivalent to far objects.
.np
&product predefines the macros
.kwm huge
and
.kwm _huge
to be equivalent to the
.kwm __huge
keyword.
.note __based
.ix 'keywords' '__based'
.ix 'segment references'
&product supports the
.kwm __based
keyword to describe pointers to objects that appear in other segments
or the objects themselves.
See the section entitled :HDREF refid='wccbase'. for an explanation of
the
.kwm __based
keyword.
.np
&product predefines the macro
.kwm _based
to be equivalent to the
.kwm __based
keyword.
.*
.note __segment
.ix 'keywords' '__segment'
.ix 'segment references'
&product supports the
.kwm __segment
keyword which is used when describing objects of type segment.
See the section entitled :HDREF refid='wccbase'. for an explanation of
the
.kwm __segment
keyword.
.np
&product predefines the macro
.kwm _segment
to be equivalent to the
.kwm __segment
keyword.
.*
.note __segname
.ix 'keywords' '__segname'
.ix 'segname references'
&product supports the
.kwm __segname
keyword which is used when describing segname constant based pointers
or objects.
See the section entitled :HDREF refid='wccbase'. for an explanation of
the
.kwm __segname
keyword.
.np
&product predefines the macro
.kwm _segname
to be equivalent to the
.kwm __segname
keyword.
.*
.note __self
.ix 'keywords' '__self'
.ix 'self references'
&product supports the
.kwm __self
keyword which is used when describing self based pointers.
See the section entitled :HDREF refid='wccbase'. for an explanation of
the
.kwm __self
keyword.
.np
&product predefines the macro
.kwm _self
to be equivalent to the
.kwm __self
keyword.
.*
.note __restrict
.ix 'keywords' '__restrict'
&product provides the
.kw __restrict
type qualifier as an alternative to the ISO C99
.kw restrict
keyword; it is supported even when C99 keywords aren't visible. This type
qualifier is used as an optimization hint. Any object accessed through a
.kw restrict
qualified pointer may only be accessed through that pointer and the compiler
may assume that there will be no aliasing.
.*
.note _Packed
.ix 'keywords' '_Packed'
.ix 'segment references'
&product supports the
.kwm _Packed
keyword which is used when describing a structure.
If specified before the
.kw struct
keyword, the compiler will force the structure to be packed (no
alignment, no gaps) regardless of the setting of the command-line
option or the
.kw #pragma
controlling the alignment of members.
.*
.note __cdecl
.ix 'keywords' '__cdecl'
&product supports the
.kwm __cdecl
keyword to describe C functions that are called using a special
convention.
.autonote Notes:
.note
All symbols are preceded by an underscore character.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The calling routine will remove the arguments from the stack.
.note
Floating-point values are returned in the same way as structures.
When a structure is returned, the called routine returns a pointer
in register AX/EAX to the return value which is stored in the data
segment (DGROUP).
.note
For the 16-bit compiler, registers AX, BX, CX and DX, and segment
register ES are not saved and restored when a call is made.
.note
For the 32-bit compiler, registers EAX, ECX and EDX are not saved and
restored when a call is made.
.endnote
.np
&product predefines the macros
.kwm cdecl
.ct ,
.kwm _cdecl
.ct ,
.kwm _Cdecl
and
.kwm SOMLINK
(16-bit only) to be equivalent to the
.kwm __cdecl
keyword.
.*
.note __pascal
.ix 'keywords' '__pascal'
&product supports the
.kwm __pascal
keyword to describe Pascal functions that are called using a special
convention described by a pragma in the "stddef.h" header file.
.np
&product predefines the macros
.kwm pascal
.ct ,
.kwm _pascal
and
.kwm _Pascal
to be equivalent to the
.kwm __pascal
keyword.
.*
.note __fortran &optdag.
.ix 'keywords' '__fortran'
&product supports the
.kwm __fortran
keyword to describe functions that are called from FORTRAN.
It converts the name to uppercase letters and suppresses the "_" which
is appended to the function name for certain calling conventions.
.np
&product predefines the macros
.kwm fortran
and
.kwm _fortran
to be equivalent to the
.kwm __fortran
keyword.
.*
.note __interrupt &optdag.
.ix 'keywords' '__interrupt'
.ix 'interrupt routine'
&product supports the
.kwm __interrupt
keyword to describe a function that is an interrupt handler.
.exam begin 8
#include <i86.h>

void __interrupt int10( union INTPACK r )
{
        .
        .
        .
}
.exam end
.pc
The code generator will emit instructions to save all registers.
The registers are saved on the stack in a specific order so that they
may be referenced using the "INTPACK" union as shown in the DOS
example above.
.ix 'DS segment register'
The code generator will emit instructions to establish addressability
to the program's data segment since the DS segment register contents
are unpredictable.
The function will return using an "IRET" (16-bit) or "IRETD" (32-bit)
(interrupt return) instruction.
.np
&product predefines the macros
.kwm interrupt
and
.kwm _interrupt
to be equivalent to the
.kwm __interrupt
keyword.
.*
.note __declspec( modifier )
.ix 'keywords' '__declspec'
&product supports the
.kwm __declspec
keyword for compatibility with Microsoft C++.
The
.kwm __declspec
keyword is used to modify storage-class attributes of functions and/or
data.
There are several modifiers that can be specified with the
.kwm __declspec
keyword:
.kwm thread
.ct ,
.kwm naked
.ct ,
.kwm dllimport
.ct ,
.kwm dllexport
.ct ,
.kwm __pragma( "string" )
.ct ,
.kwm __cdecl
.ct ,
.kwm __pascal
.ct ,
.kwm __fortran
.ct ,
.kwm __stdcall
.ct , and
.kwm __syscall
.ct .li .
These attributes are a property only of the declaration of the
object or function to which they are applied.
Unlike the
.kwm __near
and
.kwm __far
keywords, which actually affect the type of object or function (in
this case, 2- and 4-byte addresses), these storage-class attributes do
not redefine the type attributes of the object itself.
The
.kwm __pragma
modifier is supported by &cmppname only.
The
.kwm thread
attribute affects data and objects only.
The
.kwm naked
.ct ,
.kwm __pragma
.ct ,
.kwm __cdecl
.ct ,
.kwm __pascal
.ct ,
.kwm __fortran
.ct ,
.kwm __stdcall
.ct , and
.kwm __syscall
attributes affect functions only.
The
.kwm dllimport
and
.kwm dllexport
attributes affect functions, data, and objects.
For more information on the
.kwm __declspec
keyword, please see the section entitled :HDREF refid='wccdecl'..
.*
.note __export
.ix 'keywords' '__export'
.ix 'dynamic link library' 'exporting functions'
.ix 'DLL' 'exporting functions'
&product supports the
.kwm __export
keyword to describe functions and other object names that are to be
exported from a Microsoft Windows DLL,
OS/2 DLL, or Netware NLM.
See also the description of the "zu" option.
.exam begin 6
void __export _Setcolor( int color )
{
        .
        .
        .
}
.exam end
.np
&product predefines the macro
.kwm _export
to be equivalent to the
.kwm __export
keyword.
.*
.note __loadds &optdag.
.ix 'keywords' '__loadds'
.ix 'DS segment register'
&product supports the
.kwm __loadds
keyword to describe functions that require specific loading of the DS
register to establish addressability to the function's data segment.
.ix 'dynamic link library'
.ix 'DLL'
This keyword is useful in describing a function that will be placed in
a Microsoft Windows
or OS/2 1.x Dynamic Link Library (DLL).
See also the description of the "nd" and "zu" options.
.exam begin 6
void __export __loadds _Setcolor( int color )
{
        .
        .
        .
}
.exam end
.np
If the function in an OS/2 1.x Dynamic Link Library requires access to
private data, the data segment register must be loaded with an
appropriate value since it will contain the DS value of the calling
application upon entry to the function.
.np
&product predefines the macro
.kwm _loadds
to be equivalent to the
.kwm __loadds
keyword.
.*
.note __saveregs &optdag.
.ix 'keywords' '__saveregs'
&product recognizes the
.kwm __saveregs
keyword which is an attribute used by C/C++ compilers to describe a
function that must save and restore all registers.
.np
&product predefines the macro
.kwm _saveregs
to be equivalent to the
.kwm __saveregs
keyword.
.*
.note __stdcall &optdag.
.ix 'keywords' '__stdcall'
(32-bit only)
The
.kwm __stdcall
keyword may be used with function definitions, and indicates that the
32-bit Win32 calling convention is to be used.
.autonote Notes:
.note
All symbols are preceded by an underscore character.
.note
All C symbols (extern "C" symbols in C++) are suffixed by "@nnn" where
"nnn" is the sum of the argument sizes (each size is rounded up to a
multiple of 4 bytes so that char and short are size 4).
When the argument list contains "...", the "@nnn" suffix is omitted.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The called routine will remove the arguments from the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register EAX will contain address of
the space allocated for the return value.
Floating-point values are returned in 80x87 register ST(0).
.note
Registers EAX, ECX and EDX are not saved and restored when a call is
made.
.endnote
.*
.note __syscall &optdag.
.ix 'keywords' '__syscall'
(32-bit only)
The
.kwm __syscall
keyword may be used with function definitions, and indicates that the
calling convention used is compatible with functions provided by
32-bit OS/2.
.autonote Notes:
.note
Symbols names are not modified, that is, they are not adorned with
leading or trailing underscores.
.note
Arguments are pushed on the stack from right to left.
That is, the last argument is pushed first.
The calling routine will remove the arguments from the stack.
.note
When a structure is returned, the caller allocates space on the stack.
The address of the allocated space will be pushed on the stack
immediately before the call instruction.
Upon returning from the call, register EAX will contain address of
the space allocated for the return value.
Floating-point values are returned in 80x87 register ST(0).
.note
Registers EAX, ECX and EDX are not saved and restored when a call is
made.
.endnote
.np
&product predefines the macros
.kwm _syscall
.ct ,
.kwm _System
.ct ,
.kwm SOMLINK
(32-bit only)
and
.kwm SOMDLINK
(32-bit only) to be equivalent to the
.kwm __syscall
keyword.
.*
.note __far16 &optdag.
.ix 'keywords' '__far16'
(32-bit only)
&product recognizes the
.kwm __far16
keyword which can be used to define far 16-bit (far16) pointers
(16-bit selector with 16-bit offset) or far 16-bit function
prototypes.
This keyword can be used under 32-bit OS/2 to call 16-bit functions
from your 32-bit flat model program.
Integer arguments will automatically be converted to 16-bit integers,
and 32-bit pointers will be converted to far16 pointers before
calling a special thunking layer to transfer control to the 16-bit
function.
.np
&product predefines the macros
.kwm _far16
and
.kwm _Far16
to be equivalent to the
.kwm __far16
keyword.
This keyword is compatible with Microsoft C.
.np
In the OS/2 operating system (version 2.0 or higher), the first 512
megabytes of the 4 gigabyte segment referenced by the DS register is
divided into 8192 areas of 64K bytes each.
A far16 pointer consists of a 16-bit selector referring to one of the
64K byte areas, and a 16-bit offset into that area.
.np
A pointer declared as,
.illust begin
[type] __far16 *name;
.illust end
.pc
defines an object that is a far16 pointer.
If such a pointer is accessed in the 32-bit environment, the compiler
will generate the necessary code to convert between the far16
pointer and a "flat" 32-bit pointer.
.np
For example, the declaration,
.millust begin
char __far16 *bufptr;
.millust end
.pc
declares the object
.id bufptr
to be a far16 pointer to
.kw char
.ct .li .
.np
A function declared as,
.illust begin
[type] __far16 func( [arg_list] );
.illust end
.pc
declares a 16-bit function.
Any calls to such a function from the 32-bit environment will cause
the compiler to convert any 32-bit pointer arguments to far16
pointers, and any
.kw int
arguments from 32 bits to 16 bits.
(In the 16-bit environment, an object of type
.kw int
is only 16 bits.)
Any return value from the function will have its return value
converted in an appropriate manner.
.np
For example, the declaration,
.code begin
char * __far16 Scan( char *buffer, int len, short err );
.code end
.pc
declares the 16-bit function
.id Scan.
When this function is called from the 32-bit environment, the
.id buffer
argument will be converted from a flat 32-bit pointer to a far16
pointer (which, in the 16-bit environment, would be declared as
.mono char __far *.
The
.id len
argument will be converted from a 32-bit integer to a 16-bit integer.
The
.id err
argument will be passed unchanged.
Upon returning, the far16 pointer (far pointer in the 16-bit
environment) will be converted to a 32-bit pointer which describes the
equivalent location in the 32-bit address space.
.*
.note _Seg16 &optdag.
.ix 'keywords' '_Seg16'
(32-bit only)
&product recognizes the
.kwm _Seg16
keyword which has a similar but not identical function as the
.kwm __far16
keyword described above.
This keyword is compatible with IBM C Set/2 and IBM VisualAge C++.
.np
In the OS/2 operating system (version 2.0 or higher), the first 512
megabytes of the 4 gigabyte segment referenced by the DS register is
divided into 8192 areas of 64K bytes each.
A far16 pointer consists of a 16-bit selector referring to one of the
64K byte areas, and a 16-bit offset into that area.
.np
Note that
.kwm _Seg16
is
.bd not
interchangeable with
.kwm __far16
.ct .li .
.np
A pointer declared as,
.illust begin
[type] * _Seg16 name;
.illust end
.pc
defines an object that is a far16 pointer.
Note that the
.kwm _Seg16
appears on the right side of the
.mono *
which is opposite to the
.kwm __far16
keyword described above.
.np
For example,
.millust begin
char * _Seg16 bufptr;
.millust end
.pc
declares the object
.id bufptr
to be a far16 pointer to
.kw char
(the same as above).
.np
The
.kwm _Seg16
keyword may not be used to describe a 16-bit function.
A
.kw #pragma
directive must be used instead.
A function declared as,
.illust begin
[type] * _Seg16 func( [parm_list] );
.illust end
.pc
declares a 32-bit function that returns a far16 pointer.
.np
For example, the declaration,
.code begin
char * _Seg16 Scan( char * buffer, int len, short err );
.code end
.pc
declares the 32-bit function
.mono Scan.
No conversion of the argument list will take place.
The return value is a far16 pointer.
.*
.note __pragma
.ix 'keywords' '__pragma'
&cmppname supports the
.kwm __pragma
keyword to support in-lining of member functions.
The
.kwm __pragma
keyword must be followed by parentheses containing a string that names
an auxiliary pragma.
Here is a simplified example showing usage and syntax.
.exam begin
#pragma aux fast_mul = \
    "imul eax,edx" \
    parm caller [eax] [edx] \
    value struct;

struct fixed {
    unsigned v;
};

fixed __pragma( "fast_mul") operator *( fixed, fixed );

fixed two = { 2 };
fixed three = { 3 };

fixed foo()
{
    return two * three;
}
.exam end
.np
See the chapters entitled :HDREF refid='prg86'. and
:HDREF refid='prg386'. for more information on pragmas.
.*
.note __int8
.ix 'keywords' '__int8'
&product supports the
.kwm __int8
keyword to define 8-bit integer data objects.
.exam begin
static __int8 smallInt;
.exam end
.np
Also supported are signed and unsigned 8-bit integer constants.  The
.kwm __int8
data type will be unsigned by default if the compiler is invoked with the -j switch.
.*
.note __int16
.ix 'keywords' '__int16'
&product supports the
.kwm __int16
keyword to define 16-bit integer data objects.
.exam begin
static __int16 shortInt;
.exam end
.np
Also supported are signed and unsigned 16-bit integer constants.
.*
.note __int32
.ix 'keywords' '__int32'
&product supports the
.kwm __int32
keyword to define 32-bit integer data objects.
.exam begin
static __int32 longInt;
.exam end
.np
Also supported are signed and unsigned 32-bit integer constants.
.*
.note __int64
.ix 'keywords' '__int64'
&product supports the
.kwm __int64
keyword to define 64-bit integer data objects.
.exam begin
static __int64 bigInt;
.exam end
.np
Also supported are signed and unsigned 64-bit integer constants.
.begnote
.note signed __int64
Use the "i64" suffix for a signed 64-bit integer constant.
.exam begin
12345i64
12345I64
.exam end
.note unsigned __int64
Use the "ui64" suffix for an unsigned 64-bit integer constant.
.exam begin
12345Ui64
12345uI64
.exam end
.endnote
.np
The run-time library supports formatting of
.kwm __int64
items (see the description of the
.kwm printf
library function).
.exam begin
#include <stdio.h>
#include <limits.h>

void main()
{
    __int64 bigint;
    __int64 bigint2;

    bigint2 = 8I64 * (LONG_MAX + 1I64);
    for( bigint = 0;
         bigint <= bigint2;
         bigint += bigint2 / 16 ) {
        printf( "Hello world %Ld\n", bigint );
    }
}
.exam end
.begnote Restrictions
.note switch
An
.kwm __int64
expression cannot be used in a
.us switch
statement.
.note bit fields
More than 32 bits in a 64-bit bitfield is not supported.
.endnote
.*
.endnote
.*
.section *refid=wccbase Based Pointers
.*
.np
.ix 'based pointers'
Near pointers are generally the most efficient type of pointer because
they are small, and the compiler can assume knowledge about what
segment of the computer's memory the pointer (offset) refers to.
Far pointers are the most flexible because they allow the programmer
to access any part of the computer's memory, without limitation to a
particular segment.
However, far pointers are bigger and slower because of the additional
flexibility.
.np
Based pointers are a compromise between the efficiency of near
pointers and the flexibility of far pointers.
With based pointers, the programmer takes responsibility to tell the
compiler which segment a near pointer (offset) belongs to, but may
still access segments of the computer's memory outside of the normal
data segment (DGROUP).
The result is a pointer type which is as small as and almost as
efficient as a near pointer, but with most of the flexibility of a far
pointer.
.np
An object declared as a based pointer falls into one of the following
categories:
.begbull $compact
.* .bull
.* the based pointer is in the same segment as another named object,
.bull
the based pointer is in the segment described by another object,
.bull
the based pointer, used as a pointer to another object of the same
type (as in a linked list), refers to the same segment,
.bull
the based pointer is an offset to no particular segment, and must be
combined explicitly with a segment value to produce a valid pointer.
.endbull
.np
To support based pointers, the following keywords are provided:
.ix '__based'
.ix '__segment'
.ix '__segname'
.ix '__self'
.millust begin
__based
__segment
__segname
__self
.millust end
.np
The following operator is also provided:
.millust begin
:>
.millust end
.np
These keywords and operator are described in the following sections.
.np
Two macros, defined in
.fi malloc.h,
are also provided:
.ix '_NULLSEG'
.ix '_NULLOFF'
.millust begin
_NULLSEG
_NULLOFF
.millust end
.pc
They are used in a manner similar to
.kwm NULL
.ct , but are used with objects declared as
.kwm __segment
and
.kwm __based
respectively.
.*
.beglevel
.*
.section Segment Constant Based Pointers and Objects
.*
.np
.ix 'based pointers' 'segment constant'
A segment constant based pointer or object has its segment value based
on a specific, named segment.
A segment constant based object is specified as:
.illust begin
[type] __based( __segname( "segment" ) ) object_name;
.illust end
.pc
and a segment constant based pointer is specified as:
.illust begin
[type] __based( __segname( "segment" ) ) *object-name;
.illust end
.pc
where
.id segment
is the name of the segment in which the pointer or object is based.
As shown above, the segment name is always specified as a string.
There are four special segment names recognized by the compiler:
.millust begin
"_CODE"
"_CONST"
"_DATA"
"_STACK"
.millust end
.pc
The
.mono "_CODE"
segment is the default code segment.
The
.mono "_CONST"
segment is the segment containing constant values.
The
.mono "_DATA"
segment is the default data segment.
The
.mono "_STACK"
segment is the segment containing the stack.
If the segment name is not one of the recognized names, then a
segment will be created with that name.
If a segment constant based object is being defined, then it will be
placed in the named segment.
If a segment constant based pointer is being defined, then it can
point at objects in the named segment.
.np
The following examples illustrate segment constant based pointers and
objects.
.exam begin
int __based( __segname( "_CODE" ) )  ival = 3;
int __based( __segname( "_CODE" ) ) *iptr;
.exam end
.pc
.id ival
is an object that resides in the default code segment.
.id iptr
is an object that resides in the data segment (the usual place for
data objects), but points at an integer which resides in the default
code segment.
.id iptr
is suitable for pointing at
.id ival.
.exam begin
char __based( __segname( "GOODTHINGS" ) ) thing;
.exam end
.pc
.id thing
is an object which resides in the segment
.id GOODTHINGS,
which will be created if it does not already exist.
(The creation of segments is done by the linker, and is a method
of grouping objects and functions.
Nothing is implicitly created during the execution of the program.)
.*
.section Segment Object Based Pointers
.*
.np
.ix 'based pointers' 'segment object'
A segment object based pointer derives its segment value from another
named object.
A segment object based pointer is specified as follows:
.illust begin
[type] __based( segment ) *name;
.illust end
.pc
where
.id segment
is an object defined as type
.kwm __segment
.ct .li .
.np
An object of type
.kwm __segment
may contain a segment value.
Such an object is particularly designed for use with segment object
based pointers.
.np
The following example illustrates a segment object based pointer:
.exam begin
__segment            seg;
char __based( seg ) *cptr;
.exam end
.pc
The object
.id seg
contains only a segment value.
Whenever the object
.id cptr
is used to point to a character, the actual pointer value will be made
up of the segment value found in
.mono seg
and the offset value found in
.id cptr.
The object
.id seg
might be assigned values such as the following:
.begbull $compact
.bull
a constant value (e.g., the segment containing screen memory),
.bull
the result of the library function
.kw _bheapseg
.ct ,
.bull
the segment portion of another pointer value, by casting it to the
type
.kwm __segment
.ct .li .
.endbull
.*
.section Void Based Pointers
.*
.np
.ix 'based pointers' 'void'
A void based pointer must be explicitly combined with a segment value
to produce a reference to a memory location.
A void based pointer does not infer its segment value from another
object.
.ix 'operator' ':>'
.ix 'base operator'
The
.mono :>
(base) operator is used to combine a segment value and a void based
pointer.
.np
For example, on a personal computer running DOS with a color monitor,
the screen memory begins at segment 0xB800, offset 0.
In a video text mode, to examine the first character currently
displayed on the screen, the following code could be used:
.exam begin
extern void main()
{
    __segment             screen;
    char __based( void ) *scrptr;

    screen = 0xB800;
    scrptr = 0;
    printf( "Top left character is '%c'.\n",
            *(screen:>scrptr) );
}
.exam end
.np
The general form of the
.mono :>
operator is:
.illust begin
segment :> offset
.illust end
.pc
where
.id segment
is an expression of type
.kwm __segment
.ct , and
.id offset
is an expression of type
.id __based( void ) *.
.*
.section Self Based Pointers
.*
.np
.ix 'based pointers' 'self'
A self based pointer infers its segment value from itself.
It is particularly useful for structures such as linked lists, where
all of the list elements are in the same segment.
A self based pointer pointing to one element may be used to access the
next element, and the compiler will use the same segment as the
original pointer.
.np
The following example illustrates a function which will print the
values stored in the last two members of a linked list:
.exam begin
struct a {
    struct a __based( __self ) *next;
    int                         number;
};

.exam break
extern void PrintLastTwo( struct a far *list )
{
  __segment                seg;
  struct a __based( seg ) *aptr;

  seg  = FP_SEG( list );
  aptr = FP_OFF( list );
  for( ; aptr != _NULLOFF; aptr = aptr->next ) {
    if( aptr->next == _NULLOFF ) {
      printf( "Last item is %d\n",
              aptr->number );
    } else if( aptr->next->next == _NULLOFF ) {
      printf( "Second last item is %d\n",
              aptr->number );
    }
  }
}
.exam end
.np
The argument to the function
.id PrintLastTwo
is a far pointer, pointing to a linked list structure anywhere in
memory.
It is assumed that all members of a particular linked list of this
type reside in the same segment of the computer's memory.
(Another instance of the linked list might reside entirely in a
different segment.)
The object
.id seg
is given the segment portion of the far pointer.
The object
.id aptr
is given the offset portion, and is described as being based in the
segment stored in
.id seg.
.np
The expression
.id aptr->next
refers to the
.id next
member of the structure stored in memory at the offset stored in
.id aptr
and
the segment implied by
.id aptr,
which is the value stored in
.id seg.
So far, the behavior is no different than if
.id next
had been declared as,
.millust begin
struct a *next;
.millust end
.np
The expression
.id aptr->next->next
illustrates the difference of using a self based pointer.
The first part of the expression (
..ct
.id aptr->next
..ct )
occurs as described above.
However, using the result to point to the next member occurs by using
the offset value found in the
.id next
member and combining it with the segment value of the
.ul pointer used to get to that member,
which is still the segment implied by
.id aptr,
which is the value stored in
.id seg.
If
.id next
had not been declared using
.id __based( __self ),
then the second pointing operation would refer to the offset value
found in the
.id next
member, but with the default data segment (DGROUP), which may or may
not be the same segment as stored in
.id seg.
.*
.endlevel
.*
.section *refid=wccdecl The __declspec Keyword
.*
.np
.ix 'keywords' '__declspec'
&product supports the
.kwm __declspec
keyword for compatibility with Microsoft C++.
The
.kwm __declspec
keyword is used to modify storage-class attributes of functions and/or
data.
.begnote
.*
.note __declspec( thread )
.*
is used to define thread local storage (TLS).
TLS is the mechanism by which each thread in a multithreaded process
allocates storage for thread-specific data.
In standard multithreaded programs, data is shared among all threads
of a given process, whereas thread local storage is the mechanism for
allocating per-thread data.
.exam begin
__declspec(thread) static int tls_data = 0;
.exam end
.np
The following rules apply to the use of the
.kwm thread
attribute.
.begbull
.bull
The
.kwm thread
attribute can be used with data and objects only.
.bull
You can specify the
.kwm thread
attribute only on data items with static storage duration.
This includes global data objects (both
.kwm static
and
.kwm extern
.ct ),
local static objects, and static data members of classes.
Automatic data objects cannot be declared with the
.kwm thread
attribute.
The following example illustrates this error:
.exam begin
#define TLS __declspec( thread )
void func1()
{
    TLS int tls_data;           // Wrong!
}

int func2( TLS int tls_data )   // Wrong!
{
    return tls_data;
}
.exam end
.bull
The
.kwm thread
attribute must be used for both the declaration and the definition of
a thread local object, whether the declaration and definition occur in
the same file or separate files.
The following example illustrates this error:
.exam begin
#define TLS __declspec( thread )
extern int tls_data;    // This generates an error, because the
TLS    int tls_data;    // declaration and the definition differ.
.exam end
.bull
Classes cannot use the
.kwm thread
attribute.
However, you can instantiate class objects with the
.kwm thread
attribute, as long as the objects do not need to be constructed or
destructed.
For example, the following code generates an error:
.exam begin
#define TLS __declspec( thread )
TLS class A     // Wrong! Classes are not objects
{
    // Code
};
A AObject;
.exam end
.np
Because the declaration of objects that use the
.kwm thread
attribute is permitted, these two examples are semantically
equivalent:
.exam begin
#define TLS __declspec( thread )
TLS class B
{
    // Code
} BObject;      // Okay! BObject declared thread local.

class C
{
    // Code
};
TLS C CObject;  // Okay! CObject declared thread local.
.exam end
.bull
Standard C permits initialization of an object or variable with an
expression involving a reference to itself, but only for objects of
non-static extent.
Although C++ normally permits such dynamic initialization of an object
with an expression involving a reference to itself, this type of
initialization is not permitted with thread local objects.
.exam begin
#define TLS  __declspec( thread )
TLS int tls_i = tls_i;            // C and C++ error
int j = j;                        // Okay in C++; C error
TLS int tls_k = sizeof( tls_k );  // Okay in C and C++
.exam end
.np
Note that a
.kwm sizeof
expression that includes the object being initialized does not
constitute a reference to itself and is allowed in C and C++.
.endbull
.*
.note __declspec( naked )
.*
indicates to the code generator that no prologue or epilogue sequence
is to be generated for a function.
Any statements other than "_asm" directives or auxiliary pragmas are
not compiled.
.kw _asm
Essentially, the compiler will emit a "label" with the specified
function name into the code.
.exam begin
#include <stdio.h>

int __declspec( naked ) foo( int x )
{
    _asm {
#if defined(__386__)
        inc eax
#else
        inc ax
#endif
        ret
    }
}

void main()
{
    printf( "%d\n", foo( 1 ) );
}
.exam end
.np
The following rules apply to the use of the
.kwm naked
attribute.
.begbull
.bull
The
.kwm naked
attribute cannot be used in a data declaration.
The following declaration would be flagged in error.
.exam begin
__declspec(naked) static int data_object = 0;
.exam end
.endbull
.*
.note __declspec( dllimport )
.*
is used to declare functions, data and objects imported from a DLL.
.exam begin
#define DLLImport __declspec(dllimport)

DLLImport void dll_func();
DLLImport int  dll_data;
.exam end
.np
Functions, data and objects are exported from a DLL by
use of
.kwm __declspec(dllexport)
ct , the
.kwm __export
keyword (for which
.kwm __declspec(dllexport)
is the replacement), or through linker "EXPORT" directives.
.np
Note: When calling functions imported from other modules, it is not
strictly necessary to use the
.kwm __declspec(dllimport)
modifier to declare the functions. This modifier however must always be used
when importing data or objects to ensure correct behavior.
.*
.note __declspec( dllexport )
.*
is used to declare functions, data and objects exported from a DLL.
Declaring functions as
.kwm dllexport
eliminates the need for linker "EXPORT" directives.
The
.kwm __declspec(dllexport)
attribute is a replacement for the
.kwm __export
keyword.
.note __declspec( __pragma( "string" ) )
.*
is used to declare functions which adhere to the conventions described
by the pragma identified by "string".
.exam begin
#include <stdio.h>

#pragma aux my_stdcall "_*" \
        parm routine [] \
        value struct struct caller [] \
        modify [eax ecx edx];

struct list {
    struct list *next;
    int         value;
    float       flt_value;
};

#define STDCALL __declspec( __pragma("my_stdcall") )

STDCALL struct list foo( int x, char *y, double z );

void main()
{
    int a = 1;
    char *b = "Hello there";
    double c = 3.1415926;
    struct list t;

    t = foo( a, b, c );
    printf( "%d\n", t.value );
}


struct list foo( int x, char *y, double z )
{
    struct list tmp;

    printf( "%s\n", y );
    tmp.next = NULL;
    tmp.value = x;
    tmp.flt_value = z;
    return( tmp );
}
.exam end
.np
It is also possible to modify the calling convention of all methods of a class
or just an individual method.
.exam begin
#pragma aux my_thiscall "_*" \
        parm routine [ecx] \
        value struct struct caller [] \
        modify [eax ecx edx];

#define THISCALL __declspec( __pragma("my_thiscall") )

class THISCALL IWatcom: public IUnknown{
    virtual int method_a( void ) = 0;
    virtual int method_b( void ) = 0;
    virtual int __cdecl method_c( void ) = 0;
};
.exam end
.np
In this example, any calls generated to the virtual methods 'method_a' or 'method_b'
will use the THISCALL ( my_thiscall ) calling convention. Calls generated to 'method_c'
will use the prefefined
.kw __cdecl
calling convention. 
.np
It is also possible to forward define the class with modifiers for occasions where
you do not want to change original source code.
.exam begin
#pragma aux my_thiscall "_*" \
        parm routine [ecx] \
        value struct struct caller [] \
        modify [eax ecx edx];

#define THISCALL __declspec( __pragma("my_thiscall") )
class THISCALL IWatcom;

class IWatcom: public IUnknown{
    virtual int method_a( void ) = 0;
    virtual int method_b( void ) = 0;
    virtual int __cdecl method_c( void ) = 0;
};
.exam end
.np
The
.kw __pragma
modifier is supported by &cmppname only.
.note __declspec( __cdecl )
.*
is used to declare functions which conform to the Microsoft compiler
calling convention.
.note __declspec( __pascal )
.*
is used to declare functions which conform to the OS/2 1.x and Windows
3.x calling convention.
.note __declspec( __fortran )
.*
is used to declare functions which conform to the
.kw __fortran
calling convention.
.exam begin
#include <stdio.h>

#define DLLFunc __declspec(dllimport __fortran)
#define DLLData __declspec(dllimport)

#ifdef __cplusplus
extern "C" {
#endif

DLLFunc int  dll_func( int, int, int );
DLLData int  dll_data;

#ifdef __cplusplus
};
#endif

void main()
{
  printf( "%d %d\n", dll_func( 1,2,3 ), dll_data );
}

.exam end
.note __declspec( __stdcall )
.*
is used to declare functions which conform to the 32-bit Win32
"standard" calling convention.
.exam begin
#include <stdio.h>

#define DLLFunc __declspec(dllimport __stdcall)
#define DLLData __declspec(dllimport)

DLLFunc int  dll_func( int, int, int );
DLLData int  dll_data;

void main()
{
  printf( "%d %d\n", dll_func( 1,2,3 ), dll_data );
}
.exam end
.note __declspec( __syscall )
.*
is used to declare functions which conform to the 32-bit OS/2
.kw __syscall
calling convention.
.endnote
.*
.section *refid=wccgen The &company Code Generator
.*
.np
.ix 'code generation'
The &company Code Generator performs such optimizations as common
subexpression elimination, global flow analysis, and so on.
.np
In some cases, the code generator can do a better job of optimizing
code if it could utilize more memory.
This is indicated when a
.millust begin
Not enough memory to optimize procedure 'xxxx'
.millust end
.pc
message appears on the screen as the source program is compiled.
In such an event, you may wish to make more memory available to the
code generator.
.np
A special environment variable may be used to obtain memory usage
information or set memory usage limits on the code generator.
:INCLUDE file='WCGMEM'.
