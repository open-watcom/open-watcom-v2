:set symbol="msg_list" value="event list"
:set symbol="mark_menu" value="File"
.*
.chap &drwc for Windows 3.1
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&drwc'
The 16-bit version of &drwc is a post-mortem debugger for Windows 3.1.
When an exception occurs in a program, a post-mortem debugger allows
you, after the fact, to examine the state of both the program and the
system in order to determine the cause of the exception.
In addition to being a debugging tool, &drwc is a system utility that
enables you to examine task information.
.np
&drwc can be a useful tool when performing in-house testing on your
applications. By running &drwc while testing applications, all faults
are logged. This gives you important information to recreate the
errors so you can fix the problems.
.begnote
.note Note:
Use the 16-bit version of &drwc to monitor 16-bit Windows (Win16)
applications running under Windows 3.1 or Win32 based operating systems.
Use the 32-bit version of &drwc to monitor Win32 applications running
under Win32 based operating systems.
.endnote
.*
.section Using &drwc
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting &drwc
.bull
Quitting &drwc
.bull
The &drwc Menu Bar
.endbull
.np
&drwc is designed to be used effectively while minimized. You only
need to view the application window when an error or warning occurs.
When this happens the &drwc icon changes from stethoscope to an
explosion indicating that information is waiting for your attention in
the main window.
.np
Some of &drwc's main functionality can also be accessed using the
system menu while the application is minimized. This includes the log
options, task control, and log current state functions.
.np
&drwc is more useful with the Windows debug kernel. Under the debug
kernel, Windows checks for many common programming errors and
generates debugging messages when it finds them. &drwc catches the
debugging messages and displays them on the event list. Viewing these
messages helps you identify errors in your program that you might not
otherwise see.
.*
.beglevel
.*
.section Starting &drwc
.*
.ix '&drwc' 'start'
.ix 'start' '&drwc'
.np
To start &drwc, double click on the &drwc icon. This opens the &drwc
window.
.*
.section Quitting &drwc
.*
.ix '&drwc' 'leave'
.ix 'Exit' '&drwc'
.np
To exit &drwc, select Exit from the File menu. This is the only way to
quit the application. Unlike other applications, double clicking on
the system menu box or selecting close from the system menu only
minimizes the application.
.*
.section The &drwc Menu Bar
.*
.ix 'menu bar' '&drwc'
.ix '&drwc' 'menu bar'
.np
The menu bar consists of the following four menus:
.begpoint
.point File
Save to a file, work with the event list, and configure the session
.point Log
Work with the log file
.point Task
Work with tasks currently running under Windows
.point Help
Access on-line help information
.endpoint
.*
.endlevel
.*
.section Using the &drwc Window
.*
.np
.ix 'event list' 'in &drwc'
.ix '&drwc' 'event list'
When you open &drwc, a window appears which contains the event list.
The event list records the various events occurring under Windows.
Some examples of events are errors, debugging messages
(under the debug kernel of Windows only), the starting and ending of
programs, and the loading and unloading of DLLs.
.keep 16
.figure *depth='2.24' *scale=64 *file='drw1' The &drwc window contains the event list which records and displays events occurring under Windows.
.np
When a debug message is issued, the &drwc icon alerts you that an
entry has been made on the event list. The &drwc icon changes from
stethoscope to an explosion. To reset the icon you must acknowledge
the alert in one of two ways:
.begstep
.step View the event list
by double clicking on the icon, or
.step Choose Clear Alert
.ix 'Clear Alert'
from the system menu.
.endstep
.np
&drwc intercepts all exceptions that occur in the system. An exception
is an error that causes your program to terminate. &drwc prevents your
program from terminating by halting the program's execution. The state
of the program remains intact.
.np
As with non-fatal errors, &drwc makes an entry on the event list when
an exception occurs. However, instead of alerting you to the entry by
changing the icon, &drwc reveals an exception dialog. This dialog
gives you several options. Refer to the
section entitled :HDREF refid='drwcexc'. for more information.
.np
The following sections describe functions that allow you to manipulate
the event list:
.begbull $compact
.bull
Saving the Event List
.bull
Clearing the Event List
.bull
Marking the Event List
.bull
Setting the Fonts
.endbull
.*
.beglevel
.*
.section &drwc.: Saving Event List Information to a File
.*
.np
.ix '&drwc' 'save'
The Save items in the File menu allow you to save the contents of the
event list to a text file. Saving information to a file enables you to
print the output later or annotate the text file on-line.
.np
.ix 'Save' 'in &drwc'
.ix '&drwc' 'save'
Choose Save from the File menu to save the event list information into
the current working directory. A message box appears indicating the
path to which &drwc saved the file.
.np
.ix 'Save As' 'in &drwc'
.ix '&drwc' 'Save As'
Choose Save As from the File menu to specify the file to which you
want to save the information currently in the window. This opens a
Save As dialog box where you select the desired file. Click on OK when
completed. A message box appears indicating the path to which &drwc
saved the event list.
.*
.section &drwc.: Clearing the Event List
.*
.np
.ix 'event list' 'clear'
.ix 'Clear List'
Choose Clear from the File menu to delete all information from the
event list.
.*
.section &drwc.: Marking the Event List
.*
.ix '&drwc' 'marking event list'
.ix 'event list' 'marking in &drwc'
:INCLUDE file='wguimark.gml'
.*
.section &drwc.: Setting the Fonts
.*
.np
.ix 'fonts' 'setting in &drwc'
.ix '&drwc' 'set font'
The Set Font item in the File menu allows you to set the font, style,
and size for the text in the &drwc window. Choosing this item opens
the Font dialog box. Select the desired font details and click on OK
to close the dialog and apply the fonts.
.*
.section Setting the Alert Feature
.*
.np
Each time &drwc logs a debug message, its icon changes from a
stethoscope to an explosion . This indicates that there is important
information waiting for you in the event list. Some debug messages
report only minor programming anomalies that may not be errors at all.
The text for these messages is prefixed by
.us wn,
for warning. Because these messages are not urgent, you may not want
the icon to signal the message. The Alert on Warnings menu item allows
you to configure &drwc so that its icon does not change for these
minor warning messages.
.np
.ix 'Alert on Warnings option'
.ix '&drwc' 'configure'
When the Alert on Warnings item in the File menu is not checked, the
icon does not change for minor warning messages. When this item is
checked, the &drwc icon alerts you when a warning or serious error
occurs.
.*
.section Showing the DIP status
.*
.np
DIP's (Debugging Information Processors) are Dynamic Link Libraries
that &drwc loads to read debugging information. Each DIP allows &drwc
to read a different type of debugging information. Choosing the Show
DIP Status item from the File menu displays the Debugging Information
Processor Status dialog.
.keep 17
.figure *depth='2.14' *scale=68 *file='drw4' The Debugging Information Processor Status dialog shows the DIP's &drwc has loaded.
.np
This dialog contains a list of all the DIP's that &drwc attempted
to load at startup and the results of those loads. If &drwc was unable
to load a particular DIP then the dialog will show the reason for
the failure. &drwc can only read a given type of debugging information
if it has successfully loaded the appropriate DIP.
.*
.section Using the Clear Alert Function
.*
.ix 'Clear Alert function'
.ix '&drwc' 'Clear Alert'
.np
If you are running &drwc minimized then each time an error or
warning is logged, the icon changes from a stethoscope to an explosion.
Unless you reset the icon, you will not know when more events are
added to the event list.
.np
There are two ways to reset the &drwc icon:
.begstep
.step Double click on the icon
to view the event list, or
.step Choose Clear Alert
.ix 'Clear Alert'
from the system menu.
.endstep
.np
To clear the alert message, click on the &drwc icon to reveal
the system menu. Choose Clear Alert from the menu to change
the icon back to the stethoscope. &drwc still logs the event,
but you do not have to look at it immediately.
.*
.endlevel
.*
.section *refid=drwcexc &drwc.: Exception Handling
.*
.ix '&drwc' 'exception dialog'
.ix 'Exceptions' 'handling in &drwc'
.np
The &drwc exception dialog box appears on your screen when an
exception occurs in any program in the system. This indicates that a
fatal error has occurred.
.keep 24
.figure *depth='2.30' *scale=64 *file='drw5' The exception dialog appears when a fatal error occurs in a program.
.np
Instead of terminating the program, &drwc suspends it and leaves its
state intact. Because the state is undisturbed you can view the
program's state information from the exception dialog, including
memory and register values. The exception dialog displays information
about error and gives you several options. The information displayed
about the error includes:
.begpoint $break
.point Task Name
The name of the module in which the error occurred.
.point Task Path
The full path of the file from which the offending module was loaded.
.point Fault Type
The type of fault that occurred.
.point Fault CS:EIP
The address of the instruction being executed when the fault occurred.
.point Source Info
The source line on which the error occurred. This only appears if
the program running contains debugging information.
.endpoint
.np
There are four buttons on the exception dialog that you can use to
proceed.
.begnote
.note Act
.ix 'Act'
When the act button is pressed, &drwc.'s action is determined by the
option currently selected in the action to take section of the dialog.
The following describes the action taken for each option.
.begpoint $break
.*
.point Terminate Task
When the Terminate Task radio button is selected choosing Act causes
the faulting task to be terminated immediately.
.*
.point Restart Instruction
When the Restart Instruction radio button is selected choosing Act
causes the faulting task to restart execution starting with the
instruction pointed to by its CS:EIP. This option is primarily used to
restart a task after you have changed its register values using the
Task Status dialog.
.*
.point Chain to Next Handler
A handler is a piece of code, a program, or a routine that gets called
when an exception occurs. The system can have several exception
handlers. When the Chain to Next Handler radio button is selected
choosing Act passes the exception on to the next handler in the
sequence, or chain.
.endpoint
.note Act and Log
.ix 'Act and Log'
Choosing the Act and Log button first creates a log file entry
recording information about this fault then takes the same action that
would result from choosing the Act button.
.note Registers
.ix 'Registers'
Select the Registers button to open the Task Status dialog. This
dialog gives you additional information on the state of the program
when it crashed. From this window you can change the registers and
flags, view segment mappings, and do a stack trace. Refer to the
section entitled :HDREF refid='drwcex'. for further information.
.note Options
.ix 'Options'
Select the Options button to choose the information you want to
write to the log file. This opens the Log File Options dialog
explained in the section entitled :HDREF refid='drwccu'..
.endnote
.*
.beglevel
.*
.section *refid=drwcex Examining the Task Status
.*
.np
.ix '&drwc' 'Task Status'
.ix 'Task Status'
The Task Status dialog appears when you choose the Registers button on
the exception dialog.
.keep 24
.figure *depth='3.88' *scale=64 *file='drw6' The Task Status dialog provides additional information on the program state when the exception occurred.
.np
This dialog displays information in four sections:
.begbull $compact
.bull
Source Information
.bull
Registers
.bull
Flags
.bull
Instructions Around Fault
.endbull
.np
The Source Information section indicates the source file where the
error occurred and the line of that source file. In order to get
source information, your program must have debugging information.
N/A appears in the Source Information section if the program does
not contain debugging information or the exception occurred on an
assembly instruction that does not correspond to a source file.
.np
The Registers and Flags sections display information in the CPU at the
time of the exception.
.np
The Instructions Around Fault section displays the assembly
instructions surrounding the instruction that was being executed
when the fault occurred. An asterisk marks the instruction to
which the CS:EIP is currently pointing.
.np
When the Show Symbols box is checked, &drwc tries to replace addresses
in the assembly code with symbolic names. &drwc can only replace
addresses if the faulting module contained debugging information.
If this box is not checked, addresses are left as numeric values.
.*
.beglevel
.*
.section &drwc.: Changing the Registers
.*
.np
.ix 'Registers' 'changing'
In the Task Status dialog you can set up testing and debugging
situations by changing the register and flag values. To change a
register value simply enter a new value in the edit field that
contains the current register value. To change the state of a flag
register just check or uncheck the check box associated with that
flag. A flag has a value of 1 when its check box is checked.
.np
The register and flag values will not take effect until you
choose OK on the Task Status dialog.
.*
.section &drwc.: Applying Registers
.*
.ix '&drwc' 'applying registers'
.ix 'Registers' 'applying in &drwc'
.np
Choosing Apply Registers updates the information in the Task Status
dialog based on values of registers and flags to which you have made
changes.
.*
.section &drwc.: Segment Mapping
.*
.ix 'segment mapping'
.ix '&drwc' 'segment mapping'
.np
Choosing Segment Map from the Task Status dialog opens a Segment Map
dialog box which indicates the selectors to which your map segments
were mapped. There is one entry for every segment in the program.
.keep 19
.figure *depth='2.96' *scale=100 *file='drw7' The Segment Map dialog lists the selectors to which the segments were mapped.
.np
This dialog allows you to view the data contained in each of your
program's segments. To view this memory, double click on the desired
segment. This opens a Memory Display window for the selected segment.
.keep 8
.figure *depth='0.84' *scale=63 *file='drw8' The Memory Display window displays the memory stored in the selected segment.
.np
The Memory Display window displays all of the data stored in the
selected segment. This information appears in hexadecimal form. This
allows you to see if the data in memory is what you believe it should
be.
.np
There are a number of functions you can perform in the Memory
Display window. Following is a description of these functions.
.*
.beglevel
.*
.section &drwc.: Saving Window Information
.*
.ix '&drwc' 'save'
.ix 'Memory Display window' 'saving'
.np
The Save items in the File menu of the Memory Display window allow you
to save the current window information to a text file. Choosing Save
from the File menu in the Memory Display window saves the data in the
window into the current working directory. A message box appears
indicating the path to which &drwc saved the file.
.np
Choosing Save As from the File menu allows you to specify the file to
which you want to save the information currently in the window. This
opens a Save As dialog box where you select the desired file. Click on
OK when completed. A message box appears indicating the path to which
&drwc saved the file.
.*
.section &drwc.: Changing the Display
.*
.ix 'Display' 'changing in &drwc'
.np
There are five ways to display the information on the Memory Display
window. You can display the information as:
.begbull $compact
.bull
Bytes
.bull
Words
.bull
DWords
.bull
16 Bit Code
.bull
32 Bit Code
.endbull
.np
To change the form of the displayed data, select an item from the
Display menu. When displaying data in bytes, the data appears in the
window as it physically appears in memory. When the data is displayed
as words, the bytes in memory are interpreted for you so that the
values displayed are those that would be obtained by treating the
memory as an array of words. For example, if memory containing the
values:
.np
12 34 56 78 9A BC DE FF
.np
was shown as words, the memory window would display:
.np
3412 7856 BC9A FFDE
.np
A similar interpretation is performed when the data is displayed as
DWords. If the above memory was shown as DWords, the memory window
would display:
.np
78563412 FFDEBC9A
.np
This rearrangement of bytes does not affect the textual interpretation
of the data shown on the right hand side of the window.
.np
Selecting 16 or 32 Bit Code from the Display menu presents the data in
a disassembled form. Sometimes presenting the data in disassembled
form is not accurate. This is because when &drwc disassembles the data
it begins at the first byte of the memory block and disassembles it
linearly. If a byte in this block, such as a padding byte, throws off
the sequence, the disassembly will be meaningless.
.*
.section &drwc.: Setting the Offset
.*
.ix '&drwc' 'set offset'
.ix 'Offset' 'setting in &drwc'
.np
On the Memory Display window you can set the offset. This means that
you can reposition the data so that the specified line appears at the
top of the window. The offset you enter appears on the first line. It
will not necessarily be the first piece of information because
rounding often occurs to avoid breaking a line.
.begstep
.step Choose Set Offset
from the menu bar on the Memory Display window.
.result
This opens a Goto Offset dialog box.
.keep 15
.figure *depth='2.26' *scale=100 *file='drw9' In the Goto Offset dialog, enter the offset you want to appears at the top of the Memory Display window.
.step Enter the desired offset
that you want to position at the top of the Memory Display window.
You can enter the offset in decimal or hexadecimal form. However, you
must prefix hexadecimal values with 0x.
.step Click on OK.
.result
This closes the dialog and positions the data as requested.
.endstep
.*
.section &drwc.: The Handle Information Dialog Box
.*
.ix 'Handle Information Dialog'
.ix '&drwc' 'Handle Information Dialog'
.np
A Handle Information dialog box attached to the Memory Display window
provides the following information:
.begbull $compact
.bull
Block Address
.bull
Block Handle
.bull
Block Size
.bull
Lock Count
.bull
Page Lock Count
.endbull
.keep 8
.figure *depth='1.05' *scale=83 *file='drw10' The Handle Information dialog box displays information about the memory object displayed in the Memory Display Window.
.np
This box stays with the window when the Auto Position Info item under
the File menu is checked. Deactivate this function by selecting the
item again from the File menu.
.np
If the Handle Information dialog is not visible, reveal it by choosing
Show Info from the File menu in the Memory Display window. Close this
dialog by double clicking on the system menu box in its upper left
hand corner.
.*
.endlevel
.*
.section Performing a Stack Trace
.*
.ix 'Stack Trace'
.ix '&drwc' 'performing a stack trace'
.np
A program is made up of a series of functions. A function executes
when it is called by the operating system or another function. When
completed it returns to the point in the code from which it was called.
.np
Whenever a function is called, a stack frame is pushed onto the stack.
When the function returns, the stack frame is popped from the stack.
The stack frame contains information about the point from which the
function was called and to which it will return. The collection of
stack frames currently on the stack is called a call chain.
.np
.ix 'call chain'
The stack trace function lets you walk the call chain. This allows you
to determine where each function in the call chain was called from and
where it will return to. To perform a stack trace, click on the Stack
Trace button in the Task Status dialog to reveal the Stack Trace
dialog.
.keep 16
.figure *depth='2.34' *scale=63 *file='drw11' The Stack Trace dialog allows you to walk the call chain.
.np
When the Stack Trace dialog opens, it contains information from the
stack frame for the function that was executing at the time of the
fault. The Instructions Around Fault section displays the assembly
instructions around the call to the faulting function. The instruction
directly after the call to the faulting function is marked with an
asterisk. Other information includes the address of this instruction
(CS:IP) and the base stack pointer (SS:BP) at the time of the call. If
the faulting module contained debugging information and the marked
assembly instruction corresponds to a source line, then source file
information appears.
.np
Selecting the Next button displays information from the next oldest
stack frame. This is information about the point from which the
currently displayed function was called. If the currently displayed
function was called by the operating system, then the message "No more
stack frames!" appears because there are no stack frames for calls
made from the operating system.
.np
Selecting the Prev button displays information from the next most
recent stack frame. This moves you in the opposite direction along the
call chain as pressing the Next button.
.np
The Cancel button closes the window.
.*
.endlevel
.*
.section Resetting the Fault Handled Flag
.*
.ix 'Fault Handled Flag'
.ix '&drwc' 'Fault Handled Flag'
.np
The Reset Fault Handled Flag menu item is only accessible when an
exception occurs from which &drwc is unable to recover. When such an
error occurs, &drwc cannot log exceptions or be terminated until you
choose Reset Fault Handled Flag from the File menu. This will not
happen frequently.
.*
.endlevel
.*
.section &drwc.: Working with Log Files
.*
.ix '&drwc' 'Log files'
.ix 'Log files'
.np
A log file is a text file created by &drwc.. The log file consists of
entries that describe the state of the system at the time the entry
was made. &drwc gives you the opportunity to create a log file entry
each time an exception occurs. Such entries include information about
the state of the program that caused the exception, as well as
information about the state of the system. You can create a log file
entry at any time using the Log Current State menu item in the Log
menu.
.np
A log file is a permanent record of the state of the system and,
possibly, your program. With a log file you can make note of an error
and return to it at a later time to examine and debug the problem.
.*
.beglevel
.*
.section Log the Current State
.*
.ix 'Log Current State'
.ix '&drwc' 'Log Current State'
.np
Choose Log Current State from the Log menu to write to the current log
file. This tells &drwc to create a log file entry with all of the log
details except the Offending Task Information. The settings in the Log
Options dialog determine the information written to the log file.
.np
The Log Current State item is also available on the system menu.
.*
.section Erase the Log File
.*
.ix 'Erase Log File'
.ix '&drwc' 'erase log file'
.ix 'Log File' 'erase'
.np
To erase the current log file, choose Erase Log File from the Log
menu. This erases the log file specified in the Log File Options
dialog.
.np
Choosing this item reveals a message box which asks you to verify that
you want to erase the specified log file. Select yes to erase the log
file or No to cancel the request.
.*
.section *refid=drwccu Customize the Log File
.*
.ix 'Log File' 'customizing'
.ix '&drwc' 'customize log file'
.np
The Log Options item in the Log menu opens the Log Options
dialog where you indicate to &drwc the information you want to appear
in log file entries. The Log Options item is also available from the
system menu.
.keep 23
.figure *depth='3.73' *scale=64 *file='drw12' In the Log Options dialog, you customize the log file to which you write information.
.np
The following sections describe the options that make up the Log
Options dialog.
.*
.beglevel
.*
.section &drwc.: Naming the Log File
.*
.np
.ix 'Log File' 'naming'
.ix '&drwc' 'naming the log file'
In the Log File Name field you can specify the file to which &drwc
writes the log file entries. The Browse button to the right of this
field opens the Pick Log Filename dialog. This dialog lets you browse
your file structure and select the path to which &drwc writes the log
file entries.
.*
.section &drwc.: Specifying the Log File Data
.*
.ix 'Log file' 'data'
.ix '&drwc' 'specify log file data'
.np
In the Log File Data and Disassembly Options sections of the Log File
Options dialog you select the information you want to appear in the
log file. The Log File Data section lists the various types of
information that can be included in a log file. If the check box for a
piece of information is marked then that type of information will be
written to the log file. The options in this section are:
.begpoint $break Log File Data
.point Stack Trace
When this option is selected stack trace information for the
application that caused the exception is written when a log file entry
is made. This option only affects log entries made due to exceptions.
.point Current Tasks
When this option is selected a list of all tasks currently running in
the system, including information about each task is included in each
log entry.
.point All Modules
When this option is selected a list of all modules currently loaded in
the system, including information about each module is included in each
log entry.
.point GDI Heap Info
When this option is selected summary information about the state of the
GDI local heap is included in each log entry.
.point User Heap Info
When this option is selected summary information about the state of the
User local heap is included in each log entry.
.point Mem Manager Info
When this option is selected summary information about the state of
the virtual memory manager is included in each log entry.
.point Module Segments
When this option is selected log file entries list the selectors
for each module currently loaded in the system. The All Modules
option must also be selected to write this information.
.point Query for Notes
When the Query for Notes option is selected &drwc displays an
Annotate Log dialog whenever a log file entry is made. This dialog
lets you enter some explanatory text that will be included in
the log file entry.
.keep 19
.figure *depth='3.00' *scale=100 *file='drw13' In the Annotate Log dialog box, enter text you want to include in the log file entry.
.np
Enter any text you want to include in the space provided. Press the OK
button to include this text in the log file. If you press the Cancel
button &drwc continues to create the log file entry but does not
include any information from the dialog.
.endpoint
.np
The values in the Disassembly Options box only affect log entries made
due to exceptions. These values relate to the disassembly section
appearing in the log file. The value in the Disasm Backup field tells
&drwc the number of instructions before the fault you want written to
the log file. The value in the Disasm Lines field tells &drwc the
total number instructions you want it to show in the log file. This
value includes the instructions displayed before the fault.
.*
.section &drwc.: Other Options
.*
.ix 'Auto Log'
.ix '&drwc' 'Auto Log'
.ix '&drwc' 'Log Restarted Tasks'
.ix 'Log Restarted Tasks'
.np
If the Auto Log option is selected, &drwc creates a log file when an
exception occurs and then terminates the application that caused the
fault instead of revealing the exception dialog. The Auto Log option is
useful when running test streams.
.np
The Max Log File Size field allows you to set the maximum size in bytes
for a log file. When the log file exceeds this size, &drwc prompts you
with a message box when you start the application.
.keep 14
.figure *depth='2.02' *scale=100 *file='drw14' A message box prompts you to erase the log file when it becomes larger than the maximum log file size specified on the Log File Options dialog.
.np
This message box offers you the opportunity to delete your log file
before it becomes uncontrollably large.
.*
.endlevel
.*
.endlevel
.*
.section Performing Task Control
.*
.ix 'Task Control'
.ix '&drwc' 'task control'
.np
Choosing Task Control from the Task menu or the System menu opens the
Task Control dialog. This dialog displays a Task List which consists
of all programs currently running under Windows. The Task Control
dialog lets you terminate and debug tasks.
.np
The task list in the Task Control dialog is different from the
system Task List which appears when you double click on the wallpaper.
The system Task List list displays only applications that currently
have a window open on the desktop. The task list in the Task Control
dialog lists all tasks regardless of whether they have a window open.
.*
.beglevel
.*
.section Terminating a Task
.*
.ix 'Kill Task'
.ix '&drwc' 'Kill Task'
.ix 'Task Control'
.np
The Kill Task button on the Task Control dialog terminates a task
listed in the Task List. This function is extremely powerful. It
allows you to terminate programs immediately without giving them the
opportunity to clean up by saving data, unloading DLLs, and freeing
allocated memory.
.np
For this reason, you should exercise care when using this function. It
is wise to only use the Kill Task function for tasks that cannot be
terminated in the usual way.
.begstep To terminate a task:
.step Choose Task Control
from the Task menu.
.result
This opens the Task Control dialog containing a list of all tasks
currently running in the system.
.keep 19
.figure *depth='2.96' *scale=80 *file='drw15' In the Task Control dialog, you can terminate a task selected from the Task List.
.step Select the task you want to terminate
by clicking on its entry in the task list.
.step Select the Kill Task button.
.result
This terminates the selected task, removing it from the task list.
&drwc enters this event in the event list.
.endstep
.*
.section Debugging a Task
.*
.ix 'Task' 'debugging'
.ix '&drwc' 'debugging a task'
.ix 'Debugging' 'tasks in &drwc'
.np
Use the Debug Task button on the Task Control dialog to attach the
debugger to the selected task from the Task List.
.begstep To debug a task:
.step Choose Task Control from the Task menu.
.result
This opens the Task Control dialog containing a list of all tasks
currently running in the system.
.step Select the task you want to debug
by clicking on its entry in the task list.
.step Select the Debug Task button.
.result
This opens the Debugger Options dialog box.
.keep 17
.figure *depth='2.56' *scale=100 *file='drw16' In the Debugger Option dialog, you enter the command line options to pass to the debugger.
.step Enter the command line options
you want &drwc to pass to the debugger.
.step Click on OK.
.result
This starts the debugger.
.endstep
.*
.endlevel
