.*
.*
.np
This chapter deals specifically with aspects of &opsys executable
files.
.if '&exeformat' eq 'dos' .do begin
.ix 'DOS applications' 'creating'
.ix 'applications' 'creating for DOS'
The DOS executable file format will only run under the DOS operating
system.
.do end
.if '&exeformat' eq 'zdos' .do begin
.ix 'ZDOS applications' 'creating'
.ix 'applications' 'creating for ZDOS'
The ZDOS executable file format will only run under the ZDOS operating
system.
.do end
.if '&exeformat' eq 'elf' .do begin
.ix 'ELF applications' 'creating'
.ix 'applications' 'creating for ELF'
The ELF executable file format will only run under the operating
systems that support the ELF executable file format.
.do end
.if '&exeformat' eq 'win' .do begin
.ix 'Win16 applications' 'creating'
.ix 'Windows 3.x applications' 'creating'
.ix 'applications' 'creating for Windows 3.x'
The Win16 executable file format will run under Windows 3.x,
Windows 95, and Windows NT.
.do end
.if '&exeformat' eq 'win nt' .do begin
.ix 'Win32 applications' 'creating'
.ix 'Windows 32-bit applications' 'creating'
.ix 'Windows NT applications' 'creating'
.ix 'applications' 'creating for Win32'
.ix 'applications' 'creating for 32-bit Windows'
.ix 'applications' 'creating for Windows NT'
The Win32 executable file format will run under Windows 95,
Windows NT, Phar Lap's TNT DOS extender and RDOS.
It may also run under Windows 3.x using the Win32S subsystem (you are
restricted to a subset of the Win32 API).
.do end
.if '&exeformat' eq 'os2' .do begin
.ix 'OS/2 16-bit applications' 'creating'
.ix 'applications' 'creating for 16-bit OS/2'
.ix 'Phar Lap 286|Dos-Extender applications' 'creating'
.ix 'applications' 'creating for Phar Lap 286|Dos-Extender'
The OS/2 16-bit executable file format will run under the following
operating systems.
.autopoint
.point
16-bit OS/2 1.x
.point
32-bit OS/2 2.x, 3.x (Warp) and 4.x
.point
Phar Lap's 286|DOS-Extender
.endpoint
.np
.ix 'OS/2 32-bit applications' 'creating'
.ix 'applications' 'creating for 32-bit OS/2'
.ix 'CauseWay applications' 'creating'
.ix 'applications' 'creating for CauseWay'
.ix 'DOS/4G applications' 'creating'
.ix 'applications' 'creating for DOS/4G'
.ix 'FlashTek applications' 'creating'
.ix 'applications' 'creating for FlashTek'
The OS/2 32-bit linear executable file format will run under the following
operating systems.
.autopoint
.point
OS/2 2.x and later (LX format only)
.point
CauseWay DOS extender, Tenberry Software's DOS/4G and DOS/4GW DOS
extenders, and compatible products (LE format only)
.point
FlashTek's DOS Extender (LX format only)
.endpoint
.do end
.if '&exeformat' eq 'phar' .do begin
.ix 'Phar Lap 386|Dos-Extender applications' 'creating'
.ix 'applications' 'creating for Phar Lap 386|Dos-Extender'
The Phar Lap executable file format will run under the following
operating systems.
.autopoint
.point
Phar Lap's 386|DOS-Extender
.point
&company's 32-bit Windows supervisor (relocatable format only)
.endpoint
.do end
.if '&exeformat' eq 'nov' .do begin
.ix 'NetWare applications' 'creating'
.ix 'applications' 'creating for NetWare'
The Novell NetWare executable file format will only run under
NetWare operating systems.
.do end
.if '&exeformat' eq 'qnx' .do begin
.ix 'QNX applications' 'creating'
.ix 'applications' 'creating for QNX'
The QNX executable file format will only run under the QNX operating
system.
.do end
