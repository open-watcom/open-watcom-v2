.chap *refid=videxec Controlling Program Execution
.*
.np
This section describes how you can control the execution of your
program as you debug it.
.*
.section *refid=menurun The Run Menu
.*
.np
.ix 'Run menu'
The
.mi Run
menu controls how your program executes.  It contains the
following items.
.begmenu Run
.menuitem Go
.ix 'resuming execution'
.ix 'run'
Start or resume program execution.
Execution resumes at the current location and will not stop
until a breakpoint is encountered, an error occurs, or your program
terminates.
.menuitem Run to Cursor
.ix 'run' 'to cursor position'
Resume program execution until it executes the
location of the cursor in the Source or Assembly window.
Execution will stop before the cursor
position if a breakpoint is encountered or an error occurs.
.menuitem Execute to
.ix 'program' 'running to specified address'
Resume program execution until it executes a specified address.
You will be prompted to enter an address.  It can be the name of
a function or an expression that resolves to a code address.
.refalso videxpr
:INCLUDE file='symbuttn.gml'.
.np
If your program encounters a breakpoint or an error occurs before the
specified address is executed,
your request to stop at the given address is ignored.
.menuitem Step Over
.ix 'stepping' 'over calls'
.ix 'tracing' 'over calls'
Trace a single source or assembly line depending on whether the
source or assembly window is current. Step Over will not step into any
function calls.
.menuitem Trace Into
.ix 'stepping' 'into calls'
.ix 'tracing' 'into calls'
This is similar to
.mi Step Over
except that it will step into any function calls.
.menuitem Next Sequential
.ix 'loops' 'running to completion'
.ix 'run' 'until loop completes'
Run until the program executes the next sequential source line or
assembly instruction.  This is useful if the program is executing
the last statement in a loop and you wish to execute until the
loop terminates.  When using this command, be sure that the
execution path will eventually execute the next statement or
instruction.  If execution fails to reach this point then the
program may continue to execute until completion.  This situation
is like setting a breakpoint at a statement or assembly
instruction which will never be executed and then issuing a GO
command.  In this situation, the application would execute until
an error occurred or another breakpoint was encountered.
.menuitem Until Return
.ix 'run' 'until return'
.ix 'return to caller'
Resume program execution until the currently executing function
returns. Execution terminates prior to this if an error
occurs or a breakpoint is encountered.
.menuitem Skip to Cursor
.ix 'skipping code'
.ix 'code' 'skipping'
.ix 'instruction pointer' 'repositioning'
Reposition the instruction pointer at the cursor position,
"skipping" all instructions in between.  When you continue
execution, the program continues from this point.  This is useful
if you want to skip an offending line or re-execute something.
.bi Use this menu item with caution.  If you skip to an instruction which
.bi is not in the current function or skip to code that expects
.bi a different program state, your program could crash.
.menuitem Restart
.ix 'program' 'restarting'
.ix 'restarting program'
Restart your program from the beginning.
All breakpoints in your program will be preserved.
Breakpoints in DLLs will not be preserved.
.menuitem Debug Startup
Restart your program from the beginning but stop before system
initialization.
Normally the debugger puts you at the main (fmain, winmain, etc.)
entry point in your application.
This option will allow you to break much earlier in the initialization
process.
This feature is useful for debugging run-time startup code,
initializers, and constructors for static C++ objects.
.np
For DOS, Windows 3.x and Netware, the debugger will put you at the
assembly entry point of your application (i.e., it doesn't run the
"progstart" hook).
.np
Windows 3.x runs each DLL's startup code as it loads it, and the
static DLLs are really loaded by the run-time startup code, so, to
debug the startup code for a statically linked Windows 3.x DLL, you
need to do the following.
.autonote
.note
Select
.menuref 'Debug Startup' 'Run'
.dot
.note
Select
.menuref 'On Image Load' 'Break'
.dot
Type the name of the DLL in which you are interested.
.note
Select
.menuref 'Go' 'Run'
.endnote
.np
For OS/2 and Windows NT, the debugger will put you at a point after
all DLLs have been loaded, but before any DLL initialization routines
are called.
This enables you to set breakpoints in your statically referenced DLL's
startup code.
.np
If you have hard-coded int3 instructions in your DLL startup, the
debugger will skip them, unless you use
.menuref 'Debug Startup' 'Run'
.dot
.np
All breakpoints in your program will be preserved.
Breakpoints in DLLs will not be preserved.
.menuitem Save
.ix 'saving debug session'
Save the current debugging session to a file.  The file contains
commands that will allow the debugger to play your debugging
session back to its current point in a later session.
.refalso wndrep
.menuitem Restore
Restore a saved debugging session. If you run the program with different
input or if the program is a multi-threaded application, this option
may not work properly since external factors may have affected
program execution.
.refalso wndrep
.endmenu
.*
.section *refid=menuund The Undo Menu
.*
.np
.ix 'undoing changes'
.ix 'reverse execution' 'over simple statement'
.ix 'backward execution' 'over simple statement'
The debugger keeps an execution history as you debug your program.
This history is accessible using the
.mm Undo
menu. The effect of program statements as you single step through
your program are recorded.  All interactions that allow you to
modify the state of your program including modifying variable
values, changing memory and registers are also recorded.  Undo and
Redo let you browse backward and forward through this execution
history.  As you use these menu items, all recorded effects are
undone or redone, and each of the debugger's windows are updated
accordingly.
.np
You can resume program execution at any previous point in the
history.  The program history has no size restrictions aside from
the amount of memory available to the debugger, so theoretically
you could single step through your entire program and then execute
it in reverse.  There are several practical problems that get in
the way of this.  When you single step over a call or interrupt
instruction, or let the program run normally, the debugger has no
way of knowing what kind of side effects occurred.  No attempt is
made to discover and record these side effects, but the fact that
you did step over a call is recorded.  If you try to resume
program execution from a point prior to a side effect, the
debugger will give you a the option to continue or back out of the
operation.  Use caution if you choose to continue.  If an
important side effect is duplicated, you program could crash.  Of
course reversing execution over functions with no side effects is
harmless, and can be a useful debugging technique.  If you have
accidentally stepped over a call that does have a side effect, you
can use
.mi Replay
to restore your program state.
.np
.ix 'calls' 'unwinding stack'
.ix 'unwinding call stack'
Unwind and Rewind move the debugger's state up and down the call stack.
Like Undo, all windows are updated as you browse up and down the stack,
and you can resume execution from a point up the call stack.  A warning
will be issued if you try resuming from a point up the call stack
since the debugger cannot completely undo the effects of the call.
.np
Unwind is particularly useful when your program crashes in a routine
that does not contain debugging information.  strcpy() is a good example
of this.  You can use Unwind to find the call site and inspect the
parameters that caused the problem.
.np
The runtime library detects certain classes of errors and diagnoses
them as fatal runtime errors.  If this occurs when you are debugging,
the debugger will be activated and the error message will be displayed.
For example, throwing an exception in C++ without having a catch in place is a
fatal runtime error.  In C, the abort() and assert() functions are fatal
errors.
When this happens, you will be positioned in an internal C library call.
You can use Unwind to find the point in your source code that initiated
the error condition.
.np
When Unwind and Undo are used in conjunction,  Undo is the primary operation
and Unwind is secondary.  You can Undo to a previous point in the history
and then Unwind the stack.  If you Unwind the stack first and then use
Undo, the Unwind has no effect.
.np
If you modify the machine state in any way when you are browsing
backward through the execution history,  all forward information from
that point is discarded.  If you have browsed backward over a side effect
the debugger will give you the option of canceling any such operation.
.np
The Undo menu contains the following items.
.begmenu Undo
.menuitem Undo
Browse backwards through the program execution
history.
.menuitem Redo
Browse forward through the program execution history.
.menuitem Unwind Stack
Move up the call stack one level.
.menuitem Rewind Stack
Move down the call stack one level.
.menuitem Home
Return to the currently executing location, reversing the effects of
all Undo and Unwind operations.
.endmenu
.*
.section *refid=wndrep The Replay Window
.*
.figure *depth='1.50' *scale=65 *file='dbgrep' The Replay Window
.np
Choose
.menuref 'Replay' 'Code'
.ix 'restoring debug session'
.ix 'replaying debug session'
.ix 'recording debug session'
.ix 'reverse execution' 'over call'
.ix 'backward execution' 'over call'
to open the
.wnddef Replay
window.  This
window displays each of the steps that you have performed during
this debugging session that might have affected program flow.
There are three items displayed in the replay window.  First is
the address the program was executing when you took some action
that could affect the program.  These actions include setting break
points, tracing and modifying memory.  Second is the source
or assembly code found at that address.  Third is a command in the
debugger's command language that will duplicate the action you
took.  The most common use for Replay is when you accidentally
step over a function call, or the program unexpectedly runs to
completion.  If this happens, you can open the replay window, and
replay you debugging session up to any point prior to the last
action you took.
.np
There are special cases where replay will not perform as expected.
Since replay is essentially the same as playing your keystrokes
and mouse interactions back to the debugger, your program must
behave identically on a subsequent run.  Any keyboard or mouse
interaction that your program expects must be entered the same
way.  If your program expects an input file, you must run it on
the same data set.  Your program should not behave randomly or
handle asynchronous events.  Finally, your program should not be
multi-threaded.  If you have just been tracing one thread, your
program will replay correctly, but multiple threads may not
be scheduled the same way on a subsequent run.
.np
You can replay program execution to any point by double clicking
on that line or by cursoring to it and pressing ENTER.
Select any line and press the right mouse button to see the
following pop-up menu items:
.begmenu Replay Pop-up
.menuitem Goto
Replay the program until it returns to the selected level in the replay history.
.menuitem Source
Position the source window at the selected line.
.menuitem Assembly
Show the assembly code for the selected line.
.endmenu
.*
.section *refid=wndcall The Calls Window
.*
.figure *depth='3.00' *scale=70 *file='dbgcall' The Calls Window
.np
.ix 'calls' 'displaying stack'
Choose
.menuref 'Calls' 'Code'
menu to display the
.wnddef Calls
window. This
window displays the program's call stack.  Each line contains the
name of the function that was executing, and the source or assembly
code at the call site.  You can use Unwind and Rewind to obtain
this information, but the calls windows will show you the entire call
stack.
.np
.ix 'calls' 'unwinding stack'
You can Unwind to any point in the call stack by
double-clicking on a line, or by cursoring to it and pressing ENTER.
Select a line and press the right mouse button to access
the following pop-up menu items:
.begmenu Calls Pop-up
.menuitem Unwind
Unwind the stack to the level of the selected code.  This
is equivalent to using
.menuref 'Unwind' 'Undo'
or
.menuref 'Rewind' 'Undo'
.dot
.menuitem Break
.ix 'breakpoint' 'setting in caller'
.ix 'breakpoint' 'up call stack'
Set a breakpoint at the return from the selected call.
.menuitem Goto
Execute until the program returns from the selected call.
.endmenu
.*
.section *refid=wndthrd The Thread Window
.*
.np
.figure *depth='1.50' *scale=70 *file='dbgthrd' The Thread Window
.ix 'threads' 'displaying'
Choose
.menuref 'Thread' 'Code'
to display
.wnddef Thread
window.
This window displays the system ID of each thread, the state of
the thread, and under some operating systems, system specific
information about the thread including its name and scheduling priority.
.ix 'threads' 'state'
The state of each thread can be:
.begnote $break
.note current
This is the thread that was running when the debugger was entered.  It
is the thread that hit a break point or error.  When you trace through
the application,
.bi only the current thread is allowed to run.
.note runnable
This thread will be allowed to run whenever you let your program run, but
will not run when you trace the program.
.note frozen
This thread will not be allowed to run when you resume your program.
.note dead
Under some operating systems, threads that have been terminated still
show up in the list of threads.  A dead thread will never execute again.
.endnote
.np
You can make any thread current by double clicking on it or cursoring to
it and pressing ENTER.  All other debugger windows update accordingly.
Press the right mouse button to access the following pop-up menu items:
.begmenu Thread Pop-up
.menuitem Switch to
.ix 'threads' 'switching to'
Make the selected thread current.
.menuitem Freeze
.ix 'threads' 'freezing'
Change the state of the selected thread to be
.bi frozen
.dot
You cannot
freeze the current thread.
.menuitem Thaw
.ix 'threads' 'thawing'
Change the state of the selected thread to be
.bi runnable.
.dot
The current thread is always runnable.
.endmenu
