.chap *refid=vidoss Operating System Specifics
.*
.np
This section discusses the following topics:
.begnote $break
.note DOS Extender debugging
See the section entitled :HDREF refid='vid32'..
.note NLM debugging
See the section entitled :HDREF refid='vidnlm'..
.note Graphics programs
See the section entitled :HDREF refid='viddosg'..
.note Windows 3.x debugging
See the section entitled :HDREF refid='vidwin3'..
.note DLL debugging
See the section entitled :HDREF refid='viddll'..
.note Disabling 386/486 debug registers
See the section entitled :HDREF refid='viddosd'..
.note Linux debugging
See the section entitled :HDREF refid='vidlinux'..
.note QNX debugging
See the section entitled :HDREF refid='vidqnx'..
.endnote
.*
.section *refid=vid32 Debugging 32-bit DOS Extender Applications
.*
.np
.ix 'DOS extenders' 'debugging'
.ix '32-bit application debugging'
.ix 'debugging' '32-bit DOS applications'
The &dbgname supports debugging of 32-bit applications developed with
&watc32, &watf32, and assembly language.
.ix 'DOS extenders' 'debugging'
A DOS extender must be used to run the application.
The following DOS extenders are supported.
.begnote $break
.*
.note CauseWay DOS Extender
.ix 'DOS extenders' 'CauseWay'
.ix 'CauseWay'
a public domain DOS extender included in the &watc32 and &watf32 packages.
Note that this DOS extender is largely compatible with DOS/4GW and can often
be used interchangeably.
.*
.note DOS/4GW
.ix 'DOS extenders' 'DOS/4GW'
.ix 'DOS/4GW'
.ix 'DOS/4GW' 'version'
.ix 'Tenberry Software, Inc.'
a DOS extender from Tenberry Software, Inc.
DOS/4GW is a subset of Tenberry Software's DOS/4G product.
DOS/4GW is customized for use with &watc32 and &watf32 and is
included in these packages.
.*
.note 386|DOS-Extender
.ix 'DOS extenders' '386|DOS-Extender'
.ix '386|DOS-Extender'
.ix '386|DOS-Extender' 'version'
.ix 'Phar Lap Software, Inc.'
(version 2.2d or later) a DOS extender from Phar Lap Software, Inc.
.endnote
.*
.beglevel
.*
.section *refid=vidcw Debugging CauseWay 32-bit DOS Extender Applications
.*
.np
.ix 'CauseWay'
.ix 'CWSTUB.EXE'
.ix 'CW.TRP'
.ix 'trap file' 'CW.TRP'
When using the CauseWay DOS extender, the "CWSTUB.EXE" file must be located
in one of the directories listed in the DOS
.ev PATH
environment variable.
The "CWSTUB.EXE" file will usually be stored in the "BINW" directory
of the &company compiler package.
You must also use the
.sy TRap=CW
option.
The "CW.TRP" file will usually be stored in the "BINW" directory of
the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
The help file "CWHELP.EXE" must also be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
It will usually be stored in the "BINW" directory of the &company
compiler package.
.exam begin
C>&dbgcmd /trap=cw hello
  or
C>set &dbgcmd=/trap#cw
C>&dbgcmd hello
.exam end
.*
.*
.section *refid=vidrsi Debugging DOS/4G(W) 32-bit DOS Extender Applications
.*
.np
.ix 'Tenberry Software, Inc.' 'DOS4G.EXE'
.ix 'Tenberry Software, Inc.' 'DOS4GW.EXE'
.ix 'DOS4G.EXE'
.ix 'DOS4GW.EXE'
.ix 'RSI.TRP'
.ix 'trap file' 'RSI.TRP'
When using the Tenberry Software DOS extender, the "DOS4GW.EXE" or
"DOS4G.EXE" file must be located in one of the directories listed in
the DOS
.ev PATH
environment variable.
The "DOS4GW.EXE" file will usually be stored in the "BINW" directory
of the &company compiler package.
You must also use the
.sy TRap=RSI
option.
The "RSI.TRP" file will usually be stored in the "BINW" directory of
the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
The help file "RSIHELP.EXP" must also be located in one of the
directories listed in the DOS
.ev PATH
environment variable.
It will usually be stored in the "BINW" directory of the &company
compiler package.
.exam begin
C>&dbgcmd /trap=rsi hello
  or
C>set &dbgcmd=/trap#rsi
C>&dbgcmd hello
.exam end
.*
.*
.section *refid=vidpls Debugging Phar Lap 32-bit DOS Extender Applications
.*
.np
.ix 'Phar Lap Software, Inc.' 'RUN386.EXE'
.ix 'RUN386.EXE'
.ix 'TNT.EXE'
.ix 'DBGLIB.REX'
.ix 'PLS.TRP'
.ix 'trap file' 'PLS.TRP'
.ix 'PLSHELP.EXP'
.ix 'PEDHELP.EXP'
When using the Phar Lap Software, Inc. DOS extender,
the "RUN386.EXE" (or "TNT.EXE"),
"DBGLIB.REX",
"PLSHELP.EXP",
and "PEDHELP.EXP"
files must be located in one of the directories listed in the DOS
.ev PATH
environment variable.
You must also use the
.sy TRap=PLS
option.
The "PLS.TRP", "PLSHELP.EXP" and "PEDHELP.EXP" files will usually be
stored in the "BINW" directory of the &company compiler package.
You should ensure that this "BINW" directory is included in the DOS
.ev PATH
environment variable.
Otherwise, you must specify the full path name for the trap file.
.np
.ix 'Phar Lap Software, Inc.' 'RUN386.EXE'
.ix 'RUN386.EXE'
.ix 'Phar Lap Software, Inc.' 'TNT.EXE'
.ix 'TNT.EXE'
Parameters are passed to the "RUN386" or "TNT" DOS extender using the
.sy TRap
option.
The entire parameter must be placed within braces.
The following example illustrates how to debug a Phar Lap application
passing the -maxreal switch to RUN386.EXE or TNT.EXE.
.exam begin
C>&dbgcmd /trap=pls;{-maxreal 512} hello
  or
C>set &dbgcmd=/trap#pls;{-maxreal 512}
C>&dbgcmd hello
.exam end
.*
.endlevel
.*
.section *refid=vidnlm Debugging a Novell NLM
.*
.np
.ix 'Novell NLM' 'debugging'
.ix 'NLM' 'debugging Novell'
.ix 'debugging' 'Novell NLM'
Novell NLM's may only be debugged remotely. You must use either the
serial, parallel, or Novell SPX link.
There are 5 NLM's distributed in the &company package.
The following table describes their use:
.millust begin
                NetWare 3.11/3.12       NetWare 4.01

Serial                                  serserv4.nlm
Parallel        parserv3.nlm            parserv4.nlm
SPX             novserv3.nlm            novserv4.nlm
.millust end
.np
To start remote debugging, you load one of the above NLMs at the NetWare
file server console. The debugger is then invoked as in any remote debugging
session.
See the chapter entitled :HDREF refid='vidrem'. for parameter
details.
See the appendix entitled :HDREF refid='vidwire'. for parallel/serial
cable details.
.np
For example, on a NetWare 4.01 server type:
.monoon
load novserv4
.monooff
.np
On a workstation, type:
.monoon
&dbgcmdup /tr=nov mynlm
.monooff
.np
Debugging information for every running NLM is available. You can
debug any NLM in the system as if it were part of your application, as
long as you created it with debug information. If the NLM does not
have Watcom style debugging information, the debugger will attempt to
use any debugging information created by Novell's linker (NLMLINK).
.*
.section *refid=viddosg Debugging Graphics Applications
.*
.np
.ix 'graphics applications' 'debugging'
When debugging a graphics application, there are a number of &dbgname
command line options that could be specified depending on your
situation.
.autonote
.note
If you only have one monitor attached to your system, use the
.sy Swap
option.
The
.sy Swap
option specifies that the application's screen memory and the
debugger's screen memory are to be swapped back and forth using a
single page.
.note
If you have two monitors attached to your system then the
.sy Two
and
.sy Monochrome
options should be used.
The
.sy Two
option specifies that a second monitor is connected to the system.
Note that if the monitor type (
.ct
.sy Monochrome,
.sy Color,
.sy Colour,
.sy Ega43,
.sy Vga50
.ct )
is not specified then the monitor that is not currently being
used is selected for the debugger's screen.
If you specify
.sy Monochrome
then the monochrome monitor will be used for the debugger's screen.
.note
If you are debugging the graphics application using a second personal
computer and the remote debugging feature of the &dbgname then the
choice of display and operation mode for the &dbgname is
irrelevant. If one system is equipped with a graphics display and the
other with a monochrome display then you will undoubtedly use the
system equipped with the monochrome display to run the &dbgname..
.endnote
.*
.section *refid=vidwin3 Debugging Windows 3.x Applications
.*
.np
.ix 'Windows' 'Microsoft'
.ix 'Windows 3.x' 'Microsoft'
.ix 'debugging' 'windows applications'
Both a character mode and a GUI debugger are supplied that run in the
Windows environment. You must choose which of these debuggers you are
going to use. They both have advantages and disadvantages. When your
application is suspended, the GUI and character mode debuggers behave
differently. The GUI debugger allows other applications to continue
running. The character mode debugger does not. Although the GUI
debugger has a much nicer looking user interface, you should not use
it under some circumstances. You can always use the character mode
debugger. You should be aware of the following restrictions:
.autopoint
.point
If you are trying to debug an applications that uses DDE you should
.bi not
use the GUI debugger.
.point
Do
.bi not
try to use the GUI debugger to debug system modal dialogs.
.point
If you hit a break-point in a dialog callback procedure or in your
window procedure when it is receiving certain events (e.g.,
WM_MENUSELECT), the GUI debugger will lock input to itself.
When this happens, you will not be able to switch away from the
debugger, and no other application will repaint themselves. When this
happens, pop-up menus will not draw correctly and you will have to use
the
.mm Action
menu instead.  You should not try to quit the debugger when it is in this
state.
.point
Do
.bi not
try to use either of the Windows debuggers in a seamless Win-OS/2 session.
.endpoint
.np
If you find that the Windows debugger starts too slowly, try using the
.sy DIp=DWARF
option. This prevents the debugger from searching each DLL in the
system for debugging information. It will start up faster, but you
will not be able to see the name of the Windows API calls.
.np
To start the &dbgname., select the program group in which you have
installed the &dbgname..
One of the icons presented is used to start the debugger.
Double-click on the &dbgname icon.
.np
You can make special versions of the &dbgname icon using
.menuref 'Properties' 'File'
of the Windows "Program Manager".
For example, you can add any options you wish to the "Command Line"
field of the "Properties" window.
When you click on the newly created icon, the options specified in the
"Command Line" field are the defaults.
As long as no executable file name was specified in the "Command Line"
field, the &dbgname will present its prompt window.
In the prompt window, you can specify an executable file name and arguments.
.np
If you are debugging the same program over and over again, you might
wish to create an icon that includes the name of the file you wish to
debug in the "Command Line" field.
Each time you click on that icon, the &dbgname is started and it
automatically loads the program you wish to debug.
.*
.section *refid=viddll Debugging Dynamic Link Libraries
.*
.np
.ix 'debugging DLLs'
.ix 'DLL' 'debugging'
The debugger automatically detects all DLLs that your application
references when it loads the application.
When your program loads a DLL dynamically, the debugger detects this
as well.
If you have created your DLL with debugging information,
you can debug it just as if it were part of your application.
Even if it does not have debugging information, the debugger will
process system information to make the DLL entry point names visible.
There are a few limitations:
.autopoint
.point
You cannot debug your DLL initialization code.  This is the
first routine that the operating system runs when it loads the DLL.
This is not normally a problem, since most DLLs do not do much in the
way of initialization.
.point
When a DLL is loaded dynamically, its debugging information may not
be available immediately.  Try tracing a few instructions and it will
appear.
.point
If you restart an application, you will lose any break points that
you had set in dynamically loaded DLLs.  You need to trace back
over the call to LoadModule or DOSLoadModule and re-set these
break points.
.endpoint
.*
.section *refid=viddosd Disabling Use of 386/486 Debug Registers
.*
.np
.ix 'debug registers' 'disabling'
.ix 'debug registers' 'using'
It may be necessary to prevent the &dbgname from using the 386/486
Debug Registers (a hardware feature used to assist debugging).
This situation arises with certain DOS control programs that do not
properly manage Debug Registers.
If the &dbgname fails upon startup on a 386/486 system, it is a good
indication that use of the Debug Registers must be disabled.
With "STD.TRP", the trap file parameter "d" may be specified to
disable the use of Debug Registers.
The following example illustrates the specification of the "d" trap
file parameter.
.exam begin
C>&dbgcmd /trap=std;d calendar
.exam end
.*
.section *refid=vidlinux Debugging Under Linux
.*
.np
.ix 'debugging under Linux'
.ix 'Linux' 'debugging'
.ix '.wdrc'
.ix 'Linux' 'customization'
When the debugger starts up, it will attempt to open the
initialization file
.mono .wdrc
provided that you have not specified the
.sy Invoke
command line option.
It looks for this file in all the usual places (
.ct
.ev CWD,
.ev WD_PATH,
.mono /opt/watcom/wd
.ct ).
This file normally contains your customization commands.
If it is found, it is processed as the default configuration file.
You would normally place this file in your home directory.
.np
If the file does not exist, the debugger then looks for
the
.mono &dbgcmd..&dbgsuff
file.
.np
If you do not want the debugger to use the
.mono .wdrc
file then you can do one of two things &mdash. make sure that it
cannot be located (e.g., delete it) or use the
.sy Invoke
command line option (you could specify the
.mono &dbgcmd..&dbgsuff
file as the target).
.np
The supplied version of the
.mono &dbgcmd..&dbgsuff
file contains an "invoke" command referencing the file
.mono setup.&dbgsuff..
This file, in turn, contains a "configfile" command and "invoke"
commands referencing other command files.
The "configfile" command marks
.mono setup.&dbgsuff
as the default file name to use when the debugger writes out the
current configuration.
.np
The following section entitled :HDREF refid='vidlinuxs'. describes the
search order for debugger files under Linux.
.*
.beglevel
.*
.section *refid=vidlinuxs Search Order for &dbgname Support Files under Linux
.*
.np
There are several supporting files provided with the &dbgname..
These files fall into five categories.
.ix 'support files' 'dbg'
.ix 'support files' 'trp'
.ix 'support files' 'prs'
.ix 'support files' 'hlp'
.ix 'support files' 'sym'
.autonote
.note
&dbgname command files (files with the ".&dbgsuff" suffix).
.note
&dbgname trap files (files with the ".trp" suffix).
.note
&dbgname parser files (files with the ".prs" suffix).
.note
&dbgname help files (files with the ".hlp" suffix).
.note
&dbgname symbolic debugging information files (files with the ".sym"
suffix).
.endnote
.np
.ix 'support files' 'search order'
.ix 'search order' 'Linux'
The search order for &dbgname support files is as follows:
.autopoint
.point
the current directory,
.point
the paths listed in the
.ev &dbgcmdup._PATH
environment variable,
.point
the path listed in the
.ev HOME
environment variable
.point
the directory where &dbgname was started from
.point
"../wd" directory relative to the directory where &dbgname was started
from, and, finally,
.point
the "/opt/watcom/wd" directory.
.endpoint
.np
You should note the following when using the remote debugging feature
of the &dbgname..
When the
.sy REMotefiles
option is specified, the debugger also attempts to locate the &dbgname's
support files (command files, trap files, etc.) on the task machine.
.*
.endlevel
.*
.section *refid=vidqnx Debugging Under QNX
.*
.np
.ix 'debugging under QNX'
.ix 'QNX' 'debugging'
.ix '.wdrc'
.ix 'QNX' 'customization'
When the debugger starts up, it will attempt to open the
initialization file
.mono .wdrc
provided that you have not specified the
.sy Invoke
command line option.
It looks for this file in all the usual places (
.ct
.ev CWD,
.ev WD_PATH,
.mono /usr/watcom/<ver>/wd,
.mono /usr/watcom/wd
.ct ).
This file normally contains your customization commands.
If it is found, it is processed as the default configuration file.
You would normally place this file in your home directory.
.np
If the file does not exist, the debugger then looks for
the
.mono &dbgcmd..&dbgsuff
file.
.np
If you do not want the debugger to use the
.mono .wdrc
file then you can do one of two things &mdash. make sure that it
cannot be located (e.g., delete it) or use the
.sy Invoke
command line option (you could specify the
.mono &dbgcmd..&dbgsuff
file as the target).
.np
The supplied version of the
.mono &dbgcmd..&dbgsuff
file contains an "invoke" command referencing the file
.mono setup.&dbgsuff..
This file, in turn, contains a "configfile" command and "invoke"
commands referencing other command files.
The "configfile" command marks
.mono setup.&dbgsuff
as the default file name to use when the debugger writes out the
current configuration.
.np
The following section entitled :HDREF refid='vidpmd'. describes the
use of the debugger with the Postmortem dump facility.
The following section entitled :HDREF refid='vidqnxs'. describes the
search order for debugger files under QNX.
.*
.beglevel
.*
.section *refid=vidpmd Debugging Under QNX Using the Postmortem Dump Facility
.*
.np
.ix 'postmortem dump' 'QNX'
.ix 'debugging' 'postmortem dump under QNX'
A limited form of debugging of an application that has terminated and
produced a postmortem dump can be done under QNX.
.ix 'dumper'
.ix 'dumper command'
In order to use this feature, you must start the QNX "dumper" program.
.mbigbox
dumper [-d path] [-p pid] &
.embigbox
.begnote
.note dumper
is the program name for the QNX postmortem dump program.
.note -d path
The name of the directory in which postmortem dumps are written.
If not specified, the default is the user's home directory.
.note -p pid
Save a dump file for this process if it terminates for any reason.
Do not save a dump file for any other process.
.note &
must be specified so that the shell is rejoined.
.endnote
.exam begin
$ dumper &
$ dumper -d /usr/fred/dump_area &
.exam end
.np
Whenever a program terminates abnormally, a dump of the current state
of the program in memory is written to disk.
The dump file name is the same as the program name with a
.bd ~.dmp
extension.
For example, if the program name is
.bd a.out
then the dump will be written to the
.bd /home/userid/a.out.dmp
file.
.np
You can use the
.sy -d
option of the dumper program to force all dumps into a single directory
rather than into the invoking user's home directory.
.np
The
.sy -p
option lets you monitor a particular process.
You can run multiple copies of the dumper program, each monitoring a
different process.
.np
If the &dbgname was being used to debug the program at the time that
it abnormally terminated then the dump is written to the user's home
directory provided that the
.sy -d
option was not used.
.np
To examine the contents of the postmortem dump, the &dbgname may be
used.
.ix 'trap file'
The interface between the &dbgname and the postmortem dump is
contained in a special "trap" file.
The trap file is specified to the &dbgname using the
.sy TRap
option.
.ix 'pmd.trp'
.ix 'trap file' 'pmd.trp'
.mbigbox
&dbgcmd -TRap=pmd[;i] [:sym_file] file_spec
.embigbox
.begnote
.note &dbgcmd
is the program name for the &dbgname..
.note -TRap=pmd[;i]
.ix 'TRap option'
must be specified when debugging an application that has
terminated and produced a postmortem dump.
The optional ";i" is specified when the modification date of the
original program file does not match the information contained in the
dumper file.
It indicates that the symbolic debugging information in the program file
may be out-of-date.
It instructs the &dbgname to ignore the date mismatch.
Depending on the shell that you are using, it may be necessary to place
the option specification in quotation marks if you include the optional
";i".
.exam begin
$ &dbgcmd "-trap=pmd;i" myapp
.exam end
.note sym_file
is an optional symbolic information file specification.
The specification must be preceded by a colon (":").
When specifying a symbol file name, a path such as "//5/etc/" may
be included.
For QNX, the default file suffix of the symbol file is ".sym".
.note file_spec
is the file name of the dumper file to be loaded into memory.
When specifying a file name, a path such as "//5/etc/" may be
included.
If a path is omitted, the &dbgname will first attempt to locate the
file in the current directory and, if not successful, attempt to
locate the file in the default dumper directory:
.bd /usr/dumps
.ct .li .
.endnote
.np
Basically, the &dbgname is fully functional when a postmortem dump is
examined.
However, there are some operations which are not allowed.
Among these are:
.autonote
.note
Task execution cannot be restarted using
.menuref 'Go' 'Run'
.dot
.note
A register can be modified for the purposes of expression evaluation.
You can choose
.menuref 'Go' 'Run'
to restore the register contents to their original postmortem state.
.note
Memory cannot be modified.
.note
Memory outside of regions owned by the program cannot always be examined.
.note
I/O ports cannot be examined.
.endnote
.*
.section *refid=vidqnxs Search Order for &dbgname Support Files under QNX
.*
.np
There are several supporting files provided with the &dbgname..
These files fall into five categories.
.ix 'support files' 'dbg'
.ix 'support files' 'trp'
.ix 'support files' 'prs'
.ix 'support files' 'hlp'
.ix 'support files' 'sym'
.autonote
.note
&dbgname command files (files with the ".&dbgsuff" suffix).
.note
&dbgname trap files (files with the ".trp" suffix).
.note
&dbgname parser files (files with the ".prs" suffix).
.note
&dbgname help files (files with the ".hlp" suffix).
.note
&dbgname symbolic debugging information files (files with the ".sym"
suffix).
.endnote
.np
.ix 'support files' 'search order'
.ix 'search order' 'QNX'
The search order for &dbgname support files is as follows:
.autopoint
.point
the current directory,
.point
the paths listed in the
.ev &dbgcmdup._PATH
environment variable,
.point
the path listed in the
.ev HOME
environment variable, and, finally,
.point
the "/usr/watcom/wd" directory.
.endpoint
.np
You should note the following when using the remote debugging feature
of the &dbgname..
When the
.sy REMotefiles
option is specified, the debugger also attempts to locate the &dbgname's
support files (command files, trap files, etc.) on the task machine.
.*
.endlevel
