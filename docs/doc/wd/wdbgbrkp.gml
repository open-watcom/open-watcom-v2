.chap *refid=vidbrk Breakpoints
.*
.np
.ix 'breakpoint' 'defined'
.ix 'watchpoint' 'defined'
.ix 'tracepoint' 'defined'
The &dbgname. uses the single term breakpoint to refer to the group of
functions that other debuggers often call breakpoints, watchpoints,
and tracepoints.
.np
A breakpoint is traditionally defined as a place in your program where
you want execution to stop so that you can examine program variables
and data structures. A watchpoint causes your program to be executed
one instruction or source line at a time, watching for the value of an
expression to become true. Do not confuse a watchpoint with the watch
window. A tracepoint causes your program to be executed one
instruction or source line at a time, watching for the value of
certain program variables or memory-referencing expressions to change.
.np
In the &dbgname.:
.ix 'breakpoint' 'on execute'
.ix 'breakpoint' 'on write'
.ix 'Variables' 'stopping on write'
.begbull $compact
.bull
Break-on-execute refers to the traditional breakpoint
.bull
Break-on-write refers to the traditional tracepoint
.bull
A traditional watchpoint is a break-on-execute or break-on-write that
is coupled with a condition
.ix 'watchpoint'
.endbull
.np
The &dbgname unifies these three concepts by defining three parts to a
breakpoint:
.begbull $compact
.bull
the location in the program where the breakpoint occurs
.bull
the condition under which the breakpoint is triggered
.bull
the action that takes place when the breakpoint triggers
.endbull
.np
You can specify a countdown, which means that a condition must be true
a designated number of times before the breakpoint is triggered.
.ix 'breakpoint' 'countdown'
.ix 'breakpoint' 'condition'
.np
When a breakpoint is triggered, several things can happen:
.begbull $compact
.bull
program execution is stopped (a breakpoint)
.bull
an expression is executed (code splice)
.bull
a group of breakpoints is enabled or disabled
.endbull
.np
In this chapter, you will learn about the breakpoint including how to
set simple breakpoints, conditional breakpoints, and how to set
breakpoints that watch for the exact moment when a program variable,
expression, or data object changes value.
.*
.section How to Use Breakpoints during a Debugging Session
.*
.np
The following topics are discussed:
.begbull
.bull
:HDREF refid='dbgbrks'.
.bull
:HDREF refid='dbgbrkc'.
.endbull
.*
.beglevel
.*
.section *refid=dbgbrks Setting Simple Breakpoints
.*
.np
.ix 'breakpoint' 'setting simple'
When debugging, you will often want to set a few simple breakpoints to
make your program pause execution when it executes certain code. You
can set or clear a breakpoint at any location in your program by
placing the cursor on the source code line and selecting
.menuref 'Toggle' 'Break'
or by clicking on the button to the left of the source line. You can
set breakpoints in the assembly window in a similar fashion. Setting a
break-on-write breakpoint is equally simple. Select the variable with
the right mouse button and choose
.popup Break
.dot
.np
.ix 'breakpoint' 'state'
Break points have three states.  They are:
.begbull $compact
.bull
enabled
.bull
disabled
.bull
cleared (non-existent)
.endbull
.np
The button for an enabled break point is a stop sign or [!].
The button for a disabled break point is a grey stop sign or [.].
A green diamond or [ ] appears when no breakpoint exists at the given
line. The same buttons also appear in the Assembly window and the
Break window to indicate the status of a break point.
.begnote
.note Note:
Some lines in your program do not contain any machine code to execute
and therefore, you cannot set a breakpoint on them. The compiler does
not generate machine code for comments and some C constructs. All
lines of code in your program that can have a breakpoint on them have
a button to the left of the source line. You can click on them to
change their current status.
.endnote
.*
.section *refid=dbgbrkc Clearing, Disabling, and Enabling Breakpoints
.*
.np
.ix 'breakpoint' 'clearing'
.ix 'breakpoint' 'disabling'
.ix 'breakpoint' 'enabling'
Choosing
.menuref 'Toggle' 'Break'
(F9) toggles between the three different breakpoint states:
.begbull $compact
.bull
enabled
.bull
disabled
.bull
cleared (non-existent)
.endbull
.*
.endlevel
.*
.section *refid=menubrk The Break Menu
.*
.np
You can use the
.mi Break
menu to control your breakpoints. Operations including creating new
breakpoints, changing a breakpoint's status, and viewing a list of all
break points.
.*
.begmenu Break
.*
.menuitem Toggle
.*
.ix 'breakpoint' 'toggling'
Change the status of the breakpoint at the current line in the source
or assembly window. The status alternates between enabled, disabled
and cleared. The button on the source or assembly line will be updated
to reflect the status of the breakpoint.
.begnote
.note Note:
Disabled and cleared breakpoints are not the same. If you disable a
breakpoint, you can re-enable it and retain the information about the
breakpoint (i.e., conditions, countdown, and actions). When you clear
a breakpoint, you lose all information about the breakpoint. If you
disable a breakpoint, and press F9 twice to enable, you will lose the
information about the breakpoint because you cleared it before you
re-enabled it. To enable a disabled breakpoint without losing the
breakpoint information, use the Breakpoint Option dialog or the
Breakpoint window.
.endnote
.*
.menuitem At Cursor
.*
.ix 'breakpoint' 'at cursor position'
Set a breakpoint at the current line in the source or assembly window.
If the current line does not contain any executable code, the
breakpoint is set on the closest preceding line of code that does
contain executable code. When you choose
.mi At Cursor
.ct ,
the Breakpoint dialog appears.
.*
.menuitem New
.*
.ix 'breakpoint' 'setting'
This allows you to create any type of breakpoint using a dialog. You
must specify the address in the dialog.
.*
.menuitem On Image Load...
.*
.ix 'breakpoint' 'on image load'
Cause program execution to stop when an executable image (DLL) is
dynamically loaded. The menu item is only available when debugging an
Win32 or OS/2 executable. A dialogue will appear allowing you to add
and delete image names from the list. You only need to type a
substring of the actual image name. You can identify the file
"C:\PATH\IMAGE.DLL" with any substring, for example "IMAGE",
"IMAGE.DLL" or "ATH\IMAGE.DLL". Case is ignored in the image names.
.*
.menuitem On Debug Message
.*
.ix 'breakpoint' 'on debug message'
When checked, cause program execution to stop whenever Windows 3.1,
Windows NT, or Windows 95 prints a debug string. A debug string is
printed whenever the application or debug Kernel calls the
.kw OutputDebugString
function.
This option is toggled each time it is selected from the
.mi Break
menu.
.*
.menuitem View All
.*
.ix 'breakpoint' 'displaying'
Open the breakpoint window. This window will show a list of all
breakpoints. You can use the window to create, delete and modify
breakpoints.
.*
.menuitem Clear All
.*
.ix 'breakpoint' 'clearing all'
Clear all breakpoints.
.*
.menuitem Disable All
.*
.ix 'breakpoint' 'disabling all'
Disable all breakpoints, but do not delete them.
.*
.menuitem Enable All
.*
.ix 'breakpoint' 'enabling all'
Enable all breakpoints that are disabled.
.*
.menuitem Save
.*
.ix 'breakpoint' 'saving'
Save all breakpoint information to a file. This is useful when you are
creating complicated breakpoints. You can save and restore them in a
later debugging session.
.*
.menuitem Restore
.*
.ix 'breakpoint' 'restoring'
Restore a set of breakpoints that were saved by using
.menuref 'Save' 'Break'
.dot
.*
.endmenu
.*
.section *refid=wndbrk The Break Window
.*
.ix 'breakpoint' 'window'
.figure *depth='2.50' *scale=65 *file='dbgbrk' The Break Window
.np
The
.wnddef Break
window displays each breakpoint and its status. It appears when you
select the
.menuref 'View All' 'Break'
A breakpoint button appears at the left of each line. You can click on
this button to enable and disable a breakpoint. Unlike the source and
assembly windows, the button will not clear the breakpoint. Next
appears the address of the breakpoint. Finally, for break-on-execute
breakpoints, the source or assembly code at the break point location
is displayed. For break-on-write breakpoints, the current value of the
location is displayed in hex.
.np
You can modify any break point by double clicking on it, or by
cursoring to it and pressing enter. The Breakpoint Options dialog will
appear to allow you to modify the break point. Press the right mouse
button to access the following pop-up menu items:
.*
.begmenu Break Pop-up
.*
.menuitem Modify
.*
.ix 'breakpoint' 'changing'
Change the definition of the selected breakpoint. The Breakpoint dialog
will appear.
.*
.menuitem New
.*
.ix 'breakpoint' 'creating new'
Add a new breakpoint. An empty Breakpoint dialog will appear. You must
specify the address of the new Breakpoint. Refer to the section
entitled :HDREF refid='dlgbrk'. for a description of the items in the
which appear in the dialog.
.*
.menuitem Delete
.*
.ix 'breakpoint' 'deleting'
Delete the selected breakpoint.
.*
.menuitem Enable
.*
.ix 'breakpoint' 'enabling'
Enable the selected breakpoint.
.*
.menuitem Disable
.*
.ix 'breakpoint' 'disabling'
Disable the selected breakpoint.
.*
.menuitem Source
.*
.ix 'breakpoint' 'finding source code'
Display the source code associated with the break point. This
operation only makes sense for break-on-execute breakpoints.
.*
.menuitem Assembly
.*
.ix 'breakpoint' 'finding assembly code'
Display the assembly code associated with the selected line. This
operation only makes sense for break-on-execute breakpoints.
.*
.endmenu
.*
.section *refid=dlgbrk The Breakpoint Dialog
.*
.figure *depth='3.70' *scale=70 *file='dlgbrk' The Breakpoint Dialog
.np
The breakpoint dialog appears when you select
.menuref 'At Cursor' 'Break'
or
.menuref 'New' 'Break'
and whenever you attempt to modify a break point. It allows you to
define the breakpoint and set all of its conditions. A description of
the items in the dialog follows.
.begnote $break
.note Address
.ix 'breakpoint' 'specifying address'
This edit field displays the address tag associated with the selected
breakpoint.
.np
When you choose
.mi At Cursor
this field already contains an address that describes the line of code
that the cursor is on. The format of the address tag is
.monoon
symbol+offset
.monooff
where
.monoon
symbol
.monooff
is the name of the nearest function and
.monoon
offset
.monooff
is distance in bytes past that symbol where the break point is
defined. It is normally best NOT to edit this field. To change the
line of source code, leave the dialog, move the cursor to where you
want the breakpoint, and use the
.mi At Cursor
command again.
.np
When you choose
.mi New
.ct ,
this field is empty. You can type any valid address expression in this
field. It can be the name of a function, global variable. Refer to the
section entitled :HDREF refid='videxpr'. for more information about
address expressions.
:INCLUDE file='symbuttn.gml'.
.begnote
.note Note:
Be careful when using local (stack) variables for a break-on-write
breakpoint. As soon as execution leaves the scope of the variable, the
memory will change at random since the variable does not really exist
any more and the memory will be used for other variables. Also, if
execution enters that variable's scope again, the variable may not
have the same memory address.
.endnote
.note Condition
.ix 'breakpoint' 'condition'
Use this field to enter a conditions that must be met before a
breakpoint will trigger. The condition can be an arbitrary debugger
expression. These include statements in the language you are
debugging. A valid example for the C language is
.monoon
i == 1.
.monooff
.note Break on Execute
.ix 'breakpoint' 'on execute'
Check this field to create a break-on-execute breakpoints. If you
choose Execute, be sure that the address field contains a code address
(function name or code line number) and not a variable address.
Variable are never executed. If the address field names a variable,
the breakpoint will never trigger.
.note Break on 1 Byte/2 Bytes/4 Bytes/8 Bytes...
.ix 'breakpoint' 'on write'
Check one of these fields to create break-on-write breakpoints. If you
choose one of these options, be sure that the Address field contains a
variable address and not a code address. A code address will never be
written to, so the breakpoint will never trigger. The size of the
memory location is defined by the checkbox you use as follows:
.begnote $compact
.note 1 Byte
The breakpoint will trigger only when the first byte
of the memory address is written to.
.note 2 Bytes
The breakpoint will trigger when either of the first two bytes
at the memory address are written to.
.note 4 Bytes
The breakpoint will trigger if any of the first four bytes
of the memory address are written to.
.note 8 Bytes
The breakpoint will trigger if any of the first eight bytes
of the memory address are written to. This option is only supported
by certain trap files and will be grayed out if the selected trap file
does not support it.
.endnote
.begnote
.note Note:
In older versions of the debugger and traps, the break on write feature
is really a break on change: the debugger will only stop execution if the
breakpoint is hit and the value of the watched data has been changed.
For the current version of the debugger this is the still the default
behaviour but can be modified. Refer to :HDREF refid='dlgopt'. to change 
the breakpoint behaviour to true break-on-write.
.endnote
.note Countdown
.ix 'breakpoint' 'countdown'
Use this field to enter the number of times an address must be hit
before the breakpoint triggers. Every time the breakpoint conditions
are met, the countdown decreases by one. The breakpoint will trigger
only after the countdown is at zero. Once the countdown reaches zero,
the breakpoint will trigger each time the conditions are met. If you
have also set a condition, the countdown will only decrease by one
when the condition is true.
.note Total Hits
.ix 'breakpoint' 'counting'
This field displays the total number of times an address has been hit.
This includes the times the breakpoint does not trigger because a
condition failed or the countdown has not yet hit zero.
.note Reset
Click on this button to reset the Total Hits field to zero.
.note Execute when Hit
.ix 'breakpoint' 'executing debugger commands'
Use this field to enter a debugger command. When the breakpoint is
triggered, the debugger will execute this command. You can use this
field to execute arbitrary C statements, change a variable or
register, or even set other breakpoints. For a more detailed
description of commands that can be entered in this field, refer to
the section called :HDREF refid='vidcmd'.. If you want to use this
field to execute a statement in the language you are debugging, you
need to use a DO command in front of the statement. For example, you
could enter
.monoon
DO i = 10
.monooff
to have the value of 10 assigned to i each time the breakpoint
triggered.
.note Resume
Check this field if you want the program to resume execution after the
.us Execute when Hit
command has been completed. This capability can be used to patch your
code.
.note Enabled
.ix 'breakpoint' 'status'
This field displays the current status of the breakpoint. If it is
checked, the breakpoint is enabled. If it is unchecked, the breakpoint
is disabled.
.note Value
For Break-on-Execute breakpoints this field displays the source line
or the assembly line at which the break point is defined.
For Break-on-Write breakpoints, this field displays the memory
contents.
.note Clear
.ix 'breakpoint' 'deleting'
Click on the clear button to clear the breakpoint and close the
dialog.
.endnote
