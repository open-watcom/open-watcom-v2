.chap *refid=vidover Overview
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&dbgname' 'overview'
.ix 'overview'
The &dbgname. is a powerful debugging tool that helps you analyse your
programs and find out why they are not behaving as you expect. It
allows you to single step through your code, set break points based on
complex conditions, modify variables and memory, expand structures and
classes and much more. With the debugger you can debug programs that
run on the following platforms:
.ix 'platforms supported'
.begbull $compact
.bull
DOS
.bull
CauseWay DOS Extender
.bull
Tenberry Software DOS/4G Extender
.bull
Phar Lap DOS Extender
.bull
Windows 3.x
.bull
Windows NT/2000/XP
.bull
Windows 9x
.bull
16 and 32-bit OS/2
.bull
GNU/Linux
.bull
QNX 4
.bull
QNX 6 (Neutrino)
.bull
Novell NetWare
.endbull
.*
.section New Features
.*
.np
.ix 'features'
.ix 'new features'
The latest version of the debugger contains many new features that
you should know about.
.*
.beglevel
.*
.section User Interface
.*
.np
.ix 'user interface'
The debugger's user interface has been redesigned. There are GUI
versions of the debugger that run under Windows 3.x, Windows NT/2000/XP,
Windows 9x, and 32-bit OS/2. There are also character mode versions
that run under DOS, Windows 3.x, OS/2, Linux and QNX 4. All versions share a
common user interface incorporating powerful features like context
sensitive menus, eliminating the need for command oriented debugging.
.*
.section Reverse Execution
.*
.np
.ix 'reverse execution'
The debugger keeps a history of your interactions that modify the
state of the program you are debugging.  This includes the effects
of statements in your program that you trace.  The size of
this history is limited only by available memory.  Undo and Redo
allow you to step backward and forward through this history.  This
allows you to reverse the effects of tracing over simple
statements in your program.  You can also reverse any accidental interactions
that affect your program's state.
.refalso menuund
.*
.section Replay
.*
.np
.ix 'replay'
The debugger keeps a history of all interactions that affect
the execution of your program such as setting break points and
tracing.  Replay allows you to restart the application and run the
application back to a previous point.  This is particularly
useful when you accidentally trace over a call.  This replay information
may be saved to a file in order to resume a debugging session at a later
date.
.refalso wndrep
.*
.section Stack Unwinding
.*
.np
.ix 'stack unwinding'
.ix 'calls' 'unwinding stack'
You can navigate up and down the program's call stack to see
where the currently executing routine was called from.  As you do
this, all other windows in the debugger update automatically.
Local variables in the calling routines will be displayed along
with their correct values.
.refalso menuund
.*
.section Simplified Breakpoints
.*
.np
.ix 'breakpoints'
The debugger allows you to set breakpoints when code is executed or
data is modified.  These breakpoints may be conditional based on an
expression or a countdown.  Simple breakpoints are created with
a keystroke or single mouse click.  More complex breakpoints are
entered using a dialog.
.refalso dlgbrk
.*
.section Context Sensitive Menus
.*
.np
.ix 'context sensitivity'
Context sensitive menus are present in each debugger window.  To
use them, you select an item from the the screen using the right
mouse button.  A menu containing a list of actions appropriate for
that item is displayed.  You can use this capability to perform
actions such as displaying the value of an expression which you
have selected from the source window.
.*
.section Buttons
.*
.np
.ix 'buttons'
The debugger contains small buttons that appear on the left side
of some windows.  These buttons are shortcuts for the most common
operations.  For example, you can set and clear a breakpoint by
clicking on the button to the left of a source line.
.endlevel
.*
.section *refid=vidcomm Common Menu Items
.*
.np
.ix 'common menu items'
The debugger's context sensitive menus contain many useful
menu items.  Each of these items behave differently depending
upon the selected item.  A description of some of the commonly found
menu items follows:
.begmenu
.menuitem Inspect
Inspect displays the selected item.  The debugger determines how to
best display the selected item based on its type.  If you inspect a
variable or an expression, the debugger opens a new window
showing its value.  If you inspect a function, the debugger
positions the source code window at the function definition.  If
you inspect a hexadecimal address from the assembly window,
a window showing memory at that
address is opened, and so on.  Experimenting with inspect will help
you learn to use the debugger effectively.
.menuitem Modify
Modify lets you change the selected item.  You will normally be prompted
for a new value.  For example, select the name of a variable from any
window and choose Modify to change its value.
.menuitem New
New adds another item to a list of items displayed in a window.
For example, choosing New in the Break Point window lets you create
a new breakpoint.
.menuitem Delete
Delete removes the selected item from the window.
For example, you can use Delete to remove a variable from the Watches window.
.menuitem Source
Source displays the source code associated with the selected item.
The debugger will reposition the source code window at the
appropriate line.  Selecting a module name and
choosing Source will display the module's source code.
.menuitem Assembly
Assembly positions the assembly code window at the code associated with
the selected item.
.menuitem Functions
Functions shows a list of all functions associated with the
selected item or window.  For example, choose Functions in the
source window to see a list of all functions defined in that module.
.menuitem Watch
Watch adds the selected variable or expression to the Watches
window.  This allows you to watch its value change as the program
runs.
.bi Note that this is not a watchpoint.  Execution will not
.bi stop when the variable changes.
See the chapter entitled :HDREF refid='vidbrk'. for information
about setting watchpoints.
.menuitem Break
Break sets a breakpoint based on the selected item.  If a variable
is selected, the program will stop when the variable is modified.
If a function is selected, the program will stop when the function
executes.
.menuitem Globals
Globals shows a list of global variables associated with the selected
item.
.menuitem Show
Show will present a cascaded menu that let's you show things
related to the selected item.  For example, you can use
.menuref 'Line' 'Show'
in the source code window to see the line number of the selected line.
.menuitem Type
Type will present a cascaded menu that allows you to change the
display type of the window or selected item.
.endmenu
