.*
.*
.dirctv FORMAT
.*
.np
The "FORMAT" directive is used to specify the format of the executable
file that the &lnkname is to generate.
The format of the "FORMAT" directive (short form "FORM") is as follows.
.mbigbox
    FORMAT form

    form ::= DOS [COM]
                | ZDOS [SYS | HWD | FSD]
                | RAW [BIN | HEX]
                | WINDOWS [win_dll] [MEMORY] [FONT]
                | WINDOWS VXD [DYNAMIC]
                | WINDOWS NT [TNT | RDOS] [dll_attrs]
                | OS2 [os2_type] [dll_attrs | os2_attrs]
                | PHARLAP [EXTENDED | REX | SEGMENTED]
                | NOVELL [NLM | LAN | DSK | NAM | 'number'] 'description'
                | QNX [FLAT]
                | ELF [DLL]

    win_dll ::= DLL [INITGLOBAL | INITINSTANCE]

    dll_attrs ::= DLL [INITGLOBAL | INITINSTANCE]
                      [TERMINSTANCE | TERMGLOBAL]

    os2_type ::= FLAT | LE | LX

    os2_attrs ::= PM | PMCOMPATIBLE | FULLSCREEN
                        | PHYSDEVICE | VIRTDEVICE
.embigbox
.synote
.mnote DOS
(short form "D") tells the &lnkname to generate a DOS "EXE" file.
.np
The name of the executable file will have extension "exe".
If "COM" is specified, a DOS "COM" file will be generated in which case
the name of the executable file will have extension "com".
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
Not all programs can be generated in the "COM" format.
The following rules must be followed.
.autonote
.note
The program must consist of only one physical segment.
This implies that the size of the program (code and data) must be less
than 64k.
.note
The program must not contain any segment relocation.
A warning message will be issued by the &lnkname each time a segment
relocation is encountered.
.endnote
.np
A DOS "COM" file cannot contain debugging information.
If you wish to debug a DOS "COM" file, you must use the "SYMFILE"
option to instruct the &lnkname to place the debugging information in
a separate file.
.if '&target' ne 'QNX' .do begin
.np
For more information on DOS executable file formats,
see the chapter entitled :HDREF refid='doschap'..
.do end
.*
.mnote ZDOS
(short form "ZD") tells the &lnkname to generate a ZDOS "EXE" file.
.np
The name of the executable file will have extension "exe".
If "SYS", "HWD" or "FSD" is specified, a ZDOS driver file
will be generated in which case the name of the executable file will have
the extension "sys", "hwd" or "fsd".
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.if '&target' ne 'QNX' .do begin
.np
For more information on ZDOS executable file formats,
see the chapter entitled :HDREF refid='zdoschap'..
.do end
.*
.mnote RAW
(short form "R") tells the &lnkname to generate a RAW output file.
.np
If "HEX" is specified, a raw 32-bit output file in Intel Hex format with the extension
"hex" will be created. When "BIN" is specified or RAW is given without further specification,
a raw 32-bit image with the extension "bin" will be created.
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
A raw output file cannot contain debugging information. If you wish to debug a
raw file, you must use the "SYMFILE" option to instruct the &lnkname to place
the debugging information in a separate file.
.if '&target' ne 'QNX' .do begin
.np
For more information on RAW executable file formats,
see the chapter entitled :HDREF refid='rawchap'..
.do end
.*
.mnote WINDOWS
tells the &lnkname to generate a Win16 (16-bit Windows) executable
file.
.np
The name of the executable file will have extension "exe".
If "DLL" (short form "DL") is specified, a Dynamic Link Library will
be generated; the name of the executable file will also have extension
"exe".
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
Specifying "INITGLOBAL" (short form "INITG") will cause Windows to
call an initialization routine the first time the Dynamic Link Library
is loaded.
The "INITGLOBAL" option should be used with "OPTION ONEAUTODATA" (the
default for Dynamic Link Libraries).
If the "INITGLOBAL" option is used with "OPTION MANYAUTODATA", the
initialization code will be called once for the first data segment
allocated but not for subsequent allocations (this is generally not
desirable behaviour and will likely cause a program fault).
.np
Specifying "INITINSTANCE" (short form "INITI") will cause Windows to
call an initialization routine each time the Dynamic Link Library is
used by a process.
The "INITINSTANCE" option should be used with "OPTION MANYAUTODATA"
(the default for executable programs).
.np
In either case, the initialization routine is defined by the start
address.
If neither "INITGLOBAL" or "INITINSTANCE" is specified, "INITGLOBAL"
is assumed.
.np
Specifying "MEMORY" (short form "MEM") indicates that the application
will run in standard or enhanced mode.
If Windows 3.0 is running in standard and enhanced mode, and "MEMORY"
is not specified, a warning message will be issued.
The "MEMORY" specification was used in the transition from Windows 2.0
to Windows 3.0.
The "MEMORY" specification is ignored in Windows 3.1 or later.
.np
Specifying "FONT" (short form "FO") indicates that the
proportional-spaced system font can be used.
Otherwise, the old-style mono-spaced system font will be used.
The "FONT" specification was used in the transition from Windows 2.0
to Windows 3.0.
The "FONT" specification is ignored in Windows 3.1 or later.
.if '&target' ne 'QNX' .do begin
.np
For more information on Windows executable file formats,
see the chapter entitled :HDREF refid='winchap'..
.do end
.mnote WINDOWS VXD
.ix 'VxD format executable'
tells the &lnkname to generate a Windows VxD file (Virtual Device Driver).
.np
The name of the file will have extension "386". Note that this default
extension can be overridden by using the "NAME" directive to name
the driver file.
.np
Specifying "DYNAMIC" (short form "DYN") , dynamicaly loadable driver will
be generated (only for Windows 3.11 or 9x). By default the &lnkname
generate staticaly loadable driver (for Windows 3.x or 9x).
.if '&target' ne 'QNX' .do begin
.np
For more information on Windows Virtual Device Driver file format,
see the chapter entitled :HDREF refid='vxdchap'..
.do end
.mnote WINDOWS NT
.ix 'PE format executable'
tells the &lnkname to generate a Win32 executable file ("PE" format).
.np
.ix 'Phar Lap TNT'
.ix 'TNT DOS extender'
If "TNT" is specified, an executable for the Phar Lap TNT DOS
extender is created.
.ix 'PL format executable'
.ix 'PE format executable'
A "PL" format (rather than "PE") executable is created so that
the Phar Lap TNT DOS extender will always run the application
(including under Windows NT).
.np
.ix 'RDOS'
If "RDOS" is specified, an executable for the RDOS operating system
is created.
.np
If "DLL" (short form "DL") is specified, a Dynamic Link Library will
be generated in which case the name of the executable file will have
extension "dll".
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
Specifying "INITGLOBAL" (short form "INITG") will cause the
initialization routine to be called the first time the Dynamic Link
Library is loaded.
.np
Specifying "INITINSTANCE" (short form "INITI") will cause the
initialization routine to be called each time the Dynamic Link Library
is referenced by a process.
.np
In either case, the initialization routine is defined by the start
address.
If neither "INITGLOBAL" or "INITINSTANCE" is specified, "INITGLOBAL"
is assumed.
.np
It is also possible to specify whether the initialization routine is
to be called at DLL termination or not.
Specifying "TERMGLOBAL" (short form "TERMG") will cause the
initialization routine to be called when the last instance of the
Dynamic Link Library is terminated.
Specifying "TERMINSTANCE" (short form "TERMI") will cause the
initialization routine to be called each time an instance of the
Dynamic Link Library is terminated.
Note that the initialization routine is passed an argument indicating
whether it is being called during DLL initialization or DLL
termination.
If "INITINSTANCE" is used and no termination option is specified,
"TERMINSTANCE" is assumed.
If "INITGLOBAL" is used and no termination option is specified,
"TERMGLOBAL" is assumed.
.if '&target' ne 'QNX' .do begin
.np
For more information on Windows NT executable file formats,
see the chapter entitled :HDREF refid='ntchap'..
.do end
.mnote OS2
tells the &lnkname to generate an OS/2 executable file format.
.np
The name of the executable file will have extension "exe".
If "LE" is specified, an early form of the OS/2 32-bit linear
executable will be generated.
This executable file format is required by the CauseWay DOS extender,
Tenberry Software's DOS/4G and DOS/4GW DOS extenders, and similar products.
.np
In order to improve load time and minimize the size of the executable
file, the OS/2 32-bit linear executable file format was changed.
If "LX" or "FLAT" (short form "FL") is specified, the new form of the
OS/2 32-bit linear executable will be generated.
This executable file format is required by
the FlashTek DOS extender
and 32-bit OS/2 executables.
.np
If "FLAT", "LX" or "LE" is not specified, an OS/2 16-bit executable
will be generated.
.np
If "DLL" (short form "DL") is specified, a Dynamic Link Library will
be generated in which case the name of the executable file will have
extension "dll".
Note that these default extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
Specifying "INITGLOBAL" (short form "INITG") will cause the
initialization routine to be called the first time the Dynamic Link
Library is loaded.
The "INITGLOBAL" option should be used with "OPTION ONEAUTODATA" (the
default for Dynamic Link Libraries).
If the "INITGLOBAL" option is used with "OPTION MANYAUTODATA", the
initialization code will be called once for the first data segment
allocated but not for subsequent allocations (this is generally not
desirable behaviour and will likely cause a program fault).
.np
Specifying "INITINSTANCE" (short form "INITI") will cause the
initialization routine to be called each time the Dynamic Link Library
is referenced by a process.
The "INITINSTANCE" option should be used with "OPTION MANYAUTODATA"
(the default for executable programs).
.np
In either case, the initialization routine is defined by the start
address.
If neither "INITGLOBAL" or "INITINSTANCE" is specified, "INITGLOBAL"
is assumed.
.np
For OS/2 32-bit linear executable files, it is also possible to
specify whether the initialization routine is to be called at DLL
termination or not.
Specifying "TERMGLOBAL" (short form "TERMG") will cause the
initialization routine to be called when the last instance of the
Dynamic Link Library is terminated.
Specifying "TERMINSTANCE" (short form "TERMI") will cause the
initialization routine to be called each time an instance of the
Dynamic Link Library is terminated.
Note that the initialization routine is passed an argument indicating
whether it is being called during DLL initialization or DLL
termination.
If "INITINSTANCE" is used and no termination option is specified,
"TERMINSTANCE" is assumed.
If "INITGLOBAL" is used and no termination option is specified,
"TERMGLOBAL" is assumed.
.np
If "PM" is specified, a Presentation Manager application will be
created.
The application uses the API provided by the Presentation Manager and
must be executed in the Presentation Manager environment.
.np
lf "PMCOMPATIBLE" (short form "PMC") is specified, an application
compatible with Presentation Manager will be created.
The application can run inside the Presentation Manager or it can run
in a separate screen group.
An application can be of this type if it uses the proper subset of
OS/2 video, keyboard, and mouse functions supported in the
Presentation Manager applications.
This is the default.
.np
If "FULLSCREEN" (short form "FULL") is specified, an OS/2 full screen
application will be created.
The application will run in a separate screen group from the
Presentation Manager.
.np
If "PHYSDEVICE" (short form "PHYS") is specified, the executable file
is marked as a physical device driver.
.np
If "VIRTDEVICE" (short form "VIRT") is specified, the executable file
is marked as a virtual device driver.
.if '&target' ne 'QNX' .do begin
.np
For more information on OS/2 executable file formats,
see the chapter entitled :HDREF refid='os2chap'..
.do end
.mnote PHARLAP
(short form "PHAR") tells the &lnkname to generate an executable file
that will run under Phar Lap's 386|DOS-Extender.
.np
There are 4 forms of executable files:
simple, extended, relocatable and segmented.
If "EXTENDED" (short form "EXT") is specified, an extended form of the
executable file with file extension "exp" will be generated.
If "REX" is specified, a relocatable executable file with file
extension "rex" will be generated.
If "SEGMENTED" (short form "SEG") is specified, a segmented executable
file with file extension "exp" will be generated.
If neither "EXTENDED", "REX" or "SEGMENTED" is specified, a simple
executable file with file extension "exp" will be generated.
Note that the default file extensions can be overridden by using the
"NAME" directive to name the executable file.
.np
The simple form is for flat model 386 applications.
It is the only format that can be loaded by earlier versions of
386|DOS-Extender (earlier than 1.2).
.np
The extended form is used for flat model applications that have been
linked in a way which requires a method of specifying more information
for 386|DOS-Extender than possible with the simple form.
.np
The relocatable form is similar to the simple form.
Unique to the relocatable form is an offset relocation table.
This allows the loader to load the program at any location it chooses.
.np
The segmented form is used for embedded system applications like
Intel RMX.
These executables cannot be loaded by 386|DOS-Extender.
.np
A simple form of the executable file is generated in all but the
following cases.
.autonote
.note
"EXTENDED" is specified in the "FORMAT" directive.
.note
The "RUNTIME" directive is specified.
Options specified by the "RUNTIME" directive can only be specified in
the extended form of the executable file.
.note
The "OFFSET" option is specified.
The value specified in the "OFFSET" option can only be specified in
the extended form of the executable file.
.note
"REX" is specified in the "FORMAT" directive.
In this case, the relocatable form will be generated.
You must not specify the "RUNTIME" directive or the "OFFSET" option
when generating the relocatable form.
.note
"SEGMENTED" is specified in the "FORMAT" directive.
In this case, the segmented form will be generated.
.endnote
.if '&target' ne 'QNX' .do begin
.np
For more information on Phar Lap executable file formats,
see the chapter entitled :HDREF refid='phrchap'..
.do end
.mnote NOVELL
(short form "NOV") tells the &lnkname to generate a NetWare
executable file, more commonly called a NetWare Loadable Module (NLM).
.np
NLMs are further classified according to their function.
The executable file will have a file extension that depends on the
class of the NLM being generated.
The following describes the classification of NLMs.
.begpoint
.point LAN
instructs the &lnkname to generate a LAN driver.
A LAN driver is a device driver for Local Area Network hardware.
A file extension of "lan" is used for the name of the executable file.
.point DSK
instructs the &lnkname to generate a disk driver.
A file extension of "dsk" is used for the name of the executable file.
.point NAM
instructs the &lnkname to generate a file system name-space support
module.
A file extension of "nam" is used for the name of the executable file.
.point MSL
instructs the &lnkname to generate a Mirrored Server Link module. The
default file extension is "msl"
.point CDM
instructs the &lnkname to generate a Custom Device module. The
default file extension is "cdm"
.point HAM
instructs the &lnkname to generate a Host Adapter module. The
default file extension is "ham"
.point NLM
instructs the &lnkname to generate a utility or server application.
This is the default.
A file extension of "nlm" is used for the name of the executable file.
.point 'number'
instructs the &lnkname to generate a specific type of NLM using 'number'.
This is a 32 bit value that corresponds to Novell allocated NLM types.
.np
These are the current defined values:
.begpoint
.point 0 
Specifies a standard NLM (default extension .NLM)
.point 1
Specifies a disk driver module (default extension .DSK)
.point 2 
Specifies a namespace driver module (default extension .NAM)
.point 3 
Specifies a LAN driver module (default extension .LAN)
.point 4
Specifies a utility NLM (default extension .NLM)
.point 5
Specifies a Mirrored Server Link module (default .MSL)
.point 6
Specifies an Operating System module (default .NLM)
.point 7
Specifies a Page High OS module (default .NLM)
.point 8
Specifies a Host Adapter module (default .HAM)
.point 9
Specifies a Custom Device module (default .CDM)
.point 10
Reserved for Novell usage
.point 11
Reserved for Novell usage
.point 12
Specifies a Ghost module (default .NLM)
.point 13
Specifies an SMP driver module (default .NLM)
.point 14
Specifies a NIOS module (default .NLM)
.point 15
Specifies a CIOS CAD type module (default .NLM)
.point 16
Specifies a CIOS CLS type module (default .NLM)
.point 21
Reserved for Novell NICI usage
.point 22
Reserved for Novell NICI usage
.point 23
Reserved for Novell NICI usage
.point 24
Reserved for Novell NICI usage
.point 25
Reserved for Novell NICI usage
.point 26
Reserved for Novell NICI usage
.point 27
Reserved for Novell NICI usage
.point 28
Reserved for Novell NICI usage
.endpoint
.point description
is a textual description of the program being linked.
.endpoint
.if '&target' ne 'QNX' .do begin
.np
For more information on NetWare executable file formats,
see the chapter entitled :HDREF refid='novchap'..
.do end
.mnote QNX
tells the &lnkname to generate a QNX executable file.
.np
If "FLAT" (short form "FL") is specified, a 32-bit flat executable
file is generated.
.np
Under QNX, no file extension is added to the executable file name.
.np
Under other operating systems, the name of the executable file will
have the extension "qnx".
Note that this default extension can be overridden by using the "NAME"
directive to name the executable file.
.np
For more information on QNX executable file formats,
see the chapter entitled :HDREF refid='qnxchap'..
.mnote ELF
tells the &lnkname to generate an ELF format executable file.
.np
ELF format DLLs can also be created.
.if '&target' ne 'QNX' .do begin
.np
For more information on ELF executable file formats,
see the chapter entitled :HDREF refid='elfchap'..
.do end
.esynote
.np
If no "FORMAT" directive is specified, the executable file format will
be selected for each of the following host systems in the way
described.
.begnote
.note DOS
If 16-bit object files are encountered, a 16-bit DOS executable will
be created.
If 32-bit object files are encountered, a 32-bit DOS/4G executable
will be created.
.note OS/2
If 16-bit object files are encountered, a 16-bit OS/2 executable will
be created.
If 32-bit object files are encountered, a 32-bit OS/2 executable will
be created.
.note QNX
If 16-bit object files are encountered, a 16-bit QNX executable will
be created.
If 32-bit object files are encountered, a 32-bit QNX executable will
be created.
.note Windows NT
If 16-bit object files are encountered, a 16-bit Windows executable
will be created.
If 32-bit object files are encountered, a 32-bit Win32 executable will
be created.
.note Windows 95
If 16-bit object files are encountered, a 16-bit Windows executable
will be created.
If 32-bit object files are encountered, a 32-bit Win32 executable will
be created.
.endnote
