.chap *refid=videnv The &dbgname. Environment
.*
.np
This chapter describes the interactions you need in order
to use the debugger.
.*
.section Debugger Windows
.*
.np
The debugger displays its information in windows. Both the character and
the GUI-based debuggers use similar conventions for window manipulation.
.*
.beglevel
.*
.section Window Controls
.*
.np
Each window has the following controls
.begnote $break
.note Minimize, Maximize, Restore
.ix 'window' 'minimizing'
.ix 'window' 'maximizing'
.ix 'window' 'restoring'
You can control the size of each window using the Minimize, Maximize,
and Restore buttons. The buttons appear on the top right corner of the
window. The Minimize button is the down arrow. When you click on the
down arrow, the window becomes an icon at the bottom of the screen.
The Maximize button is the up arrow. When you click on the up arrow,
the window fills the whole screen. The Restore button appears only
when the window is maximized. It is an up and down arrow. Click on the
Restore button to put the window back to its original size.
.note Close
.*
.ix 'window' 'closing'
Each window has a Close button in the top left corner. Double-click on
this button to close the window.
.note System Menu
The
.mi System Menu
.ix 'window' 'System Menu'
contains menu items that operate on the window. It contains:
.begbull $compact
.bull
.mi Restore
.bull
.mi Move
.bull
.mi Size
.bull
.mi Minimize
.bull
.mi Maximize
.endbull
.np
You can activate the System Menu of the main window by clicking once
on the System Menu button (top, left-hand corner) or by typing
ALT-Space.
For Microsoft Windows, you can type ALT-Hyphen to activate a child
window's System Menu.
.note Scroll Bars
.ix 'scroll bars'
Windows that contain information that cannot fit in the window have
scroll bars. Use the scroll bars to reposition the window so the
information you want to see is visible. The small box in the scroll
bar indicates the current scroll position in the window.
.note Title
Each window is titled so that you know what information it contains.
The title appears in the bar at the top of the window.
.note Buttons
.ix 'buttons'
Many windows have small buttons on the left hand side.
These buttons are short forms for performing the most common
operations.
.endnote
.*
.section The Current Window
.*
.np
.ix 'window' 'current'
The current window is the one whose title bar is coloured.
Press CTRL-TAB to move from window to window.
.*
.section Controlling the Size and Location of Windows
.*
.np
The following window operations are possible.
.begbull
.bull
:HDREF refid='wndmove'.
.bull
:HDREF refid='wndrsiz'.
.bull
:HDREF refid='wndzoom'.
.bull
:HDREF refid='menupop'.
.bull
:HDREF refid='txtsel'.
.endbull
.*
.beglevel
.*
.section *refid=wndmove Moving Windows
.*
.np
.ix 'window' 'moving'
To move a window, click in the Title bar and drag it to a new
location. You can also choose
.mi Move
from the
.mi System Menu
and use the cursor keys to reposition the window, pressing ENTER when
the window is in the right spot.
.*
.section *refid=wndrsiz Resizing Windows
.*
.np
.ix 'window' 'resizing'
In the GUI-based version of the debugger, you can resize a window's
width, height, or both. Refer to the system documentation for details.
.np
In the character-based version of the debugger, you can only resize a
window from the corners. Move the cursor to any corner of the window.
Click and drag the mouse to resize the window.
.np
You can also choose
.mi Size
from the
.mi System Menu
to change the size of a window. Use the cursor keys to resize the
window, press ENTER when the window is the right size.
.*
.section *refid=wndzoom Zooming Windows
.*
.np
.ix 'window' 'zooming'
Choose
.menuref 'Zoom' 'Window'
to toggle a window between its maximized and normal sizes.
.*
.section *refid=menupop Context Sensitive Pop-up Menus
.*
.np
The debugger has context sensitive pop-up menus for each window in the
application.
.bi You can access the menu either by pressing the right
.bi mouse button in the window or by typing the period (.) key.
You can then choose a menu item by typing the highlighted character or
by clicking the mouse on it.
.np
.ix 'accelerator' 'for pop-up menu'
.ix 'shortcuts' 'for pop-up menu'
.ix 'keyboard equivalents' 'for pop-up menu'
.ix 'control-key shortcuts'
.ix 'menu' 'shortcuts'
.ix 'menu' 'accelerator'
.ix 'menu' 'control-key shortcuts'
If you have memorized the highlighted menu character, you can bypass
the menu and activate the menu item directly by pressing the CTRL key
in conjunction with that character. The items that appear in the menu
depend on the current window. These menus are described in detail
throughout this document.
.begnote
.note Note
The
.mi Action
.ix 'menu' 'Action'
.ix 'Action menu'
item in the main menu is identical to the the context sensitive pop-up
menu for the current window and may be used instead of pop-up menus.
.endnote
.np
For more information on the choices presented in the pop-up menus, see
the section entitled :HDREF refid='wndvar'..
.*
.section *refid=txtsel Text Selection
.*
.np
.ix 'text' 'selecting'
.ix 'selecting text'
Some windows, such as the Source and Assembly windows, allow you to
select text. For example, you might want to select a variable name or
expression. Menu items will act on the selected item.
.np
You can select text with either the left or right mouse button. If you
use the right button, the pop-up menu appears when you release the
button. With the keyboard, hold SHIFT while using the cursor keys. You
can select a single character and the debugger will automatically
extend the selection to include the entire surrounding word.
.*
.endlevel
.*
.endlevel
.*
.section Menus
.*
.np
.ix 'menus'
.ix 'accelerator' 'for menu items'
.ix 'shortcuts' 'for menu items'
.ix 'keyboard equivalents' 'for menu items'
.ix 'menu' 'alt-key shortcuts'
.ix 'menu' 'shortcuts'
.ix 'menu' 'accelerators'
.ix 'menu' 'keyboard equivalents'
At the top of the debugger window are a number of menu items. You can
select a menu item with the mouse or by pressing ALT and the
highlighted character from the menu title.
.np
Many menu items have accelerators
or keyboard equivalents. They appear to the right of the menu item. As
you learn the debugger, take time to learn the accelerators. They will
help you to use the debugger more effectively.
.*
.section The Toolbar
.*
.figure *depth='3.40' *scale=65 *file='dbgshot' The Debugger Window
.np
The
.wnddef Toolbar
appears under the menu in the GUI-based debugger. The buttons in the
Toolbar
are equivalent to menu selections. There are eight buttons in the toolbar.
Listed from left to right, they are:
.begbull $compact
.bull
.menuref 'Go' 'Run'
.bull
.menuref 'Step Over' 'Run'
.bull
.menuref 'Trace Into' 'Run'
.bull
.menuref 'Until Return' 'Run'
.bull
.menuref 'Undo' 'Undo'
.bull
.menuref 'Redo' 'Undo'
.bull
.ix 'calls' 'unwinding stack'
.menuref 'Unwind Stack' 'Undo'
.bull
.menuref 'Rewind Stack' 'Undo'
.bull
.menuref 'Home' 'Undo'
.endbull
See the sections entitled :HDREF refid='menurun'. and
:HDREF refid='menuund'. for details.
.*
.section Dialogs
.*
.ix 'dialogs' 'general description'
.figure *depth='3.66' *scale=65 *file='dlgbrk' A Typical Dialog
.np
Dialogs appear when you choose a menu item that does not perform an
immediate action. They allow you to make choices and set options.
The dialogs contain the
following:
.begnote $break
.note Edit fields
These are fields in which you can type information.
.note Buttons
You can click on buttons to perform actions.
.note Default button
The default button in a dialog is highlighted. You can select this
button by pressing ENTER.
.note Cancel
All dialogs contain a cancel button. Choose the
Cancel button or press ESC to leave a dialog without saving or implementing
changes you have made to the dialog.
.note Check Boxes
Check boxes are used to control settings in the debugger.
Click on the field, or TAB to it and press SPACE to toggle the
option between on and off.
.note Radio Buttons
Radio buttons present a set of mutually exclusive choices.
Click on a radio button to turn it on or press TAB to move to the
group of radio buttons and use the cursor keys to select a radio
button.
If this does not work, use the accelerator key to turn on the
desired radio button.
Only one radio button is on at all times.
When you select a different radio button,
the currently selected one is turned off.
.note List boxes
A list box contains a list of applicable items.
.note Drop-down List boxes
A drop down list box is a list that does not appear on the screen
until you click on the down arrow on the right of the box.  You
may then select from a list of options.
.endnote
.*
.section Accelerators
.*
.np
.ix 'accelerators'
.ix 'shortcuts'
.ix 'keyboard equivalents'
.ix 'Turbo keyboard emulation'
.ix 'CodeView keyboard emulation'
Accelerators are keys that you can press in place of selecting a menu
item or typing commands.
The debugger comes with a standard set of accelerators that you can
view by choosing
.menuref 'Accelerators' 'Window'
.dot
.np
If you are used to the CodeView debugger, you should be comfortable
with the &dbgname.'s default set of accelerators.
If you are used to using Turbo Debugger, you can select accelerators
which are similar to its accelerator definitions.
To select Turbo accelerators, choose
.menuref 'Accelerator' 'Window'
then select
.menuref 'TD Keys' 'Action'
.dot
.*
.beglevel
.*
.section *refid=defacl Default Accelerators
.*
.np
The default accelerators are:
.begnote $compact $setptnt 15
.note /
Search/Find...
.note ALT-/
Search/Next
.note CTRL-\
Search/Next
.note ?
add a new expression to the Watch window
.note F1
invoke help facility
.note F2
Data/Registers
.note F3
toggle between source level and assembly level debugging
.note F4
Window/Application
.note F5
Run/Go
.note F6
Window/Next
.note F7
Run/Run to Cursor
.note F8
Run/Trace Into
.note F9
Break/Toggle
.note F10
Run/Step Over
.note SHIFT-F9
add a new item to the Watch window
.note CTRL-F4
close the current window
.note CTRL-F5
restore the current window to its normal size
.note CTRL-F6
rotate the current window
.note CTRL-F9
minimize the current window
.note CTRL-F10
maximize the current window
.note ALT-F10
display the floating pop-up menu for the current window
.note CTRL-TAB
rotate the current window
.note CTRL-LEFT
Undo/Undo
.note CTRL-RIGHT
Undo/Redo
.ix 'calls' 'unwinding stack'
.note CTRL-UP
Undo/Unwind Stack
.note CTRL-DOWN
Undo/Rewind Stack
.note CTRL-BACKSPACE
Undo/Home
.note ALT-1
Data/Locals
.note ALT-2
Data/Watches
.note ALT-3
Code/Source
.note ALT-4
File/View...
.note ALT-5
Data/Memory at...
.note ALT-6
Data/Memory at...
.note ALT-7
Data/Registers
.note ALT-8
Data/80x87 FPU
.note ALT-9
File/Command...
.note CTRL-z
Window/Zoom
.note SPACE
Run/Step Over
.note .
display the floating pop-up menu for the current window
.note :
File/Command...
.note =
Search/Match
.note n
Search/Next
.note N
Search/Previous
.note u
Undo/Undo
.note U
Undo/Redo
.note b
Break/At Cursor
.note e
Data/Memory at...
.note g
Run/Execute to...
.note h
move cursor left one
.note i
Run/Trace Into
.note j
move cursor down one
.note k
move cursor up one
.note l
move cursor right one
.note t
Break/Toggle
.note x
Run/Next Sequential
.endnote
.*
.section Turbo Emulation Accelerators
.*
.np
The Turbo emulation accelerators are:
.begnote $compact $setptnt 15
.note F2
Break/Toggle
.note F3
Code/Modules
.note F4
Run/Run to Cursor
.note F5
Window/Zoom
.note F6
Window/Next
.note F7
Run/Trace Into
.note F8
Run/Step Over
.note F9
Run/Go
.note ALT-F2
Break/New...
.note ALT-F3
close the current window
.note ALT-F4
Undo/Undo
.note ALT-F5
Window/Application
.note ALT-F7
trace one assembly instruction
.note ALT-F8
Run/Until Return
.note ALT-F9
Run/Execute to...
.note ALT-F10
activate the pop-up menu for the current window
.note CTRL-F2
Run/Restart
.note CTRL-F4
open a new Watch window
.note CTRL-F7
add a new item to the Watch window
.endnote
.*
.endlevel
.*
.section *refid=menufil The File Menu
.*
.np
The
.mm File
menu contains items that allow you to perform file operations, such
as:
.*
.begmenu File
.*
.menuitem Open
.*
.ix 'program' 'restarting'
.ix 'program' 'arguments'
.ix 'arguments' 'changing'
.ix 'parameters' 'changing'
.ix 'Restart'
Start debugging a new program, or to restart the current program
with new arguments.
.*
.menuitem View
.*
Display a file in a window.
.*
.menuitem Command
.*
Enter a debugger command. For a description of debugger commands,
refer to the section entitled :HDREF refid='vidcmd'..
.*
.menuitem Options
.*
.ix 'options' 'setting'
.ix 'settings'
Set the global debugging options. For a full description of these
options, refer to the section entitled :HDREF refid='dlgopt'..
.*
.menuitem Window Options
.*
Set the options for the debugger's various windows. For a full
description of these options, refer to the section entitled
:HDREF refid='dlgwopt'..
.*
.menuitem Save Setup
.*
Save the debugger's current configuration. This saves the positions
and sizes of all windows as well as all options and settings. By
default, this information is saved into the file
.fi setup.dbg
.ct ,
however, you can save this information into another file to
create alternate debugger configurations.
.*
.menuitem Load Setup
.*
Load a configuration previously saved using
.mi Save Setup.
.*
.menuitem Source Path
.*
.ix 'source' 'locating files'
Modify the list of directories which will be searched when the debugger
is searching for source files.
.*
.menuitem System
.*
The menu item appears only in the character-based version of the debugger.
It spawns a new operating system shell.
.*
.menuitem Exit
.*
Close the debugger.
.*
.endmenu
.*
.beglevel
.*
.section *refid=dlgopt The Options Dialog
.*
.ix 'options' 'dialog'
.figure *depth='3.0' *scale=60 *file='dlgopt' The Options Dialog
.np
The Options
dialog allows you to change the following settings:
.begnote
.ix 'configuration' 'automatic saving of'
.ix 'configuration' 'saving'
.ix 'settings' 'automatic saving of'
.ix 'settings' 'saving'
.note Auto configuration save
When this option is on, the debugger automatically saves its configuration
upon exit.
.ix 'Bell'
.ix 'option' 'Bell'
.note Warning Bell
When this option is on, the debugger will beep when a warning or error
is issued.
.ix 'invoke files'
.note Implicit Invoke
If this option is on, the debugger will treat an unknown command as
the name of a command file and automatically try to invoke it. If this
option is off, you must use the invoke command to invoke a command
file.
.np
Under UNIX, a conflict is possible when Invoke is on. A path specified
for a command file name is confused with the short form of the DO
command (/). A similar problem occurs under DOS, OS/2, Windows 3.x,
Windows NT, or Windows 95 when a drive specifier forms part of the
file name.
.ix 'Recursive functions' 'tracing over'
.ix 'Trace Over' 'recursive functions'
.note Recursion Check
Use this option to control the way tracing over recursive function
calls is handled. When this option is on, and you trace over a
function call, the debugger will not stop if the function executes
recursively.
.ix 'Break on write'
.ix 'option' 'Break on write'
.note Break on write (not change)
Use this option (if available with the selected trap) to enable true
break-on-write breakpoints. This will break on any write access; not just
when a watch point has changed.
.note Screen flip on execution
Use this option to control whether the debugger automatically flips the
display to the application's screen upon execution.
.bi Leave this option on if you are using the character mode debugger
.bi to debug a Windows 3.x application.
.note Ignore case
.ix 'search' 'ignoring case'
.ix 'case insensitive searching'
This option controls whether or not case is ignored or respected when
the debugger is searching for a string.
.note Do not expand hex numbers
This option controls whether or not hexadecimal values are displayed in
their natural size (zero preceded) or displayed in their most compact form.
The default is to display the value in its full natural size.
.ix 'Radix' 'default'
.ix 'Radix' 'setting'
.note Default Radix
Use this option to define the default radix used by the debugger. The
debugger associates a radix with each action automatically. For
example, if you are asked to enter an address, the debugger assumes
base 16. If you double click on a decimal value, you will be prompted
for a decimal replacement value but there are occasions when the
debugger must use the default radix. If you add an arbitrary
expression to the Watches window, the default radix is used when
interpreting that expression. You can specify any radix between 2 and
36.
.note Double click mS
This option sets the amount of time in milliseconds allowed between two clicks
for the debugger to accept it as a double click.  Enter a larger value
if you are having trouble with double clicks.
.endnote
.*
.section *refid=dlgwopt The Window Options Dialog
.*
.ix 'window' 'options'
.figure *depth='3.57' *scale=60 *file='dlgwopt' The Window Options Dialog
.np
Use the Window Options dialog to define options related to the
debugger's various windows.
All of these options appear in a dialog when you choose
.menuref 'Window Options' 'File'
.dot
.np
The Window Options dialog allows you to set options for the following
windows:
.begbull $compact
.bull
Source
.bull
Modules
.bull
Functions
.bull
Assembly
.bull
Watches
.bull
Locals
.bull
File Variables
.bull
Globals
.bull
Variable
.endbull
.*
.beglevel
.*
.section *refid=asmopt The Assembly Options
.*
.np
.ix 'Assembly options'
.ix 'options' 'Assembly window'
The Assembly options allow you to define how your assembly code
appears.
You can set the following options:
.begnote $break
.note Show Source
Turn on this option if you want source code intermixed with assembly
code.
.note Hexadecimal
Turn on this option if you want immediate operands and values to be
displayed in hexadecimal.
.endnote
.*
.section *refid=varopt The Variables Options
.*
.np
.ix 'Variables options'
.ix 'options' 'Variables window'
.ix 'options' 'Watches window'
Use the Variable options to set display options and to specify which
members of a class you want displayed when a structure or class is
expanded. You can set:
.begnote $break
.note Protected
Display protected members in expanded classes.
.note Private
Display private members in expanded classes.
.note Whole Expression
Turn this option on to show the whole expression used to access fields
and array elements instead of just the element number or field name
itself.
.note Functions
Display C++ member functions in expanded classes.
.note Inherited
Display inherited members in expanded classes.
.note Compiler
Display the compiler-generated members. You will usually not want this
option turned on.
.note Members
Display members of the 'this' pointer as if they were local variables
declared within the member function.
.note Static
Display static members.
.endnote
.*
.section *refid=fileopt The File Options
.*
.np
.ix 'File options'
.ix 'options' 'File window'
You can set the display width of a tab in the File options section.
This value defaults to 8 spaces.
.*
.section *refid=funcopt The Functions and Globals Options
.*
.np
.ix 'Functions options'
.ix 'Globals options'
.ix 'options' 'Functions window'
.ix 'options' 'Globals window'
For both Functions and Global Variables windows,
you can turn on the
.mi Typed Symbols
option.
This restricts the list of symbols to those that are defined in
modules compiled with full debugging information (d2 option).
.*
.section *refid=modopt The Modules Options
.*
.np
.ix 'Modules options'
.ix 'options' 'Modules window'
You can turn on
.mi Show All
to allow the Modules window to display all modules in your program,
not just those which have been compiled with the d2 option.
.*
.endlevel
.*
.endlevel
.*
.section *refid=menucod The Code Menu
.*
.np
The
.mm Code
menu allows you to display windows that show different information
related to your code. It contains the following items:
.*
.begmenu Code
.*
.menuitem Source
.*
Open the Source window. It shows source code at the currently
executing location.
.refalso wndsrc
.*
.menuitem Modules
.*
Display a sorted list of modules contained in the current
program.
.refalso wndmod
.*
.menuitem Functions
.*
Open a sorted list of all functions in the program.
.refalso wndfunc
.*
.menuitem Calls
.*
Open the Call History window. This window displays the program's call
stack.
.refalso wndcall
.*
.menuitem Assembly
.*
Open the Assembly window. It shows assembly code at the currently
executing location.
.refalso wndasm
.*
.menuitem Threads
.*
Open a list of all threads in your program and their current state.
.refalso wndthrd
.*
.menuitem Images
.*
Open a list of the executable images which are related to the
program being debugged.  This includes a list of all loaded DLLs.
.refalso wndimg
.*
.menuitem Replay
.*
Open the program execution Replay window.  This window allows you
to restart your application and replay your debugging session to any
point.
.refalso wndrep
.*
.endmenu
.*
.section *refid=menudat The Data Menu
.*
.np
The
.mm Data
menu contains a number of windows that you can open to view the state
of your program's data. It contains the following items:
.*
.begmenu Data
.*
.menuitem Watches
.*
.ix 'Variables' 'watching'
Open a Watches window. You can add and delete variables from the
Watches window and use it to evaluate complex expressions and perform
typecasting.
.refalso wndvar
.*
.menuitem Locals
.*
.ix 'Local variables'
.ix 'Variables' 'local'
Open a Locals window. It displays the local variables of the currently
executing function.
.refalso wndvar
.*
.menuitem File Variables
.*
.ix 'Variables' 'static'
.ix 'Variables' 'global'
Open a File Variables window.
It contains a list of variables defined at file scope in
the current module.
.refalso wndvar
.*
.menuitem Globals
.*
.ix 'Variables' 'global'
Open a sorted sorted list of all global variables in your program.
Values are not displayed since it would make this window very
expensive to update, but you can select variables from this window and
add them to a Watches window.
.refalso wndglob
.*
.menuitem Registers
.*
.ix 'Registers'
Displays the CPU registers and their values.
.refalso wndreg
.*
.menuitem FPU Registers
.*
Displays the FPU registers and their values.
.refalso wndfpu
.*
.menuitem MMX Registers
.*
Displays the MMX (multi-media extension) registers and their values.
.refalso wndmmx
.*
.menuitem XMM Registers
.*
Displays the XMM (SSE) registers and their values.
.refalso wndxmm
.*
.menuitem Stack
.*
.ix 'SP'
.ix 'ESP'
Displays memory at the stack pointer.
.refalso wndmem
.*
.menuitem I/O Ports
.*
Open a window that lets you manipulate the I/O address space of the
machine.
.refalso wndio
.*
.menuitem Memory at...
.*
.ix 'memory' 'displaying'
.ix 'memory' 'changing'
.ix 'changing memory'
Display memory at a given address.
.refalso wndmem
.*
.menuitem Log
.*
Displays debugger messages and the output from debugger commands.
.refalso wndlog
.*
.endmenu
.*
.section *refid=menuwnd The Window Menu
.*
.np
The
.mm Window
menu allows you to control and arrange the windows on your screen.
.np
The
.mi Window
menu contains the following items:
.*
.begmenu Window
.*
.menuitem Application
.*
Switch to the output screen of the application.
Press any key to return to the debugger.
.*
.menuitem To Log
.*
Save the current window's contents to the log window.
Open the Log window to see the contents.
.*
.menuitem To File
.*
Save the contents of the current window to a file. You must enter a
file name and choose the drive and directory to which you want to save
the information.  This is useful for comparing program state between
debugging sessions.
.*
.menuitem Zoom
.*
Change the size of the current window. Zoom toggles the current window
between its normal and maximum sizes.
.*
.menuitem Next
.*
Rotate through the windows, choosing a new current window.
.*
.menuitem Accelerator
.*
Open the Accelerator window. This window allows you to inspect and
modify the debugger's keyboard shortcut keys.
.*
.endmenu
.*
.section *refid=menuact The Action Menu
.*
.np
Most windows in the debugger have a context sensitive pop-up menu.
The
.mm Action
menu will contain the same menu items as the current window's pop-up
menu. It may be used as an alternative to the pop-up menus. As an
alternative to selecting text with the right mouse button and using
the pop-up menu, you can select text with the left mouse button or
keyboard and use the
.mi Action
menu.
For more information on the choices presented in the pop-up menus,
see the section entitled :HDREF refid='wndvar'..
.*
.section *refid=menuhlp The Help Menu
.*
.np
The
.mm Help
menu contains items that let you use the on-line help facility.
They are:
.*
.begmenu Help
.*
.menuitem Contents
.*
Show the main table of contents of the on-line help information. This
is equivalent to pressing F1.
.*
.menuitem On Help
.*
Display help about how to use the on-line help facility.  This menu item is
not available in character-mode versions of the debugger.
.*
.menuitem Search
.*
Search the on-line help for a topic.  This menu item is
not available in character-mode versions of the debugger.
.*
.menuitem About
.*
Display the "about box".  It contains the copyright and version
information of the debugger.
.*
.endmenu
.*
.section *refid=wndstat The Status Window
.*
.np
The
.wnddef Status
window appears at the bottom of the debugger screen.
As you drag the mouse over a menu item,
descriptive text about that menu item appears in the toolbar.
Messages about the current status of the program and debugger warning
messages also appear in the Status window.
.*
.section *refid=wndlog The Log Window
.*
.figure *depth='2.5' *scale=64 *file='dbglog' The Log Window
.np
Choose
.menuref 'Log' 'Data'
to see the
.wnddef Log
window.
The Log window displays several different types of messages,
including:
.begbull $compact
.bull
status messages such as break point notification
.bull
warning and error messages
.bull
output from debugger commands
.endbull
.np
You can send the contents of any window to the Log window by selecting
.menuref 'To Log' 'Window'
.dot
This allows you to save a window's contents and review it later.
.*
.section *refid=wndacl The Accelerator Window
.*
.ix 'accelerators'
.ix 'shortcuts'
.ix 'keyboard equivalents'
.figure *depth='3.50' *scale=65 *file='dbgacc' The Accelerator Window
.np
The
.wnddef Accelerator
window allows you to control the accelerators or keyboard equivalents
used by the debugger.
Choose
.menuref 'Accelerator' 'Window'
to open this window.
The window displays 4 items relating to each accelerator definition.
They are the key name, the window to which the accelerator applies,
the type of action that the accelerator defines, and the specifics of
that action.
.np
Accelerators may either apply to all windows or to a specific window.
You could define F2 to perform a different action depending upon which
window is current. Accelerators which apply to all windows will have a
window type of
.us all.
.np
An accelerator can define one of three action types.  They are:
.begnote $break
.note pop-up
Activate a pop-up menu item in the current window.
.note menu
Activate an item from the main menu.
.note command
Perform an arbitrary debugger command.
.endnote
.np
You can modify an element of an accelerator definition
by double-clicking on it, or by cursoring to it and pressing ENTER.
Press the right mouse button to access the following menu items:
.*
.begmenu Accelerator Pop-up
.*
.menuitem Modify
.*
Change the currently selected element of an accelerator assignment. If
the key name is selected, the you will be prompted to type a new key.
If the window name is selected, you will be presented with a list of
possible window classes. If the action type or details are selected,
you will be presented with a menu in order to pick the menu item which
will be attached to the accelerator.
.*
.menuitem New
.*
Add a new accelerator assignment.  You will be prompted for all details.
.*
.menuitem Delete
.*
Delete the selected accelerator.
.*
.menuitem TD Keys
.*
Use an approximation of Borland Turbo Debugger's accelerators.
.*
.menuitem WD Keys
.*
Use the default set of accelerators.  If you are familiar with CodeView,
you will be comfortable with these key assignments.
.*
.endmenu
