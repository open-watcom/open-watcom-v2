.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="prod16"    value="16-bit &company C/C++".
:set symbol="prod32"    value="32-bit &company C/C++".
.do end
.el .if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="prod16"    value="16-bit &company F77".
:set symbol="prod32"    value="32-bit &company F77".
.do end
.*
.chap *refid=environ Use of Environment Variables
.*
.np
.ix 'environment variables' 'use'
In the &product software development package, a number of environment
variables are used.
This appendix summarizes their use with a particular component of the
package.
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.section FORCE
.np
The
.ev FORCE
environment variable identifies a file that is to be included as part
of the source input stream.
This variable is used by &product..
.ix '&setcmdup.'
.millust begin
.if '&target' eq 'QNX' .do begin
&setcmdup. &setdelim.FORCE=filespec&setdelim.
.do end
.el .do begin
&setcmdup. &setdelim.FORCE=[d:][path]filename[.ext]&setdelim.
.do end
.millust end
.pc
The specified file is included as if a
.millust begin
.if '&target' eq 'QNX' .do begin
#include "filespec"
.do end
.el .do begin
#include "[d:][path]filename[.ext]"
.do end
.millust end
.pc
directive were placed at the start of the source file.
.exam begin
&prompt.&setcmd. &setdelim.&frcvar=&pathnam.&hdrdir.&pc.common.cnv&setdelim.
&prompt.wcc report
.exam end
.pc
The
.ev FORCE
environment variable can be overridden by use of the &product "fi"
option.
.do end
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.section INCLUDE
.np
The
.ev INCLUDE
environment variable describes the location of the
C and C++ header files (files with the "&hxt" filename
extension).
This variable is used by &product..
.millust begin
.if '&target' eq 'QNX' .do begin
&setcmdup. &setdelim.&incvar=path:path...&setdelim.
.do end
.el .do begin
&setcmdup. &setdelim.&incvar=[d:][path];[d:][path]...&setdelim.
.do end
.millust end
.pc
The
.ev INCLUDE
environment string is like the
.ev PATH
string in that you can specify one or more directories separated by
&psword.s ("&ps").
.do end
.************************************************************************
.if '&lang' eq 'FORTRAN 77' .do begin
.section FINCLUDE
.np
The
.ev FINCLUDE
environment variable describes the location of the &product include
files.
This variable is used by &product..
.millust begin
.if '&target' eq 'QNX' .do begin
&setcmdup. &setdelim.FINCLUDE=path:path...&setdelim.
.do end
.el .do begin
&setcmdup. &setdelim.FINCLUDE=[d:][path];[d:][path]...&setdelim.
.do end
.millust end
.pc
The
.ev FINCLUDE
environment string is like the
.ev PATH
string in that you can specify one or more directories separated by
&psword.s ("&ps").
.do end
.************************************************************************
.* LFN
.if '&target' ne 'QNX' .do begin
.section LFN
.np
The
.ev LFN
environment variable is checked by the
&company run-time C libraries and it is used to control DOS LFN (DOS Long File Name) support.
Normally, these libraries will use DOS LFN support if it is available on host OS.
If you don't wish to use DOS LFN support, you can define the
.ev LFN
environment variable and setup it's value to 'N'.
Using the "&setcmdup." command, define the environment
variable as follows:
.millust begin
&setcmdup. &setdelim.LFN=N&setdelim.
.millust end
.pc
Now, when you run your application, the DOS LFN support will be ignored.
To undefine the environment variable, enter the command:
.millust begin
&setcmdup. &setdelim.LFN=&setdelim.
.millust end
.do end
.************************************************************************
.* LIB
.section &libvarup
.np
The use of the
.ev &pathvarup
environment variable and the &lnkname "SYSTEM" directive is
recommended over the use of this environment variable.
.np
The
.ev &libvarup
environment variable is used to select the libraries that will be used
when the application is linked.
This variable is used by the &lnkname (&lnkcmdup.&exc.).
The
.ev &libvarup
environment string is like the
.ev PATH
string in that you can specify one or more directories separated by
&psword.s ("&ps").
.if '&target' ne 'QNX' .do begin
.np
If you have the 286 development system, 16-bit applications can be
linked for DOS, Microsoft Windows, OS/2, and QNX depending on which
libraries are selected.
If you have the 386 development system, 32-bit applications can
be linked for DOS Extender systems, Microsoft Windows and QNX.
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section LIBDOS
.np
The use of the
.ev &pathvarup
environment variable and the &lnkname "SYSTEM" directive is
recommended over the use of this environment variable.
.np
If you are developing a DOS application, the
.ev LIBDOS
environment variable must include the location of the &prod16 DOS
library files (files with the "&lib" filename extension).
This variable is used by the &lnkname (&lnkcmdup.&exc).
The default installation directory for the &prod16 DOS libraries is
.fi &pathnamup&pc.LIB286&pc.DOS.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The
.ev LIBDOS
environment variable must also include the location of the &prod16
math library files.
The default installation directory for the &prod16 math libraries is
.fi &pathnamup&pc.LIB286.
.tinyexam begin
&prompt.&setcmd. &setdelim.libdos=&dr3.&pathnam.&pc.lib286&pc.dos&ps.&dr3.&pathnam.&pc.lib286&setdelim.
.tinyexam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
&prompt.&setcmd. &setdelim.libdos=&dr3.&pathnam.&pc.lib286&pc.dos&setdelim.
.exam end
.do end
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section LIBWIN
.np
The use of the
.ev &pathvarup
environment variable and the &lnkname "SYSTEM" directive is
recommended over the use of this environment variable.
.np
If you are developing a 16-bit Microsoft Windows application, the
.ev LIBWIN
environment variable must include the location of the &prod16 Windows
library files (files with the "&lib" filename extension).
This variable is used by the &lnkname (&lnkcmdup.&exc).
If you are developing a 32-bit Microsoft Windows application, see the
description of the
.ev LIBPHAR
environment variable.
The default installation directory for the &prod16 Windows libraries
is
.fi &pathnamup&pc.LIB286&pc.WIN.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The
.ev LIBWIN
environment variable must also include the location of the &prod16
math library files.
The default installation directory for the &prod16 math libraries is
.fi &pathnamup&pc.LIB286.
.tinyexam begin
&prompt.&setcmd. &setdelim.libwin=&dr3.&pathnam.&pc.lib286&pc.win&ps.&dr3.&pathnam.&pc.lib286&setdelim.
.tinyexam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.tinyexam begin
&prompt.&setcmd. &setdelim.libwin=&dr3.&pathnam.&pc.lib286&pc.win&setdelim.
.tinyexam end
.do end
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section LIBOS2
.np
The use of the
.ev &pathvarup
environment variable and the &lnkname "SYSTEM" directive is
recommended over the use of this environment variable.
.np
If you are developing an OS/2 application, the
.ev LIBOS2
environment variable must include the location of the &prod16 OS/2
library files (files with the "&lib" filename extension).
This variable is used by the &lnkname (&lnkcmdup.&exc).
The default installation directory for the &prod16 OS/2 libraries is
.fi &pathnamup&pc.LIB286&pc.OS2.
.ix 'DOSCALLS.LIB'
.ix 'OS/2' 'DOSCALLS.LIB'
The
.ev LIBOS2
environment variable must also include the directory of the OS/2
.fi DOSCALLS.LIB
file which is usually
.fi &pc.OS2.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The
.ev LIBOS2
environment variable must also include the location of the &prod16
math library files.
The default installation directory for the &prod16 math libraries is
.fi &pathnamup&pc.LIB286.
.tinyexam begin
&prompt.&setcmd. &setdelim.libos2=&dr3.&pathnam.&pc.lib286&pc.os2&ps.&dr3.&pathnam.&pc.lib286&ps.&dr3.&pc.os2&setdelim.
.tinyexam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
&prompt.&setcmd. &setdelim.libos2=&dr3.&pathnam.&pc.lib286&pc.os2&ps.&dr3.&pc.os2&setdelim.
.exam end
.do end
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section LIBPHAR
.np
The use of the
.ev &pathvarup
environment variable and the &lnkname "SYSTEM" directive is
recommended over the use of this environment variable.
.np
If you are developing a 32-bit Windows or DOS Extender application,
the
.ev LIBPHAR
environment variable must include the location of the &prod32 DOS
Extender library files or the &prod32 Windows library files
(files with the "&lib" filename extension).
This variable is used by the &lnkname (&lnkcmdup.&exc).
The default installation directory for the &prod32 DOS Extender
libraries is
.fi &pathnamup&pc.LIB386&pc.DOS.
The default installation directory for the &prod32 Windows libraries
is
.fi &pathnamup&pc.LIB386&pc.WIN.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The
.ev LIBPHAR
environment variable must also include the location of the &prod32
math library files.
The default installation directory for the &prod32 math libraries is
.fi &pathnamup&pc.LIB386.
.tinyexam begin
&prompt.&setcmd. &setdelim.libphar=&dr3.&pathnam.&pc.lib386&pc.dos&ps.&dr3.&pathnam.&pc.lib386&setdelim.
    or
&prompt.&setcmd. &setdelim.libphar=&dr3.&pathnam.&pc.lib386&pc.win&ps.&dr3.&pathnam.&pc.lib386&setdelim.
.tinyexam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
&prompt.&setcmd. &setdelim.libphar=&dr3.&pathnam.&pc.lib386&pc.dos&setdelim.
    or
&prompt.&setcmd. &setdelim.libphar=&dr3.&pathnam.&pc.lib386&pc.win&setdelim.
.exam end
.do end
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section NO87
.np
The
.ev NO87
environment variable is checked by the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&company run-time math libraries that include floating-point emulation
support.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&product run-time libraries that include floating-point
emulation support.
.do end
Normally,
.if '&ccmd' eq 'wcexp' .do begin
this library
.do end
.el .do begin
these libraries
.do end
will detect the presence of a numeric data processor (80x87) and use
it.
If you have a numeric data processor in your system but you wish to
test a version of your application that will use floating-point
emulation, you can define the
.ev NO87
environment variable.
Using the "&setcmdup." command, define the environment
variable as follows:
.millust begin
&setcmdup. &setdelim.NO87=1&setdelim.
.millust end
.pc
Now, when you run your application, the 80x87 will be ignored.
To undefine the environment variable, enter the command:
.millust begin
&setcmdup. &setdelim.NO87=&setdelim.
.millust end
.do end
.************************************************************************
.section PATH
.np
The
.ev PATH
environment variable is used by
.if '&target' eq 'QNX' .do begin
the QNX shell
.do end
.el .do begin
DOS "COMMAND.COM" or OS/2 "CMD.EXE"
.do end
to locate programs.
.millust begin
.if '&target' eq 'QNX' .do begin
&setcmdup. &setdelim.PATH=path:path...&setdelim.
.do end
.el .do begin
PATH [d:][path];[d:][path]...
.do end
.millust end
.pc
The
.ev PATH
environment variable should include the
.if '&target' ne 'QNX' .do begin
disk and
.do end
directory of the &product binary program files when using &product and
its related tools.
.np
.if '&target' eq 'QNX' .do begin
.ix 'bin directory'
The default installation directory for &product QNX binaries is
called "&pc.bin".
.exam begin
&prompt.&setcmd. &setdelim.PATH=&pc.bin&setdelim.
.exam end
.do end
.el .do begin
.us If your host system is DOS:
.np
.ix 'BINW directory'
The default installation directory for &prod16 and &prod32 DOS
binaries is called
.fi &pathnamup&pc.BINW.
.exam begin
&prompt.path &dr3.&pathnam\binw;&dr3.\dos;&dr3.\windows
.exam end
.np
.us If your host system is OS/2:
.np
.ix 'BINP directory'
The default installation directories for &prod16 and &prod32 OS/2
binaries are called
.fi &pathnamup&pc.BINP
and
.fi &pathnamup&pc.BINW.
.exam begin
[C:&pc.]path &dr3.&pathnam.&pc.binp;&dr3.&pathnam.&pc.binw
.exam end
.np
.us If your host system is Windows NT:
.np
.ix 'BINNT directory'
The default installation directories for &prod16 and &prod32
Windows NT binaries are called
.fi &pathnamup&pc.BINNT
and
.fi &pathnamup&pc.BINW.
.exam begin
&prompt.path &dr3.&pathnam.&pc.binnt&ps.&dr3.&pathnam.&pc.binw
.exam end
.do end
.np
The
.ev PATH
environment variable is also used by the following programs in the
described manner.
.autopoint
.point
&wclname to locate the &prod16 and &prod32 compilers and the
&lnkname..
.point
.if '&target' eq 'QNX' .do begin
"&dbgcmdup.&exc" to locate programs.
.do end
.el .do begin
"&dbgcmdup.&exc" to locate programs and debugger command files.
.do end
.endpoint
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section TMP
.np
The
.ev TMP
environment variable describes the location (disk and path) for
temporary files created by the &prod16 and &prod32 compilers and the
&lnkname..
.millust begin
.if '&target' eq 'QNX' .do begin
&setcmdup. &setdelim.TMP=path&setdelim.
.do end
.el .do begin
&setcmdup. &setdelim.TMP=[d:][path]&setdelim.
.do end
.millust end
.np
Normally, &product will create temporary spill files in the current
directory.
However, by defining the
.ev TMP
environment variable to be a certain disk and directory, you can tell
&product where to place its temporary files.
The same is true of the &lnkname temporary file.
.keep
.np
Consider the following definition of the
.ev TMP
environment variable.
.exam begin
&prompt.&setcmd. &setdelim.&tmpvar=&dr4.&pathnam.&pc.tmp&setdelim.
.exam end
.pc
The &product compiler and &lnkname will create its temporary files in
.fi &dr4.&pathnam.&pc.tmp.
.do end
.************************************************************************
.if '&target' eq 'QNX' .do begin
.section TMPDIR
.np
The
.ev TMPDIR
environment variable describes the location (path) for temporary files
created by the &prod16 and &prod32 compilers and the &lnkname..
.millust begin
&setcmdup. &setdelim.TMPDIR=path&setdelim.
.millust end
.np
Normally, &prod16 and &prod32 will create temporary spill files in the
current directory.
However, by defining the
.ev TMPDIR
environment variable to be a certain path, you can tell &product where
to place its temporary files.
The same is true of the &lnkname temporary file.
.keep
.np
Consider the following definition of the
.ev TMPDIR
environment variable.
.exam begin
&prompt.&setcmd. &setdelim.&tmpdirvar=&dr4.&pc.tmp&setdelim.
.exam end
.pc
The &product compiler and &lnkname will create its temporary files in
.fi &dr4.&pc.tmp.
.do end
.************************************************************************
.section &pathvarup
.np
.ix 'library path'
In order for the &lnkname to locate the &prod16 and &prod32 library
files, the
.ev &pathvarup
environment variable should be defined.
.if '&target' eq 'QNX' .do begin
When using
.kw &wclcmdup
.ct , it is not necessary to define this environment variable since it
uses another technique for identifying the location of the library
files to the &lnkname..
However, you should do so when you begin to use the &lnkname directly
without the aid of this utility program.
.do end
The
.ev &pathvarup
environment variable is used to locate the libraries that will be used
when the application is linked.
The default directory for &prod16 and &prod32 files is "&pathnamup".
.exam begin
&prompt.&setcmd. &setdelim.&pathvar.=&dr3.&pathnam.&setdelim.
.exam end
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&target' eq 'QNX' .do begin
.sr envvar='WCC'
.do end
.el .do begin
.sr envvar='wcc'
.do end
.section WCC
.np
The
.ev WCC
environment variable can be used to specify commonly-used options for
the 16-bit C compiler.
.millust begin
&setcmdup. &setdelim.WCC=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the command
line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "ox" (compile
for maximum number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&envvar.=&sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WCC
environment variable has been defined, those options listed become the
default each time the
.if '&target' eq 'QNX' .do begin
.kw wcc
.do end
.el .do begin
.kw WCC
.do end
command is used.
.do end
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&target' eq 'QNX' .do begin
.sr envvar='WCC386'
.do end
.el .do begin
.sr envvar='wcc386'
.do end
.section WCC386
.np
The
.ev WCC386
environment variable can be used to specify commonly-used options for
the 32-bit C compiler.
.millust begin
&setcmdup. &setdelim.WCC386=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the command
line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "ox" (compile
for maximum number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&envvar.=&sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WCC386
environment variable has been defined, those options listed become the
default each time the
.if '&target' eq 'QNX' .do begin
.kw wcc386
.do end
.el .do begin
.kw WCC386
.do end
command is used.
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section WCL
.np
The
.ev WCL
environment variable can be used to specify commonly-used &wclcmdup16
options.
.millust begin
&setcmdup. &setdelim.WCL=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "mm" (compile
code for medium memory model), "d1" (include line number debug
information in the object file), and "ox" (compile for maximum
number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.wcl=&sw.mm &sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WCL
environment variable has been defined, those options listed become the
default each time the &wclcmdup16 command is used.
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section WCL386
.np
The
.ev WCL386
environment variable can be used to specify commonly-used &wclcmdup32
options.
.millust begin
&setcmdup. &setdelim.WCL386=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "3s" (compile
code for stack-based argument passing convention), "d1" (include line
number debug information in the object file), and "ox" (compile for
maximum number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.wcl386=&sw.3s &sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WCL386
environment variable has been defined, those options listed become the
default each time the &wclcmdup32 command is used.
.do end
.************************************************************************
.section WCGMEMORY
.np
:INCLUDE file='WCGMEM'.
.************************************************************************
.section &dbgvarup
.np
The
.ev &dbgvarup
environment variable can be used to specify commonly-used &dbgname
options.
.if '&target' ne 'QNX' .do begin
This environment variable is not used by the Windows version of the
debugger, &dbgcmdup.W.
.do end
.millust begin
&setcmdup. &setdelim.&dbgvarup=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "noinvoke"
(do not execute the
.fi profile.dbg
file) and "reg=10" (retain up to 10 register sets while tracing).
.exam begin
.if '&target' eq 'QNX' .do begin
&prompt.&setcmd. &setdelim.&dbgvar=&sw.noinvoke &sw.reg=10&setdelim.
.do end
.el .do begin
&prompt.&setcmd. &setdelim.&dbgvar=&sw.noinvoke &sw.reg#10&setdelim.
.do end
.exam end
.np
Once the
.ev &dbgvarup
environment variable has been defined, those options listed become the
default each time the &dbgcmdup command is used.
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section &dbgvarup.W
.np
The
.ev &dbgvarup.W
environment variable can be used to specify commonly-used &dbgname
options.
This environment variable is used by the Windows version of the
debugger, &dbgcmdup.W.
.millust begin
&setcmdup. &setdelim.&dbgvarup.W=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified in the
&dbgcmdup.W prompt dialogue box.
The following example defines the default options to be "noinvoke"
(do not execute the
.fi profile.dbg
file) and "reg=10" (retain up to 10 register sets while tracing).
.exam begin
&prompt.&setcmd. &setdelim.&dbgvar.w=&sw.noinvoke &sw.reg#10&setdelim.
.exam end
.np
Once the
.ev &dbgvarup.W
environment variable has been defined, those options listed become the
default each time the &dbgcmdup.W command is used.
.do end
.************************************************************************
.if '&target' eq 'QNX' .do begin
.section &dbgvarup._PATH
.np
The
.ev &dbgvarup._PATH
environment variable is used by &dbgcmd to locate &dbgname support
files.
These files fall into five categories.
.ix 'support files' 'dbg'
.ix 'support files' 'trp'
.ix 'support files' 'prs'
.ix 'support files' 'hlp'
.ix 'support files' 'sym'
.autonote
.note
&dbgname command files (files with the ".dbg" suffix).
.note
&dbgname trap files (files with the ".trp" suffix).
.note
&dbgname parser files (files with the ".prs" suffix).
.note
&dbgname help files (files with the ".hlp" suffix).
.note
&dbgname symbolic debugging information files (files with the ".sym"
suffix).
.endnote
.millust begin
&setcmdup. &setdelim.&dbgvarup._PATH=path:path...&setdelim.
.millust end
.np
By default, &dbgname looks in the
.fi /usr/watcom/wd
directory for command files so it is not necessary to include this
directory in the
.ev &dbgvarup._PATH
environment variable string.
.do end
.************************************************************************
.if '&lang' eq 'FORTRAN 77' .do begin
.* WFC
.section &cvarup16
.np
The
.ev &cvarup16
environment variable can be used to specify commonly-used &cmpname
options.
.millust begin
&setcmdup. &setdelim.&cvarup16=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "om" (compile
with math optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&cvar16=&sw.d1 &sw.om&setdelim.
.exam end
.np
Once the
.ev &cvarup16
environment variable has been defined, those options listed become the
default each time the &ccmdup16 command is used.
.do end
.************************************************************************
.if '&lang' eq 'FORTRAN 77' .do begin
.* WFC386
.section &cvarup32
.np
The
.ev &cvarup32
environment variable can be used to specify commonly-used &cmpname
options.
.millust begin
&setcmdup. &setdelim.&cvarup32=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "om" (compile
with math optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&cvar32=&sw.d1 &sw.om&setdelim.
.exam end
.np
Once the
.ev &cvarup32
environment variable has been defined, those options listed become the
default each time the &ccmdup32 command is used.
.do end
.************************************************************************
.if '&lang' eq 'FORTRAN 77' .do begin
.section WFL
.np
The
.ev WFL
environment variable can be used to specify commonly-used WFL options.
.millust begin
&setcmdup. &setdelim.WFL=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "mm" (compile
code for medium memory model), "d1" (include line number debug
information in the object file), and "ox" (default optimizations).
.exam begin
&prompt.&setcmd. &setdelim.wfl=&sw.mm &sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WFL
environment variable has been defined, those options listed become the
default each time the WFL command is used.
.do end
.************************************************************************
.if '&lang' eq 'FORTRAN 77' .do begin
.section WFL386
.np
The
.ev WFL386
environment variable can be used to specify commonly-used WFL386
options.
.millust begin
&setcmdup. &setdelim.WFL386=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the
command line.
The following example defines the default options to be "mf" (flat
memory model), "d1" (include line number debug information in the
object file), and "ox" (default optimizations).
.exam begin
&prompt.&setcmd. &setdelim.wfl386=&sw.mf &sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WFL386
environment variable has been defined, those options listed become the
default each time the WFL386 command is used.
.do end
.************************************************************************
.if '&target' ne 'QNX' .do begin
.section WLANG
.np
.ix 'language'
.ix 'diagnostic messages' 'language'
.ix 'English diagnostic messages'
.ix 'Japanese diagnostic messages'
The
.ev WLANG
environment variable can be used to control which language is
used to display diagnostic and program usage messages by various
&company software tools.
The two currently-supported values for this variable are "English" or
"Japanese".
.millust begin
&setcmdup. &setdelim.WLANG=English&setdelim.
&setcmdup. &setdelim.WLANG=Japanese&setdelim.
.millust end
.pc
Alternatively, a numeric value of 0 (for English) or 1 (for Japanese)
can be specified.
.exam begin
&prompt.&setcmd. &setdelim.wlang=0&setdelim.
.exam end
.np
By default, Japanese messages are displayed when the current codepage
is 932 and English messages are displayed otherwise.
Normally, use of the
.ev WLANG
environment variable should not be required.
.do end
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&target' eq 'QNX' .do begin
.sr envvar='WPP'
.do end
.el .do begin
.sr envvar='wpp'
.do end
.section WPP
.np
The
.ev WPP
environment variable can be used to specify commonly-used options for
the 16-bit C++ compiler.
.millust begin
&setcmdup. &setdelim.WPP=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the command
line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "ox" (compile
for maximum number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&envvar.=&sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WPP
environment variable has been defined, those options listed become the
default each time the
.if '&target' eq 'QNX' .do begin
.kw wpp
.do end
.el .do begin
.kw WPP
.do end
command is used.
.do end
.************************************************************************
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&target' eq 'QNX' .do begin
.sr envvar='WPP386'
.do end
.el .do begin
.sr envvar='wpp386'
.do end
.section WPP386
.np
The
.ev WPP386
environment variable can be used to specify commonly-used options for
the 32-bit C++ compiler.
.millust begin
&setcmdup. &setdelim.WPP386=&sw.option1 &sw.option2 ...&setdelim.
.millust end
.pc
These options are processed before options specified on the command
line.
The following example defines the default options to be "d1" (include
line number debug information in the object file) and "ox" (compile
for maximum number of code optimizations).
.exam begin
&prompt.&setcmd. &setdelim.&envvar.=&sw.d1 &sw.ox&setdelim.
.exam end
.np
Once the
.ev WPP386
environment variable has been defined, those options listed become the
default each time the
.if '&target' eq 'QNX' .do begin
.kw wpp386
.do end
.el .do begin
.kw WPP386
.do end
command is used.
.do end
