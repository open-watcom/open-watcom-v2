.chap *refid=vidrem Remote Debugging
.*
.if &e'&dohelp eq 0 .do begin
.section Overview
.do end
.*
.np
.ix 'remote debugging'
.ix 'debugging' 'remote'
Remote debugging allows you to run the debugger on one side of a
communication link and the application being debugged on the other.
Remote debugging is required when there is not enough memory to run
both the debugger and the application on the same machine. Remote
debugging may also be required for debugging graphical applications.
.np
The DOS debugger runs in protected mode (above the 1M mark), with a
small memory footprint in the first 640k. Newer operating systems such
as OS/2 and Windows NT/9x have eliminated the 640k barrier, so there
is little need for remote debugging due to memory limitations alone.
However, remote debugging is often helpful when debugging graphical
or interactive application where the debugger interferes with the user
application.
Remote debugging is also required to debug Novell NetWare applications,
or specialized embedded systems that cannot run the entire debugger.
.np
There are many different communication links supported. Some
communicate between two machines. In this case an external
communication medium is used. Some links communicate between two
operating systems shells on the same machine. In either case, the
concepts are the same.
.np
While remote debugging, you may want to reference a file that is found
on one machine or the other. See the section entitled
:HDREF refid='rfxfile'. for details about remote and local file names.
.np
The debugger is broken down into 4 parts.
.begnote
.note The Debugger
This is the portion of the debugger that contains the user interface.
It is the largest part of the debugger.
Its name is either &dbgcmdup..EXE, &dbgcmdup.W.EXE or &dbgcmdup.C.EXE
.note The Debug Kernel
.ix 'debug kernel'
The debugger interprets your requests and sends low level requests to
the debug kernel. It is a small executable that is dynamically loaded
by the debugger or a remote debug server and used to control your
application. It can be called STD.TRP, STD.DLL, RSI.TRP or PLS.TRP
.note Remote Trap Files&mdash.
.ix 'remote trap files'
.ix 'trap file' 'remote'
These are versions of the debug kernel file that take requests and
send them across a communications link to a remote debug server. You
choose a trap file using the debugger's "trap" option.
.refalso comopt
Trap files have a 3 letter file name that represents the name of the
communications layer being used. The file extension is TRP or DLL.
.note Remote Debug Servers&mdash.
These executable files receive requests from a communications link and
pass them to a debug kernel. Remote debug server names all start with
???SERV. The first 3 letters represent the communication layer being
used and correspond to the trap file that is used on the other side of
the link.
.endnote
.np
In the following examples,
.millust begin
A>cmd1
B>cmd2
.millust end
.pc
indicates that
.monoon
cmd1
.monooff
is to be run on one machine and
.monoon
cmd2
.monooff
is to be run on the other.
.np
A normal non-remote debugging session just uses the user interface and
the debug kernel. All components run on the same machine. This simple
debugging session would be started with the command:
.millust begin
A>&dbgcmd. app

+-----------+ +----------+     +----------+
|  WD.EXE   | | STD.TRP  |     | APP.EXE  |
|          / /           |     |          |
|          \ \           |     |          |
|           | |          |     |          |
+-----------+ +----------+     +----------+
.millust end
.np
Debugging a Tenberry Software DOS/4GW (32-bit extended DOS)
application is the same except you must use a different trap file to
control the application.
.millust begin
A>&dbgcmd. /trap=rsi app

+-----------+ +----------+     +----------+
|  WD.EXE   | | RSI.TRP  |     | APP.EXE  |
|          / /           |     |          |
|          \ \           |     |          |
|           | |          |     |          |
+-----------+ +----------+     +----------+
.millust end
.np
A remote debugging session adds a remote debug server and a remote
trap file as well. For example, using the parallel port to debug
between two machines would be accomplished using the following
components:
.millust begin
A>parserv
B>&dbgcmd. /tr=par app

+-----------+ +----------+
|  WD.EXE   | | PAR.TRP  |
|          / /           |
|          \ \           |
|           | |          |
+-----------+ +----------+
                  |
   +-- parallel --+
   |   cable
   |
+-----------+ +----------+     +----------+
|  PARSERV  | | STD.TRP  |     | APP.EXE  |
|  .EXE    / /           |     |          |
|          \ \           |     |          |
|           | |          |     |          |
+-----------+ +----------+     +----------+
.millust end
.np
In order to start the above remote debugging session, you must follow
these steps.
.autopoint
.point
Connect the two machines with a parallel cable.
.refalso vidwire
.point
Start the remote debug server (PARSERV) on one machine.
.point
Start the debugger with the option "/trap=PAR" on the other machine.
This causes the debugger to load the remote trap file (PAR). This will
communicate across the remote link to PARSERV.EXE, which will in turn
communicate with the debug kernel (STD) in order to debug the
application.
.endpoint
.np
The rest of the debugger command line is identical to the command you
would type if you were debugging the application locally.
.np
You must start the remote debug server first. If you do not, the
remote trap file will not be able to establish a communication link
and the debugger will refuse to start.
.np
It is important to realize that the application to be debugged must
reside on the debug server machine.
It must be possible for the debug server to locate the application
to be debugged.  It can be in the current working directory of the
debugger server machine, or in the
.ev PATH,
or a path to locate the application on the debug server machine can
be specified on the debugger command line. Alternatively, you can ask
the debugger to download the application to the debug server machine
if the application resides on the debugger machine.
.millust begin
A>parserv
B>&dbgcmd. /down /tr=par app
.millust end
See the description of the "download" option in the
section entitled :HDREF refid='comopt'..
.np
If you are remote debugging a 32-bit application, you must
.bi use the correct trap file on the remote debug server side
of the link. The trap file specification must come first before any
other arguments on the command line.
.millust begin
A>serserv /tr=rsi
B>&dbgcmd. /tr=ser app

+-----------+ +----------+
|  WD.EXE   | | SER.TRP  |
|          / /           |
|          \ \           |
|           | |          |
+-----------+ +----------+
                  |
   +--- serial ---+
   |    cable
   |
+-----------+ +----------+     +----------+
|  SERSERV  | | RSI.TRP  |     | APP.EXE  |
|  .EXE    / /           |     |          |
|          \ \           |     |          |
|           | |          |     |          |
+-----------+ +----------+     +----------+
.millust end
.np
Following is an example of an internal remote link. This example shows
you how to use the OS/2 version of the debugger to debug a DOS
application.
.millust begin
+-----------+ +----------+
|  WD.EXE   | | NMP.DLL  |
|          / /           |
|          \ \           |
|           | |          |
+-----------+ +----------+
                     |
   +-- OS/2 NP API --+
   |
+-----------+ +----------+     +----------+
|  NMPSERV  | | STD.DLL  |     | APP.EXE  |
|  .EXE    / /           |     |          |
|          \ \           |     |          |
|           | |          |     |          |
+-----------+ +----------+     +----------+
.millust end
.np
The communication medium employed in this case is OS/2 Named Pipes.
.np
The debugger provides the following remote link capabilities:
.begnote
.note NOV
This link uses Novell's SPX layer for communication.
Supported under DOS, OS/2, Windows 3.x, Windows NT/2000/XP and NetWare.
.note NET
This link uses NetBIOS to communicate. If your network software
supports NetBIOS, you can use this link.
Supported under DOS, OS/2, Windows 3.x, and NetWare.
.note PAR
This link supports communication using the parallel or printer port.
Several different cable configurations are supported.
.refalso vidwire
Supported under DOS, OS/2, Windows 3.x, Windows 9x, Windows NT/2000/XP, NetWare, Linux and QNX 4.
.note SER
This link uses a serial port to communicate. Rates of up to 115K
BAUD are supported.
.refalso vidwire
Supported under DOS, OS/2, Windows 9x, Windows NT/2000/XP and QNX 4.
.note WIN
This link will communicate between two Windows DOS boxes.
Supported under Windows 3.x and Windows 9x (for DOS applications
only).
.note NMP
This link will use Named Pipes to communicate internally between OS/2
sessions. OS/2, DOS and Win-OS/2 sessions are supported. If your
network supports Named Pipes, and you have at least one OS/2 machine
on the network, you can communicate between OS/2, DOS and Windows 3.x
machines on the network.
Supported under OS/2 (DOS, OS/2 and Windows 3.x applications).
.note VDM
This link is a subset of the NMP link. It is supported under OS/2
and Windows NT.
The application being debugged must be a DOS or seamless Win-OS/2
application.
Supported under OS/2 and Windows NT (DOS, OS/2 and Windows 3.x
applications).
.note TCP
This link will use TCP/IP to communicate internally or over a network
between sessions.
Supported under DOS, OS/2, Windows 9x, Windows NT/2000/XP, Linux and QNX.
.endnote
.np
Communication parameters may be passed to the remote trap file and the
remote server. They are passed to the remote trap file by following
the name of the trap file with a semi-colon and the parameter. For
example:
.millust begin
A>serserv 2.4800
.millust end
.pc
passes the parameter 2.4800 to the remote debug server. To pass the
same parameter to the remote trap file, use:
.millust begin
B>&dbgcmd. /tr=ser;2.4800 app
.millust end
.np
These link parameters are specific to each remote link and are
described in the following section.
.np
.ix 'Once argument'
.ix 'remote debugging' 'Once argument'
Each of the debug servers can accept an optional "Once" parameter. The
"Once" parameter is used by the &wide.. Usually, a server stays
running until terminated by the user. If the "Once" option is
specified, the remote server will terminate itself as soon as the
debugger disconnects from it.
.*
.section Link Descriptions
.*
.np
The following communication links are described:
.begbull
.bull
:HDREF refid='remnov'.
.bull
:HDREF refid='remnet'.
.bull
:HDREF refid='rempar'.
.bull
:HDREF refid='remser'.
.bull
:HDREF refid='remwin'.
.bull
:HDREF refid='remnmp'.
.bull
:HDREF refid='remvdm'.
.bull
:HDREF refid='remtcp'.
.endbull
.*
.beglevel
.*
.section *refid=remnov NOV (Novell SPX)
.*
.ix 'Novell SPX remote debugging'
.ix 'remote debugging' 'Novell SPX'
.np
This link communicates over a Novell Network. In order to use this
link, you must have a NetWare requester installed on both machines. Be
sure that it is configured to include the SPX option. Consult your
NetWare documentation for details.
.np
The parameter to this link is an arbitrary name to be used for the
connection. This allows multiple network users users to remote debug
simultaneously. The default name is NovLink. If the remote server will
not start, try specifying a different name for the link. The following
example shows how to use the default link parameters:
.millust begin
A>novserv
B>&dbgcmd. /tr=nov app
.millust end
.np
The following example shows how to name "john" as a link parameter:
.millust begin
A>novserv john
B>&dbgcmd. /tr=nov;john app
.millust end
.*
.section *refid=remnet NET (NetBIOS)
.*
.np
This link communicates over NetBIOS. In order to use this link, you
must have NetBIOS installed on both machines. Consult your network
documentation for details.
.np
The parameter to this link is an arbitrary name to be used for the
connection. This allows multiple network users users to remote debug
simultaneously. The default name is NetLink. The following example
shows how to use the default link parameters.
.millust begin
A>netserv
B>&dbgcmd. /tr=net app
.millust end
.np
The following example shows how to use the name "tammy" as a link
parameter.
.millust begin
A>netserv tammy
B>&dbgcmd. /tr=net;tammy app
.millust end
.*
.section *refid=rempar PAR (Parallel)
.*
.ix 'remote debugging' 'over parallel port'
.ix 'parallel port remote debugging'
.np
This link communicates over the parallel port. Three different cable
types may be used. They are called the LapLink, Flying Dutchman, and
Watcom cables. Although the Watcom cable will communicate considerably
faster than the other two, we have found it to be unreliable on some
printer cards.
.refalso vidwire
.np
The parameter to this link is a number from 1 to 3 or the letter "p"
followed by a hexadecimal printer I/O port address. This tells the
software which parallel port the cable is connected to (LPT1, LPT2,
LPT3). The default is 1. The parameter used on each side of the link
depends on which printer port the cable is plugged into on that
machine. It need not be the same on both sides. The following example
shows how to debug across a parallel cable plugged into printer port 3
on one machine and port 2 on the other.
.millust begin
A>parserv 3
B>&dbgcmd. /tr=par;2 app
.millust end
.np
As an alternative, you can specify a port address to use. It is less
convenient than specifying a port number but will work on systems like
OS/2 where the actual I/O port address cannot be obtained from the
system. The following example shows how to debug across a parallel
cable plugged into I/O port 0x378 on one machine and port 2 on the
other.
.millust begin
A>parserv p378
B>&dbgcmd. /tr=par;2 app
.millust end
.np
.ix 'dbgport.sys'
.ix 'dbginst.exe'
Windows NT Note: Under Windows NT/2000/XP you must have the dbgport.sys
device driver installed and loaded on your machine before the host
debugger will be able to access the parallel port. To install this file
manually, first copy it from %WATCOM%\binnt\dbgport.sys to
%WINDOWS%\system32\drivers. Once the file has been copied, run the
%WATCOM%\binnt\dbginst.exe program to install the device driver and
activate it. You will need to have system administrator permissions in
order to be able to run the dbginst.exe program. Once these two steps are
done, the parallel port debugger can work. Note also that if you
wish to do parallel port debuging on a target machine running Windows
NT/2000/XP, you will need to follow the same set of steps on the target
machine as well.
.np
If you are going to debug a DOS extender application, then you must
also specify a trap file to the server program. The trap file must be
specified before the port number. The following example shows how to
debug a 32-bit DOS/4GW application across a parallel cable plugged
into printer port 2 on one machine and port 3 on the other.
.millust begin
A>parserv /tr=rsi 2
B>&dbgcmd. /tr=par;3 app
.millust end
.np
The "RSI" trap file is specified for DOS/4G(W) applications.
You can specify other trap files for the other DOS extenders
(e.g., "CW" for CauseWay).
Do not forget to include other required files in the path.
.begnote
.*
.note CW
Both "CWSTUB.EXE" and the loader help file "CWHELP.EXE" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidcw'. for more information
on debugging applications that use the CauseWay DOS extender.
.*
.note RSI
Both "DOS4GW.EXE" and the loader help file "RSIHELP.EXP" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidrsi'. for more information
on debugging applications that use the DOS/4GW DOS extender.
.*
.note PLS
One or more of "RUN386.EXE" (or "TNT.EXE"), "DBGLIB.REX",
"PLSHELP.EXP", and "PEDHELP.EXP" must be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidpls'. for more information
on debugging applications that use the Phar Lap DOS extender.
.*
.endnote
.*
.section *refid=remser SER (Serial)
.*
.ix 'remote debugging' 'over serial port'
.ix 'serial port remote debugging'
.np
This link communicates over the serial port. See the appendix entitled
:HDREF refid='vidwire'. for wiring details. The debugger and server
will automatically synchronize on a communications speed. They may
communicate at rates as high as 115kB. The DOS and OS/2 "mode" command
or the QNX "stty" commands need not be used.
.np
The parameter to this link takes the form
.millust begin
port_number.baud_rate
.millust end
.np
.mono port_number
is a number from 1 to 3 indicating which serial port the cable is
connected to. The default is 1.
.np
.mono baud_rate
is the maximum BAUD rate at which to communicate.
If you already know the maximum BAUD rate at which the two
machines will communicate, this parameter will speed up
the connection time by eliminating some of the synchronization protocol.
.np
.mono baud_rate
may be any of 115200, 57600, 38400, 19200, 9600, 4800, 2400, or 1200.
It may be shortened to the first 2 digits.
.np
A special BAUD rate of 0 is also allowed. This should be used
if the serial port has been pre-assigned using the "mode" or "stty"
commands. The pre-assigned BAUD rate is used and the BAUD rate
negotiation is avoided.  This will allow you to debug over a modem.
.np
The following example shows how to debug across a serial cable
using default settings:
.millust begin
A>serserv
B>&dbgcmd. /tr=ser app
.millust end
.np
The following example shows how to debug across a serial cable
using serial port 2 on each machine setting the maximum BAUD
rate to 9600:
.millust begin
A>serserv 2.9600
B>&dbgcmd. /tr=ser;2.9600 app
.millust end
.begnote
.note QNX 4 Note:
Under QNX 4, a node id may be specified followed by a comma if the
serial port is not located on the current node.
The command "serserv 3,1.9600" would use the device //3/dev/ser1 at a
BAUD rate of 9600.
Alternatively, you can specify a device such as /dev/foobar.
To specify the maximum line speed, you can specify something like
/dev/foobar.56.
Of course, you can also include a node id such as //5/dev/foobar.
.millust begin
A>serserv //3/dev/ser2.9600
B>&dbgcmd. /tr=ser;//5/dev/ser2.9600 app
.millust end
.endnote
.np
If you are going to debug a DOS extender application, then you must
also specify a trap file to the server program. The trap file must be
specified before the port number and BAUD rate. The following example
shows how to debug a 32-bit DOS/4GW application across a serial cable
using serial port 1 on one machine and serial port 2 on the other
machine setting the maximum BAUD rate to 9600 for each:
.millust begin
A>serserv /tr=rsi 1.9600
B>&dbgcmd. /tr=ser;2.9600 app
.millust end
.np
The "RSI" trap file is specified for DOS/4G(W) applications.
You can specify other trap files for the other DOS extenders
(e.g., "CW" for CauseWay).
Do not forget to include other required files in the path.
.begnote
.*
.note CW
Both "CWSTUB.EXE" and the loader help file "CWHELP.EXE" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidcw'. for more information
on debugging applications that use the CauseWay DOS extender.
.*
.note RSI
Both "DOS4GW.EXE" and the loader help file "RSIHELP.EXP" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidrsi'. for more information
on debugging applications that use the DOS/4GW DOS extender.
.*
.note PLS
One or more of "RUN386.EXE" (or "TNT.EXE"), "DBGLIB.REX",
"PLSHELP.EXP", and "PEDHELP.EXP" must be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidpls'. for more information
on debugging applications that use the Phar Lap DOS extender.
.*
.endnote
.*
.section *refid=remwin WIN (Windows 3.x/9x Virtual DOS Machine)
.*
.np
.ix 'Windows' 'remote debugging'
.ix 'remote debugging' 'with Windows'
.ix 'Windows' 'enhanced mode'
This link communicates between 2 Windows DOS boxes. In order to use
this link, you must have Windows 3.x or Windows 95 installed on your
machine. You must run Windows 3.x in enhanced mode. You must also
include the "device" specification listed below in the [386Enh]
section of your "SYSTEM.INI" file (this line is usually added during
the &company software installation process).
.millust begin
DEVICE=C:&pathnamup.\BINW\WDEBUG.386
.millust end
.np
In order for this link to work properly, you must ensure that this
link runs in a DOS box that has background execution enabled.
.np
The parameter to this link is an arbitrary name to be used
for the connection. This allows you to have multiple remote
debug sessions active simultaneously. The default name is WinLink.
The following examples show how to use the default name or specify a
link name using the Windows 3.x/95 VDM link.
.millust begin
A>winserv
B>&dbgcmd. /tr=win app

A>winserv whats_in_a_name
B>&dbgcmd. /tr=win;whats_in_a_name app
.millust end
.np
The following examples show how to debug a 32-bit extended DOS
application using the Windows 3.x/95 VDM link.
.millust begin
A>winserv /tr=rsi
B>&dbgcmd. /tr=win app

A>winserv /tr=rsi whats_in_a_name
B>&dbgcmd. /tr=win;whats_in_a_name app
.millust end
.np
The "RSI" trap file is specified for DOS/4G(W) applications.
You can specify other trap files for the other DOS extenders
(e.g., "CW" for CauseWay).
Do not forget to include other required files in the path.
.begnote
.*
.note CW
Both "CWSTUB.EXE" and the loader help file "CWHELP.EXE" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidcw'. for more information
on debugging applications that use the CauseWay DOS extender.
.*
.note RSI
Both "DOS4GW.EXE" and the loader help file "RSIHELP.EXP" must also be
located in one of the directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidrsi'. for more information on
debugging applications that use the DOS/4GW DOS extender.
.*
.note PLS
One or more of "RUN386.EXE" (or "TNT.EXE"), "DBGLIB.REX",
"PLSHELP.EXP", and "PEDHELP.EXP" must be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
See the section entitled :HDREF refid='vidpls'. for more information on
debugging applications that use the Phar Lap DOS extender.
.*
.endnote
.*
.section *refid=remnmp NMP (Named Pipes)
.*
.ix 'Named Pipes' 'remote debugging'
.ix 'remote debugging' 'with Named Pipes'
.np
The named pipes link allows you to communicate between any two
sessions on an OS/2 machine. You can also debug remotely between DOS,
Windows 3.x and OS/2 machines if you have installed remote named pipe
support on these machines. See your network documentation for details
on remote named pipes.
.np
In order to use named pipes, you must first run the NMPBIND program.
This may run any OS/2 machine on the network. It can be run detached,
by putting the following line into your OS/2 CONFIG.SYS.
.millust begin
RUN=C:&pathnamup.\BINP\NMPBIND.EXE
.millust end
.np
If you run NMPSERVW under Win-OS/2, it must be run as a seamless
Windows session. This is due to the fact that full screen Win-OS/2
sessions may not get any processor time when they are not in the
foreground.
.np
The parameter to this link can take the following forms:
.millust begin
name
name@machine
.millust end
.np
.id name
is an arbitrary name to be used for the connection. This allows you to
have multiple remote debug sessions active simultaneously. The default
name is NMPLink.
.np
.id machine
is the name of the machine on which the NMPBIND program is running.
This allows you to use remote named pipes.
.np
The following example shows you how to use the named pipe link between
two sessions on the same OS/2 machine.
.millust begin
A>nmpserv
B>&dbgcmd. /tr=nmp app
.millust end
.np
The following example assumes that there is a machine named HAL with a
remote named pipe server on the network which is running NMPBIND.
.millust begin
A>nmpserv mylink@hal
B>&dbgcmd. /tr=nmp;mylink@hal app
.millust end
.*
.section *refid=remvdm VDM (Virtual DOS Machine)
.*
.ix 'OS/2' 'remote debugging'
.ix 'remote debugging' 'with OS/2'
.ix 'Windows NT' 'remote debugging'
.ix 'remote debugging' 'with Windows NT'
.np
VDM is actually a limited version of named pipes that does not require
the NMPBIND program to be running.
It has several restrictions however.
.autopoint
.point
It does not support network debugging.
.point
Under OS/2, the debugger (user interface) must run in an OS/2 (not a
DOS) session.
The debugger may also be started under Windows NT but not Windows 95
since it does not support named pipes.
.point
Under OS/2, the remote debug server must run in a seamless Win-OS/2 or
a DOS session.
.point
Under Windows NT, the remote debug server will be run in a Windows NT
Virtual DOS Machine.
.point
Under Windows 95, the remote debug server can be started but since
Windows 95 does not support named pipes it will not work properly.
See the section entitled :HDREF refid='remwin'. for an alternative.
.point
If you are running VDMSERVW under Win-OS/2, it must be run as a
seamless Windows session.
This is due to the fact that full screen Win-OS/2 sessions may not get
any processor time when they are not in the foreground.
.endpoint
.np
The parameter to this link is an arbitrary name to be used for the
connection. This allows you to have multiple VDM debug sessions active
simultaneously.
The default name is VDMLink.
The following example shows how to use the VDM link:
.millust begin
A>vdmserv
B>&dbgcmd. /tr=vdm app
.millust end
.np
The following example shows how to use the VDM link specifying "brian"
as the link name.
.millust begin
A>vdmserv brian
B>&dbgcmd. /tr=vdm;brian app
.millust end
.*
.section *refid=remtcp TCP/IP (Internet Packets)
.*
.ix 'TCP/IP' 'remote debugging'
.ix 'remote debugging' 'with TCP/IP'
.ix 'Internet Protocol' 'remote debugging'
.ix 'remote debugging' 'with Internet Protocol'
.np
The TCP/IP link allows you to communicate between any two sessions
using TCP/IP if you have installed TCP/IP support.
You can also debug remotely between OS/2 and Windows NT/95 machines
if you have installed TCP/IP support on these machines.
See your network documentation for details on installing TCP/IP
support.

To use TCP/IP support under DOS, you need to configure the WATTCP
client and install a packet driver for your network card.
.np
.ix 'TCPSERV'
In order to use TCP/IP to remotely debug a program, you must start the
TCPSERV server program first.
.exam begin
A>tcpserv
Socket port number: 3563
&company TCP/IP Debug Server Version &ver..&rev
&cpyrit 1988
Press 'q' to exit
.exam end
.pc
.ix 'socket port number'
The server program displays an available socket port number on the
screen.
.np
.ix 'TCP/IP services'
You may specify a TCP/IP "service" as an argument on the command line.
TCPSERV will check the TCP/IP services list to find a matching
service.
.ix 'tcplink service name'
.ix 'service name' 'tcplink'
If no argument is specified on the command line, TCPSERV uses
"tcplink" as the service name.
If no matching service name is found, TCPSERV attempts to convert the
argument to a numeric port number and use that.
.ix 'TCP/IP socket'
.ix 'socket port number' 'default'
If the argument can not be converted to a number, port number 3563 is
used.
.np
The TCP/IP services list is stored in different places depending on
the operating system.
.begnote
.note OS/2
.mono d:\TCPIP\ETC\SERVICES
depending on the drive where TCP/IP is installed
.note Linux and QNX
.mono /etc/services
.note Windows 9x
.mono d:\windows\SERVICES
depending on the drive and directory where Windows 95 is installed
.note Windows NT
.mono d:\WINNT\SYSTEM32\DRIVERS\ETC\SERVICES
depending on the drive where Windows NT is installed
.endnote
.np
.ix 'IP address'
You will also need to know the Internet Protocol (IP) address of the
machine running the TCPSERV program.
This can be in alphanumeric or numeric form
(e.g., jdoe.watcom.on.ca or 172.31.0.99).
With the alphanumeric form, it is not necessary to specify the domain
name portion if the two machines are in the same domain.
.np
To use the remote TCP/IP server, you must specify the TCP/IP trap file
name to the debugger along with an argument consisting of
your IP address, optionally followed by a ":" and the service name or
socket port number used by TCPSERV.
You must also include the name of the application you wish to run and
debug on the remote machine.
.exam begin 4 1
A>tcpserv
B>&dbgcmd. /tr=tcp;jdoe app
        or
B>&dbgcmd. /tr=tcp;172.31.0.99 app
.exam end
.exam begin 4 2
A>tcpserv 1024
B>&dbgcmd. /tr=tcp;jdoe:1024 app
        or
B>&dbgcmd. /tr=tcp;jdoe.watcom.on.ca:1024 app
        or
B>&dbgcmd. /tr=tcp;172.31.0.99:1024 app
.exam end
.exam begin 4 3
A>tcpserv dbgservice
B>&dbgcmd. /tr=tcp;jdoe:dbgservice app
        or
B>&dbgcmd. /tr=tcp;jdoe.watcom.on.ca:dbgservice app
        or
B>&dbgcmd. /tr=tcp;172.31.0.99:dbgservice app
.exam end
.np
.ix 'Internet'
The TCP/IP remote debug service permits debugging of applications
anywhere on the Internet.
However, response will vary with the distances involved.
.*
.* The &company Debugger travels the information superhighway.
.*
.endlevel
.*
.section *refid=rfxfile Specifying Files on Remote and Local Machines
.*
.np
In order to identify files on either the local or remote machine, two
special prefixes are supported.
.begnote
.mnote @L
.ix 'local file specifier prefix' '@L'
.ix '@L'
The "@L" prefix is used to indicate that the file resides on the local
machine (the one on which the debugger is running).
.millust begin
@L[d:][path]filename[.ext]
.millust end
.pc
When "[path]" is not specified, the current directory of the specified
drive of the local machine is assumed.
When "[d:]" is not specified, the current drive of the local machine is
assumed.
.exam begin
@LOUTPUT.LOG
@LD:\CMDS
@LD:\CMDS\DATA.TMP
.exam end
.mnote @R
.ix 'remote file specifier prefix' '@R'
.ix '@R'
The "@R" prefix is used to indicate that the file resides on the remote
machine.
.millust begin
@R[d:][path]filename[.ext]
.millust end
.pc
When "[path]" is not specified, the current directory of the specified
drive of the remote machine is assumed.
When "[d:]" is not specified, the current drive of the remote machine is
assumed.
.exam begin
@RMYAPPL.DAT
@RD:\PROGRAMS\EXE\MYAPPL.LNK
@R\PROGRAMS\SRC
@R\PROGRAMS\SRC\UILIB.C
.exam end
.endnote
.pc
Thus a file may be identified in three different ways.
.millust begin
[d:][path]filename[.ext]
@L[d:][path]filename[.ext]
@R[d:][path]filename[.ext]
.millust end
.pc
A file of the first form resides on either the local or remote machine
depending on whether the current drive is a local or remote drive.
A file of the second form always resides on the local machine.
A file of the third form always resides on the remote machine.
.autonote Notes:
.note
In the each form, the omission of "[d:]" indicates the current drive.
.millust begin
[path]filename[.ext]
@L[path]filename[.ext]
@R[path]filename[.ext]
.millust end
.note
In the each form, the omission of "[path]" indicates the current path
of the specified drive.
.millust begin
[d:]filename[.ext]
@L[d:]filename[.ext]
@R[d:]filename[.ext]
.millust end
.pc
Observe that if "[d:]" is omitted also then the following forms are
obtained:
.millust begin
filename[.ext]
@Lfilename[.ext]
@Rfilename[.ext]
.millust end
.note
The special drive prefixes "@L" and "@R" cannot be used in your own
application to reference files on two different machines.
These prefixes are recognized by the &dbgname only.
Should the situation arise where one of your filenames begins with the
same prefix ("@L", "@l", "@R" or "@r") then "@@" can be used.
For example, if your wish to refer to the file on disk called "@link@"
then you could specify "@@link@".
Note that ".\@link@" would also suffice.
.endnote
