.chap *refid=cplibr The &product Libraries
.*
.np
.ix 'libraries'
The &product library routines are described in the
.book &company C Library Reference
manual, and the
.book &company C++ Class Library Reference
manual.
.if '&target' eq 'QNX' .do begin
.*
.section &product Library Directory Structure
.*
.np
.ix 'libraries' 'location'
The &product libraries are located under the
.fi &pathnam&libdir
directory.
.millust begin
                        /usr
                          |
             ---.---------+---------.---
                |                   |
               lib               include
                |                   |
.millust end
.do end
.el .do begin
.*
.section &product Library Directory Structure
.*
.np
.ix 'libraries' 'directory structure'
Since &product supports both 16-bit and 32-bit application
development, libraries are grouped under two major subdirectories.
The
.fi LIB286
directory is used to contain libraries for 16-bit application
development.
The
.fi LIB386
directory is used to contain libraries for 32-bit application
development.
.np
For 16-bit application development, the Intel x86 processor-dependent
libraries are placed under the
.fi &pathnamup&pc.LIB286
directory.
.np
For 32-bit application development, the Intel 386 and
upward-compatible processor-dependent libraries are placed under the
.fi &pathnamup&pc.LIB386
directory.
.np
Since &product also supports several operating systems, including
DOS, OS/2, Windows 3.x and Windows NT, system-dependent libraries are
grouped under different directories underneath the processor-dependent
directories.
.np
.ix 'DOS subdirectory'
For DOS applications, the system-dependent libraries are placed in
.fi &pathnamup\LIB286\DOS
(16-bit applications) and
.fi &pathnamup\LIB386\DOS
(32-bit applications).
.np
.ix 'OS2 subdirectory'
For OS/2 applications, the system-dependent libraries are
placed in
.fi &pathnamup\LIB286\OS2
(16-bit applications) and
.fi &pathnamup\LIB386\OS2
(32-bit applications).
.np
.ix 'WIN subdirectory'
For Microsoft Windows applications, the system-dependent libraries are
placed in
.fi &pathnamup\LIB286\WIN
(16-bit applications) and
.fi &pathnamup\LIB386\WIN
(32-bit applications).
.np
.ix 'NT subdirectory'
For Microsoft Windows NT applications, the system-dependent libraries
are placed in
.fi &pathnamup\LIB386\NT
(32-bit applications).
.np
.ix 'NETWARE subdirectory'
For Novell NetWare 386 applications, the system-dependent libraries
are placed in
.fi &pathnamup\LIB386\NETWARE
(32-bit applications).
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
.do end
.* --------------------
.*
.section &product C Libraries
.*
.np
Due to the many code generation strategies possible in the 80x86
family of processors, a number of versions of the libraries are
provided.
You must use the libraries which coincide with the particular
.if '&target' ne 'QNX' .do begin
architecture, operating system, and
.do end
code generation strategy or model that you have selected.
For the type of code generation strategy or model that you intend to
use, refer to the description of the "m?" memory model compiler option.
The various code models supported by &product are described in the
chapters entitled :HDREF refid='mdl86'. and :HDREF refid='mdl386'..
.np
We have selected a simple naming convention for the libraries that are
provided with &product..
Letters are affixed to the file name to indicate the particular
strategy with which the modules in the library have been compiled.
.begnote 16-bit only
.ix 'C libraries' 'small'
.ix 'small model' 'libraries'
.mnote S
denotes a version of the &product libraries which have been compiled
for the "small" memory model (small code, small data).
.ix 'C libraries' 'medium'
.ix 'medium model' 'libraries'
.mnote M
denotes a version of the &product libraries which have been compiled
for the "medium" memory model (big code, small data).
.ix 'C libraries' 'compact'
.ix 'compact model' 'libraries'
.mnote C
denotes a version of the &product libraries which have been compiled
for the "compact" memory model (small code, big data).
.ix 'C libraries' 'large'
.ix 'large model' 'libraries'
.mnote L
denotes a version of the &product libraries which have been compiled
for the "large" memory model (big code, big data).
.ix 'C libraries' 'huge'
.ix 'huge model' 'libraries'
.mnote H
denotes a version of the &product libraries which have been compiled
for the "huge" memory model (big code, huge data).
.if '&target' ne 'QNX' .do begin
.mnote MT
denotes a version of the &product libraries which may be used with
OS/2 multi-threaded applications.
.mnote DL
denotes a version of the &product libraries which may be used when
creating an OS/2 Dynamic Link Library.
.do end
.endnote
.begnote 32-bit only
.ix '&lang libraries' 'flat'
.ix 'flat model' 'libraries'
.ix '&lang libraries' 'small'
.ix 'small model' 'libraries'
.mnote 3R
denotes a version of the &product libraries that will be used by
programs which have been compiled for the "flat/small" memory models
using the "3r", "4r", "5r" or "6r" option.
.mnote 3S
denotes a version of the &product libraries that will be used by
programs which have been compiled for the "flat/small" memory models
using the "3s", "4s", "5s" or "6s" option.
.endnote
.* WCC ---------------------------------------------------------
.np
.if '&target' eq 'DOS' .do begin
The &product 16-bit libraries are listed below by directory.
.np
.cp 8
.us Under
.fi &pathnamup\LIB286\DOS
.ix 'CLIBS.LIB'
.ix 'CLIBM.LIB'
.ix 'CLIBC.LIB'
.ix 'CLIBL.LIB'
.ix 'CLIBH.LIB'
.ix 'GRAPH.LIB'
.ix 'DOSLFNS.LIB'
.ix 'DOSLFNM.LIB'
.ix 'DOSLFNC.LIB'
.ix 'DOSLFNL.LIB'
.ix 'DOSLFNH.LIB'
.millust begin
CLIBS.LIB    (DOS small model support)
CLIBM.LIB    (DOS medium model support)
CLIBC.LIB    (DOS compact model support)
CLIBL.LIB    (DOS large model support)
CLIBH.LIB    (DOS huge model support)
GRAPH.LIB    (model independent, DOS graphics support)
DOSLFNS.LIB  (DOS LFN small model support)
DOSLFNM.LIB  (DOS LFN medium model support)
DOSLFNC.LIB  (DOS LFN compact model support)
DOSLFNL.LIB  (DOS LFN large model support)
DOSLFNH.LIB  (DOS LFN huge model support)
.millust end
.np
.cp 13
.us Under
.fi &pathnamup\LIB286\OS2
.ix 'CLIBMTL.LIB'
.ix 'CLIBDLL.LIB'
.ix 'DOSPMS.LIB'
.ix 'DOSPMM.LIB'
.ix 'DOSPMC.LIB'
.ix 'DOSPML.LIB'
.ix 'DOSPMH.LIB'
.millust begin
CLIBS.LIB    (OS/2 small model support)
CLIBM.LIB    (OS/2 medium model support)
CLIBC.LIB    (OS/2 compact model support)
CLIBL.LIB    (OS/2 large model support)
CLIBH.LIB    (OS/2 huge model support)
CLIBMTL.LIB  (OS/2 multi-thread, large model support)
CLIBDLL.LIB  (OS/2 DLL, large model support)
DOSPMS.LIB   (Phar Lap 286 PM small model support)
DOSPMM.LIB   (Phar Lap 286 PM medium model support)
DOSPMC.LIB   (Phar Lap 286 PM compact model support)
DOSPML.LIB   (Phar Lap 286 PM large model support)
DOSPMH.LIB   (Phar Lap 286 PM huge model support)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup\LIB286\WIN
.ix 'CLIBS.LIB'
.ix 'CLIBM.LIB'
.ix 'CLIBC.LIB'
.ix 'CLIBL.LIB'
.ix 'WINDOWS.LIB'
.ix 'Windows SDK' 'Microsoft'
.millust begin
CLIBS.LIB    (Windows small model support)
CLIBM.LIB    (Windows medium model support)
CLIBC.LIB    (Windows compact model support)
CLIBL.LIB    (Windows large model support)
WINDOWS.LIB  (Windows API library)
.millust end
.do end
.el .if '&target' eq 'QNX' .do begin
The &product 16-bit libraries are listed below.
.ix 'clibs.lib'
.ix 'clibm.lib'
.ix 'clibc.lib'
.ix 'clibl.lib'
.ix 'clibh.lib'
.millust begin
clibs.lib    (small model support)
clibm.lib    (medium model support)
clibc.lib    (compact model support)
clibl.lib    (large model support)
clibh.lib    (huge model support)
.millust end
.do end
.* WCC386 ------------------------------------------------------
.if '&target' eq 'QNX' .do begin
The &product 32-bit libraries are listed below.
.ix 'clib3r.lib'
.ix 'clib3s.lib'
.millust begin
clib3r.lib   (flat/small models, "3r", "4r", "5r" or "6r" option)
clib3s.lib   (flat/small models, "3s", "4s", "5s" or "6s" option)
.millust end
.do end
.el .if '&target' eq 'DOS' .do begin
The &product 32-bit libraries are listed below by directory.
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\DOS
.ix 'CLIB3R.LIB'
.ix 'CLIB3S.LIB'
.ix 'GRAPH.LIB'
.ix 'DOSLFN3R.LIB'
.ix 'DOSLFN3S.LIB'
.millust begin
CLIB3R.LIB   (flat/small models, "3r", "4r", "5r" or "6r" option)
CLIB3S.LIB   (flat/small models, "3s", "4s", "5s" or "6s" option)
GRAPH.LIB    (flat/small models, DOS graphics support)
DOSLFN3R.LIB (flat/small models, DOS LFN support,
                                 "3r", "4r", "5r" or "6r" option)
DOSLFN3S.LIB (flat/small models, DOS LFN support,
                                 "3s", "4s", "5s" or "6s" option)
.millust end
.pc
The graphics library
.fi GRAPH.LIB
is independent of the argument passing conventions.
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\OS2
.ix 'CLIB3R.LIB'
.ix 'CLIB3S.LIB'
.millust begin
CLIB3R.LIB   (flat/small models, "3r", "4r", "5r" or "6r" option)
CLIB3S.LIB   (flat/small models, "3s", "4s", "5s" or "6s" option)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\WIN
.ix 'CLIB3R.LIB'
.ix 'CLIB3S.LIB'
.ix 'WIN386.LIB'
.ix 'Windows SDK' 'Microsoft'
.millust begin
CLIB3R.LIB   (flat/small models, "3r", "4r", "5r" or "6r" option)
CLIB3S.LIB   (flat/small models, "3s", "4s", "5s" or "6s" option)
WIN386.LIB   (32-bit Windows API)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup\LIB386\NT
.ix 'CLIB3R.LIB'
.ix 'CLIB3S.LIB'
.millust begin
CLIB3R.LIB   (flat/small models, "3r", "4r", "5r" or "6r" option)
CLIB3S.LIB   (flat/small models, "3s", "4s", "5s" or "6s" option)
.millust end
.do end
.*
.if '&target' eq 'QNX' .do begin
.*
.section &cmpcname 16-bit Shared Library
.*
.np
.ix 'shared library'
A portion of the 16-bit &cmpcname Library is also stored in a
memory-resident library called the system
.us shared library.
On multi-tasking systems, it makes sense that commonly-used library
routines such as
.us read
and
.us write
be shared among processes.
By sharing the same code, the memory requirement for applications is
reduced.
The functions in the shared library are memory model independent so
they can be used by any small/large code, small/large/huge data
applications.
:cmt. .np
:cmt. .ix '_NO_SLIB'
:cmt. If the
:cmt. .kwm _NO_SLIB
:cmt. macro is
:cmt. .us not
:cmt. defined before header files are included when compiling a C source
:cmt. file, then the shared library version of a function is called directly
:cmt. (provided that the function is in the shared library).
:cmt. Not all the functions in the C library have been placed in the shared
:cmt. library, but many of the commonly-used functions are to be found
:cmt. there.
:cmt. By default, your application will use shared library functions.
:cmt. .np
:cmt. If the
:cmt. .kwm _NO_SLIB
:cmt. macro is defined before header files are included, then a direct
:cmt. reference is made to the function being invoked.
:cmt. Also, if the address of a shared library function (e.g.,
:cmt. .us &read
:cmt. .ct )
:cmt. is requested by the application, then a direct reference has been
:cmt. made.
:cmt. Normally, this results in the actual code for the routine being
:cmt. included in the executable image (a non-shared copy).
:cmt. However, if you wish to use the routines in the shared library,
:cmt. interface libraries for each memory model are provided.
:cmt. For example, the interface routine for
:cmt. .us read
:cmt. calls the actual
:cmt. .us read
:cmt. function in the shared library.
:cmt. The &product shared-library interface libraries are listed below.
:cmt. .ix 'stubs.lib'
:cmt. .ix 'stubm.lib'
:cmt. .ix 'stubc.lib'
:cmt. .ix 'stubl.lib'
:cmt. .ix 'stubh.lib'
:cmt. .millust begin
:cmt. stubs.lib    (small model support)
:cmt. stubm.lib    (medium model support)
:cmt. stubc.lib    (compact model support)
:cmt. stubl.lib    (large model support)
:cmt. stubh.lib    (huge model support)
:cmt. .millust end
:cmt. .np
:cmt. .kw cc
:cmt. defines
:cmt. .kwm _NO_SLIB
:cmt. if any of the
:cmt. .sy &sw.g,
:cmt. .sy &sw.Ci
:cmt. or
:cmt. .sy &sw.Cn
:cmt. options are used.
:cmt. .kw cc
:cmt. causes the linker,
:cmt. .kw wlink,
:cmt. to search the appropriate
:cmt. .fi stub?.lib
:cmt. if any of the
:cmt. .sy &sw.Cd
:cmt. or
:cmt. .sy &sw.Ci
:cmt. options are used.
.do end
.*
.section &product Class Libraries
.*
.np
.ix 'libraries' 'class'
The &product Class Library routines are described in the
.book &cmppname Class Library Reference
manual.
.np
The &cmppname 16-bit Class Libraries are listed below.
.if '&target' eq 'QNX' .do begin
.ix 'plibs.lib'
.ix 'plibm.lib'
.ix 'plibc.lib'
.ix 'plibl.lib'
.ix 'plibh.lib'
.ix 'cplxs.lib'
.ix 'cplxm.lib'
.ix 'cplxc.lib'
.ix 'cplxl.lib'
.ix 'cplxh.lib'
.ix 'cplx7s.lib'
.ix 'cplx7m.lib'
.ix 'cplx7c.lib'
.ix 'cplx7l.lib'
.ix 'cplx7h.lib'
.millust begin
    (iostream and string class libraries)
plibs.lib    (small model support)
plibm.lib    (medium model support)
plibc.lib    (compact model support)
plibl.lib    (large model support)
plibh.lib    (huge model support)
    (complex class library for "fpc" option)
cplxs.lib    (small model support)
cplxm.lib    (medium model support)
cplxc.lib    (compact model support)
cplxl.lib    (large model support)
cplxh.lib    (huge model support)
    (complex class library for "fpi..." options)
cplx7s.lib   (small model support)
cplx7m.lib   (medium model support)
cplx7c.lib   (compact model support)
cplx7l.lib   (large model support)
cplx7h.lib   (huge model support)
.millust end
.pc
These libraries are independent of the operating system.
.do end
.el .do begin
.np
.cp 8
.us Under
.fi &pathnamup\LIB286
.ix 'PLIBS.LIB'
.ix 'PLIBM.LIB'
.ix 'PLIBC.LIB'
.ix 'PLIBL.LIB'
.ix 'PLIBH.LIB'
.ix 'PLIBMTL.LIB'
.ix 'PLIBDLL.LIB'
.ix 'CPLXS.LIB'
.ix 'CPLXM.LIB'
.ix 'CPLXC.LIB'
.ix 'CPLXL.LIB'
.ix 'CPLXH.LIB'
.ix 'CPLX7S.LIB'
.ix 'CPLX7M.LIB'
.ix 'CPLX7C.LIB'
.ix 'CPLX7L.LIB'
.ix 'CPLX7H.LIB'
.millust begin
    (iostream and string class libraries)
PLIBS.LIB    (small model support)
PLIBM.LIB    (medium model support)
PLIBC.LIB    (compact model support)
PLIBL.LIB    (large model support)
PLIBH.LIB    (huge model support)
PLIBMTL.LIB  (OS/2 multi-thread, large model support)
PLIBDLL.LIB  (OS/2 DLL, large model support)
    (complex class library for "fpc" option)
CPLXS.LIB    (small model support)
CPLXM.LIB    (medium model support)
CPLXC.LIB    (compact model support)
CPLXL.LIB    (large model support)
CPLXH.LIB    (huge model support)
    (complex class library for "fpi..." options)
CPLX7S.LIB   (small model support)
CPLX7M.LIB   (medium model support)
CPLX7C.LIB   (compact model support)
CPLX7L.LIB   (large model support)
CPLX7H.LIB   (huge model support)
.millust end
.pc
These libraries are independent of the operating system (except those
designated for OS/2).
.do end
The "7" designates a library compiled with the "7" option.
.np
The &cmppname 32-bit Class Libraries are listed below.
.if '&target' eq 'QNX' .do begin
.ix 'plib3r.lib'
.ix 'plib3s.lib'
.ix 'cplx3r.lib'
.ix 'cplx3s.lib'
.ix 'cplx73r.lib'
.ix 'cplx73s.lib'
.millust begin
    (iostream and string class libraries)
plib3r.lib   (flat models, "3r", "4r", "5r" or "6r" option)
plib3s.lib   (flat models, "3s", "4s", "5s" or "6s" option)
    (complex class library for "fpc" option)
cplx3r.lib   (flat models, "3r", "4r", "5r" or "6r" option)
cplx3s.lib   (flat models, "3s", "4s", "5s" or "6s" option)
    (complex class library for "fpi..." options)
cplx73r.lib  (flat models, "3r", "4r", "5r" or "6r" option)
cplx73s.lib  (flat models, "3s", "4s", "5s" or "6s" option)
.millust end
.pc
These libraries are independent of the operating system.
The "3r" and "3s" suffixes refer to the argument passing convention
used.
.do end
.el .do begin
.np
.cp 8
.us Under
.fi &pathnamup\LIB386
.ix 'PLIB3R.LIB'
.ix 'PLIB3S.LIB'
.ix 'CPLX3R.LIB'
.ix 'CPLX3S.LIB'
.ix 'CPLX73R.LIB'
.ix 'CPLX73S.LIB'
.millust begin
    (iostream and string class libraries)
PLIB3R.LIB   (flat models, "3r", "4r", "5r" or "6r" option)
PLIB3S.LIB   (flat models, "3s", "4s", "5s" or "6s" option)
PLIBMT3R.LIB (multi-thread library for OS/2 and Windows NT)
PLIBMT3S.LIB (multi-thread library for OS/2 and Windows NT)
    (complex class library for "fpc" option)
CPLX3R.LIB   (flat models, "3r", "4r", "5r" or "6r" option)
CPLX3S.LIB   (flat models, "3s", "4s", "5s" or "6s" option)
    (complex class library for "fpi..." options)
CPLX73R.LIB  (flat models, "3r", "4r", "5r" or "6r" option)
CPLX73S.LIB  (flat models, "3s", "4s", "5s" or "6s" option)
.millust end
.pc
These libraries are independent of the operating system (except those
designated for OS/2 and Windows NT).
The "3R" and "3S" suffixes refer to the argument passing convention
used.
.do end
The "7" designates a library compiled with the "7" option.
.*
.section *refid=libmath &product Math Libraries
.*
.np
.ix 'libraries' 'math'
In general, a Math library is required when floating-point
computations are included in the application.
The Math libraries are operating-system independent.
.if '&target' eq 'QNX' .do begin
.ix '&libdir'
The Math libraries are placed under the
.fi &pathnam&libdir
directory.
.do end
.el .do begin
.np
.ix 'LIB286'
For the 286 architecture, the Math libraries are placed under the
.fi &pathnamup&pc.LIB286
directory.
.np
.ix 'LIB386'
For the 386 architecture, the Math libraries are placed under the
.fi &pathnamup&pc.LIB386
directory.
.do end
.if '&target' ne 'QNX' .do begin
.np
.ix '80x87 emulator'
.ix 'floating-point emulator'
.ix 'emulator' '80x87'
.ix 'emulator' 'floating-point'
An 80x87 emulator library,
.fi emu87.lib
.ct , is also provided which is both operating-system and architecture
dependent.
.do end
.np
The following situations indicate that one of the Math libraries
should be included when linking the application.
.autonote
.note
When one or more of the functions described in the
.fi math.h
header file is referenced, then a Math library must be included.
.keep 4
.note
.ix '_fltused_'
.ix 'floating-point' '_fltused_'
If an application is linked and the message
.millust begin
"_fltused_ is an undefined reference"
.millust end
.pc
appears, then a Math library must be included.
.*
.note
(16-bit only)
If an application is linked and the message
.ix '__init_87_emulator'
.ix 'floating-point' '__init_87_emulator'
.millust begin
"__init_87_emulator is an undefined reference"
.millust end
.pc
appears, then one of the modules in the application was compiled with
one of the "fpi", "fpi87", "fp2", "fp3" or "fp5" options.
.ix 'emu87.lib'
.ix 'noemu87.lib'
If the "fpi" option was used, the 80x87 emulator library (
.ct
.fi emu87.lib
.ct
) or the 80x87 fixup library (
.ct
.fi noemu87.lib
.ct
) should be included when linking the application.
.np
If the "fpi87" option was used, the 80x87 fixup library
.fi noemu87.lib
should be included when linking the application.
.if '&target' ne 'QNX' .do begin
.np
The 80x87 emulator is contained in
.fi emu87.lib.
Use
.fi noemu87.lib
in place of
.fi emu87.lib
when the emulator is not wanted.
.do end
.note
(32-bit only)
If an application is linked and the message
.ix '__init_387_emulator'
.ix 'floating-point' '__init_387_emulator'
.millust begin
"__init_387_emulator is an undefined reference"
.millust end
.pc
appears, then one of the modules in the application was compiled with
one of the "fpi", "fpi87", "fp2", "fp3" or "fp5" options.
.ix 'emu387.lib'
.ix 'noemu387.lib'
If the "fpi" option was used, the 80x87 emulator library (
.ct
.fi emu387.lib
.ct ) should be included when linking the application.
.np
If the "fpi87" option was used, the empty 80x87 emulator library
.fi noemu387.lib
should be included when linking the application.
.if '&target' ne 'QNX' .do begin
.np
The 80x87 emulator is contained in
.fi emu387.lib.
Use
.fi noemu387.lib
in place of
.fi emu387.lib
when the emulator is not wanted.
.do end
.*
.endnote
.np
Normally, the compiler and linker will automatically take care of
this.
Simply ensure that the
.ev &pathvarup
environment variable includes the location of the &product libraries.
.*
.section &product 80x87 Math Libraries
.*
.np
.ix 'libraries' '80x87 math'
.ix 'options' 'fpi'
.ix 'options' 'fpi87'
.ix 'options' 'fp2'
.ix 'options' 'fp3'
.ix 'options' 'fp5'
.*
One of the following Math libraries must be used if any of the modules
of your application were compiled with one of the &product "fpi",
"fpi87", "fp2", "fp3" or "fp5" options and your application requires
floating-point support for the reasons given above.
.* WCC ---------------------------------------------------------
.np
.us 16-bit libraries:
.ix 'C libraries' 'small'
.ix 'small model' 'libraries'
.ix 'C libraries' 'medium'
.ix 'medium model' 'libraries'
.ix 'C libraries' 'compact'
.ix 'compact model' 'libraries'
.ix 'C libraries' 'large'
.ix 'large model' 'libraries'
.ix 'C libraries' 'huge'
.ix 'huge model' 'libraries'
.if '&target' eq 'QNX' .do begin
.ix 'math87s.lib'
.ix 'math87m.lib'
.ix 'math87c.lib'
.ix 'math87l.lib'
.ix 'math87h.lib'
.millust begin
math87s.lib (small model)
math87m.lib (medium model)
math87c.lib (compact model)
math87l.lib (large model)
math87h.lib (huge model)
noemu87.lib
emu87.lib   (QNX dependent)
.millust end
.do end
.el .do begin
.ix 'MATH87S.LIB'
.ix 'MATH87M.LIB'
.ix 'MATH87C.LIB'
.ix 'MATH87L.LIB'
.ix 'MATH87H.LIB'
.millust begin
MATH87S.LIB (small model)
MATH87M.LIB (medium model)
MATH87C.LIB (compact model)
MATH87L.LIB (large model)
MATH87H.LIB (huge model)
NOEMU87.LIB
DOS&pc.EMU87.LIB (DOS dependent)
OS2&pc.EMU87.LIB (OS/2 dependent)
WIN&pc.EMU87.LIB (Windows dependent)
WIN&pc.MATH87C.LIB (Windows dependent)
WIN&pc.MATH87L.LIB (Windows dependent)
.millust end
.do end
.* WCC386 ------------------------------------------------------
.np
.us 32-bit libraries:
.ix 'C libraries' 'flat'
.ix 'flat model' 'libraries'
.ix 'C libraries' 'small'
.ix 'small model' 'libraries'
.if '&target' eq 'QNX' .do begin
.ix 'math387r.lib'
.ix 'math387s.lib'
.millust begin
math387r.lib (flat/small models, "3r", "4r", "5r" or "6r" option)
math387s.lib (flat/small models, "3s", "4s", "5s" or "6s" option)
emu387.lib   (QNX dependent)
.millust end
.do end
.el .do begin
.ix 'MATH387R.LIB'
.ix 'MATH387S.LIB'
.millust begin
MATH387R.LIB (flat/small models, "3r", "4r", "5r" or "6r" option)
MATH387S.LIB (flat/small models, "3s", "4s", "5s" or "6s" option)
DOS&pc.EMU387.LIB (DOS dependent)
WIN&pc.EMU387.LIB (Windows dependent)
OS2&pc.EMU387.LIB (OS/2 dependent)
NT&pc.EMU387.LIB (Windows NT dependent)
.millust end
.do end
.pc
.ix 'numeric data processor'
.ix 'math coprocessor'
.ix 'options' 'fpi'
The "fpi" option causes an 80x87 numeric data processor emulator to be
linked into your application in addition to any 80x87 math routines
that were referenced.
.if '&target' eq 'QNX' .do begin
For QNX, there is a common 80x87 emulator task that is used so that
there is one copy of the emulator in memory at any one time.
.do end
This emulator will decode and emulate 80x87 instructions when an 80x87
is not present in the
.if '&target' eq 'QNX' .do begin
system.
.do end
.el .do begin
system or if the environment variable
.ev NO87
has been set (this variable is described below).
.do end
.if '&target' ne 'QNX' .do begin
.np
For 32-bit &company Windows-extender applications or 32-bit
applications run in Windows 3.1 DOS boxes, you must also include the
.fi WEMU387.386
file in the
.mono [386enh]
section of the
.fi SYSTEM.INI
file.
.exam begin
device=C:&pathnamup.\binw\wemu387.386
.exam end
Note that the
.fi WDEBUG.386
file which is installed by the &company Installation software contains
the emulation support found in the
.fi WEMU387.386
file.
.do end
.np
.ix 'options' 'fpi87'
When the "fpi87" option is used exclusively, the emulator is not
included.
In this case, the application must be run on personal computer systems
equipped with the numeric data processor.
.*
.section &product Alternate Math Libraries
.*
.np
.ix 'libraries' 'alternate math'
.ix 'options' 'fpc'
One of the following Math libraries must be used if any of the modules
of your application were compiled with the &product "fpc" option and
your application requires floating-point support for the reasons given
above.
The following Math libraries include support for floating-point which
is done out-of-line through run-time calls.
.* WCC ---------------------------------------------------------
.np
.us 16-bit libraries:
.if '&target' eq 'QNX' .do begin
.ix 'maths.lib'
.ix 'mathm.lib'
.ix 'mathc.lib'
.ix 'mathl.lib'
.ix 'mathh.lib'
.millust begin
maths.lib (small model)
mathm.lib (medium model)
mathc.lib (compact model)
mathl.lib (large model)
mathh.lib (huge model)
.millust end
.do end
.el .do begin
.ix 'MATHS.LIB'
.ix 'MATHM.LIB'
.ix 'MATHC.LIB'
.ix 'MATHL.LIB'
.ix 'MATHH.LIB'
.millust begin
MATHS.LIB (small model)
MATHM.LIB (medium model)
MATHC.LIB (compact model)
MATHL.LIB (large model)
MATHH.LIB (huge model)
WIN&pc.MATHC.LIB (Windows dependent)
WIN&pc.MATHL.LIB (Windows dependent)
.millust end
.do end
.* WCC386 ------------------------------------------------------
.np
.us 32-bit libraries:
.ix 'C libraries' 'flat'
.ix 'flat model' 'libraries'
.ix 'C libraries' 'small'
.ix 'small model' 'libraries'
.if '&target' eq 'QNX' .do begin
.ix 'math3r.lib'
.ix 'math3s.lib'
.millust begin
math3r.lib (flat/small models, "3r", "4r", "5r" or "6r" option)
math3s.lib (flat/small models, "3s", "4s", "5s" or "6s" option)
.millust end
.do end
.el .do begin
.ix 'MATH3R.LIB'
.ix 'MATH3S.LIB'
.millust begin
MATH3R.LIB (flat/small models, "3r", "4r", "5r" or "6r" option)
MATH3S.LIB (flat/small models, "3s", "4s", "5s" or "6s" option)
.millust end
.do end
.* ------------------------------------------------------
.np
.ix 'numeric data processor'
.ix 'math coprocessor'
Applications which are linked with one of these libraries do not
require a numeric data processor for floating-point operations.
If one is present in the system, it will be used; otherwise
floating-point operations are simulated in software.
.if '&target' ne 'QNX' .do begin
The numeric data processor will not be used if the environment
variable
.ev NO87
has been set (this variable is described below).
.do end
.*
.if '&target' ne 'QNX' .do begin
.*
.section *refid=libno87 The NO87 Environment Variable
.*
.np
If you have a numeric data processor (math coprocessor) in your system
but you wish to test a version of your application that will use
floating-point emulation ("fpi" option) or simulation ("fpc" option),
you can define the
.ev NO87
environment variable.
.ix 'options' 'fpc'
.ix 'options' 'fpi'
.np
(16-bit only)
.ix 'emu87.lib'
The application must be compiled using the "fpc" (floating-point
calls) option and linked with the appropriate
.fi math?.lib
library or the "fpi" option (default) and linked with the appropriate
.fi math87?.lib
and
.fi emu87.lib
libraries.
.np
(32-bit only)
The application must be compiled using the "fpc" (floating-point
calls) option and linked with the appropriate
.fi math3?.lib
library or the "fpi" option (default) and linked with the appropriate
.fi math387?.lib
library.
.np
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
.do end
.*
.*
.section *refid=liblfn The LFN Environment Variable
.*
.np
If you have the application compiled and linked to use DOS Long file name
support (LFN) then you can define the
.ev LFN
environment variable to suppress usage of DOS LFN on run-time.
.np
.ix '&setcmdup' 'LFN environment variable'
Using the "&setcmdup" command, define the environment variable
as follows:
.millust begin
&prompt.&setcmdup LFN=N
.millust end
.pc
Now, when you run your application, DOS LFN support will be ignored.
To undefine the environment variable, enter the command:
.millust begin
&prompt.&setcmdup LFN=
.millust end
.*
.section *refid=cplibrt The &product Run-time Initialization Routines
.*
.np
.ix 'run-time initialization'
Source files are included in the package for the &product application
startup (or initialization) sequence.
.* WCC ---------------------------------------------------------
.np
(16-bit only)
.if '&target' eq 'QNX' .do begin
These files are located in the directory:
.millust begin
/usr/lib/src/startup (QNX initialization)
.millust end
.np
The following is a summary list of the startup files for QNX.
.ix 'cstart_s.asm'
.ix 'cstart_m.asm'
.ix 'cstart_c.asm'
.ix 'cstart_l.asm'
.ix 'cstart_h.asm'
.ix 'cstart.asm'
.ix 'models.inc'
.ix 'cmain.c'
.ix 'mdef.inc'
.millust begin
cstart_s.asm  (startup for small memory model)
cstart_m.asm  (startup for medium memory model)
cstart_c.asm  (startup for compact memory model)
cstart_l.asm  (startup for large memory model)
cstart_h.asm  (startup for huge memory model)
models.inc    (included by cstart_*.asm)
cstart.asm    (included by cstart_*.asm)
mdef.inc      (macros included by cstart.asm)
cmain.c       (final part of initialization sequence)
.millust end
.pc
.ix 'cstart.asm'
.ix 'cmain.c'
The assembler file
.fi cstart.asm
contains the first part of the initialization code and the remainder
is continued in the file
.fi cmain.c.
.ix 'cstart_*.asm'
The assembler files,
.fi cstart_*.asm
.ct , define the type of memory model and include
.fi cstart.asm.
It is
.fi cmain.c
that calls your mainline routine
.id (main).
.do end
.el .do begin
.ix 'DOS' 'initialization'
.ix 'DOS/16M' 'initialization'
.ix 'Windows' 'initialization'
.ix 'OS/2' 'initialization'
.ix 'initialization' 'DOS'
.ix 'initialization' 'DOS/16M'
.ix 'initialization' 'Windows'
.ix 'initialization' 'OS/2'
The initialization code directories/files are listed below:
.np
.ix 'WILDARGV.C'
.ix '8087CW.C'
.cp 8
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP
.millust begin
WILDARGV.C    (wild card processing for argv)
8087CW.C      (value loaded into 80x87 control word)
.millust end
.np
.ix 'CSTRT086.ASM'
.ix 'DOS16M.ASM'
.ix 'CMAIN086.C'
.ix 'MDEF.INC'
.cp 15
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP&pc.DOS
(DOS initialization)
.millust begin
CSTRT086.ASM  (startup for 16-bit apps)
DOS16M.ASM    (startup code for Tenberry Software's DOS/16M)
CMAIN086.C    (final part of initialization sequence)
MDEF.INC      (macros included by assembly code)
.millust end
.keep
.np
.ix 'CSTRTW16.ASM'
.ix 'LIBENTRY.ASM'
.ix 'MDEF.INC'
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP&pc.WIN
(Windows initialization)
.millust begin
CSTRTW16.ASM  (startup for 16-bit Windows apps)
LIBENTRY.ASM  (startup for 16-bit Windows DLLs)
MDEF.INC      (macros included by assembly code)
.millust end
.np
.ix 'CMAIN086.C'
.ix 'MAINO16.C'
.ix 'CSTRTO16.ASM'
.ix 'EXITWMSG.H'
.ix 'WOS2.H'
.ix 'INITFINI.H'
.ix 'MDEF.INC'
.cp 13
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP&pc.OS2
(OS/2 initialization)
.millust begin
CMAIN086.C    (final part of initialization sequence)
MAINO16.C     (middle part of initialization sequence)
CSTRTO16.ASM  (startup for 16-bit OS/2)
EXITWMSG.H    (header file required by MAINO16.C)
WOS2.H        (header file required by MAINO16.C)
INITFINI.H    (header file required by MAINO16.C)
MDEF.INC      (macros included by assembly code)
.millust end
.np
The following is a summary description of the startup files for DOS.
The startup files for Windows and OS/2 are similar.
.ix 'CSTRT086.ASM'
.ix 'CMAIN086.C'
The assembler file
.fi CSTRT086.ASM
contains the first part of the initialization code and the remainder
is continued in the file
.fi CMAIN086.C.
It is
.fi CMAIN086.C
that calls your main routine
.id (main).
.np
.ix 'Tenberry Software' 'DOS/16M'
.ix 'DOS/16M'
.ix 'DOS Extender' '286' 'Tenberry Software'
The
.fi DOS16M.ASM
file is a special version of the
.fi CSTRT086.ASM
file which is required when using the Tenberry Software, Inc. DOS/16M
286 DOS extender.
.do end
.* WCC386 ------------------------------------------------------
.np
(32-bit only)
.if '&target' eq 'QNX' .do begin
These files are located in the directory:
.millust begin
/usr/lib/src/startup (QNX initialization)
.millust end
.np
The following is a summary list of the startup files for QNX.
.ix 'cstrt386.asm'
.ix 'cmain.c'
.ix 'mdef.inc'
.millust begin
cstrt386.asm  (startup for small memory model)
mdef.inc      (macros included by cstrt386.asm)
cmain.c       (final part of initialization sequence)
.millust end
.pc
.ix 'cstrt386.asm'
.ix 'cmain.c'
The assembler file
.fi cstrt386.asm
contains the first part of the initialization code and the remainder
is continued in the file
.fi cmain.c.
It is
.fi cmain.c
that calls your mainline routine
.id (main).
.do end
.el .do begin
The initialization code directories/files are listed below:
.np
.cp 8
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP
.ix 'WILDARGV.C'
.ix '8087CW.C'
.millust begin
WILDARGV.C   (wild card processing for argv)
8087CW.C     (value loaded into 80x87 control word)
.millust end
.np
.cp 8
.us Under
.fi &pathnamup&pc.SRC&pc.STARTUP&pc.386
.ix 'CSTRT386.ASM'
.ix 'CSTRTW32.ASM'
.ix 'CSTRTX32.ASM'
.ix 'CMAIN386.C'
.millust begin
CSTRT386.ASM (startup for most DOS Extenders)
CSTRTW32.ASM (startup for 32-bit Windows)
CSTRTX32.ASM (startup for FlashTek DOS Extender)
CMAIN386.C   (final part of initialization sequence)
.millust end
.np
.ix 'CMAIN386.C'
The assembler files
.fi CSTRT*.ASM
contain the first part of the initialization code and the remainder is
continued in the file
.fi CMAIN386.C.
It is
.fi CMAIN386.C
that calls your main routine
.id (main).
.do end
.* ------------------------------------------------------
.np
The source code is provided for those who wish to customize the
initialization sequence for special applications.
.if '&target' ne 'QNX' .do begin
.np
The file
.fi wildargv.c
contains an alternate form of "argv" processing in which wild card
command line arguments are transformed into lists of matching file
names.
Wild card arguments are any arguments containing "*" or "?" characters
unless the argument is placed within quotes (").
Consider the following example in which we run an application called
"TOUCH" with the argument "*.c".
.millust begin
&prompt.touch *.c
.millust end
.pc
Suppose that the application was linked with the object code for the
file
.fi wildargv.c.
Suppose that the files
.fi ap1.c
.ct ,
.fi ap2.c
and
.fi ap3.c
are stored in the current directory.
The single argument "*.c" is transformed into a list of arguments such
that:
.millust begin
argc == 4
argv[1] points to "ap1.c"
argv[2] points to "ap2.c"
argv[3] points to "ap3.c"
.millust end
.pc
The source file
.fi wildargv.c
must be compiled to produce the object file
.fi wildargv&obj..
This file must be specified before the &product libraries in the
linker command file in order to replace the standard "argv"
processing.
.do end
