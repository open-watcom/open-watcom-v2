.chap *refid=fplibr The &product Libraries
.*
.np
The &product library routines (intrinsic functions) are described in
the
.us &product Language Reference
manual.
Additional run-time routines are described in the
chapter entitled :HDREF refid='flib'..
Since &product supports two major architectures, the 286
architecture (which includes the 8088) and the 386 architecture
(which includes the 486 and Pentium processors), libraries are
grouped under two major directories.
.np
For the 286 architecture, the processor dependent libraries are placed
under the
.fi &pathnamup&pc.LIB286
directory.
.np
For the 386 architecture, the processor dependent libraries are placed
under the
.fi &pathnamup&pc.LIB386
directory.
.np
Since &product also supports several operating systems, including DOS,
Windows 3.x, Windows 95, Windows NT, OS/2 and NetWare, system-dependent
libraries are grouped under different directories underneath the
processor-dependent directories.
:CMT. .sr c0=&sysin+5
:CMT. .sr c1=&sysin+15
:CMT. .sr c2=&sysin+45
:CMT. .tb &c0 &c1
:CMT. .tb set $
.millust begin
.ix 'DOS subdirectory'
System      16-bit applications     32-bit applications
---------   ---------------------   ---------------------
DOS         &pathnamup\LIB286\DOS      &pathnamup\LIB386\DOS

.ix 'OS2 subdirectory'
OS/2        &pathnamup\LIB286\OS2      &pathnamup\LIB386\OS2

.ix 'WIN subdirectory'
Windows 3.x &pathnamup\LIB286\WIN      &pathnamup\LIB386\WIN

.ix 'NT subdirectory'
Windows NT                          &pathnamup\LIB386\NT
Windows 95

.ix 'NETWARE subdirectory'
NetWare                             &pathnamup\LIB386\NETWARE
.millust end
.millust begin
                  &pathnam
                     |
         .-----------+----------------.
         |                            |
      lib286                       lib386
         |                            |
 .-------+-------.    .-------.-------+-------.-------.
 |       |       |    |       |       |       |       |
dos     os2     win  dos     os2     win      nt   netware
 |       |       |    |       |       |       |       |
.millust end
.np
Due to the many code generation strategies possible in the 80x86
family of processors, a number of versions of the libraries are
provided.
You must use the libraries which coincide with the particular
architecture, operating system, and
code generation strategy or model that you have selected.
.ix 'options' 'm?'
For the type of code generation strategy or model that you intend to
use, refer to the description of the "m?" memory model compiler option
in the chapter entitled :HDREF refid='fpopts'..
The various code models supported by &product are described in the
chapters entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.np
We have selected a simple naming convention for the libraries that are
provided with &product..
Letters are affixed to the file name to indicate the particular
strategy with which the modules in the library have been compiled.
.begnote
.ix 'FORTRAN 77 libraries' 'medium'
.ix 'medium model' 'libraries'
.mnote M
denotes a version of the 16-bit &product libraries which have been
compiled for the "medium" memory model (big code, small data).
.ix 'FORTRAN 77 libraries' 'large'
.ix 'large model' 'libraries'
.ix 'FORTRAN 77 libraries' 'huge'
.ix 'huge model' 'libraries'
.mnote L
denotes a version of the 16-bit &product libraries which have been
compiled for the "large" or "huge" memory models (big code, big data
or huge data).
.ix 'FORTRAN 77 libraries' 'in-line 80x87 instructions'
.ix 'in-line 80x87 instructions' 'libraries'
.mnote 7
denotes a version of the &product libraries which should be used when
compiling with the "fpi" or "fpi87"
option.
Otherwise the libraries have been compiled using the "fpc" compiler option.
.ix 'FORTRAN 77 libraries' 'flat'
.ix 'flat model' 'libraries'
.ix 'FORTRAN 77 libraries' 'small'
.ix 'small model' 'libraries'
.mnote S
denotes a version of the 32-bit &product libraries which have been
compiled using the "sc" option (stack calling conventions).
.endnote
.np
The 16-bit &product libraries are listed below by directory.
.np
.cp 8
.us Under
.fi &pathnamup\LIB286\DOS
.millust begin
FLIBM.LIB   (DOS medium model)
FLIB7M.LIB  (DOS medium model, in-line 80x87)
FLIBL.LIB   (DOS large/huge model)
FLIB7L.LIB  (DOS large/huge model, in-line 80x87)
CLIBM.LIB   (DOS i/o system medium model)
CLIBL.LIB   (DOS i/o system large/huge model)
GRAPH.LIB   (DOS graphics support)
.millust end
.ix 'WINDOWS.LIB'
.ix 'Windows SDK' 'Microsoft'
.np
.cp 8
.us Under
.fi &pathnamup\LIB286\WIN
.millust begin
FLIBM.LIB   (Windows medium model)
FLIB7M.LIB  (Windows medium model, in-line 80x87)
FLIBL.LIB   (Windows large/huge model)
FLIB7L.LIB  (Windows large/huge model, in-line 80x87)
CLIBM.LIB   (Windows i/o system medium model)
CLIBL.LIB   (Windows i/o system large/huge model)
WINDOWS.LIB (Windows API library)
.millust end
.np
.cp 13
.us Under
.fi &pathnamup\LIB286\OS2
.ix 'DOSPMM.LIB'
.ix 'DOSPML.LIB'
.millust begin
FLIBM.LIB   (OS/2 medium model)
FLIB7M.LIB  (OS/2 medium model, in-line 80x87)
FLIBL.LIB   (OS/2 large/huge model)
FLIB7L.LIB  (OS/2 large/huge model, in-line 80x87)
CLIBM.LIB   (OS/2 i/o system medium model)
CLIBL.LIB   (OS/2 i/o system large/huge model)
DOSPMM.LIB  (Phar Lap 286 PM medium model)
DOSPML.LIB  (Phar Lap 286 PM large/huge model)
.millust end
.np
The 32-bit &product libraries are listed below.
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\DOS
.millust begin
FLIB.LIB     (floating-point calls)
FLIB7.LIB    (in-line 80x87)
FLIBS.LIB    (floating-point calls, stack conventions)
FLIB7S.LIB   (in-line 80x87, stack conventions)
CLIB3R.LIB   (i/o system)
CLIB3S.LIB   (i/o system, stack conventions)
GRAPH.LIB    (DOS graphics support)
.millust end
.pc
The graphics library
.fi GRAPH.LIB
is independent of the argument passing conventions or floating-point model.
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\WIN
.millust begin
FLIB.LIB     (floating-point calls)
FLIB7.LIB    (in-line 80x87)
FLIBS.LIB    (floating-point calls, stack conventions)
FLIB7S.LIB   (in-line 80x87, stack conventions)
CLIB3R.LIB   (i/o system)
CLIB3S.LIB   (i/o system, stack conventions)
WIN386.LIB   (32-bit Windows API)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\OS2
.millust begin
FLIB.LIB     (floating-point calls)
FLIB7.LIB    (in-line 80x87)
FLIBS.LIB    (floating-point calls, stack conventions)
FLIB7S.LIB   (in-line 80x87, stack conventions)
CLIB3R.LIB   (i/o system)
CLIB3S.LIB   (i/o system, stack conventions)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\NT
.millust begin
FLIB.LIB     (floating-point calls)
FLIB7.LIB    (in-line 80x87)
FLIBS.LIB    (floating-point calls, stack conventions)
FLIB7S.LIB   (in-line 80x87, stack conventions)
CLIB3R.LIB   (i/o system)
CLIB3S.LIB   (i/o system, stack conventions)
.millust end
.*
.section &product 80x87 Emulator Libraries
.*
.ix 'options' 'fpi'
.np
One of the following libraries must be used if any of the modules
of your application were compiled with the "fpi" option.
.np
.us 16-bit Libraries
.millust begin
NOEMU87.LIB
DOS&pc.EMU87.LIB (DOS dependent)
WIN&pc.EMU87.LIB (Windows dependent)
OS2&pc.EMU87.LIB (OS/2 dependent)
.millust end
.np
.cp 7
.us 32-bit Libraries
.millust begin
NOEMU387.LIB
DOS&pc.EMU387.LIB (DOS dependent)
WIN&pc.EMU387.LIB (Windows dependent)
OS2&pc.EMU387.LIB (OS/2 dependent)
NT&pc.EMU387.LIB (Windows NT dependent)
.millust end
.pc
.ix 'numeric data processor'
.ix 'math coprocessor'
.ix 'options' 'fpi'
The "fpi" option causes an 80x87 numeric data processor emulator to be
linked into your application.
This emulator will decode and emulate 80x87 instructions when an 80x87
is not present in the
system or if the environment variable
.ev NO87
has been set (this variable is described below).
.np
If you have compiled your application using the "fpi" option, you can
also link with the 16-bit "noemu87.lib" or 32-bit "noemu387.lib"
library, depending on which compiler you are using.
However, your application will only run on a machine equipped with a
80x87 numeric data processor since the actual emulator is not linked
into your application.
.np
.ix 'options' 'fpi87'
When the "fpi87" option is used exclusively, the emulator is not included.
In this case, the application must be run on personal computer systems
equipped with the numeric data processor.
.*
.section The "NO87" Environment Variable
.*
.np
If you have a math coprocessor in your system but you wish to test a
version of your application that will use floating-point emulation
("fpi" option) or simulation ("fpc" option), you can define the
.ev NO87
environment variable.
.ix 'options' 'fpc'
.ix 'options' 'fpi'
.ix 'emu87.lib'
The 16-bit application must be compiled using the "fpc"
(floating-point calls) option and linked with the appropriate
.fi flib?.lib
library or the "fpi" option (default) and linked with the appropriate
.fi flib7?.lib
and
.fi emu87.lib
libraries.
The 32-bit application must be compiled using the "fpc"
(floating-point calls) option and linked with the appropriate
.fi flib?.lib
library or the "fpi" option (default) and linked with the appropriate
.fi flib7?.lib
and
.fi emu387.lib
libraries.
.ix '&setcmdup' 'NO87 environment variable'
Using the "&setcmdup" command, define the environment variable
as follows:
.millust begin
&prompt.&setcmdup NO87=1
.millust end
.pc
Now, when you run your application, the 80x87 will be ignored.
To undefine the environment variable, enter the command:
.millust begin
&prompt.&setcmdup NO87=
.millust end
