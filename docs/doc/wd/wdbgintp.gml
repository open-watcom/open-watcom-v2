.chap *refid=vidintr Interrupting a Running Program
.*
.if &e'&dohelp eq 0 .do begin
.section Overview
.do end
.*
.np
.ix 'interrupting a running program'
.ix 'program' 'interrupting'
.ix 'infinite loop' 'interrupting'
It is not unusual for your code to contain an endless loop that
results in the program getting stuck in one spot. You then want to
interrupt the program so that you can see where it's getting stuck.
The process to give control back to the debugger is different for each
operating system.
.*
.section DOS
.*
.np
Press the Print Screen key. This will work if the program is stuck in
a loop. If it has misbehaved in some other way, Print Screen may have
no effect since a misbehaved application may overwrite code, data, the
debugger, or operating system code.
.ix 'CauseWay'
.np
When debugging with the CauseWay DOS extender, by default the running
program can be interrupted by pressing CTRL-ALT. The key combination is
configurable in the CWHELP.CFG file (located in "BINW" directory).
.*
.section Windows 3.x
.*
.np
Press CTRL-ALT-F. Windows must be running in enhanced mode and the
device WDEBUG.386 must be installed the [386Enh] section of SYSTEM.INI
for this to work. You cannot interrupt a running program under
Win-OS/2.
.*
.section Windows NT, Windows 95
.*
.np
If you are using the non-GUI version of the debugger, switch focus to
the debugger screen and press CTRL-BREAK.
.np
If you are using the GUI-based version of the debugger or one of the
remote debug servers, switch focus to the debugger or debug server
screen and click anywhere.
When you switch to the debugger screen, you will see a pop-up stating
that:
.millust begin
The debugger cannot be used while the application is
running.  Do you want to interrupt the application?
.millust end
.pc
If you select "Yes", the debugger will attempt to interrupt the
application.
If you select "No", the debugger will resume waiting for the
application to hit a breakpoint or terminate.
.np
If you select "Yes" and the debugger cannot interrupt the application,
you can click on the debugger again and it will display a pop-up
asking:
.millust begin
The debugger could not sucessfully interrupt your
application.  Do you want to terminate the application?
.millust end
.pc
If you select "Yes", the debugger will terminate your application.
If you select "No", the debugger will resume waiting for the program
to hit a breakpoint or terminate.
.begnote
.note Note:
Under Windows 95, it is very difficult to interrupt a program that is
in an infinite loop or spending most of its time in system API's.
:cmt. Windows 95 does not provide correct information about the thread's
:cmt. register state, so the debugger cannot set a breakpoint to halt
:cmt. execution.
:cmt. Windows 95 also ignores all attempts to turn on the single-step bit in
:cmt. the thread and interrupt it that way.
Under Windows 95, you can only interrupt a program that is responding
to messages (or looping in its own thread code).
If your program is an infinite loop, interrupting the program will
likely fail.
The only option in this case is to terminate the program.
.np
This is not an issue under Windows NT which has a superior debug API.
.endnote
.np
If you press CTRL-BREAK when the application has focus, you will
terminate the application being debugged rather than interrupting it.
.*
.section OS/2
.*
.np
Use the program manager to switch focus to the debugger screen
then press CTRL-BREAK.
If you press CTRL-BREAK when the application has focus, you will
terminate the application being debugged rather than interrupting it.
.*
.section NetWare
.*
.np
On the NetWare file server console, press ALT-ESCAPE while holding down
both SHIFT keys. In some instances, this may cause the system debugger
to become active instead of the &dbgname..
.*
.section Linux
.*
.np
Switch focus to the debugger console and press CTRL-C.
Alternatively, you may send any unhandled signal to the application
being debugged. Consult your Linux documentation for details.
.*
.section QNX
.*
.np
Switch focus to the debugger console and press CTRL-BREAK.
Alternatively, you may send any unhandled signal to the application
being debugged. Consult your QNX system documentation for details.
.*
