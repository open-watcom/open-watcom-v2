.*
.*
.chap Overview
.*
.*
.np
CauseWay is a 386 DOS extender package for use with &product. C/C++
programs.  It is provided as a stub executable for which can be easily 
linked into DOS extended applications.
.*
.section Introduction
.*
.np
Within the standard DOS, Windows and OS/2 DOS box environments,
CauseWay supports 32-bit memory models for applications on PC
compatibles with an 80386SX processor or above without the need to use
overlays or crude stopgap measures such as EMS/XMS swapping.  To do
this, the DOS extender runs applications in protected mode, rather than
the real mode normally used in the DOS environments.  CauseWay supports
both 16-bit and 32-bit protected mode applications operating under a DOS
environment.  It makes full use of 386-level chip capabilities
including demand paging of code and data, variable-sized segments up to
4GB in length, mixing 16- and 32-bit segments as well as support for flat
(non-segmented) memory addressing models.  The CauseWay implementation
of these powerful capabilities provides all their benefits while being
transparent to the application user.
.np
.ix 'VCPI'
.ix 'DPMI'
Applications created using CauseWay are compatible with the VCPI and
DPMI standards and run equally well on systems with no protected mode
drivers or programs. CauseWay applications work with such diverse
environments as normal DOS, DesqView, Windows 3.0 and above in both
standard and enhanced modes, as well as DOS windows within OS/2 2.0 and
above, Windows 95 and above and Windows NT and later. CauseWay allocates
memory from DPMI, VCPI, XMS, and INT 15H
services, in addition to conventional DOS memory.  This allows CauseWay
applications to allocate memory through the CauseWay DOS extender
without the need to detect or manipulate the various memory handling
schemes.
.np
A primary objective of CauseWay development was to ensure minimal effort
would be needed by programmers to adapt their code to work with the
CauseWay DOS e
tender.  As a result, most &product. C/C++ and many realmode
assembly language programs need no or minor changes to produce a fully
operational CauseWay protected mode application.
.*
.section Minimum System Requirements
.*
.np
CauseWay for &product C/C++ requires a 386SX based
computer or better.  The required operating environment is MS-DOS or
PC-DOS 3.3 or higher, Windows 3.0 or higher, OS/2 2.0 and above,
Windows 95 and above, Windows NT or higher or a
compatible operating system that provides a DPMI or VCPI DOS
environment.
.np
CauseWay is to a large extent compatible with Tenberry Software's DOS/4GW.
Most applications built for DOS/4GW will run with CauseWay without any 
changes.
.*
.section CauseWay Memory Requirements
.*
.np
The recommended minimum amount of total free physical memory for
CauseWay applications is 500KB total.  100-150KB of this memory must be
conventional DOS memory, the remainder may be extended memory.  CauseWay
applications can run in less memory, down to the 300KB range, provided
sufficient virtual (disk-based) memory is available, but application
performance will decline significantly.  More physical memory improves a
program's performance, reducing virtual memory disk access overhead.
.*
.section Operating CauseWay
.*
.np
When using CauseWay, simply follow the standard
edit-compile/assemble-link programming cycle familiar to C and assembly
language programmers.
.np
Users compiling with WCL386 need to add the switch &sw.l=CauseWay
to the command line.
.exam begin
WCL386 &sw.l=CauseWay myprog.c
.exam end
This switch can be automated by adding &sw.l#CauseWay to the 
.ev WCL386
environment variable, making CauseWay the default when compiling via WCL386.
.np
&product. users linking with WLINK should add the statement
.mono system CauseWay
to the link command.  
.exam begin
WLINK system CauseWay file myprog.obj
.exam end
.np
.ix 'conventional memory'
When running the DOS-extended application, DOS first loads the CauseWay
DOS extender in conventional memory.  CauseWay establishes the protected
mode environment, retrieves the application from the executable file &mdash
loading it first into extended memory, then conventional memory if
extended is exhausted, then virtual (disk-based) memory if conventional
is exhausted &mdash sets up the application for execution, and finally
passes control to the application to begin operation.  No additional
files are required to make your application run in 386 protected mode
using the CauseWay DOS extender.
.np
.fi CWSTUB.EXE
will execute stand-alone LE-format files in the same
way as 
.fi DOS4GW.EXE
does if the full file name, including extension, is
listed after CWSTUB, e.g. 
.mono CWSTUB RUNME.EXE. 
CWSTUB will override the extender bound to the application EXE, if any, 
with the CauseWay DOS extender version in 
.fi CWSTUB.EXE.
.*
.section Debugging Using WD
.*
.np
.ix 'WD'
To debug CauseWay programs with the &product. debugger after installing the
CauseWay files, simply use the &sw.tr=cw command line option.
.exam begin
WD &sw.tr=cw myprog
.exam end
This process can be automated by adding 
.mono &sw.tr#cw 
to your 
.ev WD 
environment variable. Use the 
.mono set WD=tr#cw
command.
.np
By default, CauseWay uses a Ctrl-Alt keypress to interrupt the WD
debugger, rather than the Print-Screen key.  This can be changed to any
two, three, or four keypress value by modifying the ASCII file
.fi CWHELP.CFG 
at the BreakKeys line.  See comments in this file for
further detail.  Note that a single keypress value will not work properly.
.*
.section Operational Considerations When Using CauseWay
.*
.np
.ix 'virtual memory'
The 
.ev TEMP
, 
.ev TMP
and 
.ev CAUSEWAY=SWAP 
environment variables are used by
CauseWay to determine where to build its virtual memory swap file when
an application is not operating under Windows or OS/2 (Windows and OS/2
provide their own virtual memory management).  Since CauseWay has integrated
virtual memory, disk space is considered part of total memory.  If you
use the 
.ev TEMP
, 
.ev TMP
or 
.ev SWAP 
environment variable to point to a small RAM
sk or almost full disk, free memory will be affected accordingly.  If
virtual (disk-based) memory is less than physical (installed on machine)
memory, Cause Way turns off virtual memory. On the other hand, if you
have a disk 300MB free, CauseWay will have no problem reporting 300MB
free memory to your program, provided that virtual memory is not
inhibited or limited by the 
.ev CAUSEWAY
environment variable memory settings.
.np
Memory operates differently under Windows and OS/2. With OS/2, the DPMI
setting for the session determines available memory. With Windows,
available memory is the total of physical memory plus the swap file size
less any memory already in use by Windows or another Windows application.
.np
.ix 'swap file'
When creating a VMM swap file at application startup under DOS, CauseWay
builds a list of possible paths in order of priority. CauseWay then works
through the list until one of the entries provides both a valid drive
and path specification and sufficient free space to being operation. The
first entry to succeed becomes the swap file drive with no further
processing of the list. If CauseWay reaches the end of the list without
finding a valid drive, it disables the virtual memory manager. The order
of priority is 
.ev CAUSEWAY=SWAP
, 
.ev TEMP
, 
.ev TMP
and application execution path.
.np
If end users reboot the system or turn off power while executing a
CauseWay application under DOS, a temporary file will be left on the
system by CauseWay. This will usually be a zero length file unless the
application was large enough to exceed physical memory and CauseWay had
started using its virtual memory manager. The temporary file name is
requested using standard DOS functions, meaning the name will vary with
different versions of DOS. It typically is a mixture of letters and
numbers with no extension, although .$$$ extension may be presented when
operating under a network. Make sure you do not delete this temporary
file while the CauseWay application is still active, as improper or
erratic program operation, including lock-ups, may occur.
.np
Application startup times may increase significantly if the free
physical memory is less than the executable size. In such cases, not
only must the executable be loaded into physical memory, but a virtual
memory file of the executable file size must also be built. This file
holds the portions of the executable that do not fit into physical
memory and which have not been recently requested. After startup is
complete, the program will operate normally, paging to and from virtual
memory as necessary.
.np
CauseWay automatically sets aside 32KB of low DOS memory for allocation
and use by developer routines via the GetMemDOS API function. The 32K
memory block is available even if CauseWay needs to use virtual memory
just to load an application. The set-aside amount can be increased by
using the 
.ev CAUSEWAY 
environment variable LOWMEM option, although the
additional set-aside goal is not guaranteed to be reached if too little
conventional memory is left for CauseWay's operating requirements.
.*
.section Environment Variables
.*
.np
CauseWay can make use of three environment variables at runtime:
.ev TEMP
, 
.ev TMP
and 
.ev CAUSEWAY
.*
.beglevel
.*
.section TEMP and TMP Environment Variables
.np
The 
.ev TEMP 
and 
.ev TMP 
environment variables specify the directory and drive
where a swap file is built by CauseWay's virtual memory manager (VMM)
when operating under DOS. Windows and OS/2 provide their own memory
management functions which override CauseWay's use of the 
.ev TEMP 
and 
.ev TMP
environment variables. The path indicated by 
.ev TEMP 
will be used under DOS
if both 
.ev TEMP
and 
.ev TMP 
environment variables exist. Both settings are
superseded by the 
.ev CAUSEWAY=SWAP 
environment variable setting.
.millust begin
SET TMP=C:\SWAP
.millust end
The example above directs the CauseWay DOS extender to create its swap
file, if any, in the C:\SWAP directory.
.np
If no 
.ev TEMP
, 
.ev TMP 
and 
.ev CAUSEWAY=SWAP 
settings are present or are invalid,
the current drive is used when creating a swap file.  If free drive
space is less than physical memory (extended and conventional) available
at startup, then the DOS extender VMM is disabled, no swap file is
created, and virtual memory is not available to the application.
.*
.section CAUSEWAY Environment Variable
.np
The 
.ev CAUSEWAY 
environment variable controls operation of the DOS extender
at application runtime.  Eleven (11) options are supported, although
they are ignored in a Windows or OS/2 DPMI environment.  Use any
combination of the options in the following format:
.millust begin
SET CAUSEWAY=[<setting_1>;][<setting_2>;][<setting_n>;]
.millust end
Items in square brackets ([ ]) are optional. Do not actually type the 
brackets if you use the optional items.  Items in brackets (< >) should 
be replaced with actual values, separated by semicolons.  Following is 
a description of the valid settings:
.np
.begnote
.note BIG1
.br
.ix 'BIG1 setting'
.ix 'DPMI host'
.ix 'INT 15h'
Force CauseWay to use an alternate method to
determine available extended memory under RAW memory environments (no
DPMI host, no HIMEM.SYS loaded), allowing CauseWay to see more than 64MB
of memory on machines which do not support more than 64MB under original
INT 15h method. This method uses INT 15h function 0e801h to determine
available extended memory, falling back to the original function if
0e801h fails. Note that old machines may not support this function and
there is a slight chance that some older machines may not work if this 
setting is used.
.*
.note DPMI
.br
.ix 'DPMI setting'
.ix 'DPMI'
Force DPMI rather than default VCPI usage
whenever possible (recommended for 386Max and BlueMax users).  The
memory manager must support DPMI or else this setting is ignored.
.*
.note EXTALL
.br
.ix 'EXTALL setting'
Force CauseWay to use all extended memory and
sub-allocate memory from the bottom up instead of the default top-down
approach.  This setting is most useful for processor intensive
environments which have a small hardware cache that does not cover the 
entire physical address range.  Use of this setting
means that no extended memory will be available for other programs while
the application is loaded (including shelling to DOS).
.*
.note HIMEM:<nnn>
.br
.ix 'HIMEM setting'
Set maximum physical (conventional plus extended)
memory that can be consumed by CauseWay where 
.mono <nnn> 
is the decimal number of
kilobytes that can be consumed.  If memory allocation requests exceed
this figure, CauseWay will use virtual memory, even if additional
physical memory is present.  If the HIMEM memory value exceeds available
physical memory, then memory allocations operate normally. For example,
HIMEM:2048 on a 4MB machine would force virtual memory use after 2MB of
memory allocations (including loading the executable file). The
remaining 2MB of memory could be used by other applications while the
CauseWay application is active.
.*
.note LOWMEM:<nnn>
.br
.ix 'LOWMEM setting'
.ix 'conventional memory'
Set DOS (conventional) memory to restrict it from use by
CauseWay.  This memory is in addition to the default 32KB low DOS memory
block reserved by CauseWay for use by any applications which need to
allocate DOS memory.  
.mono <nnn> 
is the decimal number of kilobytes to
reserve.  If there is not enough conventional memory to satisfy the
.mono <nnn> 
request value, then CauseWay will leave all conventional memory
free that is not required by the extender to operate. Note that this
option does not guarantee the amount of free DOS memory, just how much
needs to be free before CauseWay will consume DOS memory after
exhausting all extended memory.  For example, LOWMEM:200 will attempt to
reserve 200KB of DOS memory, even if CauseWay has exhausted all extended
memory and is using conventional memory to fill memory allocation requests.
.*
.note MAXMEM:<nn>
.br
.ix 'MAXMEM setting'
Set maximum linear address space provided by CauseWay where
.mono <nn> 
is the decimal number of megabytes of linear address space. This
setting is similar to HIMEM except that it includes any virtual memory.
For example, MAXMEM:32 on a 16MB memory system restricts VMM disk space
usage to 32MB, even if more disk space is present.  MAXMEM:8 on the same
system would restrict the application to 8MB of memory (all physical).
Note that the setting is in megabytes, rather than kilobytes used in the
LOWMEM and HIMEM options.
.*
.note NAME:<filename>
.br
.ix 'NAME setting'
Set a name, without a
pathspec, to use the virtual memory temporary swap file.  To set a path
for the swap file, use the 
.ev CAUSEWAY=SWAP
,
.ev TEMP
, or 
.ev TMP
environment variable. The filename
must be valid, 12 characters or less.  Additional characters are
truncated or invalidate the filename, depending upon how DOS handles it
(e.g., multiple periods make an invalid file name whereas a
five-character extension is truncated to three).  If the filename
specified is invalid, CauseWay shuts off virtual memory.  It makes no
further attempts for a temporary file name.  If a pre-existing file name
is specified, CauseWay overwrites the file.
.np
In conjunction with the PRE
setting, the NAME setting can be a very powerful tool.  Not only can no
clusters be lost due to reset/reboot, but the leftover temporary file
can be forced to a known name and location.  Erase the swap file prior
to running the application or leave it as a "permanent" swap file for
CauseWay.
.np
Note: In a multi-user or
muti-CauseWay application situation, do not use the NAME setting unless
it generates a unique file for each user and application.  Otherwise,
applications will be stepping on others' temporary files.
.*
.note NOEX
.br
.ix 'NOEX setting'
Force CauseWay to not patch the INT 21h, function
4bh (EXEC) vector to turn off CauseWay's INT 31h extensions when the
EXEC function is called. CauseWay normally turns off support of its INT
31h extensions with an EXEC call to be well behaved and avoid conflicts
with other extenders or programs which may add their own extensions to
INT 31h. However, if your CauseWay extended application shells out to
DOS and passes the shelled-to application a callback address pointing to
a routine within the parent CauseWay application, the callback will not
work properly if the protected mode code uses the CauseWay extensions.
With the NOEX setting present, CauseWay still supports its INT 31h
extensions for those users who need to operate with callbacks in this
fashion. Be aware that when the NOEX setting is present, CauseWay is
less "well-behaved" about other programs which might add their own INT
31h extensions.
.*
.note NOVM
.br
.ix 'NOVM setting'
.ix 'virtual memory'
Disable all virtual memory use by CauseWay.  If
physical memory is exhausted, CauseWay will fail  further memory
allocation requests.
.*
.note PRE:<nnn>
.br
.ix 'PRE setting'
Pre-allocates a swap file size, under non-DPMI
environments, at start-up, where
.mono <nnn> 
is file size in megabytes, not kilobytes (same as MAXMEM).
.np
There are at least two uses for this
feature.  First, to pre-allocate a virtual memory file size for
applications with a total memory allocation (including EXE image) that
does not exceed the set size.  For example:
.millust begin
SET CAUSEWAY=PRE:4
.millust end
pre-allocates a virtual memory file of
4MB.  If an enduser resets or powers off the computer while the
application is running and virtual memory is in use, the enduser's
machine will not have lost clusters.  There is only a 4MB temporary file
to find and erase.  If virtual memory usage exceeds 4MB, then SCANDISK
must be used to recover lost clusters above and beyond what was
pre-allocated.
.np
Secondly, PRE can be used to allow your
application to stake a claim to disk space before it needs it.
.np
PRE may be used in conjunction with MAXMEM
to ensure that virtual memory does not exceed the pre-allocation setting.
.*
.note SWAP:<path>
.br
.ix 'SWAP setting'
.ix 'swap file'
Set CauseWay's virtual memory manager swap file
path.  This path takes precedence for choosing the location of a swap
file over the 
.ev TEMP 
and 
.ev TMP 
environment variables.
.endnote
.endlevel
