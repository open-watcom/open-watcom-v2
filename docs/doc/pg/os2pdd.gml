.chap *refid=os2pdd Developing an OS/2 Physical Device Driver
.*
.np
.ix 'OS/2 physical device drivers'
.ix 'OS/2 PDD'
.ix 'physical device drivers'
.ix 'PDD'
In this chapter, we discuss the development of Physical Device Drivers
(PDD) for OS/2.
The tools used in the creation of the sample PDD are:
.begbull
.bull
the 16-bit &cmpcname compiler
.bull
the &asmname
.bull
the &makname utility
.endbull
.np
.ix 'HRTIMER.SYS'
The sample Physical Device Driver that we are going to build,
.fi HRTIMER.SYS,
provides access to a high resolution timer.
Additional sources of information on PDDs can be found in the
following:
.autonote
.note
.book OS/2 2.0 Technical Library - Physical Device Driver Reference
.note
.book Writing OS/2 2.1 Device Drivers in C
by Steve J. Mastrianni
.note
.book An OS/2 High Resolution Software Timer
by Derek Williams,
an article which appeared in the Fall 1991 issue of
.book IBM Personal Systems Developer
magazine.
The source code for this device driver was adapted from the magazine
article.
For detailed information on the way this device driver works, please
read that article.
.endnote
.np
.fi HRTIMER.SYS
is a 16-bit device driver which runs under OS/2 1.x and 2.x/3.x.
It has a resolution of 840 nanoseconds (i.e., 1 tick of the Intel
8253/8254 timer = 840 nanoseconds).
.np
Here are some notes on creating Physical Device Drivers using
&company software tools.
.autonote
.note
.ix 'DLL'
A Physical Device Driver is linked as a DLL.
.note
The first segment must be a data segment, the next a code segment.
.note
By default only the first two segments remain after initialization,
extra segments have to be marked IOPL.
.note
The assembler file,
.fi DEVSEGS.ASM,
defines the segment ordering.
.note
.mono #pragma dataseg
and
.mono #pragma codeseg
are used to get various pieces of code and data into the correct
segments.
.note
.ix '_HEADER'
The
.id _HEADER
segment contains the device header and must be at the beginning of the
data segment.
.note
.ix '_INITCODE'
.ix '_INITDATA'
The
.id _INITCODE
and
.id _INITDATA
segments are used to place initialization code and data at the end so
it can be discarded by OS/2.
.endnote
.np
To compile the source code for the 16-bit Physical Device Drivers, we
use the following options:
.begnote
.note &sw.bt=os2
build target is OS/2
.note &sw.ms
16-bit small code/small data model
.note &sw.5
Pentium optimizations (this is optional)
.note &sw.omi
inline math and intrinsic functions (this is optional)
.note &sw.s
no stack checking
.note &sw.zdp
DS is pegged to DGROUP
.note &sw.zff
FS floats, i.e. not fixed to a segment
.note &sw.zgf
GS floats, i.e. not fixed to a segment
.note &sw.zu
SS != DGROUP
.note &sw.zl
remove default library information
.endnote
.np
To link the object code for the 16-bit Physical Device Drivers, we
use the following options:
.begnote $setptnt 15
.note name hrtimer.sys
to name the executable file.
.note sys os2 dll initglobal
to link a 16-bit OS/2 DLL.
.ix 'INITGLOBAL'
Specifying INITGLOBAL will cause the initialization routine to be
called the first time the dynamic link library is loaded.
.note option map
to generate a map file.
.note option quiet
to minimize the number of linker informational messages.
.note lib os2
.ix 'OS2.LIB'
to include the 16-bit
.fi OS2.LIB
library file.
.note file ...
to include the component object files of the device driver.
.endnote
.np
The sample files used to create the Physical Device Driver and the
programs that use it are located in the
.fi &pathnamup.\SRC\OS2\PDD
directory.
The Physical Device Driver files are:
.begnote $setptnt 15
.note DEVSEGS.ASM
.ix 'segment ordering'
This small assembler file orders the segment definitions in the
executable file.
.millust begin
Data Segments   _HEADER
                CONST
                CONST2
                _DATA
                _BSS
                _INITDATA (discardable)
Code Segments   _TEXT
                _INITCODE (discardable)
.millust end
.note HEADER.C
.ix 'EXE header'
.ix 'device driver header'
.ix 'header' 'EXE'
.ix 'header' 'device driver'
The first thing that must follow the EXE Header is the Device
Driver Header.
.note STRATEGY.C
.ix 'Strategy routine'
This is the resident portion of the Strategy routine.
.note STRATINI.C
This is the discardable portion of the Strategy routine, the
initialization code and data.
.note HRTIMER.H
This file contains the definition of the timer "timestamp" structure.
.note HRDEV.H
.ix 'i8253 timer'
.ix 'timer' 'i8523'
This file contains definitions for the Intel 8253 hardware timer.
.note DEVHDR.H
.ix 'header' 'device driver'
This file contains definitions for the Device Driver Header structure
(see page 3-2, "Physical Device Driver Header" of PDD Reference).
.note DEVDEFS.H
This file provides type definitions.
.note DEVREQP.H
.ix 'request packets'
This file contains definitions for the Device Driver Request Packets.
.note DEVAUX.H
.ix 'DevHlp'
This file contains definitions for the Device Driver Help (DevHlp)
routines.
.endnote
.np
The demonstration program files are:
.begnote
.note HRTEST.C
This file is a sample C program that shows how to use the device
driver to calculate elapsed times.
It demonstrates how to open the device driver, read timestamps from it
and close it.
It factors in the overhead of the read and has a function that is used
to calculate elapsed time from a start and stop timestamp.
.note TIMER.C
This file is a sample C program that can be used to time other
applications.
It also uses the device driver.
.endnote
.np
To build the device driver and demonstration programs, set your
current directory to
.fi &pathnamup.\SRC\OS2\PDD
and type:
.millust begin
&makcmd
.millust end
.np
.ix 'CONFIG.SYS'
To install the device driver, put the following statement in your
.fi CONFIG.SYS
file.
.ix 'DEVICE='
.millust begin
DEVICE=&pathnamup.\SRC\OS2\PDD\HRTIMER.SYS
.millust end
.np
You must then reboot OS/2.
.np
To run the test program, use the following command-line:
.millust begin
HRTEST [milliseconds]
.millust end
.np
For
.mono [milliseconds],
you can enter any number (e.g., 2000 which is 2 seconds).
.np
.ix 'HRTEST.EXE'
.ix 'DosSleep'
.fi HRTEST.EXE
will issue a DosSleep for the amount of milliseconds specified or will
use a default if no command-line parameter is given.
It will get a timestamp from the device driver before and after the
DosSleep and will calculate the elapsed time of that sleep and display
the results.
It will do this continuously until Ctrl/C or Ctrl/Break is pressed.
.np
Keep in mind that DosSleep has a granularity of 32 milliseconds.
Any discrepancy between the number of milliseconds used for the
DosSleep and the elapsed time results from the timer are the fault of
this granularity, not a problem with the timer.
DosSleep is used solely as a convenient method of displaying the
capabilities of the driver.
.np
.ix 'TIMER.EXE'
.ix 'program timer'
To run the timer program, use the following command-line:
.millust begin
TIMER program_name [program_args]
.millust end
.np
For example, to time an OS/2 Directory command, issue the following
command.
.exam begin
timer cmd /c dir c:
.exam end
