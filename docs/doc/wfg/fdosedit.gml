.chap Using the &product Editor
.*
.remark
The editor interface is only available when running &cmpname on a
DOS-hosted system.
.eremark
.*
.section Selecting the &cmpname Editor Interface
.*
.np
.ix 'editor integration'
As we mentioned in the first chapter, a second way of using &cmpname
is with an integrated editor.
.ix 'editor' '&cmpname Editor'
When invoked through &cmpname, the &cmpname Editor can be used to
create, modify, and run FORTRAN programs.
In this mode, the editor and compiler are completely memory
resident.
A program can be modified and then tested without needing to
store the source code on disk each time you wish to compile and
execute it.
While this mode of operation uses additional memory, it provides
a complete program development environment.
.np
To use the editor interface, you must place a copy of the program
"WEDITF.EXE" in the current directory or one of the directories
listed in the "PATH" environment string.
If the software has been installed on your system by someone else
then this may already have been done.
In any case, it will become readily apparent if this procedure
has not been done when you first try to use &cmpname with the
editor.
.remark
The &cmpname Editor, "WEDITF.EXE", may be used stand-alone as a
general purpose text editor.
When used in this manner, the "RUN" command described in the following
sections is disabled.
.eremark
.*
.section Invoking &cmpname and the Editor
.*
.np
.ix '&cmpname starting'
From the command line, &cmpname and the editor are invoked as follows:
.ix 'invoking &cmpname'
.ix 'invoking editor'
.mbigbox begin
&cmpcmdup./EDIT[[options] [d:][path]filename[.ext]]

           or

&cmp2cmdup./EDIT[[options] [d:][path]filename[.ext]]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.ix '&cmpcmdup'
.note &cmpcmdup.
is the version of &cmpname which does not require the 80x87 numeric
data processor.
.ix '&cmp2cmdup'
.note &cmp2cmdup.
is the version of &cmpname which requires the 80x87 numeric data
processor.
.note EDIT
is the option that selects the editor interface.
.note options
.ix 'options specifying'
is a list of other &cmpname options, each preceded by a slash.
The list of options may not contain any blank characters.
Options may be specified in any order.
These options are used as defaults each time a program is compiled
and executed from the editor.
.note d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as "\SRC\PROGRAMS\".
If not specified, the current directory is assumed.
.note filename
is the file name of the file to be compiled.
.note ext
is the file extension of the file to be compiled.
If omitted, a file extension of "FOR" is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.endnote
.np
The following tutorial will illustrate the use of &cmpname with the
integrated editor.
Afterwards, you may wish to consult the section of the book
entitled "The WATCOM Editor" for a more in-depth introduction to the
editor.
.np
.ix '&cmpcmdup'
In the following sections, we will use &cmpcmdup. to illustrate how to
use &cmpname and the Editor on the PC.
.ix '&cmp2cmdup'
The name &cmp2cmdup. may be used in an equivalent manner when
the math coprocessor is present.
.np
Let us assume that the default drive is "A" and that
the compiler and editor reside on this diskette.
.ix '&cmpcmdup'
Type the command
.millust &cmpcmdup /EDIT
and press the "Enter" key.
.exam begin
C>&cmpcmd /edit
.exam end
.pc
.ix 'EDIT option'
.ix 'options' 'EDIT'
The "EDIT" option indicates that we wish to select the integrated
editor interface to &cmpname..
If any other options are specified on the command line, these will be
default options while you are using the editor.
.np
Once &cmpname is loaded into memory, the screen will clear and the
following lines will appear approximately in the centre, and to the
left side, of the screen.
:picture file='UGPRG0' text='WATFOR-77 Editor'.
:cmt. .cp 20
:cmt. .cbox begin
:cmt. .in -2
:cmt. &cmpname Editor 4.0 PC/DOS Copyright by WATCOM Systems Inc. 1981,1988
:cmt. ==|no_name&cxt.|=======================================================
:cmt.
:cmt.
:cmt.
:cmt.
:cmt. <beginning of file>
:cmt. <end of file>
:cmt.
:cmt.
:cmt.
:cmt.
:cmt.
:cmt. ======================================================================
:cmt. _
:cmt. F1: ...
:cmt. .in +2
:cmt. .cbox end
.np
This indicates that the &cmpname Editor is now ready for use.
If the message "Unable to start editor" appears then &cmpname could
not invoke "WEDITF.EXE".
Check that this file has been placed in the current directory or one
of the directories listed in the "PATH" environment string.
.np
.ix 'editor' 'cursor'
A small bar called the
.us cursor
is at the left near the bottom of the screen.
.ix 'editor' 'command entry area'
This line is known as the editor
.us command entry area.
.ix 'editor' 'message area'
The line above the command line is called the editor's
.us message area.
The command entry area is used by you to give commands to the editor,
whereas the message area is used by the editor to communicate various
items of information to the user.
.np
The line below the command line displays a terse summary of
what some of the program function keys will do.
.*
.section Entering Commands
.*
.np
.ix 'editor' 'entering commands'
.ix 'shell'
.ix 'command shell'
When entering a command to the PC command processor or "shell", you
must press the "Enter" key as a signal to the shell to process what
you have typed.
The same is true of the editor.
To cause the editor to process a command, you must type in the command
and then press the "Enter" key.
In the following tutorial, we will dispense with the instruction to
"press the Enter key" when a command is to be entered.
.np
The editor can be used to create a new program or examine an existing
program.
We will use the editor to examine, run and modify an existing program.
We will also show you how to save the program on a disk and how to
print it.
.*
.section Examining the Directory
.*
.np
.ix 'editor' 'examining directories'
To examine the names of files on disk from the editor, you can use the
editor's "directory" command.
Some examples follow:
.ix 'editor commands' 'DIRECTORY'
.exam begin
directory
dir c:
di c:\watcom\src\fortran
.exam end
.np
The commands "dir" and "di" are short forms of the "directory" command.
This command functions in a manner similar to the shell's "DIR" command.
A list of programs and files will appear on the screen and either
the word <more> or the word <hold> will appear in the lower right-hand
corner.
If the word <more> is on the screen then pressing the "Enter" key
causes more of the list to appear.
If the word <hold> appears on the screen, pressing the "Enter" key
causes the list to disappear.
.*
.section Loading a Program
.*
.np
One of the programs included with the &cmpname software can now be
loaded into the computer and examined.
Enter a command similar to the following:
.ix 'editor commands' 'EDIT'
.exam begin
edit c:\watcom\src\fortran\sample.&langsuff
.exam end
.pc
where "c:\watcom\src\fortran\" is one of several possible designations
for the path (i.e., drive and directory) containing the &cmpname
software.
Use the path that corresponds to the place where you installed the
&cmpname software.
After a short pause, the program will be loaded into the computer's
memory.
.np
The program will appear on the screen in the following format:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg1
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG1' text='A Sample Program'.
.np
This program is quite simple; when it is run, it will display the
line:
.millust This is my first &langabrv program
We will be working with this program throughout this introductory
tutorial.
.*
.section Running the Program
.*
.np
The program can now be run on the computer.
Enter the command:
.ix 'editor commands' 'RUN'
.millust run
The screen will appear as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg2
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG2' text='Running the Sample Program'.
.np
The output from the program is displayed on the screen.
Note that the compiler reads the program source from the
editor's workspace.
We do not have to store the program on disk first although
we will eventually want to do this so that we have a permanent
copy of the program.
Pressing the "Enter" key will cause the output to disappear.
We can now use the editor again.
.*
.section Using the Editor
.*
.np
In the next several sections, we will modify the &lang program
"SAMPLE.&langsuffup" in different ways to illustrate the basic
capabilities of the editor.
The editor has many commands and is very useful in constructing and
examining programs.
.*
.section Moving the Cursor
.*
.np
There are two important features or markers on the screen:
.begpoint
.point (i)
There is the cursor which at this moment is at the left near the
bottom of the screen.
.point (ii)
One line of the display is highlighted; it has an intensity which is
different from the other lines on the screen.
.ix 'editor' 'current line'
This line is referred to as the
.us current line.
If you are using a monochrome display and no line on the screen
appears highlighted then you should adjust the display using the
"contrast" knob on the monitor.
.endpoint
.np
.ix 'editor' 'cursor keys'
There are four cursor keys on the keyboard which control these
two features.
Each of these four keys has an arrow which indicates a
direction.
The cursor keys are grouped on the right side of the keyboard.
Pressing any of the cursor keys may cause a number to appear
at the cursor.
.ix 'Num Lock key'
.ix 'keys' 'Num Lock'
.ix 'numeric lock'
If this happens then the keypad is in "numeric lock" mode.
.np
To cancel "numeric lock", press the "Num Lock" key.
Numeric lock converts the keypad on the right side of the keyboard
to a numeric role until it is depressed again.
.np
We can now use the four cursor keys to control the cursor and the
highlighted line.
Pressing the "cursor right" key (the one which points to the right)
will move the cursor to the right;
pressing the "cursor left" key will move the cursor to the left.
A different line can be highlighted by pressing the cursor "up"
and "down" keys respectively.
Now that we see how the two markers on the screen can be controlled
we shall try some other techniques.
.np
.ix 'editor' 'function keys'
An area of the keyboard contains a number of special function keys.
These keys may be labelled F1, F2, etc. or perhaps PF1, PF2, etc.
.np
.ix 'editor' 'F4 key'
Press function key 4 (F4 or PF4); the cursor will move to the beginning
of the highlighted line and the highlighting will disappear.
For the remainder of these exercises, we will refer to function
keys using the short form "Fx" where the "x" is one of the
numbers from 1 to 10.
The cursor can now be moved from line to line and along a line by using
the cursor control keys.
.ix 'editor' 'text entry'
The cursor is in an area of the screen called the
.us text entry area.
.*
.section Changing a Program
.*
.np
We will now change our &langabrv program so that it displays a
slightly different message.
At this point the cursor should be in the text entry area of the
screen.
If it is not, simply press F4.
Now use the four cursor keys to move the cursor to the letter "f"
in the word "first" on the line that contains the words:
.millust This is my first
Type the word "other" over the word "first".
Now press F4 and the cursor will return to the command entry area of
the screen.
.np
We can enter the "run" command to run the program.
The screen should now appear as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg3
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG3' text='Running Modified Sample Program'.
.np
Pressing the "Enter" key will cause the output to disappear.
.np
Let us now change the third line in the workspace to read
.millust begin
PRINT *, 'This is a FORTRAN program which I changed'
.millust end
.pc
This requires deleting the words "my other" and replacing them
with the letter "a" and inserting the words "which I changed" at
the end of the line.
To begin making these changes press F4 to move the cursor to the
text entry area.
Move the cursor to the letter "m" in "my" and type the letter "a".
The cursor will now move to the "y".
.ix 'Del key'
.ix 'keys' 'Del'
The characters "y other" can be deleted by depressing the delete
key (marked "Del") once for each letter or space to be deleted.
To enter the extra characters after the word "program",
move the cursor to the apostrophe (') after the letter "m".
.ix 'Ins key'
.ix 'keys' 'Ins'
Press the insert key (marked "Ins") and type " which I changed".
Note that the cursor appears larger when "insert mode" is active.
.begnote Notes:
.note (i)
To cancel "insert mode", simply press the "Ins" key again.
The cursor will return to normal size.
.note (ii)
If you make an error in typing, you can use the cursor keys and
the "Ins" and "Del" keys to correct your error.
.endnote
.np
This program can now be run by pressing F4 to return to the command
entry area and entering the "run" command.
The screen should now appear as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg5
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG4' text='Another Run'.
.np
Pressing the "Enter" key will cause the output to disappear.
.*
.section Saving a Program
.*
.np
.ix 'editor' 'saving a program'
The program which we have just changed can be saved on a diskette.
First ensure that the cursor is in the command entry area.
Recall that function key F4 can be used to do this.
Enter the command:
.ix 'editor commands' 'PUT'
.millust put myprog.&langsuff
After a short pause, a message will appear in the message area of
the screen.
.millust begin
myprog.&langsuff - Lines transferred = 4
.millust end
.pc
The message area is just above the command line.
The message will disappear the next time you press the "Enter"
key.
.begnote Notes:
.note (i)
The message area is important since it is used to display
the status of an editor command.
If the editor cannot perform the requested command, it will
display a message reflecting this situation.
Thus you should always check the message area after entering
an editor command.
.note (ii)
In the previous example of the "put" command, it was not
necessary to specify the file
.us extension
"&langsuff".
&cmpname will automatically supply this extension when the "." and
extension are omitted as follows:
.millust put myprog
Similarly, it is not necessary to specify the file extension
when using the "edit" command.
.note (iii)
The program has now been saved on a diskette using the "put"
command ("p" is an abbreviation) and you can use the "dir"
command to verify that
.mono "MYPROG.&langsuffup"
is there.
.note (iv)
Sometimes the "put" command may not succeed and a different message
appears in the message area.
For example, the message
.millust begin
Attempt to write on write-protected diskette
.millust end
.pc
could appear.
The diskette in the default drive may have a write-protect tab placed
over the notch which prevents you from modifying the diskette.
Check that you are using the correct diskette.
If the diskette contains the &cmpname software then you may wish to
remove it and insert another diskette that will accept new files.
.note (v)
There are other reasons why you may be unable to store the program on
a disk.
The message should explain what the problem is and permit you to take
corrective action.
.endnote
.*
.section Adding a Line to a Program
.*
.np
We may also want to add lines to an existing program or create a new
program.
The program you have just saved can be used to illustrate the addition
of lines to a program.
.np
.ix 'Tab' 'setting TAB stops'
.ix 'Tab key'
.ix 'keys' 'Tab'
Before we do this, it might be appropriate to introduce you to two new
editor commands.
The PC keyboard contains a special key for moving the cursor to
certain places on the line called TAB stops.
By default, pressing the TAB key moves the cursor to one of columns 9,
17, 25, etc. (i.e., 1 + some multiple of 8).
Pressing both the SHIFT key and the TAB key causes the cursor to move
back to the previous TAB stop (Reverse TAB).
While this arrangement is adequate for the entry of text, it would be
nice if we could define a different set of TAB stops for the entry of
FORTRAN statements.
.ix 'XLINE option'
.ix 'options' 'XLINE'
FORTRAN requires that we enter statements between columns 7 and 72
inclusive (you can use the &cmpname "XLINE" option to extend the line
length to 132).
Let us define a set of TAB stops suitable for FORTRAN programs.
.ix 'editor commands' 'TABSET'
Enter the command:
.millust tabset 1 7 11 15 19 23
This command will redefine the default TAB stops (but only while you
are using the editor).
When you return to the operating system shell, they will revert back
to the default.
Additional TAB stops or other ones can be defined according to
preference.
.np
.ix 'margin bell'
If any text is typed beyond column 72, it will be ignored by &cmpname
according to the rules of FORTRAN.
.ix 'editor commands' 'BELL'
Since we don't want to enter FORTRAN statements that go past column
72, enter the following command:
.millust bell 72
This will cause the editor to issue an audible alarm (a beep) whenever
the cursor moves from column 72 into column 73.
This will indicate that the FORTRAN statement should be continued on a
new line (using the rules concerning continuation of FORTRAN
statements).
.np
Move the cursor to the text entry area using F4 and then position the
cursor on the line that contains the words "This is a".
.ix 'F5 key'
.ix 'keys' 'F5'
Press function key 5 (F5) to create a blank line following this line.
The cursor will automatically move to the beginning of the new line
and change height to indicate that you are now in text insertion mode.
Recall that the "Ins" key can be pressed to switch into and out of
text insertion mode.
You can now press the Tab key and type:
.millust begin
PRINT *, 'to illustrate the editor'
.millust end
.pc
The new line should be aligned with the line above.
After the line is completely typed (do not forget the apostrophes),
you can press F4 to return to the command entry area and then run the
program.
The screen should appear as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg7
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG5' text='Adding Lines'.
.np
If you make an error in typing, you may get an error message which
indicates the program is not correct.
For example, if the last apostrophe was not included in the line just
added, the screen would appear as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt. .im ugprg8
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG6' text='Diagnosing Errors'.
.np
Return to the editor by pressing the "Enter" key.
You may now correct the program using the techniques we have
discussed above.
.*
.section Deleting a Line
.*
.np
Deleting a line from a program is quite simple.
Move the cursor to the text entry area by pressing function key F4
and then move the cursor to the line that was just added to the
program.
.ix 'editor' 'F6 key'
Press function key F6 and the line on which the cursor is located will
disappear.
Note that pressing Shift together with function key F6 will cause the
line to reappear.
.*
.section Printing a Program
.*
.np
.ix 'editor' 'printing a program'
The program in which we have just deleted a line can be printed on
a printer.
To print the program, move the cursor to the command entry area
using the F4 key.
Enter the command:
.millust put prn
After a short pause, a message will appear in the message area of
the screen.
.millust begin
prn - Lines transferred = 4
.millust end
.pc
This message will disappear the next time you press the "Enter" key.
.np
The program has now been printed on the printer using the "put"
command.
.begnote Notes:
.note (i)
Make sure your printer is on and the on-line switch has been pressed
before you start printing.
.note (ii)
If you are using a network then your program may not actually be
printed.
Instead your program file may be held in a print "queue" until you
issue a special network command to the operating system shell to
release the file for printing.
.endnote
.*
.section Returning to the Operating System Shell
.*
.np
To conclude, we will show how to exit from &cmpname and return to the
shell.
Enter the command
.ix 'editor commands' 'BYE'
.millust bye
and one of two things will happen:
.begnote
.note (i)
the shell prompt will reappear on the screen, or
.note (ii)
the message
.millust File modified - use QUIT to quit anyway
will appear in the message area.
.endnote
.np
If the latter occurs then some change was made to the program and it
has not been "put" to disk since that change.
.ix 'editor commands' 'QUIT'
If you are not interested in saving the modified program, simply enter
the command
.millust quit
and the shell prompt will reappear on the screen.
Otherwise, use the "put" command to save your program and then issue
the "bye" command.
.*
.section Summary
.*
.np
This brief discussion has shown you how to use the editor to
.begnote
.note (i)
get FORTRAN programs from disk using the "edit" command;
.note (ii)
run FORTRAN programs using the "run" command;
.note (iii)
put FORTRAN programs to disk using the "put" command;
.note (iv)
modify FORTRAN programs;
.note (v)
exit from the editor and &cmpname using the "bye" command.
.endnote
.np
Although you have enough information to use the &cmpname system and
editor, there are many different ways to perform the functions
described here.
To acquire further knowledge about the editor, you should read the
section of the book entitled "The WATCOM Editor".
.*
.section The "RUN" Command
.*
.np
.ix 'RUN'
.ix 'editor commands' 'RUN'
In the above examples, we showed you the simplest form of the
editor's "RUN" command.
It can also be used to pass options to the &cmpname compiler and
to compile other programs.
.ix 'invoking &cmpname'
.mbigbox begin
RUN[options] [[d:][path]filename[.ext] [prog_parms]] [options]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.note options
.ix 'options' 'specifying'
is a list of valid &cmpname options, each preceded by a slash.
The list of options may not contain any blank characters.
Options may be specified in any order.
.note d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as "\SRC\PROGRAMS\".
If not specified, the current directory is assumed.
.note filename
is the file name of the file to be compiled.
.note ext
is the file extension of the file to be compiled.
If omitted, a file extension of "for" is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.note prog_parms
is an optional list of program parameters that may be accessed by
the executing program through use of the library function
.id FGETCMD
or the functions
.id IARGC
and
.id IGETARG.
See the chapter entitled "The &cmpname Subprogram Library" for
more information on these functions.
.endnote
.autonote Notes:
.note
If no options are specified then the currently configured defaults
are used.
The default options include any that were specified on the operating
system shell's command line.
.note
If no file is specified then the contents of the current workspace
are processed.
:cmt. .note
:cmt. If you wish to specify parameters to the executing program
:cmt. but do not want to type the current workspace name
:cmt. then you can specify an "*" to represent the current workspace.
:cmt. .exam begin
:cmt. run * arg1, arg2, arg3
:cmt. run/debug * arg1, arg2, arg3
:cmt. .exam end
.note
If a file name is specified and the specification matches the name of
a workspace then that workspace is processed.
.note
With the "RUN" command, you can request &cmpname to process:
.begpoint
.point (i)
the current workspace,
.point (ii)
another workspace, or
.point (iii)
a file on disk.
.endpoint
.note
.ix 'options' 'summary'
If you enter the command:
.millust RUN ?
then &cmpname will display its command line options summary.
.note
If you are not familiar with the concept of workspaces then you should
read the section of the book entitled "The WATCOM Editor".
.endnote
.*
.section Compiling FORTRAN Programs
.*
.np
Consider the following program which attempts to compute the square
root of a number and display the result.
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt.
:cmt. <beginning of file>
:cmt. *     Program to compute a square root
:cmt.
:cmt.       NUMBER = -144
:cmt.       SQROOT = SQRT( NUMBER )
:cmt.       PRINT *, SQROOT
:cmt.       END
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG7' text='Computing a Square Root'.
.np
.ix 'comment line'
.ix 'statement'
This program consists of a
.us comment line
and some FORTRAN
.us statements.
.np
If you want to change the workspace name, you can use the editor's
"NAME" command as follows:
.millust name root&cxt.
To compile and execute this program, enter the command:
.millust run/list
.ix 'LIST option'
.ix 'options' 'LIST'
This command requests &cmpname to process the current workspace and
ensures that a listing of the source program is produced on disk.
.np
.ix 'message interpretation'
There are deliberate errors in the program so the message
.code begin
root&cxt.(4): *ERR* LI-12 column 15, INTEGER argument type is invalid
for this generic function
.code end
.pc
is displayed on the screen.
How should this message be interpreted?
.np
The message
.mono *ERR*
means that some violation of the rules of FORTRAN has been committed.
.ix 'message error'
.ix 'error'
.ix 'line'
Specifically, on the fourth line of the source file
:FNAME.root&cxt.:eFNAME. near column 15 an error was detected.
The error says that the argument of the generic FORTRAN function that
was used (in this case
.id SQRT
.ct )
cannot be an
.kw INTEGER
argument.
The
.us error code
.ix 'error code'
.mono LI-12
is composed of an error category and number.
The error code may be used to look up the message in the
diagnostic summary in the appendix entitled "&cmpname Diagnostic
Messages" at the end of this guide.
This appendix provides an explanation of all diagnostic messages
issued by &cmpname..
.np
In addition to the message that is displayed on the screen, a
source program listing file,
:FNAME.root.lst:eFNAME., is created by the compiler.
.ix 'listing source'
To examine the listing file, enter the command:
.millust edit root.lst
The contents of the screen will appear similar to the following.
:cmt. .cp 25
:cmt. .cbox begin
:cmt. .in -2
:cmt. <beginning of file>
:cmt. &product &ver &cdate
:cmt. .cpyrit 1984
:cmt. .trdmrk
:cmt.
:cmt. Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
:cmt. arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover
:cmt.
:cmt.         *     Program to compute a square root
:cmt.
:cmt.       1       NUMBER = -144
:cmt.       2       SQROOT = SQRT( NUMBER )
:cmt.                       $
:cmt. *ERR* LI-12 INTEGER argument type is invalid for this generic function
:cmt.       3       PRINT *, SQROOT
:cmt.       4       END
:cmt.
:cmt.
:cmt. Compile time (in seconds):          0  Execution time (in seconds):     0
:cmt. Size of object code:                0  Number of extensions:            0
:cmt. Size of local data area(s):         0  Number of warnings:              0
:cmt. Size of global data area:           0  Number of errors:                1
:cmt. Object bytes free:                  0  Statements Executed:             0
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .cbox end
:picture file='UGPRG8' text='The Square Root Listing File'.
.np
In the listing file, we find a copy of the original source program and
some additional lines which the compiler has added.
The line that was in error is followed by a line containing an error
indicator or marker (a dollar sign "$").
An error message, similar to the one that was displayed on the screen,
follows that line.
.np
Near the start of the program listing is a line indicating which
options were in effect at the time the compilation of the program
began.
These and other options are described in the chapter entitled
"Compiler Options".
.ix 'statistics' 'compile-time'
.ix 'statistics' 'execution-time'
.ix 'compiler statistics'
At the end of the program listing, we find some statistics such as how
long it took to compile the program and the number of error messages
that were issued.
The compile and execution times are reported as actual elapsed time in
seconds using the "time-of-day" clock.
.np
Some statistics on memory utilization also appear.
The object code size is the number of bytes of executable code
generated by &cmpname for the program.
The local data area size includes scalar variables (undimensioned
variables), numeric and logical constants, and temporaries used
in evaluating expressions.
The global data area size includes character variables, dimensioned
variables, and common blocks.
The amount of unused memory is reflected in the "Object bytes free"
statistic.
This number represents the amount of memory still available to the
compiler for object code.
.ix 'CODESIZE option'
.ix 'options' 'CODESIZE'
The amount of memory used for object code can be set by using the
"CODESIZE=" option.
The default is 512k.
.np
A number appears to the left of
.us some
of the FORTRAN source lines.
This number is generated by the compiler for each FORTRAN statement.
It is used primarily at execution time when the compiler detects an
error and issues a message.
.ix 'statement number'
.ix 'internal statement number'
.ix 'ISN'
The statement number is sometimes referred to as an "Internal
Statement Number" or ISN.
We will demonstrate the usefulness of the ISN in a subsequent example.
.np
Upon checking out the rules about the use of the
.id SQRT
function, we find that the argument must be a floating-point
quantity of type real, double precision, complex, or double
precision complex.
Because of the rules of FORTRAN,
.id NUMBER
is an integer variable by default.
The program must be corrected either by changing the name of the
variable or by explicitly declaring the type of the variable.
.np
This is just one example of how &cmpname handles compile-time
errors.
.ix 'compile-time'
We use the term "compile-time" since the error was detected
before any execution of the program was attempted.
.np
Let us take this example and correct the error but also introduce a
new one.
First we should empty the contents of the listing workspace using
the editor's "DELETE" command.
Enter the command:
.millust *delete
and the screen will display:
:cmt. .cp 8
:cmt. .mbox begin
:cmt. .in -2
:cmt.
:cmt. <beginning of file>
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG9' text='Deleting Workspace Contents'.
.np
We erase the contents of the listing workspace for two reasons:
.begpoint
.point (i)
to ensure that as much memory as possible is available for
processing the FORTRAN source program, and
.point (ii)
to ensure that the next time we edit the listing file, we
will get a fresh copy from the disk.
.endpoint
.np
To return to our original workspace, we can enter the command:
.millust edit
.np
Change the program by adding a variable declaration so that it
resembles the following:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt.
:cmt. <beginning of file>
:cmt. *     Program to compute a square root
:cmt.
:cmt.       REAL NUMBER
:cmt.       NUMBER = -144
:cmt.       SQROOT = SQRT( NUMBER )
:cmt.       PRINT *, SQROOT
:cmt.       END
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG10' text='Square Root Revision 1'.
.np
Run the revised program using the "run/list" command.
This time a different message is displayed on the screen:
.code begin
*ERR* LI-03 argument must not be negative
- Executing in MAIN PROGRAM, statement 3 in file root&cxt.
.code end
.pc
.ix 'execution-time'
This message is issued at execution time.
.ix 'trace-back'
Execution-time messages are accompanied by a program "trace-back".
The trace-back is a sequence of one or more messages which describes
the state of the executing program at the time the error was detected.
.np
If you edit the listing file, you will find something similar to the
following.
:cmt. .cp 25
:cmt. .cbox begin
:cmt. .in -2
:cmt. <beginning of file>
:cmt. &product &ver &cdate
:cmt. .cpyrit 1984
:cmt. .trdmrk
:cmt.
:cmt. Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
:cmt. arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover
:cmt.
:cmt.         *     Program to compute a square root
:cmt.
:cmt.       1       REAL NUMBER
:cmt.       2       NUMBER = -144
:cmt.       3       SQROOT = SQRT( NUMBER )
:cmt.       4       PRINT *, SQROOT
:cmt.       5       END
:cmt.
:cmt. *ERR* LI-03 argument must not be negative
:cmt. - Executing in MAIN PROGRAM, statement 3 in file root&cxt.
:cmt.
:cmt. Compile time (in seconds):          0  Execution time (in seconds):     0
:cmt. Size of object code:              114  Number of extensions:            0
:cmt. Size of local data area(s):        80  Number of warnings:              0
:cmt. Size of global data area:           0  Number of errors:                1
:cmt. Object bytes free:             524244  Statements Executed:             2
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .cbox end
:picture file='UGPRG11' text='Listing of Revision 1'.
.np
.ix 'ISN'
Here, we see the use of the ISN.
The error has occurred in the third
.us statement
of the FORTRAN program.
Note that this is
.us not
the third line in the file.
.np
The name of the file which contained the program is also
displayed in the trace-back messages (in this case
:FNAME.root&cxt.:eFNAME.).
This is done since FORTRAN source lines can come from several
different files.
.np
The above illustrates the execution-time error detection
(or diagnostic) capability of &cmpname..
.np
We would like to close this discussion by correcting the program
so that it produces the desired result.
Delete all the lines in the listing file workspace.
Use the "EDIT" command to return to the original workspace and
change the program as follows:
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt.
:cmt. <beginning of file>
:cmt. *     Program to compute a square root
:cmt.
:cmt.       REAL NUMBER
:cmt.       NUMBER = 144
:cmt.       SQROOT = SQRT( NUMBER )
:cmt.       PRINT *, SQROOT
:cmt.       END
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG12' text='Square Root Revision 2'.
.np
When you run the program, the result
.millust begin
              12.0000000
.millust end
.pc
is displayed on the screen.
This is indeed the square root of the number 144.
The decimal point and trailing zeroes indicate that the result is
a floating-point quantity.
.np
If you edit the listing file, you will find something similar to
the following.
:cmt. .cp 25
:cmt. .cbox begin
:cmt. .in -2
:cmt. <beginning of file>
:cmt. &product &ver &cdate
:cmt. .cpyrit 1984
:cmt. .trdmrk
:cmt.
:cmt. Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
:cmt. arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover
:cmt.
:cmt.         *     Program to compute a square root
:cmt.
:cmt.       1       REAL NUMBER
:cmt.       2       NUMBER = 144
:cmt.       3       SQROOT = SQRT( NUMBER )
:cmt.       4       PRINT *, SQROOT
:cmt.       5       END
:cmt.
:cmt.
:cmt. Compile time (in seconds):          0  Execution time (in seconds):     0
:cmt. Size of object code:              114  Number of extensions:            0
:cmt. Size of local data area(s):        80  Number of warnings:              0
:cmt. Size of global data area:           0  Number of errors:                0
:cmt. Object bytes free:             524100  Statements Executed:             4
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .cbox end
:picture file='UGPRG13' text='Listing of Revision 2'.
.np
You may have noticed that the printed result of this program appeared
on the screen but not in the listing file.
.ix 'listing'
.ix 'printed result'
.ix 'NOXTYPE option'
.ix 'options' 'NOXTYPE'
&cmpname provides an option ("NOXTYPE") which allows you to direct
such printed results to the listing file instead of the screen.
Rerun the program but this time also specify the "NOXTYPE" option.
.millust run/list/noxtype
Don't forget to delete the listing file workspace and return to the
program workspace.
See the chapter entitled "Compiler Options" for more information on
the "NOXTYPE" option.
.*
.section Saving the Finished Program
.*
.np
Now that you have a completely correct FORTRAN program, you probably
will want to save a copy of it on the disk.
As we saw in an earlier section, you can use the editor's "PUT"
command to save the program.
.ix 'editor commands' 'PUT'
.millust put
If you forget to do this and exit from &cmpname then any changes since
the last "put" will be lost.
.hint
If you have an up-to-date listing file, you could extract the lost
source lines from this file but it can be a lot of work!
.ehint
.np
It is a good policy to save your program on disk from time to time
("checkpointing") just in case something goes wrong.
For example, you might accidentally delete the program from the
workspace.
Checkpointing also allows you to back up a step when the latest
revision to a program is disastrously wrong.
The editor has an automatic checkpointing capability.
Look at the description of the editor's "AUTOSAVE" command.
.*
.section More Examples Using Compiler Options
.*
.np
Assume that the workspace contains the following statements.
:cmt. .cp 15
:cmt. .mbox begin
:cmt. .in -2
:cmt.
:cmt. <beginning of file>
:cmt.       X=12.
:cmt.       Y=X*-X
:cmt.       PRINT *,X,Y
:cmt.       END
:cmt. <end of file>
:cmt.
:cmt. .in +2
:cmt. .mbox end
:picture file='UGPRG14' text='Another FORTRAN Program'.
.np
Run this program using the following command.
.millust run/list/type
On the screen will appear something similar to the following:
:cmt. .cp 18
:cmt. .cbox begin
:cmt. .in -2
:cmt. &product &ver &cdate
:cmt. .cpyrit 1984
:cmt. .trdmrk
:cmt.
:cmt. Options: list,extensions,reference,warnings,terminal,run,xtype,check,
:cmt. arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover
:cmt.
:cmt.       1       X=12.
:cmt.       2       Y=X*-X
:cmt.                   $
:cmt. *ERR* SX-02 bad sequence of operators
:cmt.       3       PRINT *,X,Y
:cmt.       4       END
:cmt.
:cmt.
:cmt. Compile time (in seconds):          0  Execution time (in seconds):     0
:cmt. Size of object code:                0  Number of extensions:            0
:cmt. Size of local data area(s):         0  Number of warnings:              0
:cmt. Size of global data area:           0  Number of errors:                1
:cmt. Object bytes free:                  0  Statements Executed:             0
:cmt. .in +2
:cmt. .cbox end
:picture file='UGPRG15' text='Screen Output'.
.np
.ix 'TYPE option'
.ix 'options' 'TYPE'
In this case, we have used the "TYPE" option.
This option causes &cmpname to output the program listing,
complete with diagnostics, to the screen instead of a disk file.
.millust begin
run/list/nowarn/ext d:example1
.millust end
.pc
This command requests &cmpname to compile and execute the file
:FNAME.d:example1&cxt.:eFNAME.,
specifying that no warning messages are to be issued, and that
extension messages are to be issued.
In this case, we have specified a file on disk (we are assuming
that there is no workspace in memory with the name
:FNAME.d:example1&cxt.:eFNAME.).
.ix 'TYPE option'
.ix 'options' 'TYPE'
Since we did not use the "TYPE" option, &cmpname produces a file
with an extension of "LST" using the source filename as the
filename for the listing file.
.ix 'listing file'
If you were to examine the contents of the current directory
of the default disk, you would find the listing file:
.millust begin
EXAMPLE1.LST
.millust end
.np
.ix 'ERRORFILE option'
.ix 'options' 'ERRORFILE'
The following example is similar to the previous one except that the
"ERRORFILE" option is also specified.
.millust begin
run/list/nowarn/ext/errorfile d:example1
.millust end
.np
If any diagnostic messages are issued, &cmpname produces a file with
an extension of "ERR" using the source filename as the filename for
the error file.
.ix 'error file'
If diagnostic messages were issued and you were to examine the
contents of the current directory of the default disk, you
would find the error file:
.millust begin
EXAMPLE1.ERR
.millust end
.np
The error file will contain a summary of the error messages that
also appear in the listing file.
The messages are presented in the same form as they appear on the
screen.
.cp 25
.remark
The listing and error file names are constructed by using only
the filename part of the full source file specification.
.ix 'listing file'
.ix 'error file'
.millust begin
C:\IMSL\SINGLE\MAIN\LINEQSMN.FOR
.millust end
.pc
Given the source file specification above, &cmpname constructs
the following names.
.millust begin
Listing file name: LINEQSMN.LST
Error file name:   LINEQSMN.ERR
.millust end
.np
If the current directory is
.millust begin
B:\TESTS
.millust end
.pc
then the full listing and error file specifications are:
.millust begin
Listing file: B:\TESTS\LINEQSMN.LST
Error file:   B:\TESTS\LINEQSMN.ERR
.millust end
.eremark
.*
.section Halting an Executing FORTRAN Program
.*
.np
.ix 'compilation' 'halting'
.ix 'execution' 'halting'
.ix 'interrupting execution'
.ix 'interrupting compilation'
When a FORTRAN program is "RUN" two steps are performed.
The program is first compiled, that is, it is translated into
computer instructions.
It is then placed into execution, provided that there were no
compile-time errors.
.np
.ix 'Ctrl/Break'
While the FORTRAN program is being compiled, &cmpname may be halted by
entering a Ctrl/Break sequence from the keyboard.
Compilation of the program will be terminated.
.np
.ix 'Ctrl/Break'
While a FORTRAN program is executing, it may also be halted by
entering a Ctrl/Break sequence from the keyboard.
A message will appear indicating that the program was interrupted and
then a program traceback will follow.
.ix 'DEBUG option'
.ix 'options' 'DEBUG'
If the "DEBUG" option was specified with the "RUN" command then the
&cmpname debugger will be entered.
Otherwise, a prompt will appear which will allow you to resume or
terminate execution.
