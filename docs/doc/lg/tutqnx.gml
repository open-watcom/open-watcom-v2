.*
.*
This guide describes only the QNX executable file format.
.np
.ix '&lnkcmdup' 'command line format'
.ix '&lnkcmdup command line' 'invoking &lnkcmdup'
.ix 'command line format' '&lnkcmdup'
.ix 'invoking &lnkname'
The &lnkname command line format is as follows.
.mbigbox
&lnkcmdup {directive}
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
.note com
16-bit x86 DOS ".COM" executable
.note dos
16-bit x86 DOS executable
.note dos4g
32-bit x86 DOS/4GW executable
.note dos4gnz
non-zero based 32-bit x86 DOS/4GW executable
.note netware
32-bit x86 NetWare Loadable Module
.note novell
synonym for "netware"
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
32-bit x86 Phar Lap TNT dos style executable
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
.section Linking 16-bit QNX Executable Files
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
.section Linking 32-bit QNX Executable Files
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
