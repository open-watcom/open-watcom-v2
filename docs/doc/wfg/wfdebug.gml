.chap The &product Debugger
.*
.section Description
.*
.np
The execution-time debugger is selected by specifying the "DEBUG"
compiler option.
If there are no source errors in the FORTRAN program, execution of the
program will commence.
The debugger is entered before the execution of the first FORTRAN
statement.
The debugger prompt is "DEBUG>".
The statement that is about to be executed is displayed along with its
internal statement number (ISN).
This information is extracted from the listing file.
If no listing file was created, as would be the case if the "TYPE"
option was specified, a message would be issued by the debugger
indicating it could not open the listing file.
The user may set breakpoints, trace statement execution, and examine
or modify variables.
.*
.section Invoking the &cmpname Debugger
.*
.ix 'debugger' 'invoking'
.ix 'invoking debugger'
.if '&machsys' = 'VM/SP CMS' .do begin
.***************************************
.mbigbox begin
&cmpcmdup. <fn> [<ft>] [<fm>] (DEBUG [<options>]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.note <fn>
is the CMS file name of the file to be compiled.
It also may be the name of a file currently being edited with IBM's
XEDIT editor.
.note <ft>
is the CMS file type of the file to be compiled.
If omitted, a file type of "FORTRAN" is assumed.
.note <fm>
is the CMS file mode of the file to be compiled.
If omitted, a file mode of "*" is assumed.
.note DEBUG
is the option used to select the execution-time debugger.
.note <options>
is a list of other &cmpname options.
.endnote
.exam begin
&cmpcmdup. DEMO (DEBUG
.exam end
.***************************************
.do end
.if '&machsys' = 'DOS' .do begin
.***************************************
.mbigbox begin
&cmpcmdup. /DEBUG [options] [d:][path]filename[.ext] [options]

    or

&cmp2cmdup. /DEBUG [options] [[d:][path]filename[.ext]] [options]

    or (using the integrated editor)

RUN /DEBUG [[options] [d:][path]filename[.ext] [options]]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.note &cmpcmdup.
is the version of &cmpname which does not require the 80x87 numeric
data processor.
.note &cmp2cmdup.
is the version of &cmpname which requires the 80x87 numeric data
processor.
.note DEBUG
is the &cmpname option that selects the execution-time debugger.
.note options
is a list of other &cmpname options, each preceded by a slash.
The list of options may not contain any blank characters.
Options may be specified in any order.
.note d:
is an optional drive specification such as :FNAME.&dr1:eFNAME.,
:FNAME.&dr2:eFNAME., etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as
:FNAME.&pc.SRC&pc.PROGRAMS&pc:eFNAME..
If not specified, the current directory is assumed.
.note filename
is the file name of the file to be compiled.
.note ext
is the file extension of the file to be compiled.
If omitted, a file extension of :FNAME.&cxt:eFNAME. is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.endnote
.exam begin
&cmpcmdup./DEBUG DEMO
.exam end
.***************************************
.do end
.if '&machsys' = 'VMS' .do begin
.***************************************
.mbigbox begin
&cmpcmdup./DEBUG[/qualifiers] file-spec-list[/qualifiers]

    or (from EVE)

RUN /DEBUG[/qualifiers] [file-spec-list]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.note /DEBUG
is the qualifier used to select the execution-time debugger.
.note /qualifiers
is a list of other &cmpname options, each preceded by a slash.
.note file-spec-list
specifies the name of one or more VAXTPU buffers or FORTRAN source
files to be compiled.
If you do not specify a file type for a disk file, &cmpname uses the
default file type of "FOR".
.endnote
.exam begin
$ &cmpcmd./debug demo
.exam end
.***************************************
.do end
.if '&machsys' = 'QNX' .do begin
.***************************************
.mbigbox begin
&cmpcmd. +debug [<options>] <file-spec>

    or (using the integrated editor)

run +debug [[<options>] <file-spec>]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.note +debug
is the option used to select the execution-time debugger.
.note <options>
is a list of other &cmpname options, each preceded by a blank.
.note <file-spec>
is the QNX file specification of the file to be compiled.
.endnote
.exam begin
% &cmpcmd. +debug demo
.exam end
.***************************************
.do end
.pc
The above example requests &cmpname to compile and execute the file
:FNAME.demo.&langsuff.:eFNAME..
Before execution of the program begins, the &cmpname debugger is
entered.
.*
.section Debugger Commands
.*
.np
.ix 'debugger commands'
In the following debugger command descriptions, upper case letters
denote short forms for the command and lower case letters may be
optionally specified.
For example, "HELP", "HEL", "HE", "H" (typed in either upper or lower
case letters) are acceptable forms of the "help" command.
.np
Items shown inside square brackets ([]) are optional.
The "or" bar (|) denotes alternate choices for the operand of a
debugger command.
Items shown inside angle brackets (<>) are explained in the command
descriptions.
When specifying such an item, the brackets must not be entered.
.np
The internal statement number or ISN, as it is used below, may be
specified in several ways.
Each executable statement has its own unique ISN.
ISN's are shown to the left of statements in the listing file.
The current statement is the statement which will next be executed
when program execution is resumed.
To refer to the ISN of this statement, you may use the short form ".".
You may refer to other statements by specifying a number with a
preceding "+" or "-" sign.
In this case, the actual ISN is calculated by adding or subtracting
(depending on the sign) the specified number from the ISN of the
current statement.
Alternately, you may specify the actual ISN of the statement or
statements.
.np
Specifically, <isn> may be specified as
.begpoint
.point (a)
a "." (the current ISN),
.point (b)
a negative sign ("-") followed by a number (the current ISN - number),
.point (c)
a positive sign ("+") followed by a number (the current ISN + number),
or
.point (d)
a number.
.endpoint
.*
.beglevel
.*
.section The Breakpoint Command
.*
.np
.ix 'debugger commands' 'breakpoint'
The
.us breakpoint
command is used to set or clear breakpoints.
There are two forms of the
.us breakpoint
command.
The first form sets or displays current breakpoint information.
.mbox begin
Breakpoint [<isn>]
.mbox end
.np
When an ISN is specified, execution of the program will continue until
the specified statement is about to be executed.
If no ISN is specified, the debugger will display the ISN of all the
statements at which breakpoints have been set.
No output is produced if no breakpoints have been set.
.np
The second form clears any breakpoints that have been set.
.mbox begin
Breakpoint /c
.mbox end
.np
.exam begin
DEBUG>b 120
DEBUG>break .
DEBUG>b /c
.exam end
.*
.section The Display Command
.*
.ix 'debugger commands' 'display'
.mbox begin
Display [* | <isn>[:<isn>]]
.mbox end
.np
The
.us display
command is used to display executable FORTRAN statements in the
current program unit.
If no ISN or ISN range is specified, the 5 statements before and after
the current statement about to be executed are displayed.
The current statement is marked by an asterisk in the first column.
.np
In addition, any statement or range of statements may be displayed.
The current statement may be specified as ".".
An ISN range may be specified in absolute terms (e.g., 11:20) or in
terms relative to the current statement (e.g., -5:+22) or in
combinations of both (e.g., 9:+3).
A negative sign ("-") indicates the number of statements before the
current statement and a positive sign ("+") indicates the number of
statements after the current statement.
When no sign is present, the number is assumed to be an ISN.
If "*" is specified then the entire program is displayed.
.exam begin
DEBUG>d
DEBUG>disp 8:22
DEBUG>d .
DEBUG>d -5:+12
DEBUG>d 132:+20
DEBUG>display *
.exam end
.*
.section The Go Command
.*
.ix 'debugger commands' 'go'
.mbox begin
Go [<isn>]
.mbox end
.np
The
.us go
command resumes execution from the point where the program was last
stopped.
Alternatively, an ISN may be specified in which case execution resumes
until the specified ISN is reached.
.exam begin
DEBUG>g
DEBUG>go 12
DEBUG>g -1
DEBUG>g +2
.exam end
.*
.section The Help Command
.*
.ix 'debugger commands' 'help'
.mbox begin
Help
.mbox end
.np
The
.us help
command provides a summary of the debugger commands available and
their options.
.*
.section The Logio Command
.*
.ix 'debugger commands' 'logio'
.mbox begin
Logio [<unit>]
.mbox end
.np
The
.us logio
command can be used to toggle the logging status of all input and
output on a specified unit.
Initially, input and output on a unit is not logged.
To start logging of a unit, specify the
.us logio
command with the number of the unit to be logged.
The log is displayed on the screen.
Formatted records are displayed in
.if '&machsys' = 'VM/SP CMS' .do begin
EBCDIC format.
.do end
.el .do begin
ASCII format.
.do end
Unformatted records are displayed in hexadecimal (base 16) notation.
To stop logging of a particular unit, issue a second
.us logio
command specifying the unit number.
.np
If no unit number is specified, a summary of the units that are being
logged is displayed.
.np
Input records are displayed with a unit number prefix and the
.mono "<"
character (to indicate "input").
Output records are displayed with a unit number prefix and the
.mono ">"
character (to indicate "output").
.exam begin
DEBUG>l 6
DEBUG>logio
.if '&machsys' = 'QNX' .do begin
6    [33]$tty0
.do end
.if '&machsys' = 'VMS' .do begin
6    SYS$OUTPUT
.do end
.if '&machsys' = 'VM/SP CMS' .do begin
6    TERMINAL
.do end
.if '&machsys' = 'DOS' .do begin
6    CON
.do end
DEBUG>l 2
DEBUG>logio 3
DEBUG>g
2<This is record 1
3>This is record 1
2<This is record 2
3>This is record 2
.exam end
.pc
The above example illustrates logging of units 2, 3 and 6.
Two records are read from unit 2 and written to unit 3.
.*
.section The Quit Command
.*
.ix 'debugger commands' 'quit'
.mbox begin
Quit
.mbox end
.np
The
.us quit
command terminates the debugger and returns to the system.
.*
.section The Trace Command
.*
.ix 'debugger commands' 'trace'
.mbox begin
Trace
.mbox end
.np
The
.us trace
command places the program into a mode that causes the debugger to be
re-entered after the execution of a single statement (single-step
mode).
Immediately after specifying the
.us trace
command, the debugger will display a line which presents the user with
a number of options.
.illust begin
Into["I" key]    Over[space bar]    Next["N" key]    Exit["Enter" key]
.illust end
.np
If the "I" key is pressed, the debugger will execute a statement.
If the statement has a reference to a subroutine or function,
execution of the subroutine or function is traced.
Otherwise tracing will continue at the next statement to be executed
in your program.
.np
If the space bar is pressed, the debugger will execute a statement and
continue tracing at the next statement to be executed in your program.
Tracing of subroutines or functions is skipped.
.np
If the "N" key is pressed, tracing will advance to the next sequential
statement immediately following the current statement.
This is particularly useful when you are at the bottom of a loop and
you do not wish to retrace all the statements in the loop.
.*
.section The Unit Command
.*
.ix 'debugger commands' 'unit'
.mbox begin
Unit [<unit>]
.mbox end
.np
The
.us unit
command is used to display information regarding FORTRAN units.
If
.mono <unit>
is omitted, all connected units and their corresponding file
names are displayed.
.if '&machsys' = 'VM/SP CMS' .do begin
.exam begin
DEBUG>unit
1     CUSTOMER REC      A
6     TERMINAL
5     TERMINAL
.exam break
DEBUG>u 1
FILE=   CUSTOMER REC       A
RECL=   80
BLANKS= null
STATUS= unknown
ACCESS= sequential
FORM=   formatted
REC=    1
IOSTAT= 0
.exam end
.do end
.if '&machsys' = 'VMS' .do begin
.exam begin
DEBUG>unit
1     DRB2:[&cmpcmdup..EXAMPLES]CUSTOM.REC;1
6     SYS$OUTPUT
5     SYS$INPUT
.exam break
DEBUG>u 1
FILE=   DRB2:[&cmpcmdup..EXAMPLES]CUSTOM.REC;1
RECL=   132
BLANKS= null
STATUS= unknown
ACCESS= sequential
FORM=   formatted
REC=    1
IOSTAT= 0
.exam end
.do end
.if '&machsys' = 'DOS' .do begin
.exam begin
DEBUG>unit
1     C:\CUSTOM.REC
6     CON
5     CON
.exam break
DEBUG>u 1
NAME=            C:\CUSTOM.REC
ACCESS=          sequential
ACTION=          readwrite
BLANK=           null
BLOCKSIZE=       16384
CARRIAGECONTROL= no
FORM=            formatted
IOSTAT=          0
NEXTREC=         1
RECL=            256
RECORDTYPE=      default
STATUS=          unknown
.exam end
.do end
.if '&machsys' = 'QNX' .do begin
.exam begin
DEBUG>unit
1     2:/custom.rec
6     [33]$tty0
5     [33]$tty0
.exam break
DEBUG>u 1
FILE=   2:/custom.rec
RECL=   80
BLANKS= null
STATUS= unknown
ACCESS= sequential
FORM=   formatted
REC=    1
IOSTAT= 0
.exam end
.do end
.*
.section The Where Command
.*
.ix 'debugger commands' 'where'
.mbox begin
Where
.mbox end
.np
This command provides a program execution traceback and displays the
next line to be executed.
.millust begin
DEBUG>w
- Executing in SUB2, statement 7 in file hello.for
- Called from SUB1, statement 4 in file hello.for
- Called from MAIN PROGRAM, statement 1 in file hello.for
       7        print *, 'hello world'
.millust end
.*
.section Executing System Commands
.*
.ix 'debugger commands' '!'
.mbox begin
! [<command>]
.mbox end
.np
The
.us !
command allows operating system commands, or other programs to be
executed from the debugger.
.np
If
.mono <command>
is specified, an attempt to execute the command is made.
Control will then immediately return to the debugger.
.if '&machsys' = 'VM/SP CMS' .do begin
.exam begin
DEBUG>! type demo listing
.exam end
.do end
.if '&machsys' = 'DOS' .do begin
.exam begin
DEBUG>! type demo.for
DEBUG>! cd examples
.exam end
.do end
.if '&machsys' = 'QNX' .do begin
.exam begin
DEBUG>! p demo.for
DEBUG>! cd examples
DEBUG>! chattr myprog p=+r
.exam end
.do end
.if '&machsys' = 'VMS' .do begin
.exam begin
DEBUG>! set def [-.examples]
%SYSTEM-S-NORMAL, normal successful completion
DEBUG>
.exam end
.do end
.np
.if '&machsys' = 'VM/SP CMS' .do begin
If
.mono <command>
is omitted then you will be placed at the system level so that more
than one command can be executed.
.exam begin
DEBUG>sys
type demo listing
erase demo temp
return
.exam end
.pc
Note, in the last example, that the CMS "RETURN" command must be used
to return to the debugger.
.do end
.if '&machsys' = 'DOS' .do begin
If
.mono <command>
is omitted, a new command processor (or shell) will be started.
Commands may be entered until an "EXIT" command is entered.
The "EXIT" command terminates the current command processor and
returns control to the debugger.
.np
If you are running on a DOS-hosted system:
.tinyexam begin
DEBUG>!

Microsoft(R) MS-DOS(R) Version 5.00
             (C)Copyright Microsoft Corp 1981-1991.

C>type demo.for
C>erase demo.tmp
C>exit
DEBUG>
.tinyexam end
.np
If you are running on an OS/2-hosted system:
.tinyexam begin
DEBUG>!

Operating System/2 Command Interpreter Version 2.0

[C:\]type demo.for
[C:\]erase demo.tmp
[C:\]exit
DEBUG>
.tinyexam end
.do end
.if '&machsys' = 'QNX' .do begin
If
.mono <command>
is omitted, another QNX shell is invoked.
Commands may be entered until a CTRL/D key sequence is entered.
CTRL/D terminates the shell and returns control to the debugger.
.exam begin
DEBUG>sys

% p demo.for
% frel demo.tmp
% <CTRL/D>
DEBUG>
.exam end
.autonote Notes:
.note
There must be enough unused memory in the computer to run the QNX
shell and specified program.
.note
Entering a CTRL/Z sequence will also invoke another QNX shell.
.endnote
.do end
.if '&machsys' = 'VMS' .do begin
If
.mono <command>
is omitted, a process is spawned.
Commands may be entered until "logoff" is entered.
This terminates the process and returns control to the debugger.
.exam begin
DEBUG>sys
$ set def [-.examples]
$ logoff
%SYSTEM-S-NORMAL, normal successful completion
DEBUG>
.exam end
.do end
.*
.section Modifying Data
.*
.ix 'debugger commands' '/'
.mbox begin
/ <item> <value>
.mbox end
.np
The
.us /
command assigns the storage units identified by
.mono <item>
with
.mono <value>.
.mono <value>
can be any input string that can be specified in list-directed input.
See the section entitled "List-Directed Formatting" in the chapter
entitled "Format" in the Language Reference manual.
.mono <value>
and
.mono <item>
must follow the rules of list-directed input.
See the section entitled "List-Directed Input" in the same chapter.
.exam begin
DEBUG>/ X 1.5
DEBUG>/ A 3.1415926,2*4.243,7*65.12654
DEBUG>/ a 3.1415926
4.243, -4.243
7*65.12654
DEBUG>/ a(5) 73.33
DEBUG>/ L .TRUE.
DEBUG>/ C 'abc'
DEBUG>/ c(4:6) 'def'
DEBUG>/ D(2)(4:5) 'gh'
.exam end
.pc
In the above examples,
.id X
is a real variable,
.id A
is a real array containing 10 elements,
.id L
is a logical variable,
.id C
is a character variable, and
.id D
is a character array.
One example above illustrates that the elements of an array may be
entered on several lines of input.
Prompting continues until enough values have been read to fill the
.if '&machsys' = 'VM/SP CMS' .do begin
array or a terminal "end-of-file" is encountered.
A terminal end-of-file is generated by pressing the line-entering key
without typing anything (including spaces).
.do end
.if '&machsys' = 'DOS' .do begin
array or a console end-of-file (Ctrl/Z followed by the line-entering
key) is encountered.
.do end
.if '&machsys' = 'VMS' .do begin
array or a terminal end-of-file (Ctrl/Z followed by the line-entering
key) is encountered.
.do end
.if '&machsys' = 'QNX' .do begin
array or a terminal end-of-file (CTRL/D)
is encountered.
.do end
.*
.section Display Variable Information
.*
.ix '? command'
.ix 'debugger commands' '?'
.mbox begin
? [<format>] [<item>][,<item>...]
.mbox end
.np
The "?" command may be used to display the contents of one or more
variables, array elements, character substrings, or arrays.
If
.mono <item>
is a variable, array element or character substring, the contents of
the storage location it occupies are displayed.
If
.mono <item>
is an array, each array element is displayed.
If
.mono <item>
is omitted, all names of variables and arrays in the program unit are
displayed followed by their contents.
In this case, the contents of arrays are not displayed; only dimension
information is provided.
.np
The
.mono <format>
specification is optional.
By default, variables are displayed using the rules for list-directed
output.
Under some circumstances it may be desirable to display the values of
variables using a different format.
Two alternate formats are allowed.
They are
.begpoint
.note (Z)
which displays the value(s) in hexadecimal format, or
.note (A)
which displays the value(s) in
.if '&machsys' = 'VM/SP CMS' .do begin
EBCDIC format, the character set of the IBM 370 computer.
.do end
.if '&machsys' = 'DOS' .do begin
ASCII format, the character set of the IBM Personal Computer.
.do end
.if '&machsys' = 'VMS' .do begin
ASCII format, the character set of the VAX computer.
.do end
.if '&machsys' = 'QNX' .do begin
ASCII format, the character set of the Unisys ICON computer.
.do end
.endnote
.pc
The parentheses must be specified.
Specifying "(Z)" or "(z)" is similar to using the "Z" format edit
descriptor in FORTRAN output statements.
Specifying "(A)" or "(a)" is similar to using the "A" format edit
descriptor in FORTRAN output statements.
.exam begin
DEBUG>?
A(4500)
I =           16
DEBUG>?(Z) B
 00000010
.exam end
.np
The display of variable and array contents can be halted by
.if '&machsys' = 'VM/SP CMS' .do begin
pressing the line-entering key.
.do end
.if '&machsys' = 'QNX' .do begin
.ix 'Ctrl/Rubout'
entering a CTRL/RUBOUT sequence at the keyboard.
.do end
.if '&machsys' = 'DOS' .do begin
.ix 'Ctrl/Break'
entering a Ctrl/Break sequence at the keyboard.
.do end
.if '&machsys' = 'VMS' .do begin
.ix 'Ctrl/X'
entering a Ctrl/X sequence at the keyboard.
.do end
.*
.endlevel
.*
.section Debugging Example
.*
.np
.ix 'debugger' 'example'
Let us debug the following "DEMO" program.
.millust begin
      INTEGER I, J, SQR
      DO I = 0, 2
         J = SQR( I )
         PRINT *, I, J
      ENDDO
      END

      INTEGER FUNCTION SQR( N )
      SQR = N * N
      END

.if '&machsys' = 'VM/SP CMS' .do begin
&cmpcmd. demo (debug noext
.do end
.if '&machsys' = 'DOS' .do begin
C>&cmpcmd./debug/noext demo
.do end
.if '&machsys' = 'VMS' .do begin
$ &cmpcmd./debug/noext demo
.do end
.if '&machsys' = 'QNX' .do begin
% &cmpcmd. +debug -ext demo
.do end
.millust end
In the following debug session, we will use the "Trace" command.
After each statement is displayed, we press the "I" key so that we
trace execution into subprogram calls.
.code begin

Debugger entered at MAIN PROGRAM
       2       DO I = 0, 2
DEBUG>t
Into["I" key]  Over[space bar]  Next["N" key]  Exit["Enter" key]
       2       DO I = 0, 2
       3         J = SQR( I )
Debugger entered at SQR
       7       INTEGER FUNCTION SQR( N )
       8       SQR = N * N
       9       END
Returned from SQR
       4         PRINT *, I, J
           0           0
       5       ENDDO
       3         J = SQR( I )
Debugger entered at SQR
       7       INTEGER FUNCTION SQR( N )
       8       SQR = N * N
       9       END
Returned from SQR
       4         PRINT *, I, J
           1           1
       5       ENDDO
       3         J = SQR( I )
Debugger entered at SQR
       7       INTEGER FUNCTION SQR( N )
       8       SQR = N * N
       9       END
Returned from SQR
       4         PRINT *, I, J
           2           4
       5       ENDDO
       6       END
Returned from MAIN PROGRAM
.code end
.np
In the above example, we simply traced the execution of all statements
in the program.
.np
.ix 'interrupting execution'
.ix 'debugger' 'interrupting execution'
While the program is executing, it may be halted by
.if '&machsys' = 'VM/SP CMS' .do begin
pressing the line-entering key.
.do end
.if '&machsys' = 'DOS' .do begin
.ix 'Ctrl/Break'
entering a Ctrl/Break sequence at the keyboard.
.do end
.if '&machsys' = 'VMS' .do begin
.ix 'Ctrl/X'
entering a Ctrl/X sequence at the keyboard.
.do end
.if '&machsys' = 'QNX' .do begin
.ix 'Ctrl/Rubout'
entering a CTRL/RUBOUT sequence at the keyboard.
.do end
An error message will appear indicating that the program was
interrupted followed by the debugger prompt.
You will now be able to enter any debugger command.
.if '&machsys' = 'VM/SP CMS' .do begin
Alternatively, you may type a debugger command and then press the
line-entering key.
Again, an error message will appear indicating that the program was
interrupted.
The debugger will execute the command that was entered and then prompt
for another.
.do end
.np
If, during the course of execution, an error develops (e.g., an
undefined variable), the debugger will be entered.
The user may attempt to correct the error and resume execution.
If the error is a result of some limit being exceeded (e.g., time
limit exhausted, statement limit exhausted), the limit is reset and
execution may be resumed.
