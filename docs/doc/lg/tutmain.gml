.*
.*
.im dosfiles
.*
.chap *refid=tutchap Linking Executable Files for Various Systems
.*
.np
.ix '&lnkcmdup' 'command line format'
.ix '&lnkcmdup command line' 'invoking &lnkcmdup'
.ix 'command line format' '&lnkcmdup'
.ix 'invoking &lnkname'
The &lnkname command line format is as follows.
.mbigbox
&lnkcmd {directive}
.embigbox
.np
where
.bi directive
is a series of &lnkname directives specified on the command line or in
one or more files.
If the directives are contained within a file, the "@" character is
used to reference that file.
If no file extension is specified, a file extension of "lnk" is
assumed.
.exam begin
&lnkcmd name testprog @first @second option map
.exam end
.np
In the above example, directives are specified on the command line
(e.g., "name testprog" and "option map") and in files (e.g.,
.fi first.lnk
and
.fi second.lnk
.ct ).
.*
.section Using the SYSTEM Directive
.*
.np
.ix 'SYSTEM directive'
For each executable file format that can be created using the
&lnkname, a specific SYSTEM directive may be used.
The SYSTEM directive selects a subset of the available directives
necessary to create each specific executable file format.
.begnote
.notehd1 System
.notehd2 Description
.note causeway
32-bit x86 CauseWay executable
.note cwdllr
32-bit x86 CauseWay Dynamic Link Library (register calling convention)
.note cwdlls
32-bit x86 CauseWay Dynamic Link Library (stack calling convention)
.note com
16-bit x86 DOS ".COM" executable
.note dos
16-bit x86 DOS executable
.note dos4g
32-bit x86 DOS/4GW executable
.note dos4gnz
non-zero based 32-bit x86 DOS/4GW executable
.note netware
32-bit x86 NetWare Loadable Module. Uses original Novell developer kit 
(NOVH + NOVI). This is a legacy system type. It is recommended to 
use one of the netware_clib or netware_libc system types instead.
.note novell
synonym for "netware". This is a legacy system type. It is recommended to 
use one of the netware_clib or netware_libc system types instead.
.note netware_libc
32-bit x86 NetWare Loadable Module. Targetted for Novells LibC based 
environment on NetWare 5 and later. Uses the full Open Watcom run-time 
library for NetWare.
.note netware_libc_lite
32-bit x86 NetWare Loadable Module. Targetted for Novells LibC based 
environment on NetWare 5 and later. Uses the thin Open Watcom run-time 
library support for NetWare and consumes C library functionality from
the server libraries.
.note netware_clib
32-bit x86 NetWare Loadable Module. Targetted for Novells traditional 
CLIB based environment on NetWare 3 and later. Uses the full Open Watcom 
run-time library for NetWare.
.note netware_clib_lite
32-bit x86 NetWare Loadable Module. Targetted for Novells traditional 
CLIB based environment on NetWare 3 and later. Uses the thin Open Watcom 
run-time library support for NetWare and consumes C library functionality 
from the server libraries.
.note os2
16-bit x86 OS/2 executable
.note os2_dll
16-bit x86 OS/2 Dynamic Link Library
.note os2_pm
16-bit x86 OS/2 Presentation Manager executable
.note os2v2
32-bit x86 OS/2 executable
.note os2v2_dll
32-bit x86 OS/2 Dynamic Link Library
.note os2v2_pm
32-bit x86 OS/2 Presentation Manager executable
.note pharlap
32-bit x86 Phar Lap executable
.note tnt
32-bit x86 Phar Lap TNT executable
.note rdos
32-bit x86 RDOS executable
.note rdos_dll
32-bit x86 RDOS Dynamic Link Library
.note qnx
16-bit x86 QNX executable
.note qnx386
32-bit x86 QNX executable
.note x32r
32-bit x86 FlashTek executable using register-based calling conventions
.note x32rv
32-bit x86 virtual-memory FlashTek executable using register-based calling
conventions
.note x32s
32-bit x86 FlashTek executable using stack-based calling conventions
.note x32sv
32-bit x86 virtual-memory FlashTek executable using stack-based calling
conventions
.note windows
16-bit x86 Windows 3.x executable
.note windows_dll
16-bit x86 Windows 3.x Dynamic Link Library
.note win_vxd
32-bit x86 Windows 3.x or 9x Virtual Device Driver
.note win95
32-bit x86 Windows 9x executable
.note win95 dll
32-bit x86 Windows 9x Dynamic Link Library
.note nt
32-bit x86 Windows NT character-mode executable
.note nt_win
32-bit x86 Windows NT windowed executable
.note win32
synonym for "nt_win"
.note nt_dll
32-bit x86 Windows NT Dynamic Link Library
.if '&alpha' eq 'AXP' .do begin
.note ntaxp
AXP Windows NT character-mode executable
.note ntaxp_win
AXP Windows NT windowed executable
.note ntaxp_dll
AXP Windows NT Dynamic Link Library
.do end
.note win386
32-bit x86 &company extended Windows 3.x executable or Dynamic Link Library
.endnote
.np
The various systems that we have listed above are defined in special
linker directive files which are plain ASCII text files that you can
edit.
These files are called :FNAME.wlink.lnk:eFNAME.
and :FNAME.wlsystem.lnk:eFNAME..
.*
.im wlinklnk
.*
.np
In the following sections, we show some of the typical directives that
you might use to create a particular executable file format.
The common directives are described in the chapter
entitled :HDREF refid='genchap'..
They are "common" in the sense that they may be used with any
executable format.
There are other, less general, directives that may be specified for a
particular executable format.
In each of the following sections, we refer you to chapters in which
you will find more information on the directives available with the
executable format used.
.np
At this point, it should be noted that various systems have adopted
particular executable file formats. For example, the CauseWay DOS extender,
Tenberry Software DOS/4G(W) and FlashTek DOS extenders all
support one of the OS/2 executable file
formats. It is for this reason that you may find that we direct you to
a chapter which would, at first glance, seem unrelated to the
executable file format in which you are interested.
.np
To summarize, the steps that you should follow to learn about creating
a particular executable are:
.autonote
.note
Look for a section in this chapter that describes the executable
format in which you are interested.
.note
See the chapter entitled :HDREF refid='genchap'. for a description of
the common directives.
.note
If you require additional information, see also the chapter to which
we have referred you.
.note
Also check the
.book &company C/C++ Programmer's Guide
or
.book &cmpfname Programmer's Guide
for more information on creating specific types of applications.
.endnote
.*
.section Linking 16-bit x86 Executable Files
.*
.np
.ix '16-bit executables'
The following sections describe how to link a variety of 16-bit
executable files.
.*
.beglevel
.*
.section Linking 16-bit x86 DOS Executable Files
.*
.np
.ix '16-bit DOS executables'
To create this type of file, use the following structure.
.millust begin
system   dos
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='doschap'..
.*
.section Linking 16-bit x86 DOS .COM Executable Files
.*
.np
.ix '16-bit DOS .COM'
To create this type of file, use the following structure.
.millust begin
system   com
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='doschap'..
.*
.section Linking 16-bit x86 OS/2 Executable Files
.*
.np
.ix '16-bit OS/2 executables'
To create this type of file, use the following structure.
.millust begin
system   os2
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 16-bit x86 OS/2 Dynamic Link Libraries
.*
.np
.ix '16-bit OS/2 DLLs'
To create this type of file, use the following structure.
.millust begin
system   os2 dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 16-bit x86 QNX Executable Files
.*
.np
.ix '16-bit QNX executables'
To create this type of file, use the following structure.
.millust begin
system   qnx
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='qnxchap'..
.*
.section Linking 16-bit x86 Windows 3.x Executable Files
.*
.np
.ix '16-bit Windows 3.x executables'
To create this type of file, use the following structure.
.millust begin
system   windows
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='winchap'..
.*
.section Linking 16-bit x86 Windows 3.x Dynamic Link Libraries
.*
.np
.ix '16-bit Windows 3.x DLLs'
To create this type of file, use the following structure.
.millust begin
system   windows_dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='winchap'..
.*
.endlevel
.*
.section Linking 32-bit x86 Executable Files
.*
.np
.ix '32-bit executables'
The following sections describe how to create a variety of 32-bit
executable files.
.*
.beglevel
.*
.section Linking 32-bit x86 CauseWay Executable Files
.*
.np
.ix '32-bit CauseWay executables'
To create this type of file, use the following structure.
.millust begin
system   causeway
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 CauseWay Dynamic Link Libraries
.*
.np
.ix '32-bit CauseWay DLL'
To create this type of file, use the following structure.
.millust begin
system   cwdllr or cwdlls
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 DOS/4GW Executable Files
.*
.np
.ix '32-bit DOS/4GW executables'
To create this type of file, use the following structure.
.millust begin
system   dos4g
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 FlashTek Executable Files
.*
.np
.ix '32-bit FlashTek executables'
To create these files, use one of the following structures.
.millust begin
system   x32r
.im tutsteps
.millust end
.np
If the system is
.kw x32r
.ct , a FlashTek executable file is created for an application using the
register calling convention.
.millust begin
system   x32rv
.im tutsteps
.millust end
.np
If the system is
.kw x32rv
.ct , a virtual-memory FlashTek executable file is created for an
application using the register calling convention.
.millust begin
system   x32s
.im tutsteps
.millust end
.np
If the system is
.bd x32s
.ct , a FlashTek executable file is created for an application using the
stack calling convention.
.millust begin
system   x32sv
.im tutsteps
.millust end
.np
If the system is
.bd x32sv
.ct , a virtual-memory FlashTek executable file is created for an
application using the stack calling convention.
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 Novell NetWare Loadable Modules
.*
.np
.ix '32-bit Netware NLMs'
To create this type of file, use the following structure.
.millust begin
system   netware_(clib|libc)[_lite]
.im tutsteps
module   mod_name
.millust end
For more information, see the chapter entitled 
:HDREF refid='novchap'..
.*
.section Linking 32-bit x86 OS/2 Executable Files
.*
.np
.ix '32-bit OS/2 executables'
To create this type of file, use the following structure.
.millust begin
system   os2v2
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 OS/2 Dynamic Link Libraries
.*
.np
.ix '32-bit OS/2 DLLs'
To create this type of file, use the following structure.
.millust begin
system   os2v2 dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 OS/2 Presentation Manager Executable Files
.*
.np
.ix '32-bit OS/2 PM executables'
To create this type of file, use the following structure.
.millust begin
system   os2v2_pm
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='os2chap'..
.*
.section Linking 32-bit x86 Phar Lap Executable Files
.*
.np
.ix '32-bit Phar Lap executables'
To create this type of file, use the following structure.
.millust begin
system   pharlap
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='phrchap'..
.*
.section Linking 32-bit x86 Phar Lap TNT Executable Files
.*
.np
.ix '32-bit TNT executables'
To create this type of file, use the following structure.
.millust begin
system   tnt
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 RDOS Executable Files
.*
.np
.ix '32-bit RDOS executables'
To create this type of file, use the following structure.
.millust begin
system   rdos
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 RDOS Dynamic Link Libraries
.*
.np
.ix '32-bit RDOS DLLs'
To create this type of file, use the following structure.
.millust begin
system   rdos_dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 QNX Executable Files
.*
.np
.ix '32-bit QNX executables'
To create this type of file, use the following structure.
.millust begin
system   qnx386
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='qnxchap'..
.*
.section Linking 32-bit x86 Extended Windows 3.x Executable
.*
.np
.ix '32-bit Windows 3.x executables'
To create this type of file, use the following structure.
.millust begin
system   win386
.im tutsteps
.millust end
.np
After linking this executable, you must bind the &company 32-bit
Windows-extender to the executable (a
.fi .REX
file)
to produce a Windows executable (a
.fi .EXE
file).
.millust begin
wbind -n app_name
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='winchap'..
.*
.section Linking 32-bit x86 Extended Windows 3.x Dynamic Link Libraries
.*
.np
.ix '32-bit Windows 3.x DLLs'
To create this type of file, use the following structure.
.millust begin
system   win386
.im tutsteps
.millust end
.np
After linking this executable, you must bind the &company 32-bit
Windows-extender for DLLs to the executable (a
.fi .REX
file)
to produce a Windows Dynamic Link Library (a
.fi .DLL
file).
.millust begin
wbind -n -d app_name
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='winchap'..
.*
.section Linking 32-bit x86 Windows 3.x or 9x Virtual Device Driver
.*
.np
.ix '32-bit Windows VxD'
There are two type of the Virtual Device Driver.
.np
Staticaly loaded Virtual Device Driver used by Windows 3.x or 9x.
To create this type of file, use the following structure.
.millust begin
system   win_vxd
.im tutsteps
.millust end
.np
Dynamicaly loaded Virtual Device Driver used by Windows 3.11 or 9x.
To create this type of file, use the following structure.
.millust begin
system   win_vxd dynamic
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='vxdchap'..
.*
.section Linking 32-bit x86 Windows 95 Executable Files
.*
.np
.ix '32-bit Windows 95 executables'
To create this type of file, use the following structure.
.millust begin
system   win95
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 Windows 95 Dynamic Link Libraries
.*
.np
.ix '32-bit Windows 95 DLLs'
To create this type of file, use the following structure.
.millust begin
system   win95 dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 Windows NT Character-Mode Executable Files
.*
.np
.ix '32-bit Win NT character-mode executables'
To create this type of file, use the following structure.
.millust begin
system   nt
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 Windows NT Windowed Executable Files
.*
.np
.ix '32-bit Win NT windowed executables'
To create this type of file, use the following structure.
.millust begin
system   nt_win
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking 32-bit x86 Windows NT Dynamic Link Libraries
.*
.np
.ix '32-bit Win NT DLLs'
To create this type of file, use the following structure.
.millust begin
system   nt_dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.if '&alpha' eq 'AXP' .do begin
.*
.section Linking AXP Windows NT Character-Mode Executable Files
.*
.np
.ix 'AXP Win NT character-mode executables'
To create this type of file, use the following structure.
.millust begin
system   ntaxp
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking AXP Windows NT Windowed Executable Files
.*
.np
.ix 'AXP Win NT windowed executables'
To create this type of file, use the following structure.
.millust begin
system   ntaxp_win
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.section Linking AXP Windows NT Dynamic Link Libraries
.*
.np
.ix 'AXP Win NT DLLs'
To create this type of file, use the following structure.
.millust begin
system   ntaxp_dll
.im tutsteps
.millust end
.np
For more information,
see the chapter entitled :HDREF refid='ntchap'..
.*
.do end
.*
.endlevel
