.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&machine' eq '8086' .do begin
:set symbol="newauto"   value="NEWAUTO.BAT".
.do end
.if '&machine' eq '80386' .do begin
:set symbol="newauto"   value="AUTOC386.BAT".
.do end
:set symbol="newos2"    value="NEWCONF.OS2".
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&machine' eq '8086' .do begin
:set symbol="newauto"   value="F16AUTO.BAT".
.do end
.if '&machine' eq '80386' .do begin
:set symbol="newauto"   value="F32AUTO.BAT".
.do end
:set symbol="newos2"    value="F32CONF.OS2".
.do end
.*
.section Setting Up Environment Variables
.*
.np
.if '&target' ne 'QNX' .do begin
.if '&ccmd' ne 'watfor77' .do begin
Applications for various target systems
.if '&machine' eq '8086' .do begin
(including DOS, Microsoft Windows 3.x, and OS/2 V1.x)
.do end
.if '&machine' eq '80386' .do begin
(including extended DOS,
.if '&cmpclass' ne 'load-n-go' .do begin
Microsoft Windows 3.x,
Microsoft Windows NT,
.do end
and OS/2 2.0)
.do end
can be created using &cmpname and related tools on host systems
including DOS,
.if '&cmpclass' ne 'load-n-go' .do begin
Microsoft Windows 3.x,
.if '&machine' eq '80386' .do begin
Microsoft Windows NT,
.do end
.do end
and OS/2 2.0.
.do end
Essentially, the host system is the computer system on which you will
be doing your compiling and linking.
The target system is the system on which you will run and, possibly,
debug your application.
.np
.do end
Before beginning to use &cmpname and its related tools, the following
environment variables should be defined.
.if '&target' eq 'QNX' .do begin
The first one,
.ev PATH,
should be defined before using the
.kw &wclcmdup.
program which is introduced next.
The
.kw &wclcmdup.
program simplifies and ties together the use of both compiler and
linker.
The
.ev &incvarup.
and
.ev &pathvarup.
environment variables should be defined before using &cmpname and the
&lnkname. on their own.
.do end
.begnote
.mnote PATH
In order for
.if '&target' eq 'QNX' .do begin
QNX
.do end
.el .do begin
DOS
.if '&machine' eq '80386' .do begin
.ct , Windows NT
.do end
or OS/2
.do end
to locate the &product. binary program files, the
.ev PATH
environment variable must be defined to include the disk and
directories of these programs.
.if '&target' eq 'QNX' .do begin
.np
.ix 'bin directory'
The default directory for &product. QNX binaries is called
"&pc.bin".
.exam begin 1
&prompt.&setcmd. PATH=&ps.&pc.bin&ps.&pc.home&pc.bin
.exam end
.do end
.if '&target' ne 'QNX' .do begin
.keep
.np
.us If your host system is DOS:
.np
.ix 'BINW directory'
The default installation directory for &product. DOS binaries is
called :FNAME.&pathnam.&pc.binw:eFNAME..
.exam begin 1
&prompt.path &dr3.&pathnam.&pc.binw
.exam end
.if '&machine' eq '80386' .do begin
.keep
.np
.us If your host system is Windows NT:
.np
.ix 'BINNT directory'
.ix 'BINW directory'
The default installation directories for &product NT binaries and
related files are
called :FNAME.&pathnam.&pc.binnt:eFNAME. and
:FNAME.&pathnam.&pc.binw:eFNAME..
.exam begin 1
path=&dr3.&pathnam.&pc.binnt&ps.&dr3.&pathnam.&pc.binw
.exam end
.np
.ix 'system initialization' 'Windows NT'
Use the "System" icon in the
.bd Control Panel
to define environment variables.
.do end
.keep
.np
.us If your host system is OS/2:
.np
.ix 'BINP directory'
.ix 'BINW directory'
The default installation directories for &product OS/2 binaries are
called :FNAME.&pathnam.&pc.binp:eFNAME. and
:FNAME.&pathnam.&pc.binw:eFNAME..
.exam begin 1
[C:\]path &dr3.&pathnam.&pc.binp&ps.&dr3.&pathnam.&pc.binw
.exam end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.mnote &incvarup.
In order for &cmpname to locate the &product. header files such as
:FNAME.stdio.h:eFNAME., the
.ev &incvarup.
environment variable should be defined.
.if '&target' eq 'QNX' .do begin
When using
.kw &wclcmdup.
.ct , it is not necessary to define this environment variable since it
uses a different mechanism for identifying the location of the
&product. header files to &cmpname..
However, you should do so when you begin to use &cmpname directly
without the aid of this utility program.
.do end
The
.ev &incvarup.
environment variable must include the location of the &product. header
files (files with the "&hxt." filename extension).
.exam begin 1
&prompt.&setcmd. &incvar.=&dr3.&pathnam.&hdrdir.
.exam end
.pc
The
.ev &incvarup.
environment string is like the
.ev PATH
string in that you can specify one or more directories separated by
&psword.s ("&ps.").
.do end
.mnote &pathvarup.
In order for the &lnkname. to locate the &product. library files, the
.ev &pathvarup.
environment variable should be defined to be the directory
in which you installed &product..
.if '&target' eq 'QNX' .do begin
When using
.kw &wclcmdup.
.ct , it is not necessary to define this environment variable since it
uses another technique for identifying the location of the library
files to the &lnkname..
However, you should do so when you begin to use the &lnkname. directly
without the aid of this utility program.
.do end
The
.ev &pathvarup.
environment variable is used to locate the libraries that will be used
when the application is linked.
.se *installup=&pathnamup.
.se *install=&pathnam.
.if '&target' eq 'QNX' .do begin
.se *installup=&pathnamup.&libdirup.
.se *install=&pathnam.&libdir.
.do end
The default installation directory for &product. files is
:FNAME.&*installup.:eFNAME..
.exam begin 1
&prompt.&setcmd. &pathvar.=&dr3.&*install.
.exam end
.endnote
.*
.if '&target' ne 'QNX' .do begin
.*
.section Automatic Definition of Environment Variables
.*
.np
.ix 'system initialization file' '&newauto.'
.ix '&newauto.' 'system initialization file'
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT' 'system initialization file'
If you are running DOS and the required environment variables are not
defined, you should check that the file
:FNAME.&newauto:eFNAME. which was created by the installation
procedure has been merged into your DOS system initialization file,
:FNAME.autoexec.bat:eFNAME..
Once this has been done, the environment variables will be defined
each time your system is booted.
Alternatively, you can invoke the :FNAME.&newauto:eFNAME. batch file before
each session with &product..
.np
The :FNAME.&newauto:eFNAME. file
can be found in the &product. installation directory (default -
:FNAME.&pathnamup:eFNAME.).
.np
.fi AUTOEXEC.BAT
must be placed in the root directory of your boot disk.
.if '&machine' eq '80386' .do begin
.np
.ix 'system initialization' 'Windows NT'
If you are running Windows NT and the required environment variables
are not defined, you must use the "System" icon in the Control Panel
to define the environment variables.
.do end
.np
.ix 'system initialization file' '&newos2.'
.ix '&newos2.' 'system initialization file'
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS' 'system initialization file'
If you are running OS/2 and the required environment variables are not
defined, you should check that the file
:FNAME.&newos2:eFNAME.
which was created by the installation procedure has been merged into
your OS/2 system initialization file,
:FNAME.config.sys:eFNAME..
Once this has been done, the environment variables will be defined
each time your system is booted.
Alternatively, you can invoke the
:FNAME.&newos2:eFNAME. command file before each
session with &product..
.np
The :FNAME.&newos2:eFNAME. file
can be found in the &product. installation directory (default -
:FNAME.&pathnamup:eFNAME.).
.np
The :FNAME.CONFIG.SYS:eFNAME. file
must be placed in the root directory of your boot disk.
.*
.do end
.*
.section Displaying Environment Variables
.*
.np
.ix '&setcmdup. command'
You can check that the environment variables are properly defined by
entering the "&setcmdup." command at the prompt.
.exam begin
&prompt.&setcmd.
.if '&target' eq 'QNX' .do begin
SHELL=ksh
TERM=qnx
LOGNAME=fred
PATH=:/bin:/home/bin
HOME=/home/fred
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&incvarup.=&dr3.&pathnam.&hdrdir.
.do end
&pathvar.=&dr3.&pathnam.&libdir.
.do end
.el .do begin
PROMPT=$d $t $p$_$n$g
COMSPEC=d:&pc.dos&pc.command.com
PATH=G:\;E:\CMDS;C:&pathnamup.\BINW;D:\DOS
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&incvarup.=&dr3.&pathnam.&hdrdir.
.do end
&pathvarup.=&dr3.&pathnam.
.do end
.exam end
