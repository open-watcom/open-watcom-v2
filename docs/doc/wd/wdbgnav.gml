.chap *refid=vidnav Navigating Through a Program
.*
.np
This section describes how to use the debugger to browse through your
program.
.*
.section *refid=menusch The Search Menu
.*
.np
.ix 'searching'
.ix 'strings' 'finding'
The
.mi Search
menu allows you to search a window for a given
string. It contains the following items:
.begmenu Search
.menuitem Find
Search the current window for the first appearance of a
given string. You will be prompted for the string.
.refalso dlgsrch
.menuitem Next
Find a subsequent occurrence of a search string.
.menuitem Previous
Find a previous occurrence of a search string.
.menuitem All Modules
.ix 'strings' 'matching incrementally'
.ix 'searching' 'incrementally'
This will search through the source code of
all the modules contained in your program for a given string.
.refalso dlgsrch
.menuitem Match
Find a string in a sorted window by incremental matching.
Once you select match, the text you type appears in the status window,
and the window you are searching repositions itself as
you type each character.  Press ESC to leave this mode.
.endmenu
.*
.beglevel
.*
.section *refid=dlgsrch Entering Search Strings
.*
.ix 'search' 'entering strings'
.ix 'strings' 'entering search'
.figure *depth='2.70' *scale=60 *file='dlgsrch' Entering a search string
.np
When you choose
.menuref 'Find' 'Search'
or
.menuref 'All Modules' 'Search'
.ct ,
you must enter the search string that you are looking
for and set the parameters for the search.
The Search screen consists of the following items:
.begnote $break
.note Enter Search String
Enter the string to be found in this edit box. The larger list below
shows other strings that you have searched for during this
debugging session. You can select these by clicking on them or by
using the up and down arrow keys.
The most recent search string appears at the top of the list.
.note Regular Expression
Check this box if the string is to be interpreted as a regular expression.
You can click on the Edit button to edit the set of regular expression
characters that will be used. For a description of regular expressions,
see the Editor manual.
.note Ignore Case
.ix 'searching' 'ignoring case'
Check this box if you want the debugger to match the search string
regardless of case.
.endnote
.*
.endlevel
.*
.section *refid=wndsrc The Source Window
.*
.figure *depth='2.80' *scale=70 *file='dbgsrc' The Source Window
.np
The
.wnddef Source
window displays your program's source code.  As you trace through
your program, it repositions itself at the currently execution location.
If you have an Assembly window open, the Source and Assembly windows will
always be positioned at the same code.  If you scroll in one, the other
follows.
.np
Source lines that have code associated with them have a button at the
left of the screen. You can click on this button to set, disable and
clear break points.
.np
You can Inspect any item displayed in the source window by
double-clicking on it, or by cursoring to it and pressing ENTER.
Press the right mouse button to access the following pop-up menu items:
.begmenu Source Pop-up
.menuitem Inspect
.ix 'variables' 'inspecting'
.ix 'functions' 'inspecting'
Inspect the selected item in an appropriate window.
You can select function names, variable
names, or any valid expression.
.menuitem Run to Cursor
Resume program execution until the selected line is executed.
.menuitem Break
.ix 'variables' 'break on write'
.ix 'breakpoint' 'on write'
Add a breakpoint based on the selected text.
If a variable is selected, the program will
stop when its value changes. If a function name is selected the program will
stop when that function is executed.
.bi This does not set a break at the current line.
Use
.menuref 'Toggle' 'Break'
or
.menuref 'At Cursor' 'Break'
to set a breakpoint at the current line.
.menuitem Enter Function
.ix 'run' 'until function entered'
Resume program execution until the selected function is entered.
.menuitem Watch
.ix 'expressions' 'evaluating'
.ix 'expressions' 'watching'
Add the selected item to the Watches window for further inspection
or modification.
.menuitem Find
Search for other occurrences of
the selected string in the Source window.
.menuitem Home
Reposition the window to show the currently executing location. The cursor will move to
the next line of the program to be executed.
.menuitem Show/Assembly
Show the assembly code associated with the selected line.
.menuitem Show/Functions
.ix 'functions' 'showing list of'
Show the list of all functions contained in the source file.
.menuitem Show/Address
.ix 'source code' 'examining at address'
Reposition the window at a new address.  You will be prompted for an
expression.  Normally you would type a function name but you can type
any expression that resolves to a code address.
For example, you might
type the name of a variable that contains a pointer to a function.
.refalso videxpr
.menuitem Show/Module
.ix 'source code' 'examining a module'
Show the code for a different module. You will be prompted for its
name in a dialog. As a shortcut, you can type the beginning of a
module name and click the Module... button. This will display a list
of all modules that start with the text you typed.
.menuitem Show/Line
.ix 'source code' 'displaying line number'
.ix 'source code' 'going to line number'
Move to a different source line.  You can also find
out what line you are looking at. The edit field will be initialized with
the current line number.
.endmenu
.*
.section *refid=wndfil The File Window
.*
.np
A
.wnddef File
.ix 'file' 'viewing'
window is Similar to a source window except that it displays a file
which is not part of the program being debugged. Menu items related to
execution such as
.popup Break
are not available.
.*
.section *refid=wndmod The Modules Window
.*
.figure *depth='2.80' *scale=70 *file='dbgmod' The Modules Window
.np
The
.wnddef Modules
.ix 'modules' 'showing list of'
window displays a list of the modules that make up the current
program. To open the Modules window, choose
.menuref 'Modules' 'Code'
.ct .li .
.np
Three items are displayed for each module.  At the left, there is
a button.  You can click the mouse on it to see the source or
assembly associated with the module.  This can also be
accomplished by double-clicking on the module name or cursoring to
it and pressing ENTER.  Next is the module name. Third, if the
module is contained in an executable image other than the one
being debugged, is the name of that image.
.np
Since this window is sorted
.menuref 'Match' 'Search'
can be used to find a module.  Choose
.menuref 'Match' 'Search'
(or press =) and begin
typing the name of the module.
.np
Press the right mouse button to access the following pop-up menu items:
.begmenu Modules Pop-up
.menuitem Source
Show the source code associated with the selected module.
.menuitem Assembly
Show the assembly code associated with the selected module.
.menuitem Functions
Show the list of all functions contained in this module.
.menuitem Break All
Set a breakpoint at each function in this module.
.menuitem Clear All
Delete all breakpoints which are set at addresses with this module.  This
does not affect break-on-write break points.
.menuitem Show All
Toggle between showing all modules and just modules which were compiled
with full debugging information (d2).
:INCLUDE file='wdbgopt.gml'
.endmenu
.*
.section *refid=wndglob The Globals Window
.*
.figure *depth='2.20' *scale=70 *file='dbgglob' The Globals Window
.np
.ix 'global variables' 'showing list'
You can open the
.wnddef Globals
window by choosing
.menuref 'Globals' 'Data'
.dot
This window displays the names of all global variables defined in the program.
You can add a variable to the Watches window by
double-clicking on it, or cursoring to it and pressing ENTER.
.np
Press the right mouse button to access the following pop-up menu items:
.begmenu Globals Pop-up
.menuitem Watch
.ix 'variables' 'displaying'
.ix 'global variables' 'displaying'
Add the selected variable to the Watches window.
.menuitem Raw Memory
Display the memory associated with the selected variable.
.menuitem Typed Symbols
Toggle between showing all symbols and just those defined in modules
compiled with the d2 option.
Variables from the C/C++ library and assembly code are suppressed.
:INCLUDE file='wdbgopt.gml'
.endmenu
.*
.section *refid=wndfunc The Functions Window
.*
.figure *depth='2.40' *scale=70 *file='dbgfunc' The Functions Window
.np
.ix 'functions' 'showing list of'
The
.wnddef Functions
window can display a list of all functions contained in a module,
executable image or program. To the left of each function name is a
button. You can click on these buttons to set and clear breakpoints at
the various functions. This can also be accomplished by
double-clicking on the function name or cursoring to a function and
pressing ENTER.
.np
Press the right mouse button to access the following pop-up menu items:
.begmenu Functions Pop-up
.menuitem Break
Set a breakpoint at the selected function.   A dialog will appear so that
you can fill in detailed breakpoint information.  For more
information, refer to the section entitled :HDREF refid='dlgbrk'..
.menuitem Source
Show the source code for the selected function.
.menuitem Assembly
Show the assembly code associated with the selected function.
.menuitem Typed Symbols
Toggle between showing all symbols and just those defined in modules
compiled with the d2 option.
Variables from the C/C++ library and assembly code are suppressed.
:INCLUDE file='wdbgopt.gml'
.endmenu
.*
.section *refid=wndimg The Images Window
.*
.figure *depth='2.00' *scale=70 *file='dbgimg' The Images Window
.np
.ix 'DLL' 'showing list of'
.ix 'DLL' 'debugging'
.ix 'debugging' 'DLLs'
.ix 'NLM' 'showing list of'
.ix 'images' 'showing list of'
Choose
.menuref 'Images' 'Code'
to open the
.wnddef Images
window.  It displays a list of executable images
associated with the program that you are currently debugging.
Executable images include the program executable, DLLs (Windows,
OS/2 and Windows NT), and NLMs (NetWare). This window
displays the name of the executable image, the name of the symbolic
debugging information file (if available), and the debugging
information type.
.np
Different debugging information types are generated by different compilers.
.np
.ix 'debugging information'
Valid information types are:
.begnote $break
.note DWARF
This information is generated by the &company compilers.
.note Watcom
This information is optionally generated by the &company compilers.
.note CodeView
In addition to &company compilers, several other products, including
Microsoft's, can generate CodeView style information.
.note MAPSYM
This information is generated by Microsoft's or IBM's MAPSYM utility.
MAPSYM processes linker map file and outputs a .sym file. Symbol files
in MAPSYM format are often available for OS/2 system DLLs. MAPSYM files
only contain information about global symbols, but usually provide much
more detail than just exports information.
.note EXPORTS
This information is contained in the executable file itself, and is
used by the operating system.  Under OS/2, Windows and Windows NT,  DLLs
have export tables which define the names and addresses of entry points.
Exports information lets
you see the names of system entry points and APIs.
Novell NLMs also have entry point tables.  In addition, they may
have Novell style debugging information, created with Novell's
linker (NLMLINK) or using the &company Linker's "debug novell" option.
This information is made available to the debugger.
.endnote
.np
You can add new debugging information to an image by double-clicking
on the image name or cursoring to it and pressing ENTER.
.np
Press the right mouse button to access the following pop-up menu items:
.begmenu Images Pop-up
.menuitem New Symbols
Add symbolic debugging information
for the selected image.  This is useful if you know that a separate debug
information file contains the appropriate debugging information that
was not found by the debugger.
.menuitem Delete Symbols
Delete any symbolic debugging information associated with
the selected image.
.menuitem Modules
Show a list of modules contained in the selected image.
.menuitem Functions
Show a list of functions contained in the selected image.
.menuitem Globals
Show a list of all global variables contained in the selected image.
.endmenu
