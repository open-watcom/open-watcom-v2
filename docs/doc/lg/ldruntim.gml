.*
.*
.dirctv RUNTIME
.*
.np
For Win32 applications, the "RUNTIME" directive specifies the
environment under which the application will run.
.np
For PharLap applications, the "RUNTIME" directive describes
information that is used by 386|DOS-Extender to setup the environment
for execution of the program.
.np
For ELF applications, the "RUNTIME" directive specifes ABI type and version
under which the application will run.
.*
.beglevel
.*
.section RUNTIME - Win32 only
.*
.np
The "RUNTIME" directive specifies the environment under which the
application will run.
The format of the "RUNTIME" directive (short form "RU") is as follows.
.mbigbox
     RUNTIME  env[=major[.minor]]

     env ::= NATIVE | WINDOWS | CONSOLE | POSIX | OS2 | DOSSTYLE
.embigbox
.synote
.*
.mnote env=major.minor
.ix 'RUNTIME options' 'version'
.ix 'runtime version option'
Specifying a system version in the form "major" or "major.minor"
indicates the minimum operating system version required for the
application.
For example, the following indicates that the application requires
Windows 95.
.millust begin
runtime windows=4.0
.millust end
.*
.mnote NATIVE
.ix 'RUNTIME options' 'NATIVE'
.ix 'NATIVE runtime option'
(short form "NAT") indicates that the application is a native Windows
NT application.
.*
.mnote WINDOWS
.ix 'RUNTIME options' 'WINDOWS'
.ix 'WINDOWS runtime option'
(short form "WIN") indicates that the application is a Windows
application.
.*
.mnote CONSOLE
.ix 'RUNTIME options' 'CONSOLE'
.ix 'CONSOLE runtime option'
(short form "CON") indicates that the application is a character-mode
(command line oriented) application.
.*
.mnote POSIX
.ix 'RUNTIME options' 'POSIX'
.ix 'POSIX runtime option'
(short form "POS") indicates that the application uses the POSIX
subsystem available with Windows NT.
.*
.mnote OS2
.ix 'RUNTIME options' 'OS2'
.ix 'OS2 runtime option'
indicates that the application is a 16-bit OS/2 1.x application.
.*
.mnote DOSSTYLE
.ix 'RUNTIME options' 'DOSSTYLE'
.ix 'DOSSTYLE runtime option'
(short form "DOS") indicates that the application is a Phar Lap TNT
DOS extender application that uses INT 21 to communicate to the DOS
extender rather than calls to a DLL.
.*
.esynote
.*
.section RUNTIME - PharLap only
.*
.np
The "RUNTIME" directive describes information that is used by
386|DOS-Extender to setup the environment for execution of the
program.
The format of the "RUNTIME" directive (short form "RU") is as follows.
.mbigbox
    RUNTIME run_option{,run_option}

    run_option ::= MINREAL=n | MAXREAL=n | CALLBUFS=n | MINIBuf=n
                           | MAXIBUF=n | NISTACK=n | ISTKSIZE=n
                           | REALBREAK=offset | PRIVILEGED | UNPRIVILEGED

    offset ::= n | symbol_name
.embigbox
.synote
.*
.im lnkvalue
.*
.mnote symbol_name
is a symbol name.
.*
.mnote MINREAL
.ix 'RUNTIME options' 'MINREAL'
.ix 'MINREAL runtime option'
(short form "MINR") specifies the minimum number of bytes of
conventional memory required to be free after a program is loaded by
386|DOS-Extender.
Note that this memory is no longer available to the executing program.
The default value of
.sy n
is 0 in which case 386|DOS-Extender allocates all conventional memory
for the executing program.
The &lnkname truncates the specified value to a multiple of 16.
.sy n
must be less than or equal to hexadecimal 100000 (64K*16).
.*
.mnote MAXREAL
.ix 'RUNTIME options' 'MAXREAL'
.ix 'MAXREAL runtime option'
(short form "MAXR") specifies the maximum number of bytes of
conventional memory than can be left free after a program is loaded by
386|DOS-Extender.
Note that this memory is not available to the executing program.
The default value of
.sy n
is 0 in which case 386|DOS-Extender allocates all conventional memory
for the executing program.
.sy n
must be less than or equal to hexadecimal ffff0.
The &lnkname truncates the specified value to a multiple of 16.
.*
.mnote CALLBUFS
.ix 'RUNTIME options' 'CALLBUFS'
.ix 'CALLBUFS runtime option'
(short form "CALLB") specifies the size of the call buffer allocated
for switching between 32-bit protected mode and real mode.
This buffer is used for communicating information between real-mode
and 32-bit protected-mode procedures.
The buffer address is obtained at run-time with a 386|DOS-Extender
system call.
The size returned is the size of the buffer in kilobytes and is less
than or equal to 64.
.np
The default buffer size is zero unless changed using the "CALLBUFS"
option.
The &lnkname truncates the specified value to a multiple of 1024.
.sy n
must be less than or equal to 64K.
Note that
.sy n
is the number of bytes, not kilobytes.
.*
.mnote MINIBUF
.ix 'RUNTIME options' 'MINIBUF'
.ix 'MINIBUF runtime option'
(short form "MINIB") specifies the minimum size of the data buffer
that is used when DOS and BIOS functions are called.
The size of this buffer is particularly important for file I/O.
If your program reads or writes large amounts of data, a large value
of
.sy n
should be specified.
.sy n
represents the number of bytes and must be less than or equal to 64K.
The default value of
.sy n
is 1K.
The &lnkname truncates the specified value to a multiple of 1024.
.*
.mnote MAXIBUF
.ix 'RUNTIME options' 'MAXIBUF'
.ix 'MAXIBUF runtime option'
(short form "MAXIB") specifies the maximum size of the data buffer that
is used when DOS and BIOS functions are called.
The size of this buffer is particularly important for file I/O.
If your program reads or writes large amounts of data, a large value
of
.sy n
should be specified.
.sy n
represents the number of bytes and must be less than or equal to 64K.
The default value of
.sy n
is 4K.
The &lnkname truncates the specified value to a multiple of 1024.
.*
.mnote NISTACK
.ix 'RUNTIME options' 'NISTACK'
.ix 'NISTACK runtime option'
(short form "NIST") specifies the number of stack buffers to be
allocated for use by 386|DOS-Extender when switching from 32-bit
protected mode to real mode.
By default, 4 stack buffers are allocated.
.sy n
must be greater than or equal to 4.
.*
.mnote ISTKSIZE
.ix 'RUNTIME options' 'ISTKSIZE'
.ix 'ISTKSIZE runtime option'
(short form "ISTK") specifies the size of the stack buffers allocated
for use by 386|DOS-Extender when switching from 32-bit protected mode
to real mode.
By default, the size of a stack buffer is 1K.
The value of
.sy n
must be greater than or equal to 1K and less than or equal to 64K.
The &lnkname truncates the specified value to a multiple of 1024.
.*
.mnote REALBREAK
.ix 'RUNTIME options' 'REALBREAK'
.ix 'REALBREAK runtime option'
(short form "REALB") specifies how much of the program must be loaded
into conventional memory so that it can be accessed and/or executed in
real mode.
If
.sy n
is specified, the first
.sy n
bytes of the program must be loaded into conventional memory.
If
.sy symbol
is specified, all bytes up to but not including the symbol
must be loaded into conventional memory.
.*
.mnote PRIVILEGED
.ix 'RUNTIME options' 'PRIVILEGED'
.ix 'PRIVILEGED runtime option'
.ix 'ring 0'
.ix 'privilege' 'ring 0'
(short form "PRIV") specifies that the executable is to run at Ring 0
privilege level.
.*
.mnote UNPRIVILEGED
.ix 'RUNTIME options' 'UNPRIVILEGED'
.ix 'UNPRIVILEGED runtime option'
.ix 'ring 3'
.ix 'privilege' 'ring 3'
(short form "UNPRIV") specifies that the executable is to run at Ring
3 privilege level (i.e., unprivileged).
This is the default privilege level.
.*
.esynote
.*
.section RUNTIME - ELF only
.*
.np
The "RUNTIME" directive specifies the Application Binary Interface (ABI)
type and version under which the application will run.
The format of the "RUNTIME" directive (short form "RU") is as follows.
.mbigbox
     RUNTIME  ABIVER[=abinum.abiversion] | abispec

     abispec ::= abiname[=abiversion]

     abiname ::= SVR4 | LINUX | FREEBSD | NETBSD | SOLARIS
.embigbox
.synote
.*
.mnote abi=abinum.abiversion
.ix 'RUNTIME options' 'version'
.ix 'runtime version option'
Specifying ABI/OS type and optional version indicates specific ABI that an
ELF application is written for. This information may affect how the ELF
executable will be interpreted by the operating system. If ABI version is
not specified, zero will be used. A list of official ABI types may be found
in the System V Application Binary Interface specification.
.np
For example, both of the following example indicate that the application
requires Linux, but does not specify ABI version (numeric value zero).
.millust begin
runtime linux
runtime abiver=3.0
.millust end
.*
.mnote SVR4
.ix 'RUNTIME options' 'SVR4'
.ix 'SVR runtime option'
indicates that the application is a generic ELF application conforming to
the System V Release 4 ABI. This is the default.
.*
.mnote LINUX 
.ix 'RUNTIME options' 'LINUX'
.ix 'LINUX runtime option'
(short form "LIN") indicates that the application is a Linux application.
.*
.mnote FREEBSD
.ix 'RUNTIME options' 'FREEBSD'
.ix 'FREEBSD runtime option'
(short form "FRE") indicates that the application is a FreeBSD application.
.*
.mnote NETBSD
.ix 'RUNTIME options' 'NETBSD'
.ix 'NETBSD runtime option'
(short form "NET") indicates that the application is a NetBSD application.
.*
.mnote SOLARIS
.ix 'RUNTIME options' 'SOLARIS'
.ix 'SOLARIS runtime option'
(short form "SOL") indicates that the application is a Sun Solaris application.
.*
.mnote ABIVER
.ix 'RUNTIME options' 'ABIVER'
.ix 'ABIVER runtime option'
(short form "ABI") specifies the numeric ABI type and optionally version. This
method allows specification of ABI types not explicitly supported by
the &lnkname..
.*
.esynote
.endlevel
