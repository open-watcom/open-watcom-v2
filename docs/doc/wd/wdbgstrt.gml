.chap *refid=vidstrt Starting Up the Debugger
.*
.np
The following topics are discussed:
.begbull
.bull
:HDREF refid='vidcmdl'.
.bull
:HDREF refid='comopt'.
.bull
:HDREF refid='dwopt'.
.bull
:HDREF refid='dosopt'.
.bull
:HDREF refid='winopt'.
.bull
:HDREF refid='linuxopt'.
.bull
:HDREF refid='qnxopt'.
.bull
:HDREF refid='videvar'.
.endbull
.*
.section *refid=vidcmdl &dbgname Command Line
.*
.np
There are several versions of the debugger.
.begnote
.note binw&pc.&dbgcmd..exe
This is the DOS character-mode debugger.
.note binw&pc.&dbgcmd.c.exe
This is the Windows 3.x character-mode debugger.
.note binw&pc.&dbgcmd.w.exe
This is the Windows 3.x windowed (GUI) debugger.
.note binnt&pc.&dbgcmd..exe
This is the Windows NT/9x character-mode debugger.
.note binnt&pc.&dbgcmd.w.exe
This is the Windows NT/9x windowed (GUI) debugger.
.note binp&pc.&dbgcmd..exe
This is the OS/2 character-mode debugger.
.note binp&pc.&dbgcmd.w.exe
This is the OS/2 windowed (GUI) debugger.
.note wd
This is the name of the debugger used on UNIX platforms.
.endnote
.np
See the sections entitled
:HDREF refid='vidoss'.
and
:HDREF refid='vidrem'.
for information on which version to select for your situation.
.np
On the debugger command line, you can specify options that you want to
use during your debugging session. Acceptable option short forms are
indicated in capital letters. For example, the
.us /TRap
option may be shortened to
.us /tr.
.*
.section *refid=comopt Common Switches
.*
.np
The following switches are applicable to all operating systems.
.begnote $break

.cw !
.note /TRap=trap_file[;trap_parm]
.cw ;
.ix 'options' 'TRap'
.ix 'TRap option'
.ix 'DOS extenders' 'trap option'
specifies an executable helper program that the debugger uses to
control the application being debugged, or to communicate across a
remote link.
It is called a "trap file" since the interrupts used
for debugging are sometimes called "traps".  The
.sy trap
option selects
the appropriate trap file to use.
.bi This option must be specified when remote debugging,
.bi debugging DOS extender applications
.bi or debugging OS/2 exception handlers.
.np
The remote trap files themselves have startup parameters.
This is specified following the semi-colon.
.refalso vidrem
.ix '32-bit debugging' 'trap file'
.ix 'trap file'
.ix 'extensions' '.TRP'
.ix 'TRP extension'
Normally you do not have to specify a trap file. If you do not specify
the
.sy trap
option, the default trap file that supports local debugging is loaded.
There are several exceptions.
.autopoint
.point
To debug a CauseWay 32-bit application, you must use
.us /TRAP=CW.
.point
To debug a Tenberry Software 32-bit DOS/4G(W) application, you must
use
.us /TRAP=RSI.
.point
To debug a Phar Lap 32-bit application, you must use
.us /TRAP=PLS.
.point
.ix 'exception handler' 'OS/2'
.ix 'OS/2 exception handler'
.ix 'debugging an OS/2 exception handler'
To debug an OS/2 exception handler, you must use
.us /TRAP=STD&semi.2
which tells the debugger to catch exceptions only on the second chance
(normally it would be the debugger that traps the exception).
.point
To debug an OS/2 16-bit application under Phar Lap's RUN286 DOS
extender, you must use
.us /TRAP=STD16.
.endpoint

.note /LInes=n
.ix 'display' 'changing lines'
.ix 'screen' 'number of lines'
.ix 'options' 'LInes'
.ix 'LInes option'
controls the number of lines used by a character mode debugger.  The number
of lines available depends on the operating system and your video card.
The values 25, 43 and 50 are often supported.

.note /COlumns=n
.ix 'display' 'changing columns'
.ix 'screen' 'number of columns'
.ix 'options' 'COlumns'
.ix 'COlumns option'
controls the number of columns used by a character mode debugger.  The number
of columns available depends on the operating system and your video card.
If your system does not support the requested number of columns,
this option is ignored

.note /Invoke=file
.ix 'options' 'Invoke'
.ix 'Invoke option'
may be used to specify an alternate name for the debugger configuration file
which is to be used at start-up time.
The default file name is "&dbgcmdup..&dbgsuffup".
Debugger configuration files are found in the current directory or one
of the directories in your
.ev PATH
.dot

.note /NOInvoke
.ix 'options' 'NOInvoke'
.ix 'NOInvoke option'
specifies that the default debugger configuration file is not to be invoked.

.note /NOMouse
.ix 'options' 'NOMouse'
.ix 'NOMouse option'
requests that the debugger ignore any attached mouse.
.ix 'mouse' 'sharing'
.ix 'mouse events' 'debugging'
.ix 'debugging' 'mouse events'
.bi This may be necessary if you are trying to debug mouse events
.bi received by your application.
This option ensures that the debugger
will not interfere with the mouse.

.note /DYnamic=number
.ix 'options' 'DYnamic'
.ix 'DYnamic option'
specifies the amount of dynamic storage that the debugger is to
set aside for its own use on startup.  The default amount that is
set aside is 500K bytes.  The larger the amount, the less memory
will be available for the application to be debugged.
.bi You only
.bi need to use this option if the debugger runs out of memory, or is
.bi causing your application to run out of memory.
If you are using the remote debugging feature, the debugger will use
as much available memory as available.

.note /NOExports
.ix 'options' 'NOExports'
.ix 'NOExports option'
specifies that no exports (system symbols) should be loaded.
It helps to speed up load time when debugging remotely and marginally
so when debugging locally.

.note /LOcalinfo=local_file
.ix 'options' 'LOcalinfo'
.ix 'LOcalinfo option'
is used primarily, but not exclusively, in conjunction with the remote
debugging capabilities of the debugger. It causes the debugger to use one or
more local files as sources of debugging information if the right
conditions are met. When the debugger observes that an executable file
or Dynamic Link Library (DLL) is being loaded with the same name
(i.e., the path and extension have been stripped) as one of the
.sy /localinfo
files, then the named local file is used as a source of debugging
information. The named file can be an executable file, a DLL file
(.dll), a symbolic information file (.sym), or any other file with
debugging information attached.
.tinyexam begin
wd /local=c:\dlls\mydll.sym /local=c:\exes\myexe.exe /tr=par myexe
.tinyexam end
.np
In the above example, the debugger would obtain debugging information
for any executable or DLL called
.mono myexe
or
.mono mydll
from
.fi c:\exes\myexe.exe
or
.fi c:\dlls\mydll.sym
respectively.
Note that no path searching is done for local files.
The debugger tries to open the file exactly as specified in the
.sy localinfo
option.
.np
See the section entitled :HDREF refid='vidrem'. for an explanation of
remote debugging.

.note /DOwnload
.ix 'options' 'DOwnload'
.ix 'DOwnload option'
specifies that executable file to be debugged is to be downloaded to
the task machine from the debugger machine.
The debugger searches for the executable file in the local path, and
downloads it to the debug server's current working directory on the
remote machine before starting to debug.
Debugging information is not downloaded, but is obtained locally, as
in the
.sy localinfo
option.
Note: Only the executable is downloaded; any required DLLs must be
present on the remote machine.
Downloading is relatively fast if you are using one of the TCP/IP
(TCP) or Netware (NOV) remote links.
Be sure to specify the file extension if it is not ".exe".
.exam begin
wd /tr=nov;john /download sample.exe
wd /tr=nov;john /download sample.nlm
.exam end
.np
The debugger does not erase the file when the debugging session ends.
So if you debug the application again, it will check the timestamp,
and if the file is up-to-date, it doesn't bother re-downloading it.
.np
See the section entitled :HDREF refid='vidrem'. for an explanation of
remote debugging.

.note /REMotefiles
.ix 'options' 'REMotefiles'
.ix 'REMotefiles option'
is used in conjunction with the remote debugging capabilities of the
debugger.  It causes the debugger to look for all source files
and debugger files on the remote machine.
When
.sy remotefiles
is specified, all debugger files (except "trap" files) and application
source files are opened on the task machine rather than the debugger
machine.
The trap file must be located on the debugger machine.
.np
The
.ev PATH
environment variable on the task machine is always used in locating
executable image files.
When
.sy remotefiles
is specified, the debugger also uses the task machine's
.ev PATH
environment variable to locate debugger command files.
See the section entitled :HDREF refid='vidrem'. for an explanation of
remote debugging.
See the section entitled :HDREF refid='rfxfile'. for an explanation of
remote and local file names.

.note /NOFpu
.ix 'options' 'NOFpu'
.ix 'NOFpu option'
requests that the debugger ignore the presence of a math coprocessor.

.note /NOSYmbols
.ix 'options' 'NOSYmbols'
.ix 'NOSYmbols option'
requests that the debugger omit all debugging information when loading
an executable image.
.bi This option is useful if the debugger detects and
.bi tries to load debugging information which is not valid.

.note /DIp=dipname
.ix 'options' 'DIp'
.ix 'DIp option'
used to load a non-default Debug Information Processor (DIP).  This option
is generally not needed since the debugger loads all DIPs that it finds
by default.
.refalso wndimg

.endnote
.*
.section *refid=dwopt DOS and Windows Options
.*
.np
The following switches apply to the DOS (binw&pc.&dbgcmd)
and Windows 3.x character-mode (binw&pc.&dbgcmd.c)
debuggers.
Refer to the sections called :HDREF refid='dosopt'. and
:HDREF refid='winopt'. for more switches relating to these
environments.
.begnote $break

.note /Monochrome
.ix 'options' 'Monochrome'
.ix 'Monochrome option'
When two display devices are present in the system, this option
indicates that the Monochrome display is to be used as the debugger's
output device, leaving the Color display for the application to use.
Use this option in conjunction with the
.sy Two
option described below.

.note /Color, /Colour
.ix 'options' 'Color'
.ix 'Color option'
.ix 'options' 'Colour'
.ix 'Colour option'
When two display devices are present in the system, this option
indicates that the Colour display is to be used as the debugger's
output device.
This option is used in conjunction with the
.sy Two
option described below.

.note /Ega43
.ix 'display' 'changing lines'
.ix 'screen' 'number of lines'
.ix 'options' 'Ega43'
.ix 'Ega43 option'
.ix 'EGA lines'
When an Enhanced Graphics Adapter (EGA) is present, 43 lines of
output are displayed by a character mode debugger.

.note /Vga50
.ix 'display' 'changing lines'
.ix 'screen' 'number of lines'
.ix 'options' 'Vga50'
.ix 'Vga50 option'
.ix 'VGA lines'
When a Video Graphics Array (VGA) is present, 50 lines of output
are displayed by a character mode debugger.

.note /Overwrite
.ix 'options' 'Overwrite'
.ix 'Overwrite option'
specifies that the debugger's output can overwrite program output.
In this mode, the application and the debugger are forced to share the
same display area.
.np
.bi Do not use this option if you wish to debug a DOS graphics-mode application.

.note /Two
.ix 'options' 'Two'
.ix 'Two option'
specifies that a second monitor is connected to the system.
If the monitor type
.sy (Monochrome, Color, Colour, Ega43, Vga50)
is not specified then the monitor that is not currently being used is
selected for the debugger's screen.
If the monitor type is specified then the monitor corresponding to that
type is used for the debugger's screen.
.bi This option may be used when debugging a DOS graphics-mode application on
.bi the same machine and a second monitor is available.

.endnote
.*
.section *refid=dosopt DOS Specific Options
.*
.np
Use the following switches for the DOS debuggers. For more DOS options,
refer to the section called :HDREF refid='dwopt'..
.begnote $break

.note /Page
.ix 'options' 'Page'
.ix 'Page option'
specifies that page 0 of screen memory is to be used for the
application's screen and that page 1 of screen memory should be used
for the debugger's screen.
This option may be selected when using a graphics adapter such as the
CGA, EGA or VGA.
Using the
.sy Page
option results in faster switching between the application and debugger
screens and makes use of the extra screen memory available with the
adapter.  This is the default display option.
.bi Do not use this option if you wish to debug a DOS graphics-mode application.

.note /Swap
.ix 'options' 'Swap'
.ix 'Swap option'
specifies that the application's screen memory and the debugger's
screen memory are to be swapped back and forth using a single page.
The debugger allocates an area in its own data space for the inactive
screen.
This reduces the amount of memory available to the application.
It also takes more time to switch between the application and debugger
screens.
.bi This option MUST be used when debugging a DOS graphics-mode application
.bi and a second monitor is not available.

.np
The default display options are:
.autopoint
.point
If you have a two display system, the debugger uses both displays with
the program output appearing on the active monitor and the debugger
output appearing on the alternate monitor.
In other words, the
.sy Two
option is selected by default.
.point
If you have one of the CGA, EGA or VGA graphics adapters installed in
your system then the debugger selects the
.sy Page
option by default.
.point
Under all other circumstances, the debugger selects the
.sy Swap
option by default.
.endpoint

.note /CHecksize=number
.ix 'options' 'CHecksize'
.ix 'CHecksize option'
.ix 'SYSTEM'
specifies the minimum amount of storage, in kilobytes, that the
debugger is to provide to DOS for the purpose of spawning a program
while the debugger is active.
.bi This option is useful when the application that is being debugged uses
.bi up most or all of available storage, leaving insufficient memory to
.bi spawn secondary programs.
In order to provide the requested amount of free memory to DOS, the
debugger will checkpoint as much of the application as required.
.np
Checkpointing involves temporarily storing a portion of the
memory-resident application on disk and then reusing the part of
memory that it occupied for the spawned program. When the spawned
program terminates, the checkpointed part of the application is
restored to memory.
.np
The default amount is 0K bytes. In this case, the spawned program may
or may not be run depending on how much free storage is available to
DOS to run the program.
.begnote
.note Warning:
If the application being debugged installs one or more interrupt
handlers, the use of this option could hang your system. Your system
could lock up if the debugger checkpoints a portion of the
application's code that contains an interrupt handler.
.endnote

.note /NOCHarremap
.ix 'options' 'NOCHarremap'
.ix 'NOCHarremap option'
turns off the character re-mapping that the DOS debugger uses for
displaying dialogs and window frames.
.bi Use this option when trying to debug in
.bi an environment where character remapping is not available.
Windowed DOS boxes under OS/2 do not support character re-mapping.

.note /NOGraphicsmouse
.ix 'options' 'NOGraphicsmouse'
.ix 'NOGraphicsmouse option'
Turn off the graphics mouse emulation code that makes the mouse
cursor look like an arrow instead of a block.
.bi Use this option
.bi if the mouse cursor appears as 4 line drawing characters
.bi instead of an arrow.

.endnote
.*
.section *refid=winopt Windows Specific Options
.*
.np
Use the following switches for the Windows character-mode debugger.
Refer to the section called :HDREF refid='dwopt'. for more Windows
options.
.begnote $break

.note /Fastswap
.ix 'options' 'Fastswap'
.ix 'Fastswap option'
specifies that Windows 3.x screen memory and the debugger's screen
memory are to be swapped back and forth using a technique that is
faster than the default method of screen swapping but not
guaranteed to work for all video adapters.  This option applies to
Windows 3.x only.  By default, the Windows 3.x version of the
debugger uses a more conservative (and slower) method that works
with all video adapters.

.endnote
.*
.section *refid=linuxopt Linux Options
.*
.np
You can use the following switch for the Linux debugger.
.begnote $break

.note -Console=console_spec
.ix 'options' 'Console'
.ix 'Console option'
specifies the virtual console to use for debugger windows.
This may be a console number as in the following example.
.exam begin
-console=2
.exam end
.np
You may also use a full device name.
.exam begin
-console=/dev/tty
.exam end
.np
In this case, the debugger will use that device for its input and
output. The debugger/application screen flipping features will be
disabled.
.np
You can also optionally follow the device name with a colon and a
terminal type.
.exam begin
-con=/dev/tty:vt240
.exam end
.pc
This will let the debugger know what kind of terminal it's talking to
so it can initialize the user interface appropriately.

.note -COlumns=n
.ix 'options' 'COlumns'
.ix 'COlumns option'
specifies the number of columns of the screen/window that the debugger
should attempt to establish.

.note -XConfig=string
.ix 'options' 'XConfig'
.ix 'XConfig option'
specifies a set of X Windows configuration options to pass to
.bd xterm
.ct .li .
The following example sets the
.bd xterm
font size to 12 point.
.exam begin
-xc=-fs -xc=12
.exam end

.endnote
.*
.section *refid=qnxopt QNX Options
.*
.np
You can use the following switch for the QNX debugger.
.begnote $break

.note -Console=console_spec
.ix 'options' 'Console'
.ix 'Console option'
specifies the virtual console to use for debugger windows.
This may be a console number as in the following example.
.exam begin
-console=2
.exam end
.np
You may also use a full device name.
.exam begin
-console=//23/dev/ser1
.exam end
.np
In this case, the debugger will use that device for its input and
output. The debugger/application screen flipping features will be
disabled.
.np
You can also optionally follow the device name with a colon and a
terminal type.
.exam begin
-con=/dev/ttyp1:vt240
.exam end
.pc
This will let the debugger know what kind of terminal it's talking to
so it can initialize the user interface appropriately.

.note -COlumns=n
.ix 'options' 'COlumns'
.ix 'COlumns option'
specifies the number of columns of the screen/window that the debugger
should attempt to establish.

.note -XConfig=string
.ix 'options' 'XConfig'
.ix 'XConfig option'
specifies a set of X Windows configuration options to pass to
.bd xqsh
.ct .li .

.endnote
.*
.section *refid=videvar Environment Variables
.*
.np
You can use the
.ev &dbgvarup.
environment variable to specify default options to be used by the
debugger. Once you have defined the environment variable, those
options are used each time you start the debugger.
.*
.beglevel
.*
.section &dbgcmdup Environment Variable
.*
.np
If the specification of an option involves the use of an "="
character, use the "#" character in its place. This is required by
the syntax of the "SET" command.  Options found in the environment
variable are processed before options specified on the command line.
The following example illustrates how to define default options for the
debugger:
.ix 'environment variables' '&dbgcmdup'
.ix '&dbgcmdup environment variable'
.ix 'options' 'default'
.exam begin
C>set &dbgcmd=/swap/lines#50
.exam end
.*
.section &dbgcmdup Environment Variable in Linux
.*
.np
.ix 'environment variables' '&dbgcmdup'
.ix '&dbgcmdup environment variable'
.ix 'options' 'default'
The following example illustrates how to define default options for the
debugger under Linux:
.exam begin
$ export &dbgcmdup="-rows=50 -cols=100"
.exam end
.pc
Under Linux, care must be taken to specify the environment variable name
entirely in uppercase letters.
.np
.*
.section &dbgcmdup Environment Variable in QNX
.*
.np
.ix 'environment variables' '&dbgcmdup'
.ix '&dbgcmdup environment variable'
.ix 'options' 'default'
The following example illustrates how to define default options for the
debugger under QNX:
.exam begin
$ export "&dbgcmdup=-nofpu -console=3"
.exam end
.pc
Under QNX, care must be taken to specify the environment variable name
entirely in uppercase letters.
.np
.endlevel
