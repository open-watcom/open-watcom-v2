.chap *refid=vidcmd Debugger Commands
.*
.np
This section describes the syntax of debugger commands as well as
a description of each of the debugger commands.
.*
.section Syntax Definitions
.*
.np
.ix 'command' 'syntax'
.ix 'syntax' 'for commands'
A debugger command may contain any of the following syntax elements:
.begbull
.bull
A word in angle brackets, like <anything> is a defined term. Its
definition will appear after the syntax description of the command.
.bull
[x] indicates that "x" is an optional item. It may or may not be
included in the command.
.bull
[x|y|z] indicates that on of x, y or z should be included in the
command.
.bull
[x [x [...]]] indicates that x may be repeated zero or more times in
the command.
.bull
CApital indicates that ca,cap,capi,... are accepted short forms for
the command "capital".
.bull
(GUI only) indicates that this command is only available in a GUI
debugger.
.bull
(character-based) indicates that this command is only available in a
character mode debugger.
.bull
<expr> indicates an expression. These may include any of the
variables, etc in the program being debugged, and are evaluated in the
current program context.
.refalso videxpr
.bull
<integer> is an integer constant
.bull
<intexpr> is an an expression which evaluates to an integral value.
.refalso videxpr
.bull
<command> is any debugger command or group of debugger commands.
.np
You can group debugger commands with braces and separate them with
semi-colons. The resulting compound command may be considered as an
atomic command.
.code begin
{<command>;<command>;<command>}
.code end
.bull
<address> is any expression which evaluates to an address.
.refalso videxpr
.bull
<string> is a string of text, optionally enclosed in braces. For
example,
.code begin
this_is_a_string
{so is this}
.code end
.bull
<wndname> is the name of a window. Valid window names (with acceptable
short forms indicated in capitals) are:
.begbull $compact
.bull
ASsembly
.bull
ALl
.bull
BReak
.bull
Calls
.bull
Watch
.bull
FIle
.bull
FPu
.bull
FUnctions
.bull
FILEScope
.bull
LOCals
.bull
LOG
.bull
MEmory
.bull
MOdules
.bull
Register
.bull
SOurce
.bull
STack
.bull
Thread
.bull
IO
.bull
Globals
.bull
Variable
.bull
BInary
.bull
IMage
.bull
GLobalfunctions
.bull
Accelerators
.bull
TMPFile
.bull
REPlay
.bull
CUrrent
.endbull
.np
.bull
<file> represents any valid operating system file name. For example,
.code begin
c:\autoexec.bat
.code end
.bull
<path> represents
any valid operating system directory path. For example,
.code begin
c:\dir1\dir2
.code end
.endbull
.*
.section Command Summary
.*
.np
.ix 'command' 'summary'
A summary of each command follows.
.beglevel
.cmddef Accelerate
.np
This command behaves as if a menu item from the main menu was selected:
.code begin
Accelerate main <menu> {<menu_string>}
.code end
.np
This command behaves as if the named menu item in the
floating pop-up menu for the current window was selected:
.code begin
Accelerate {<menu_string>}
.code end
.begnote $break
.note <menu>
the string appearing on the main menu bar (File, Run, Break, Code, etc)
.note <menu_string>
is enough of the text appearing in a menu to uniquely identify it.
.endnote
.np
For example:
.code begin
accelerate main run {until return}
.code end
.pc
behaves as if "Until return" is selected from the run menu
.code begin
accelerate {Home}
.code end
.pc
behaves as if "home" were picked from the floating pop-up menu of the
current window.

.cmddef Break
.np
This command prints a list of all breakpoints into the log window:
.code begin
Break
.code end
.np
This command sets a break point.  See the section entitled
:HDREF refid='vidbrk'. for details about breakpoint operation.
.code begin
Break [|/Set|/Byte|/Word|/DWord|/Modify]
          <address> [ {<do_command>} [ { <condition> } [ <countdown> ]]]
.code end
.np
This command deactivates a breakpoint:
.code begin
Break/Deactivate <brkid>
.code end
.np
This command enables a breakpoint:
.code begin
Break/Activate <brkid>
.code end
.np
This command clears a breakpoint:
.code begin
Break/Clear <brkid>
.code end
.np
This command toggles a breakpoint through the active/inactive/deleted
states:
.code begin
Break/Toggle <brkid>
.code end
.np
This command turns on the resume option in the breakpoint:
.code begin
Break/Resume <brkid>
.code end
.np
This command turns off the resume option in the breakpoint:
.code begin
Break/UnResume <brkid>
.code end
.np
This command turns on/off the breakpoint for appropriate image(DLL) load:
.code begin
Break/Image [/Clear] <image(DLL) name>
.code end
.np

The Break options are:
.begnote $break $compact
.note /Set (default)
the breakpoint triggers when <address> is executed
.note /Byte
the breakpoint triggers when the byte at <address> is modified
.note /Word
the breakpoint triggers when the word at <address> is modified
.note /DWord
the breakpoint triggers when the double word at <address> is modified
.note /Modify
the breakpoint triggers when integer at <address> is modified
.note <condition>
an expression that must be true (non-zero value)
before the breakpoint stops program execution
.note <countdown>
an integer.  The breakpoint will not stop program
execution until <countdown> is decremented to zero.
.begnote
.note Note:
If you specify both <condition> and <countdown>, <countdown>
decrements only when <condition> evaluates to true.
.endnote
.note <do_command>
a command that is executed each time the breakpoint stops
program execution
.note <brkid>
option can be three possible values:
.begnote
.note <address>
Perform the operation on breakpoint with the given address.
.note *
Perform the operation on all breakpoints.
.note #<integer>
Names a breakpoint by its index.  This index can be discovered on
the title line of the Breakpoint dialog.
.endnote
.endnote
.np
Some examples of the break command and a description follow:
.np
This command sets a breakpoint at "foo" the 20th time that i equals 10.
When this occurs 'do j7' is executed:
.code begin
Break /Set foo {do j7} {i10} 20
.code end
.np
This command clears the breakpoint at foo:
.code begin
Break /Clear foo
.code end
.np
This command activates breakpoint #1:
.code begin
Break /Activate #1
.code end
.np
This command deactivates all breakpoints:
.code begin
Break /Deactivate *
.code end

.cmddef Call
.np
Use the Call command to call a routine. The Call command options are:
.code begin
Call [/Far|/Interrupt|/Near]
         <address>
         [([<parm>[,<parm>[...]]])] [/|<printlist>]
.code end
.np
This command calls the routine at <address> with parameters.
.begnote
.note /Far
Use a far call instruction.
.note /Near
Use a near call instruction.
.note /Interrupt
Call the routine as if it were an interrupt handler.
.note <parm>
is [/<location>] <expr>
.note <location>
is [/|<regset>]
.begnote
.note /
means to put the parm on the stack.
.note /<regset>
means to put the parm into the named registers.
.note <regset> is a register aggregate.
.refalso videxpr
.endnote
.note <printlist>
See the print command for details.
.endnote
.np
Some examples of the Call command follow:
This command calls the function foo:
.code begin
call foo
.code end
.np
This command calls the function bar passing the parameters 1, 2, and 3:
.code begin
call bar( 1,2,3 )
.code end
.np
This command calls foo putting 1 on the stack, 2 in AX and 3 in CX:BX
printing out the value of AX and DX in decimal and hexadecimal
respectively on return:
.code begin
call /near foo( // 1, /ax 2, /[cx bx] 3 ) {%d %x} ax,dx
.code end
.np
The Call command only uses very basic symbolic information - it's
designed that way so that it can work even when no symbolic
information is available.
This has a number of implications.
The first is that the debugger pays no attention to any information on
where parameters are stored.
For example, in 32-bit applications, unless explictly instructed
otherwise, the first parm is placed in EAX, the second in EDX, and so
on (as defined by the "set call" command).
That means that you have to do something like:
.code begin
call foo( // &a, // &b )
.code end
.pc
to get things on to the stack.
This leads to a second, very important consideration.
.np
The debugger has no idea of the memory model that the program is
compiled in (recall that the 32-bit compiler does support large memory
models and far pointers, even if we don't supply versions of
the libraries for it).
That means that the debugger has no idea on whether the address of a
symbol should be far or near.
It always assumes far, since that never loses information.
A far pointer would be truncated to a near pointer when moved into a
32-bit register like EAX but not so when pushed onto the stack.
In this case,
.id // &a
and
.id // &b
cause 48-bit far pointers to be pushed onto the stack (they are
actually pushed as 64 bits for alignment reasons).
Thus the pointer to
.id b
is in the wrong place for the routine to access it (assuming it is
expecting near pointers) and this will likely cause a task exception.
To avoid this problem and properly pass arguments to the routine, you
need to do the following:
.code begin
call foo( // (void near *)&a, // (void near *)&b )
.code end
.pc
This forces the debugger to push near pointers onto the stack.
.np
Similar considerations apply for the 16-bit case.

.cmddef CAPture
.np
Use the Capture command to execute a command and put the resulting
program output into a window. The format of the command follows:
.code begin
CAPture <command>
.code end
.np
For example, this command calls a routine, foo, and puts its output
into a debugger window.
.code begin
capture call foo
.code end

.cmddef COnfigfile
.code begin
COnfigfile
.code end
.np
Used by the debugger to save and restore the configuration. When
"configfile" appears in a command file, it identifies that file as the
default configuration file. The debugger will overwrite the command
file when autosaving the current configuration. Also, the name of this
file is displayed in the filename field when the "Save Setup" dialog
initially appears.
.np
If more than one file is encountered containing the "configfile"
command, the last one seen is used to establish the configuration
file name.

.cmddef Display
.np
The display command allows you to open any window.
The general Display command is:
.code begin
Display <wndname> [/Open|/Close|/New|/MInimize|/MAximize|/Restore] [<ord>,<ord>,<ord>,<ord>]
.code end
.np
This command causes the debugger screen to repaint:
.code begin
Display
.code end
.np
This command displays the toolbar as either fixed (default) or floating:
.code begin
Display TOolbar [/Open] [/FLoating/Fixed] [<ord>]
.code end
.np
This command closes the toolbar:
.code begin
Display TOolbar [/Close]
.code end
.np
This command opens the status line:
.code begin
Display Status [/Open]
.code end
.np
This command closes the status line:
.code begin
Display Status /Close
.code end
.np
This command brings a window to the front:
.code begin
Display <wndname>
.code end
.np
The options for the Display command follow:
.begnote $break
.note <ord>
The height to be used for toolbar buttons.
.note <ord>,<ord>,<ord>,<ord>
These are the x and y coordinates of the top left
corner, and the width and the height of the window respectively.
0,0,10000,10000 is a window covering the entire screen.
.note /Open
Open a new window or resize an existing one.
.note /New
Open a new window regardless of an existing one.
.note /Close
Close the window.
.note /MInimize
Iconize the window.
.note /MAximize
Make the window full screen size.
.note /REstore
Restore a window from a minimize or maximize.
.endnote
.np
Some examples of the display command follow:
This command opens a register window in the top left quarter of the
screen:
.code begin
display register /open 0,0,5000,5000
.code end
.np
This command minimizes the source window if it is open:
.code begin
display source /minimize
.code end

.cmddef DO (or /)
.np
Use the DO command to evaluate an arbitrary C/C++ or FORTRAN expression.
The format of the command is:
.code begin
DO <expr>
.code end
.np
For example:
.code begin
DO i = 10
.code end

.cmddef ERror
.np
Use the Error command to display a string as an error message. The
format of the command is:
.code begin
ERror <string>
.code end
.np
For example:
.code begin
error {An error has been detected}
.code end

.cmddef Examine
.np
Use the Examine command to examine memory at a specific address.
.code begin
Examine [/<type>] [<address>] [,<follow> [,<len>]]
.code end
.pc
where "<type>" is one of
.code begin
Byte
Word
Dword
Qword
Char
Short
Long
__int64
Unsigned_Char
Unsigned_Short
Unsigned_Long
Unsigned___int64
0:16_Pointer
16:16_Pointer
0:32_Pointer
16:32_Pointer
Float
Double
Extended_Float
.code end
.np
To show an assembly window at a specific address:
.code begin
Examine /Assembly [<address>]
.code end
.np
To show a source window at a specific address
.code begin
Examine /Source [<address>]
.code end
.np
To add an address to the I/O window as a byte, word, or dword:
.code begin
Examine [/IOByte|/IOWord|/IODword] [<address>]
.code end
.np
The options for the Examine command follow:
.begnote $break
.note /<type>
where "<type>" is one of
.mono Byte,
.mono Word,
.mono Dword,
.mono Qword,
.mono Char,
.mono Short,
.mono Long,
.mono __int64,
.mono Unsigned_Char,
.mono Unsigned_Short,
.mono Unsigned_Long,
.mono Unsigned___int64,
.mono 0:16_Pointer,
.mono 16:16_Pointer,
.mono 0:32_Pointer,
.mono 16:32_Pointer,
.mono Float,
.mono Double,
or
.mono Extended_Float.
Set the initial display type of the memory window.
.note /IOByte /IOWord /IODword
Set the initial display type of the line in the I/O window.
.note <address>
the address to examine.
.note <follow>
an expression which will be used if the memory window's Repeat
function is chosen.
.note <len>
an integer expression indicating the length of memory to examine.
.endnote
.np
For example, this command opens a memory window positioned at the
address of "foo".  The initial display type will be 2 byte words.
If the
.mi Repeat
menu item is used, it will follow a near pointer 4 bytes past the
beginning of the window *(.+$). The window will display 16 bytes of
data at a time:
.code begin
examine /word foo,*(.+4),16
.code end

.cmddef Flip
.np
Use the Flip command to configure screen flipping.  See the section
entitled :HDREF refid='dlgopt'. for details
.code begin
Flip ON
Flip OFf
.code end

.cmddef FOnt
.np
Use the Font command to set the font for the specified window. The
command is:
.code begin
FOnt <wndname> <fontinfo>
.code end
.begnote
.note <wndname>
the name of the affected window
.note <fontinfo>
operating system specific font data.
.endnote

.cmddef Go
.np
Use the Go command to start or continue program execution. Execution
then resumes at the specified address or at the location defined by
the current contents of the CS:IP or CS:EIP register pair. The format
of the Go command is:
.code begin
Go [/Until] [/Keep] [/Noflip] [[<start>,]<stop>]
.code end
.np
The options are:
.begnote $break
.note /Until
.ix 'until'
skips breakpoints until the specified stop address is reached.
.note /Keep
.ix 'keep'
allows you to keep a previous temporary breakpoint.  If you keep
the previous breakpoint you cannot create a new one.
.note /Noflip
.ix 'noflip'
keeps the debugger from flipping to the application's screen.
.note <start>
the <address> at which to start execution (optional).
.note <stop>
the <address> at which to stop execution.
.endnote
.np
Some examples of the Go command are:
.np
This command will resume execution until function "foo" is executed
without flipping to the application screen:
.code begin
go /noflip foo
.code end
.np
This command starts execution at "foo" and runs until "bar" is executed.
.code begin
go foo,bar
.code end

.cmddef Help
.np
Bring up the help screen:
.code begin
Help
.code end

.cmddef HOok
.np
Use the Hook command to execute a command when a defined event occurs.
The format of the Hook command is:
.code begin
HOok <event> <command>
.code end
.np
<event> can be any of the following:
.begnote $break
.note PROGStart
a program is loaded
.note PROGEnd
a program terminates
.note DLLStart
a DLL is loaded
.note DLLEnd
a DLL is unloaded
.note EXECStart
program execution is beginning
.note EXECEnd
program execution is stopped
.note Sourceinfo
the current location being examined has debugging information
.note Assemblyinfo
the current location being examined has no debugging information
.note Modulechange
the current location being examined has changed modules
.endnote
.np
This example causes the locals and source window to
come to the front whenever a region with symbolic
debugging information is entered:
.code begin
hook sourceinfo {display locals; display source}
.code end

.cmddef IF
.np
Use the If command to evaluate an expression and then, depending on the
results, execute a list of commands. The format of the If command is:
.code begin
IF <expr> { <command> }
[ ELSEIF <expr> { <command> } [ ELSEIF <expr> { <command> } [...]]]
[ELSE { <command> }]
.code end
.np
If the expression results in a non-zero value, the list of debugger
commands contained after the IF expression are executed. Otherwise,
the list of commands that appear after the ELSEIF expression are
executed.

.cmddef INvoke (or <)
.np
Use the Invoke command to invoke a file containing a number of
debugger commands. The format of the Invoke command is:
.code begin
INvoke <file> [<string> [<string> [...]]]
<      <file> [<string> [<string> [...]]]
.code end
.begnote
.note <file>
is the name of the command file to invoke.
.note <string>
will be passed as a parameter.  These parameters may be
referenced in the command file as <1>, <2>, etc.
.endnote

.cmddef Log (or >)
.np
Use the Log command to send the Dialog window output to a specified
file. The following commands start logging to a file:
.code begin
Log        <file>
Log /Start <file>
>          <file>
.code end
.np
The following commands start appending log information to a file.
.code begin
Log >       <file>
>>          <file>
Log /Append <file>
.code end
.np
The following commands stop logging:
.code begin
Log
>
.code end

.cmddef MOdify
.np
Use the Modify command to change memory at an address to the values
specified by the list of expressions.
.code begin
MOdify [/Byte|/Pointer|/Word|/Dword|/IOByte|/IOWord|/IODword] <address>[,<expr>[...]]
.code end
.np
The options for the modify command are:
.begnote
.note /Byte /Pointer /Word /Dword
Control the size of memory to be modified.
.note /IOByte /IOWord /IODword
Control the size of the I/O port to be modified.
.note <address>
The address to modify.
.note <expr>
The values to be placed in memory.
.endnote
.np
This command changes the 3 bytes at location "foo" to the values 1, 2
and 3:
.code begin
modify /byte foo 1,2,3
.code end
.np
This command changes the 4 bytes at location "foo" to the value
12345678 hex:
.code begin
modify /dword foo 0x12345678
.code end

.cmddef NEW
.np
Use the New command to initialize various items. The format of the New
command is:
.code begin
NEW [<args>]
NEW /Program [[:<symfile>] <progfile> [<args>]]
NEW /Restart [<args>]
NEW /STDIn <file>
NEW /STDOut <file>
NEW /SYmbol <file> [seg [,seg [...]]
.code end
.np
.begnote
.note <symfile>
represents a file containing the symbolic information.
.note <progfile>
represents the executable file.
.note <args>
represent the arguments to be passed to the program.
.note /Restart
Reload the current application and place it into an initial
state so that you may begin execution again. The application may have
already partially or completely executed.
.note /STDIn
associate the standard input file handle with a particular file
or device.
.note /STDOut
associate the standard output file handle with a particular
file or device.
.note /Symbol
load additional symbolic debugging information and specify the
mapping between the linker addresses and the actual execution addresses.
.endnote

.cmddef PAint
.np
Use the Paint command to define window or dialog
colours. To define the colour for windows, use the following command:
.code begin
PAint [Status|<wndname>] <wndattr> <color> ON <color>
.code end
.np
To define the colour for dialogs in the character-based version of
the debugger, use the following command:
.code begin
PAint DIalog <dlgattr> <color> ON <color>
.code end
.np
The paint options are as follows:
.begnote
.note <wndattr>
allows you to define the window attributes. You can
choose from the following items:
.begnote
.note MEnu Plain
menu text (character-based)
.note MEnu STandout
menu accelerator key (character-based)
.note MEnu Disabled
grayed menu item (character-based)
.note MEnu Active
menu item under the cursor (character-based)
.note MEnu Active STandout
menu accelerator key under the cursor (character-based)
.note MEnu Frame
frame of the menu (character-based)
.note MEnu Disabled Active
grayed menu item under the cursor (character-based)
.note TItle Disabled
a non active window's title
.note Frame Active
the frame of the active window (character-based)
.note Frame Disabled
the frame an inactive window (character-based)
.note ICon
an icon
.note Plain
normal text within a window
.note Active
window text under the cursor
.note SElected
window text being selected
.note STandout
window text the debugger wishes to highlight
.note Active STandout
window text the debugger wishes to highlight under the cursor
.note BUtton
the gadgets on the left side of a window (character-based)
.endnote
.note <dlgattr>
option allows you to define the dialog attributes. The possible
options are:
.begnote
.note Plain
normal text
.note Frame
the dialog frame
.note SHadow
the shadow of a button
.note BUtton Plain
normal button text
.note BUtton STandout
button accelerator key character
.note BUtton Active
a button which has focus
.note BUtton Active STandout
button accelerator key character of a button with focus
.endnote
.note <color>
You can choose from the following colours:
.begbull
.bull
BLAck
.bull
BLUe
.bull
GREEn
.bull
Cyan
.bull
Red
.bull
MAgenta
.bull
BROwn
.bull
White
.bull
GREY
.bull
GRAy
.bull
BRIght BLUe
.bull
BRIght GREEn
.bull
BRIght Cyan
.bull
BRIght Red
.bull
BRIght MAgenta
.bull
Yellow
.bull
BRIght BROwn
.bull
BRIght White
.endbull
.endnote
.np
Some examples of the paint command follow:
.code begin
paint all plain black on white
paints plain text black on white in all windows.
paint dialog button standout bright green on yellow
.code end

.cmddef Print (or ?)
.np
Use the Print command to prompt for an expression and then print it
to the log window. Use this command to examine the values of
variables and expressions. The Print command is:
.code begin
Print [/Program] [<printlist>]
Print /Window [<exprlist>]
.code end
.begnote
.note /Window
opens up a watch window containing the listed expressions.
.note /Program
print the results to the application's screen.
.note <printlist>
is [<format>] [<exprlist>]
.note <exprlist>
is [<expr> [,<expr> [...]]]
.note <format>
is a printf like format string.  It consists of plain
text intermixed with control sequences, which will
be substituted with values from the expression list.
The control sequences are:
.begnote
.note %i
The corresponding argument is printed out as a signed decimal integer
value.
.note %d
The corresponding argument is printed out as a signed decimal integer
value.
.note %u
The corresponding argument is printed out as an unsigned decimal
integer value.
.note %x
The corresponding argument is printed out as an unsigned hexadecimal
integer value.
Letter digits are printed in lower case (a-f).
.note %X
The corresponding argument is printed out as an unsigned hexadecimal
integer value.
Letter digits are printed in upper case (A-F).
.note %o
The corresponding argument is printed out as an unsigned octal integer
value.
.note %p
The corresponding argument is printed out as a pointer
(segment:offset) value in hexadecimal notation.
.note %c
The corresponding argument is printed out as a single character value.
.note %s
The corresponding argument is printed out as a C/C++ string value.
The argument must point to a string of characters terminated by a byte
whose value is zero.
.note %%
To print out a percentage symbol, the "%" must be doubled up (i.e.,
%%).
.note %f
The corresponding argument is printed out in floating-point
representation.
If the floating-point value has a very large or small magnitude, you
should use one of "g", "G", "e" or "E" formatting.
.note %g
The corresponding argument is printed out in floating-point
representation.
Numbers of very large or small magnitude are printed out in scientific
"E" notation (e.g., 1.54352e+16).
The exponent letter is printed in lower case.
.note %G
The corresponding argument is printed out in floating-point
representation.
Numbers of very large or small magnitude are printed out in scientific
"E" notation (e.g., 1.54352E+16).
The exponent letter is printed in upper case.
.note %e
The corresponding argument is printed out in scientific "E" notation
(e.g., 1.23456e+02).
The exponent letter is printed in lower case.
.note %E
The corresponding argument is printed out in scientific "E" notation
(e.g., 1.23456E+02).
The exponent letter is printed in upper case.
.note %r
The corresponding argument is printed out in the current default
numeric radix.
.note %a
The corresponding argument is printed out as a symbol reference
(symbol_name+offset) when possible; otherwise it is printed out
as a pointer (segment:offset) value in hexadecimal notation.
.note %l
The corresponding argument is printed out as a line number reference
(module_name@line_number+offset) when possible; otherwise it is
printed out as a pointer (segment:offset) value in hexadecimal
notation.
.endnote
.endnote
.np
Some examples of the print command follow.
This command prints the value of "i":
.code begin
? i
.code end
.np
This command prints "decimal=100 hex=0x64":
.code begin
print {decimal=%d hex=%x} 100,100
.code end
This command opens a watch window and displays the value of argv[0]:
.code begin
print/window argv[0]
.code end

.cmddef Quit
.np
Use the Quit command to leave the debugger.

.cmddef RECord
.np
Use the Record command to add a command to the replay window. This
command is for internal use only. The format
of the command is:
.code begin
REcord<expr> <command>
.code end

.cmddef Register
.np
The format of the Register command is:
.code begin
Register <intexpr>
.code end
.np
If intexpr is negative, this is
equivalent to using the menu item Undo/Undo -<intexpr> times.
If intexpr is positive, this is
equivalent to using the menu item Undo/Redo <intexpr> times.

.cmddef REMark (or *)
.np
Use the Remark command to enter lines of comments. The format of the
command is:
.code begin
REMark <string>
.code end

.cmddef Set
.np
These commands are used internally by the debugger to save and restore
the configuration. The syntax is:
.code begin
Set AUtosave [ON|OFf]
Set ASsembly [Lower|Upper] [Outside|Inside] [Source|NOSource] [Hexadecimal|Decimal]
Set Variable [Entire|Partial] [CODe|NOCODe] [INherit|NOINherit] [COMpiler|NOCOMpiler] [PRIvate|NOPRIvate] [PROtected|NOPROTected] [Members|NOMembers]
Set FUnctions [Typed|All]
Set GLobals [Typed|All]
Set REGister [Hexadecimal|Decimal] [Extended|Normal]
Set Fpu [Hexadecimal|Decimal]
Set BEll [ON|OFf]
Set BReakonwrite [ON|OFf]
Set Call [/Far|/Interrupt|/Near] [ ( [<location> [,<location> [...]]] ) ]
Set Dclick <expr>
Set Implicit [ON|OFf]
Set INput <wndname>
Set Radix <expr>
Set RECursion [ON|OFf]
Set SEarch [CASEIgnore|CASEREspect] [Rx|NORx] <string>
Set SOurce [/Add] [<path> [<path>] [...]]]
Set SYmbol [/Add|/Ignore|/Respect] [<lookspec> [<lookspec> [...]]]
Set Tab <intexpr>
Set Level [Assembly|Mixed|Source]
Set LAnguage [CPP|C|FORTRAN]
Set SUpportroutine <string>
Set MAcro <wndname> <key> <command>
Set DOntexpandhex [ON|OFf]
.code end
.begnote
.note <location>
see call command.
.note <lookspec>
[/Ignore|/Respect] <string>
.endnote

.cmddef SHow
.np
The Show commands are used internally by the debugger to save and
restore its configuration. The syntax is:
.code begin
SHow Paint
SHow Display
SHow Font
SHow Set
SHow Set AUtosave
SHow Set ASsembly
SHow Set Variable
SHow Set FUnctions
SHow Set GLobals
SHow Set REGister
SHow Set Fpu
SHow Set BEll
SHow Set BReakonwrite
SHow Set Call
SHow Set Dclick
SHow Set Implicit
SHow Set INput
SHow Set Radix
SHow Set RECursion
SHow Set SEarch
SHow Set SOurce
SHow Set SYmbol
SHow Set Tab
SHow Set Level
SHow Set LAnguage
SHow Set MAcro
SHow Set SUpportroutine
SHow Flip
SHow Hook
SHOW DOntexpandhex
.code end

.cmddef SKip
.np
Use the Skip command to set CS:EIP to a specific address. The format
of the command is:
.code begin
SKip <address>
.code end

.cmddef STackpos <intexpr>
.np
The Stackpos command is the same as using Undo/Unwind. The <intexpr>
allows you to define the number of times to undo or unwind.

.cmddef SYstem (or !)
.np
Use the System command to spawn an operating shell to execute a given
string. The format of the system command is:
.code begin
SYstem [/Remote|/Local] <string>
.code end
.np
.begnote
.note /Remote
the shell is started on the program side of a remote debug link.
.note /Local
the shell is started on the debugger side of a remote debug link.
.endnote

.cmddef THread (or ~~)
.np
Use the Thread command to manipulate the threads of execution of a
multi-threaded application under OS/2 or NetWare 386. The format of the
Thread command is:
.code begin
THread [/Show|/Freeze|/Thaw|/Change] [<threadid>]
.code end
.begnote
.note /Show
display the status of the current thread.
.note /Freeze
freeze a thread and make it unrunnable.
.note /Thaw
make a frozen thread runnable.
.note /Change
to select a specific thread.
.note <threadid>
is the identification number of the thread.
.endnote

.cmddef Trace
.np
Use the Trace command to step through the execution of your program. The
Trace command is:
.code begin
Trace [/Assembly|/Mixed|/Source] [/Into|/Next|/Over]
.code end
.np
.begnote
.note /Assembly
trace through your assembly code on instruction
at a time.
.note /Mixed
trace execution of the application one source statement at a time,
or one instruction at a time when no source text is available.
.note /Source
trace execution of the application one source statement at a time.
.note /Into
continue execution to the next statement or assembly instruction.
If the current statement or instruction invokes a routine, then the next
statement or instruction is the first one called in the routing.
.note /Next
continue execution to the next statement or assembly instruction
that immediately follows the current statement or instruction in memory.
If the current statement or instruction is one that branches, be sure
that the execution path eventually executed the statement
or instruction that follows. If the program does not executed
this point, the program may execute to completion.
.note /Over
continue execution to the next statement or
assembly instruction. If the current statement or instruction
invokes a routine, then the next statement or instruction
is the one that follows the invocation of the routine.
.endnote

.cmddef Undo
.np
The format of the Undo command is:
.code begin
Undo <intexpr>
.code end
.np
If intexpr is positive, this is
equivalent to using the menu item Undo/Undo <intexpr> times.
If intexpr is negative, this is
equivalent to using the menu item Undo/Redo -<intexpr> times.

.cmddef View
.np
Use the View command to show a file in a window. The
format of the command is:
.code begin
View [/Binary] [<file>|<module>]
.code end
.begnote
.note /Binary
show the file contents in binary.
.note <file>
the file to be shown.
.note <module>
the module to be shown.  The default is the current module.
.endnote

.cmddef While
.np
Use the While command to permit the execution of a list of commands
while the specified expression is true. The While command is:
.code begin
While <expr> { <command> }
.code end

.cmddef WIndow
.np
This command operates on the current window. It is useful when
defining accelerators that perform window operations.
.begnote $break
.note WIndow CLose
close the window
.note WIndow CURSORStart
move the cursor to start of line
.note WIndow CURSOREnd
move the cursor to end of line
.note WIndow CURSORDown
move the cursor down one line
.note WIndow CURSORLeft
move the cursor left
.note WIndow CURSORRight
move the cursor right
.note WIndow CURSORUp
move up one line
.note WIndow Dump
dump the window to a file
.note WIndow Log
dump the window to a log window
.note WIndow FINDNext
find the next occurrence of the current search string
.note WIndow FINDPrev
find the previous occurrence of the current search string
.note WIndow Next
make another window the current window
.note WIndow PAGEDown
move the window down one page
.note WIndow PAGEUp
move the window up one page
.note WIndow POpup
show the window's floating pop-up menu
.note WIndow SEarch
search for a given string
.note WIndow SCROLLDown
scroll the window down one line
.note WIndow SCROLLUp
scroll the window up one line
.note WIndow SCROLLTop
scroll the window to the very top
.note WIndow SCROLLBottom
scroll the window to the very bottom
.note WIndow TABLeft
move to the previous tabstop
.note WIndow TABRight
move to the next tabstop
.note WIndow MAXimize
maximize the window
.note WIndow MINimize
minimize the window
.note WIndow REStore
restore the window
.note WIndow TIle
tile all windows
.note WIndow CAscade
cascade all windows
:cmt..note WIndow Move
:cmt.not yet implemented
:cmt..note WIndow SIze
:cmt.not yet implemented
.note WIndow PRevious
move to the previous window
.endnote
.endlevel
