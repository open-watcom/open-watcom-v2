.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 31-jul-92	Craig Eisler	initial draft
.*
:CHAPTER id=cmds.Editor Commands
:CMT :SECTION.Introduction
This chapter describes the various editor commands that may be entered.
A command is entered by pressing the colon (':') key while in &cmdmode
:CONT.,
or by selecting the
:HILITE.Enter command
option in the
:HILITE.File
menu.
:P.
To ensure that you are in &cmdmode,
press
:HILITE.ESC
until the mode indicator says
:ILLUST.
Mode: command
:eILLUST.
Once you press the ':' key, the command entry window will appear:
:picture file='cmdenter' text='Command Entry Window'.
This position and size of the command entry window are controlled using
the
:KEYWORD.commandwindow
windowing command (character mode versions of the editor only).
Commands may be up to 512 bytes in length; the command window scrolls.
:P.
There are a special set of commands that may be entered at the command
line for controlling the various windows and menus of &edname.
:PERIOD.
These commands are discussed in the next chapter,
:HILITE.Windows and Menus
:PERIOD.
:P.
The &cmdline has a command history associated with it; the size
of this command history is controlled using the
:KEYWORD.maxclhistory
setting.  As well, the command history is preserved across sessions of
&edname if the
:KEYWORD.historyfile
parameter is set.
:P.
If a command is being executed through a mapped key (see the
:KEYWORD.map
and
:KEYWORD.mapbase
commands later in this chapter), then it can be useful to keep the command
from being added to the history.  By having a "\h" as the first characters
after the ':' character, the command will not be added to the command
history.
:SECTION.Special Keys In The Command Window
Once in the command window, a number of keys have special meaning:
:INCLUDE file='speckeys'.
:SECTION id='lineadd'.Line Addresses
Some commands take a line address range and/or a line address.
A line address is composed of a line number or a special symbol.
As well, '+' and '-' may be used to add or subtract from the current address;
e.g.:
:DEFLIST.
:DEFITEM.5+7
Indicates line 13
:DEFITEM.100-5
indicates line 95
:DEFITEM.99-11+6
indicates line 94
:eDEFLIST.
:P.
Special symbols are:
:DEFLIST.
:DEFITEM.. (dot)
Represents the current line number.
:DEFITEM.$ (dollar)
Represents the last line number.
:DEFITEM.% (percent)
Represents the address range 1,$ (the entire file)
:DEFITEM.# (pound)
Represents the current selected region
:DEFITEM.'a (front quote)
Indicates the line with the mark 'a' set; marks 'a' through 'z'
may be used.
:DEFITEM./regexp/
Indicates the first line following the current line that has
the regular expression
:CMDPARM.regexp
:PERIOD.
If
:CMDPARM.regexp
is not specified, then the last regular expression search is used.
:DEFITEM.?regexp?
Indicates the first line previous to the current line that has
the regular expression
:CMDPARM.regexp
:PERIOD.
If
:CMDPARM.regexp
is not specified, then the last regular expression search is used.
:eDEFLIST.
:P.
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:PERIOD.
:P.
If a line address is not specified, then the current line is assumed.
If + or - are the first character, then they are assumed to operate
on the current line number; e.g. specifying +1 takes you forward one
line in the file.
:P.
A line address range is two line addresses separated by a comma; these
indicate the start and end of the line address range.
:SUBSECT.Line Address Examples
:DEFLIST.
:DEFITEM..-5
5 lines before the current line
:DEFITEM.1,5
lines 1 to 5.
:DEFITEM..-10,.+10
10 lines before the current line to 10 lines past the current line
:DEFITEM.$-5
5 lines before the last line in the file
:DEFITEM.%
all lines
:DEFITEM.#
all lines in the currently selected region
:DEFITEM.'a,.
line with mark 'a' to current line.
:DEFITEM./foo/
the first line after the current line containing 'foo'
:DEFITEM.'z+5,$-10
5 lines past line with mark 'z' to 10 lines before the end of the file.
:DEFITEM./foo/+5,/bar/-1
5 lines past line next line containing 'foo' to
 1 line before the line containing 'bar'.
:eDEFLIST.
:eSUBSECT.
:SECTION.Commands
:INCLUDE file='cmdintro'.
For example, in its syntax model,
the edit command is specified as "Edit", indicating that the "e", "ed",
"edi", and "edit" are acceptable abbreviations (and that "e" is the shortest
of them).
:P.
Some commands are noted as "
:KEYWORD pc=1.EX mode
only".  This means that the command
is not available from the normal &cmdline;
it may only be used from an editor
:KEYWORD.script
or from
:KEYWORD pc=1.EX mode
(which is entered by pressing 'Q' in &cmdmode).

:FNLIST.Commands

:FUNC.> (Shift Right)
:SYNTAX.<line_range> >
This command shifts the specified line range &range to the
right
:KEYWORD.shiftwidth
spaces, inserting necessary leading tabs if
:KEYWORD.realtabs
is specified.
:EXAMPLE.1,. >
Shifts from first line to current line to the right
:KEYWORD.shiftwidth
spaces.
:SEEALSO.
:SEE.<
:SEE key=1.>
:SEE key=1.<
:eSEEALSO.
:eFUNC.

:FUNC.< (Shift Left)
:SYNTAX.<line_range> <
This command shifts the specified line range &range to the
left
:KEYWORD.shiftwidth
spaces.
:EXAMPLE..,$ <
Shifts entire file
:KEYWORD.shiftwidth
spaces to the left.
:SEEALSO.
:SEE.>
:SEE key=1.>
:SEE key=1.<
:eSEEALSO.
:eFUNC.

:FUNC.! (Filter)
:SYNTAX.<line_range> ! <cmd>
If &range is specified, then the lines are run through the specified
system command &parm1 (the command must get its input from standard in and
write its output to standard out) and replaces the lines with the output 
of the command.
:BLANKLINE.
If no range is specified, then the system command
&parm1 is run.  If &parm1 is not specified, then an operating
system shell is started.
:BLANKLINE.
The global variable %(Sysrc) contains the return code
from the last system command, and %(Syserr) contains
the errno value.
:EXAMPLE.1,$ ! sort
Takes all lines in the current edit buffer and runs them through the
:ITALICS.sort
command.  The lines are then replaced with the output of the sort
command.
:EXAMPLE.! dir
Executes the system
:ITALICS.dir
command.  After
:ITALICS.dir
is finished executing, you are prompted to press a key before returning
to the editor.
:EXAMPLE.!
Temporarily leaves &edname and enters an operating system command shell.
:SEEALSO.
:SEE.shell
:SEE key=1.!
:eSEEALSO.
:eFUNC.

:FUNC.ABBREV
:SYNTAX.* ABbrev <short> <long>
Create an abbreviation of &parm1 for &parm2.
:PERIOD.
Whenever &parm1 is typed
as a word during &tinsmode, it is expanded to &parm2.
:PERIOD.
:KEYWORD.unabbrev
is used to remove the abbreviation.
:EXAMPLE.abbrev wh while(
Whenever
:ITALICS.wh
is entered as a word, the word
:ITALICS.while(
is substituted.
:SEEALSO.
:SEE.unabbrev
:eSEEALSO.
:eFUNC.

:FUNC.ALIAS
:SYNTAX.* ALias <alias> <data>
Creates an &cmdline alias of &parm1 for &parm2.
:PERIOD.
Whenever &parm1
is typed on the &cmdline, the full command &parm2 is substituted.
:KEYWORD.unalias
is used to remove the abbreviation.
:EXAMPLE.alias ai set autoindent
Whenever
:ITALICS.ai
is entered on the &cmdline, the command
:ITALICS.set autoindent
is executed.
:SEEALSO.
:SEE.unalias
:eSEEALSO.
:eFUNC.

:FUNC.APPEND
:SYNTAX.<line_num> Append
Appends source lines after line &range.
:PERIOD.
Append is terminated when a line
with nothing but a dot ('.') is entered.
:EXMODE.
:SEEALSO.
:SEE.change
:SEE.insert
:eSEEALSO.
:eFUNC.

:FUNC.CASCADE
:SYNTAX.* CASCADE
Causes all edit buffer windows to cascade (overlap each other with top
border of each visible).
:SEEALSO.
:SEE.maximize
:SEE.minimize
:SEE.movewin
:SEE.resize
:SEE.size
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.CD
:SYNTAX.* CD <dir>
Changes current working directory to &parm1.
:PERIOD.
If &parm1 is not specified,
then the current directory is displayed in the message window.  &parm1
may be specified with drive and path.
:EXAMPLE.cd c:\tmp
Changes to the \tmp directory of the c drive
:EXAMPLE.cd
Display the current working directory
:eFUNC.

:FUNC.CHANGE
:SYNTAX.<line_range> Change
Deletes the line range &parm1, and replaces the range with inputted
source lines.  The input of text is terminated when a line with
nothing on it but a dot ('.') is entered.
:EXMODE.
:SEEALSO.
:SEE.append
:SEE.insert
:eSEEALSO.
:eFUNC.

:FUNC.COMPILE
:SYNTAX.* COMPile '"-a"' '"-A"' <script> <result>
Compiles the editor
:KEYWORD.script
&parm3
:PERIOD.
:BLANKLINE.
If
:CMDPARM.-a
is specified,
all local variables are translated at compile time (rather
than at run time) - this is useful for the
:KEYWORD.configuration script
:PERIOD.
:BLANKLINE.
If
:CMDPARM.-A
is specified, all variables (both local and global)
are translated at compile time.
:BLANKLINE.
The file will be compiled
into a file with the same name as the script and the extension ._vi.,
unless &parm4 is specified.
:BLANKLINE.
For information on editor
:KEYWORD.scripts
:CONT.,
see the chapter
:HDREF refid='scripts'.
:PERIOD.
:EXAMPLE.comp -a ed.cfg
Compiles the default
:KEYWORD.configuration script
:ITALICS.ed.cfg
and expands all local variables to any values assigned to them in
the
:KEYWORD.script
:PERIOD.
The compiled
:KEYWORD.script
file
:ITALICS.ed._vi
is generated.
:BLANKLINE.
If any errors occur while compiling, a file with the the same name
as the script and the extension .err
is created.
:EXAMPLE.comp test.vi test.out
Compiles the
:KEYWORD.script
:ITALICS.test.vi
and generates the compiled
:KEYWORD.script
file
:ITALICS.test.out
:PERIOD.
:SEEALSO.
:SEE.load
:SEE.source
:eSEEALSO.
:eFUNC.

:FUNC.COMPRESS
:SYNTAX.* COMpress
Replaces spaces in the current edit buffer with 
tabs.  Single spaces are not replaced with a tab, and spaces inside
a quoted string are not replaced with a tab.
:eFUNC.

:FUNC.COPY
:SYNTAX.<line_range> COpy <line_number>
Copies the specified range of lines &range after the line &parm1.
:PERIOD.
:EXAMPLE.% copy $
Copies the entire file and places the lines after the last line
in the file.
:EXAMPLE.1,. copy .
Copies all lines from the beginning of the file to the current line
and places the lines after the current line.
:eFUNC.

:FUNC.DATE
:SYNTAX.* DAte
Display the current time and date in the message window.
:eFUNC.

:FUNC.DELETE
:SYNTAX.<line_range> Delete <buffer>
Deletes the specified line range &range.
:PERIOD.
:BLANKLINE.
If &parm1 is not specified,
the text is deleted into the active &copybuffer
:PERIOD.
:BLANKLINE.
If &parm1 ('1'-'9', or 'a'-'z') is specified, the text is deleted into
that &copybuffer
:PERIOD.
:BLANKLINE.
The
:KEYWORD.put
command may be used to place contents of a &copybuffer into the file.
:EXAMPLE.% d
Deletes all lines into the active &copybuffer
:PERIOD.
:EXAMPLE.1,10 d a
Deletes lines 1 to 10 into the named &copybuffer
:ITALICS.a
:PERIOD.
:SEEALSO.
:SEE.move
:SEE.put
:SEE.yank
:SEE key=1.d
:SEE key=1.p
:SEE key=1.P
:SEE key=1.y
:eSEEALSO.
:eFUNC.

:FUNC.ECHO
:SYNTAX.* ECHO <line> <msg>
Echos the message &parm2 on line &parm1 of the message window.
:BLANKLINE.
If &parm1 is specified as
:ITALICS.off
:CONT.,
then nothing is echoed into the message window from then on.
:BLANKLINE.
If &parm1 is specified as
:ITALICS.on
:CONT.,
messages will start to appear in the message window again.
:BLANKLINE.
&parm2 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
:EXAMPLE.echo 1 "hello world"
The words
:ITALICS.hello world
will appear on line 1 of the message window.
:EXAMPLE.echo off
Disables output to the message window.
:EXAMPLE.echo on
Enables output to the message window.
:EXAMPLE.echo 2 /Line 2 message/
The words
:ITALICS.Line 2 message
will appear on line 2 of the message window.
:eFUNC.

:FUNC.EDIT
:SYNTAX.* Edit '"!"' <files>
Edits the specified files &parm2.
:PERIOD.
&parm2 may be a single file or a list of files.
:BLANKLINE.
Each file name may contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
:BLANKLINE.
If &parm1 is specified, then the current file is discarded before editing
the new file(s).
:BLANKLINE.
If a file is already being edited,
then control is simply transferred to the window with that file.
If the file is already being edited and you specified a different path,
then a new copy of the file is read from the disk and is edited, unless
:KEYWORD.samefilecheck
is set.
If
:KEYWORD.samefilecheck
is set, then control is transferred to the window with the original copy
of the file.
:BLANKLINE.
If you wish to have have multiple views on the same file, the
:KEYWORD.open
command is available.
:BLANKLINE.
If &parm2 is not specified, then a window containing
a list of files in the current directory is opened.
:INCLUDE file='fsdesc'.
:EXAMPLE.edit test.c
Edits the file test.c.
:EXAMPLE.edit! test2.c
Discards the current file, and edits test2.c.
:EXAMPLE.edit test.(c|h)
Edits the file test.c if it exists, and the file test.h if it exists.
:EXAMPLE.edit ([a-c])*.c
Edits all files in the current directory that start with the letters
a, b or c and have the extension .c.
:EXAMPLE.edit *
Edits all files in the current directory.
:EXAMPLE.edit
Gives a file selection display:
:picture file='filesel' text='File Selection Display'.
:SEEALSO.
:SEE.open
:SEE.read
:SEE.view
:SEE.visual
:eSEEALSO.
:eFUNC.

:FUNC.EGREP
:SYNTAX.* EGRep <regexp> <files>
Searches the file list &parm2 for the regular expression &parm1.
:PERIOD.
:BLANKLINE.
&parm1 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
:BLANKLINE.
&parm2 may be a single file, a directory, or a list of files.  &parm2 may
contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
:BLANKLINE.
If &parm2 is not specified, it defaults to the setting of
:KEYWORD.grepdefault
:PERIOD.
If a directory is specified for &parm2, then the files in that directory
are searched, based on the setting of
:KEYWORD.grepdefault
:PERIOD.
:BLANKLINE.
When all matches are found, a selection window is presented with
all the files that contained &parm1.
:PERIOD.
:BLANKLINE.
If you are not using regular
expressions, the
:KEYWORD.fgrep
command is much faster.
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:PERIOD.
:EXAMPLE.egrep ((if)|(while)) *.c
Searches all files in the current directory ending in the extension .c
for the regular expression
:ITALICS.((if)|(while))
:PERIOD.
:EXAMPLE.egrep [a-z]a+
Searches all files specified by
:KEYWORD.grepdefault
for the regular expression
:ITALICS.[a-z]a+
:PERIOD.
:EXAMPLE.egrep [a-z]a+ ..\c
Searches all files in the directory ..\c specified by
:KEYWORD.grepdefault
for the regular expression
:ITALICS.[a-z]a+
:PERIOD.
:SEEALSO.
:SEE.fgrep
:eSEEALSO.
:eFUNC.

:FUNC.EVAL
:SYNTAX.* EVAL <expr>
Evalutes a given mathematical expression, and displays the result on
line 1 of the message window.  For a full explanation of the rules
for formulating the expression &parm1, see the chapter
:HDREF refid='scripts'.
:PERIOD.
:EXAMPLE.eval 5+7*3
Evaluates
:ITALICS.5+7*3
and displays the result (26) in the message window.
:EXAMPLE.eval (5+7)*3
Evaluates
:ITALICS.(5+7)*3
and displays the result (36) in the message window.
:EXAMPLE.eval ((12+3)*100)/50-3
Evaluates
:ITALICS.((12+3)*100)/50-3
and displays the result (27) in the message window.
:eFUNC.

:FUNC.EXECUTE
:SYNTAX.* EXECUTE <str>
Execute the string &parm1.
:PERIOD.
This causes the editor to behave as if
the string &parm1 was typed at the keyboard.
:BLANKLINE.
Special keys in the string are indicated as follows:
:INCLUDE file='spmapkey'
:BLANKLINE.
See
:KEYWORD.keypush
for delayed execution of keystrokes.
:EXAMPLE.execute dd
Acts as if
:ITALICS.dd
were typed, and the current line gets deleted.
:EXAMPLE.execute :eval 5*3\n
Acts as if
:ITALICS.:eval 5*3<ENTER>
was typed, and the number 15 gets displayed in the message window.
:SEEALSO.
:SEE.keyadd
:SEE.map
:SEE.mapbase
:eSEEALSO.
:eFUNC.

:FUNC.EXITALL
:SYNTAX.* EXITALL
Exits all files.  For each file that has been modified, you are prompted
as to whether you want to save the file or not.
:SEEALSO.
:SEE.quitall
:SEE key=1.CTRL_C
:eSEEALSO.
:eFUNC.

:FUNC.EXPAND
:SYNTAX.* EXPAND
Replaces all tabs in the current edit buffer with spaces.
:eFUNC.

:FUNC.FGREP
:SYNTAX.* FGRep '"-c"' '"-u"' <string> <files>
Searches the file list &parm4 for the string &parm3.
:PERIOD.
:BLANKLINE
The search is by default case insensitive, unless
:CMDPARM.-c
is specified, which forces the search to be case sensitive.
Specifying
:CMDPARM.-u
causes the setting of
:KEYWORD.caseignore
to determine whether or not to be case sensitive in the search.
:BLANKLINE.
&parm3 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
:BLANKLINE.
&parm4 may be a single file or a list of files.  Each file name may
contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
:BLANKLINE.
If &parm4 is not specified, it defaults to the setting of
:KEYWORD.grepdefault
:PERIOD.
:BLANKLINE.
While searching for the item, &edname displays a window that shows
all files being searched.  This will appear as follows:
:picture file='grepsrch' text='Grep Working Display'.
When all matches are found, a selection window is presented with
all the files that contained &parm4.
:PERIOD.
:EXAMPLE.fgrep window *.c
Searches all files in the current directory ending in the extension .c
for the string
:ITALICS.window
:PERIOD.
This will produce out similar to the following:
:picture file='grepwin' text='Grep Result Display'.
:SEEALSO.
:SEE.egrep
:eSEEALSO.
:eFUNC.

:FUNC.FILES
:SYNTAX.* Files
Opens a window with the a list of all files current being edited.
Files that have been modified will have a '*' beside them.
A file may be selected from this list either with the keyboard or
with the mouse.
:EXAMPLE.files
Displays following screen
:picture file='files' text='Current File List'.
:eFUNC.

:FUNC.FLOATMENU
:SYNTAX.* FLOATMENU <id> <slen> <x1> <y1>
Activates the floating (popup) menu &parm1 (&parm1 may be 0, 1, 2 or 3).
:BLANKLINE.
The floating menus are defined using the &cmdline command
:KEYWORD.menu
(See the section
:HDREF refid='menucmd'.
).
:BLANKLINE.
&parm3, &parm4 specify the coordinates of the upper left hand corner
of the floating menu.  (0,0) specifies the upper left hand corner
of the screen.
:BLANKLINE.
&parm2 defines the length of a string that the menu is around; the
floating menu will try to appear around the string (&parm1 may
be 0).  This is useful if you wish to pop up a menu around some
selected characters on a line, for example.
:BLANKLINE.
If &parm2 is non-zero, then the coordinates
is assumed to be the lower right-hand side of the string that the menu
is popping up around.
:EXAMPLE.float 0 0 10 10
Bring up floating menu 0 at location (10,10) on the screen.  There is
no string, so the menu position will simply cycle around (10,10)
in an attempt to position itself.
:EXAMPLE.float 0 6 50 20
Bring up floating menu 0 at location (50,20) on the screen.  This
is assumed to be on the lower right hand side of a string of length 5
(i.e., it is assumed that the string is on line 19 and starts at column
45). If the menu cannot fit by opening at (50,20), it will try to open
at all other corners of the string.
:SEEALSO.
:SEE.menu
:SEE.input
:SEE.get
:eSEEALSO.
:eFUNC.

:FUNC.GENCONFIG
:SYNTAX.* GENCONFIG <filename>
Writes the current editor configuration out to the file &parm1.
:PERIOD.
If &parm1 is not specified, then the file name "ed.cfg" is assumed.
:eFUNC.

:FUNC.GLOBAL
:SYNTAX.<line_range> Global '"!"' /<regexp>/ <cmd>
For each line in the range &range that matches the regular expression
:CMDPARM.<regexp>
:CONT.,
the editor command &parm3 is executed.  &parm3 may contain replacement
expressions, see the chapter
:HDREF refid='rxchap'.
for more information.
:BLANKLINE.
If &parm1 is specified, then the command &parm3 is executed on every line
that does NOT match the regular expression
:CMDPARM.<regexp>
:PERIOD.
:BLANKLINE.
If &range is not specified, then the
:KEYWORD.global
command operates on the entire line range of the current edit buffer.
:EXAMPLE.g/printf/d
Deletes all lines in the current edit buffer that have the word
:ITALICS.printf
:PERIOD.
:EXAMPLE.g!/[a-c]123/ d
Deletes all lines in the current edit buffer that DO not match the
regular expression
:ITALICS.[a-c]123
:PERIOD.
:EXAMPLE.g/(abc)*/ execute Iabc\e
For every line that matches the regular expression
:ITALICS.(abc)*
:CONT.,
execute the keystrokes
:ITALICS.Iabc<ESC>
(this will insert the characters abc at the start of the line).
:SEEALSO.
:SEE.substitute
:eSEEALSO.
:eFUNC.

:FUNC.HELP
:SYNTAX.* HELP <topic>
Starts a view-only edit buffer on help for a specified topic.  Possible
topics are:
:DEFLIST.break
:DEFITEM.COMmandline
All &cmdline commands.
:DEFITEM.KEYS
What different keystrokes do in &cmdmode
:PERIOD.
:DEFITEM.REGularexpressions
How to use regular expressions.
:DEFITEM.SETtings
Everything that can be modified with the
:KEYWORD.set
&cmdline command.
:DEFITEM.SCRipts
&edname
:KEYWORD.script
guide.
:DEFITEM.STARTing
How to start the editor: switches, required files.
:eDEFLIST.
:EXAMPLE.help com
Gives help on &cmdline commands.
:EXAMPLE.help
Gives list of help topics.
:eFUNC.

:FUNC.INSERT
:SYNTAX.<line_number> Insert
Inserts text after a the specified line number &range.
:PERIOD.
Insert is terminated when a line with nothing but a '.' is entered.
:EXMODE.
:SEEALSO.
:SEE.append
:SEE.change
:eSEEALSO.
:eFUNC.

:FUNC.JOIN
:SYNTAX.<line_range> Join
Joins the lines in the specified range &range into a single line
(lines are concatenated one after another).
:EXAMPLE..,.+2 join
Joins the current line and the next 2 lines into a single line.
:SEEALSO.
:SEE key=1.J
:eSEEALSO.
:eFUNC.

:FUNC.KEYADD
:SYNTAX.* KEYAdd <string>
Adds a set of keystrokes &parm1 to the key buffer just as if they
were typed by the user.  The processing of these keystrokes is deferred
until &edname finishes its current processing, and is ready to process
keystrokes again.
This is different than the
:KEYWORD.execute
command, which processes the keystrokes immediately.
:BLANKLINE.
:KEYWORD.Keyadd
is useful in a
:KEYWORD.script
:CONT.,
because it allows keystrokes to be executed after the
:KEYWORD.script
is exited.
This prevents re-entrance of a script that is being executed by a
mapped key in input mode, for example.
:BLANKLINE.
Keys are processed in FIFO order. Multiple
:KEYWORD.keyadd
commands cause more keys to queue up for processing.
:BLANKLINE.
If you need to use one or more special keys (e.g. ENTER, F1, etc) in &parm1,
they may specified as follows:
:BLANKLINE.
:INCLUDE file='spmapkey'
:BLANKLINE.
:EXAMPLE.keyadd dd
Acts as if
:ITALICS.dd
were typed, and the current line gets deleted.
:EXAMPLE.keyadd :eval 5*3\n
Acts as if
:ITALICS.:eval 5*3<ENTER>
was typed, and the number 15 gets displayed in the message window.
:SEEALSO.
:SEE.execute
:SEE.map
:SEE.mapbase
:eSEEALSO.
:eFUNC.

:FUNC.LIST
:SYNTAX.<line_range> List
Lists lines in the specified line range.
:EXMODE.
:eFUNC.

:FUNC.LOAD
:SYNTAX.* LOAD <script>
Loads a
:KEYWORD.script
into memory for the life of the edit session.
This allows for much faster access to the script,
since the data structures for the script do not have to be built
every time the script is invoked.  This is especially important
for a
:KEYWORD.hook script
:PERIOD.
:BLANKLINE.
For information on editor
:KEYWORD.scripts
:CONT.,
see the chapter
:HDREF refid='scripts'.
:PERIOD.
:EXAMPLE.load rdme._vi
Loads the
:KEYWORD.script
:ITALICS.rdme._vi
and makes it resident.
:SEEALSO.
:SEE.compile
:SEE.source
:eSEEALSO.
:eFUNC.

:FUNC.MAP
:SYNTAX.* MAP '"!"' <key> <string>
Tells the editor to run the string of keys &parm3 whenver the
key &parm2 is pressed in &cmdmode
:PERIOD.
:BLANKLINE.
If &parm1 is specified,
then the string of keys &parm3 is executed whenever &parm2 is pressed
in &tinsmode
:PERIOD.
:BLANKLINE.
When a mapped key is pressed, it acts is if the characters in &parm3
are being typed at the keyboard.
Recursion does not occur; if a key that is mapped is executed after
it has been executed as a mapped key, then the default behaviour for that
key is used, e.g.:
:ILLUST.
map a 0a
:eILLUST.
will cause the editor to move to column 1, and then start appending
after the character in column 1.
:BLANKLINE.
If you need to specify a special key (e.g. ENTER, F1, etc) in &parm2,
you specify a symbolic name for that key.
There are a number of pre-defined keys
symbols that are recognized when specifying which key is being
mapped/unmapped. These are described in the Appendix
:HDREF refid='symkey'.
:PERIOD.
:BLANKLINE.
If you need to use one or more special keys (e.g. ENTER, F1, etc) in &parm3,
then you may enter:
:BLANKLINE.
:INCLUDE file='spmapkey'
:BLANKLINE.
To remove a mapping, use the
:KEYWORD.unmap
command.
:EXAMPLE.map K \x:next\n
Whenever K is pressed in &cmdmode, the
:KEYWORD.next
command is executed. The command window will not be displayed,
because of the
:ITALICS.\x
:PERIOD.
:EXAMPLE.map CTRL_T \x:\hda\n
Whenever CTRL_T is pressed in &cmdmode,
the current date is displayed. The command window will not be
displayed, because of the
:ITALICS.\x
:PERIOD.
The command will not be added to the command history, because of the
:ITALICS.\h
:PERIOD.
:EXAMPLE.map CTRL_W \x:fgrep \<CTRL_W>\n
Whenever CTRL_W is pressed in &cmdmode, an fgrep command, searching for
the current word, is executed.  The
:ITALICS.\x
keeps the command window from opening.
:ITALICS.\<CTRL_W>
simulates CTRL_W being pressed, so the current word is inserted into
the &cmdline
:PERIOD.
:EXAMPLE.map! CTRL_W \edwi
Whenever CTRL_W is pressed in &tinsmode, &tinsmode
is exited (\e simulates the ESC key being pressed), the current word is
deleted, and &tinsmode is re-entered.  This has the effect of
deleting the current word in &tinsmode
and appearing to remain in &tinsmode
:PERIOD.
:SEEALSO.
:SEE.execute
:SEE.keyadd
:SEE.unmap
:eSEEALSO.
:eFUNC.

:FUNC.MAPBASE
:SYNTAX.* MAPBASE <key> <string>
Tells the editor to run the string of keys &parm3 whenver the
key &parm2 is pressed in &cmdmode
:PERIOD.
:BLANKLINE.
This works the same as the
:KEYWORD.map
command, only all characters in &parm2 work as their base meaning; that is,
all key mappings are ignored and the keys have their default
behaviour.
:SEEALSO.
:SEE.execute
:SEE.keyadd
:SEE.map
:SEE.unmap
:eSEEALSO.
:eFUNC.

:FUNC.MARK
:SYNTAX.<line_number> MArk <markname>
Sets the
:KEYWORD.text mark
&parm1 on the line &range.
:PERIOD.
The mark name is a single letter from
:ITALICS.a
to
:ITALICS.z
:PERIOD.
This mark may then be referred to on the &cmdline or in &cmdmode
by using a front quote (') before the mark name, e.g.:
:ILLUST.
'a
:eILLUST.
:EXAMPLE.mark a
Sets the mark
:ITALICS.a
on the current line.  Typing the command
:ITALICS.'a
will return you to that mark.
:EXAMPLE.100 mark z
Sets the mark
:ITALICS.z
on line 100.
:SEEALSO.
:SEE key=1.m
:eSEEALSO.
:eFUNC.

:FUNC.MATCH
:SYNTAX.* match //<rx1>/<rx2>//
Set what is matched by the '%' &cmdmode command.  Defaults are "{","}"
and "(",")". For example, by pressing the percent key ('%') when
the cursor is on the first open bracket ('(') in the line:
:ILLUST.
    if( ( i=foo( x ) ) ) return;
:eILLUST.
moves the cursor to the last ')' in the line.
:BLANKLINE.
This command allows you to extend what is matched to 
general regular expressions.
:ITALICS.<rx1>
is the regular expression that opens a match,
:ITALICS.<rx2>
is the regular expression that closes a match.
:BLANKLINE.
Note that in the matching regular expressions, 
:KEYWORD.magic
is set (special characters automatically
have their meaning, and do not need to be escaped).
:BLANKLINE.
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:PERIOD.
:EXAMPLE.match /# *if/# *endif/
This adds the matching of all
:ITALICS.#if and
:ITALICS.#endif
commands (an arbitrary number of spaces is allowed to occur between
the '#' sign and the
:ITALICS.if
or
:ITALICS.endif
words.  If '%' is pressed while over a
:ITALICS.#if
statement, the cursor is moved to the corresponding
:ITALICS.#endif
statement.
:SEEALSO.
:SEE key=1.%
:eSEEALSO.
:eFUNC.

:FUNC.MAXIMIZE
:SYNTAX.* MAXimize
Maximizes the current edit buffer window.
:SEEALSO.
:SEE.cascade
:SEE.minimize
:SEE.movewin
:SEE.resize
:SEE.size
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.MINIMIZE
:SYNTAX.* MINimize
Minimizes the current edit buffer window.
:SEEALSO.
:SEE.cascade
:SEE.maximize
:SEE.movewin
:SEE.resize
:SEE.size
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.MOVE
:SYNTAX.<line_range> Move <line_number>
Deletes the specified range of lines &range and places them
after the line &parm1.
:PERIOD.
:EXAMPLE.1,10 move $
Moves the first 10 lines of the file after the last line in the file.
:EXAMPLE.1,. move .+1
Deletes all lines from the beginning of the file to the current line
and places the lines one line after the current line.
:SEEALSO.
:SEE.delete
:eSEEALSO.
:eFUNC.

:FUNC.MOVEWIN
:SYNTAX.* MOVEWin
Enter window movement mode.  The cursor keys are then used to move the
current edit buffer window.
:SEEALSO.
:SEE.cascade
:SEE.maximize
:SEE.minimize
:SEE.resize
:SEE.size
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.NEXT
:SYNTAX.* Next
Moves to the next file in the list of files being edited.
:SEEALSO.
:SEE key=1.F1
:SEE key=1.F2
:SEE.prev
:eSEEALSO.
:eFUNC.

:FUNC.OPEN
:SYNTAX.* Open <file>
Opens a new window on the specified file. 
If no file is specified,
then a new window is opened on the current edit buffer.
These new
windows are different views on the same edit buffer.
:BLANKLINE.
Once multiple views
on an edit buffer are opened, the window border contains a number indicating
which view of the edit buffer is associated with that file:
:picture file='open' text='Two Views On The Same File'.
:SEEALSO.
:SEE.edit
:SEE.view
:SEE.visual
:eSEEALSO.
:eFUNC.

:FUNC.POP
:SYNTAX.* POP
Restores the last pushed file position.
:BLANKLINE
The setting
:KEYWORD.maxpush
controls the maximum number of push commands that will be remembered.
:SEEALSO.
:SEE key=1.F11
:SEE key=1.F12
:SEE.push
:SEE.tag
:eSEEALSO.
:eFUNC.

:FUNC.PREV
:SYNTAX.* Prev
Moves to the previous file in the list of files being edited.
:SEEALSO.
:SEE key=1.F1
:SEE key=1.F2
:SEE.next
:eSEEALSO.
:eFUNC.

:FUNC.PUSH
:SYNTAX.* PUSH
Saves the current file position.  The next
:KEYWORD.pop
command will cause a return to this position.
:BLANKLINE
The setting
:KEYWORD.maxpush
controls the maximum number of push commands that will be remembered.
:BLANKLINE
The
:KEYWORD.tag
command does an implicit push.
:SEEALSO.
:SEE key=1.F11
:SEE key=1.F12
:SEE.pop
:SEE.tag
:eSEEALSO.
:eFUNC.

:FUNC.PUT
:SYNTAX.<line_number> PUt '"!"' <buffer>
Puts (pastes) the &copybuffer
&parm2 ('1'-'9', or 'a'-'z') after the
line &range.
:PERIOD.
:BLANKLINE.
If &parm2 is not specified, the active &copybuffer is assumed.
:BLANKLINE.
If &parm1 is specified, then the lines are put before the
line &range.
:PERIOD.
:EXAMPLE.put
Pastes the active &copybuffer after the current line.
:EXAMPLE.1 put!
Pastes the active  &copybuffer before the first line in the edit buffer.
:EXAMPLE.$ put a
Pastes named &copybuffer
:ITALICS.a
after the last line in the file.
:EXAMPLE.put! 4
Pastes numbered &copybuffer before the current line.
:SEEALSO.
:SEE.delete
:SEE.yank
:SEE key=1.p
:SEE key=1.P
:eSEEALSO.
:eFUNC.

:FUNC.QUIT
:SYNTAX.* Quit '"!"'
Quits the current file.
:KEYWORD.Quit
will not quit a modified file, unless &parm1 is specified, in which
case all changes are discarded and the file is exited.
:EXAMPLE.q!
Quits the current file, discarding all modifications since the last write.
:SEEALSO.
:SEE.write
:SEE.wq
:SEE.xit
:SEE key=1.Z
:eSEEALSO.
:eFUNC.

:FUNC.QUITALL
:SYNTAX.* QUITAll
Exits the editor if no files have been modified.  If files have been modified,
a prompt is displayed asking to you verify that you really want to discard
the modified file(s).  
If you do not respond with a 'y', then the command is cancelled.
:EXAMPLE.quitall
If files have been modified, the following prompt is displayed:
:picture file='quitall' text='Really Exit Prompt'.
:SEEALSO.
:SEE.exitall
:SEE key=1.CTRL_C
:eSEEALSO.
:eFUNC.

:FUNC.READ
:SYNTAX.<line_number> Read <file_name>
Reads the text from file &parm1 into the current edit buffer.
The lines are placed after line specified by &range.
:PERIOD.
If &range is not specified, the current line is assumed.
:BLANKLINE.
Line 0 may be specified as &range in order to read a file
in before the first line of the current edit buffer.
:BLANKLINE.
Each file name &parm1 may contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
:BLANKLINE.
If &parm1 is not specified, then a window containing
a list of files in the current directory is opened, from
which a file may be selected.
:BLANKLINE.
If the first character of &parm1 is a dollar sign ('$'), then this
indicates that a directory is to be read in.  If nothing follows,
then the current directory is read.  Otherwise, all characters that
follow the dollar sign are treated as a file regular expression, which
is used to read in the directory.
:EXAMPLE.0 read test.c
Reads the file test.c into the current edit buffer and places the text
before the first line of the file.
:EXAMPLE.r test.c
Reads the file test.c into the current edit buffer and places the text
after the current line.
:EXAMPLE.r $
Reads the current directory and places the data after the current line.
:EXAMPLE.r $*.c
Reads the current directory, matching only files with .c extensions,
and places the data after the current line.
:EXAMPLE.r $..\c\*.*
Reads the ..\c directory, and places the data after the current line.
:EXAMPLE.read
Gives a file selection display:
:picture file='filesel' text='File Selection Display'.
:eFUNC.

:FUNC.RESIZE
:SYNTAX.* RESize
Allows resizing of the current edit window with the keyboard.  The
cursor keys are used as follows:
:DEFLIST.break
:DEFITEM.UP
move top border up
:DEFITEM.DOWN
move top border down
:DEFITEM.LEFT
move right border left
:DEFITEM.RIGHT
move right border right
:DEFITEM.SHIFT_UP
move bottom border up
:DEFITEM.SHIFT_DOWN
move bottom border down
:DEFITEM.SHIFT_LEFT
move left border left
:DEFITEM.SHIFT_RIGHT
move left border right
:eDEFLIST.
:SEEALSO.
:SEE.cascade
:SEE.maximize
:SEE.minimize
:SEE.movewin
:SEE.size
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.SET
:SYNTAX.* SEt <variable> <value>
Certain variables within &edname may be changed after &edname is
executing.  &parm2 is assigned to &parm1.
:PERIOD.
:BLANKLINE.
If &parm1 and
&parm2 are not specified, the a window containing a list
of all boolean values is displayed.
:BLANKLINE.
If &parm1 is specified
as a
:ITALICS.2
:CONT.,
then a window containing all other values is displayed.
:BLANKLINE.
From the selection window, a variable may be selected (with ENTER or
double clicking the mouse), and a
new value entered. If the variable was boolean, then pressing
ENTER or double clicking toggles the value.
:BLANKLINE.
If a variable is a boolean variable, then it is be set via
:ILLUST.
set var    - set var to TRUE
set novar  - set var to FALSE
:eILLUST.
:BLANKLINE.
Variables other than boolean variables are set via
:ILLUST.
set var = test  - set var to 'test'
set var  test   - set var to 'test'
:eILLUST.
Note that the '=' operator is optional.
:BLANKLINE.
For information on all the different settable options,
see the chapter
:HDREF refid='edset'.
:PERIOD.
:eFUNC.

:FUNC.SETCOLOR
:SYNTAX.* SETCOLOR <c> <r> <g> <b>
Set the color number &parm1 to have the RGB value &parm2, &parm3, &parm4.
:PERIOD.
&parm1 may have a value of 0 to 15.
&parm1, &parm2, and &parm3 may have values from 0 to 63.
:BLANKLINE.
This command only has an affect under operating systems where it is possible
to remap the colors in some way (DOS).
:EXAMPLE.setcolor 1 63 0 38
This remaps color number 1 to a pink color.
:EXAMPLE.setcolor 15 25 40 38
This remaps color number 15 to a pale green color.
:eFUNC.

:FUNC.SHELL
:SYNTAX.* SHell
Escapes to an operating system shell.
:SEEALSO.
:SEE.!
:SEE key=1.!
:eSEEALSO.
:eFUNC.

:FUNC.SIZE
:SYNTAX.* SIZE <x1> <y1> <x2> <y2>
Resizes the current edit buffer window to have upper left-hand corner at
(&parm1,&parm2) and lower right-hand corner at (&parm3,&parm4).
:EXAMPLE.SIZE 0 0 10 10
Changes the size of the current edit buffer window to have
upper left-hand corner at (0,0) and lower right-hand corner at (10,10).
:SEEALSO.
:SEE.cascade
:SEE.maximize
:SEE.minimize
:SEE.movewin
:SEE.resize
:SEE.tile
:eSEEALSO.
:eFUNC.

:FUNC.SOURCE
:SYNTAX.* SOurce <script> <p1> <p2> ... <pn>
Execute &edname source
:KEYWORD.script
file &parm1.
:PERIOD.
Optional parameters &parm2 to
&parm5 may be specified, these are passed to the specified
:KEYWORD.script
:PERIOD.
:BLANKLINE.
If "." is specified as the
:KEYWORD.script
name, the current
file being edited is run as a 
:KEYWORD.script
:PERIOD.
:BLANKLINE.
For information on editor
:KEYWORD.scripts
:CONT.,
see the chapter
:HDREF refid='scripts'.
:PERIOD.
:EXAMPLE.source foo.vi abc
Executes the
:KEYWORD.script
:ITALICS.foo.vi
:CONT.,
passing it the parm
:ITALICS.abc
:PERIOD.
:EXAMPLE.source .
Executes the current edit buffer as a
:KEYWORD.script
:PERIOD.
:SEEALSO.
:SEE.compile
:SEE.load
:eSEEALSO.
:eFUNC.

:FUNC.SUBSTITUTE
:SYNTAX.<line_range> Substitute /<regexp>/<replexp>/<g><i>
Over the line range &range,
replace each occurrence of regular expression
:CMDPARM.<regexp>
with the replacement expression
:CMDPARM.<replexp>
:PERIOD.
:BLANKLINE.
Only the first occurrence on each line is replaced, unless
:CMDPARM.<g>
is specified, in which case all occurrences on a line are replaced.
:BLANKLINE.
If
:CMDPARM.<i>
is specified, each replacement is verified before it is performed.
:BLANKLINE.
See the chapter
:HDREF refid='rxchap'.
for more information on regular expression matching and substitution.
:EXAMPLE.%s/foo/bar/
Changes the first occurrence of
:ITALICS.foo
to
:ITALICS.bar
on each line of the entire file.
:EXAMPLE.1,.s/(([a-z]bc)*)/Abc\2/g
Changes all occurrences of the regular expression
:ITALICS.(([a-z]bc)*)
to the substitution expression
:ITALICS.Abc\2
:PERIOD.
The changes are only applied on lines 1 to the current line of the
current edit buffer.
:EXAMPLE.'a,'b/^abc//i
Any line that starts with
:ITALICS.abc
has the
:ITALICS.abc
changed to the null string.
The user is prompted before each change.  The changes are only applied
from the line containing mark
:ITALICS.a
to the line containing mark
:ITALICS.b
:PERIOD.
:SEEALSO.
:SEE.global
:eSEEALSO.
:eFUNC.

:FUNC.TAG
:SYNTAX.* TAG <tagname>
Searches for the tag &parm1.
:PERIOD.
Tags are kept in a special file,
which must be located somewhere in your path.
This file is controlled with the
:KEYWORD.tagfilename
setting; the default for this setting is
:ITALICS.tags
:PERIOD.
The tags file contains a collection of procedure names and typedefs,
along with the file in which they are located and a search command/line
number with which to exactly locate the tag.
:BLANKLINE.
See the appendix
:HDREF refid='ctags'
for more information.
:EXAMPLE.tag MyFunc
Locates the tag
:ITALICS.MyFunc
in the tags file, edits the source file that contains the function
:ITALICS.MyFunc
:CONT.,
and moves the cursor to the definition of the function in the source file.
:SEEALSO.
:SEE key=1.^]
:eSEEALSO.
:eFUNC.

:FUNC.TILE
:SYNTAX.* TILE '"h"' '|' '"v"' '|' <x> <y>
Tile all current file windows.  The tiling layout is specified
as an &parm5 by &parm6  grid.  The or bars ('|') in the command
syntax indicate that only one of the options may be used.
:BLANKLINE.
If no parameters are specified,
:KEYWORD.maxwindowtilex
and
:KEYWORD.maxwindowtiley
are used (this is the default tile grid).
:BLANKLINE.
If &parm1 is specified,
then files are tiled horizontally (as many as will fit).
:BLANKLINE.
If &parm3 is specified, then files are tiled vertically (as many as will fit).  
:BLANKLINE.
Specifying &parm5 &parm6 overrides the default tile grid. As a special
case, specifying &parm5 &parm6 as 1 1 causes all windows to be restored.
:EXAMPLE.tile 3 5
Tile windows 3 across and 5 high.
:EXAMPLE.tile 1 1
Untile windows.
:EXAMPLE.tile h
Tile windows horizontally.
:EXAMPLE.tile
Tile windows according to default tile grid.
:SEEALSO.
:SEE.cascade
:SEE.maximize
:SEE.minimize
:SEE.resize
:SEE.size
:SEE.movewin
:eSEEALSO.
:eFUNC.

:FUNC.UNABBREV
:SYNTAX.* UNABbrev <abbrev>
Removes the abbreviation &parm1.
:PERIOD.
See the
:KEYWORD.abbrev
command for how to set an abbreviation.
:EXAMPLE.unabbrev wh
Remove the abbreviation
:ITALICS.wh
:PERIOD.
:SEEALSO.
:SEE.abbrev
:eSEEALSO.
:eFUNC.

:FUNC.UNALIAS
:SYNTAX.* UNALias <alias>
Removes the &cmdline alias &parm1.
:PERIOD.
See the
:KEYWORD.alias
command for how to set a &cmdline alias.
:EXAMPLE.unalias ai
Remove the &cmdline alias
:ITALICS.ai
:PERIOD.
:SEEALSO.
:SEE.alias
:eSEEALSO.
:eFUNC.

:FUNC.UNDO
:SYNTAX.* Undo '"!"'
Undo the last change.  There is no limit on the number of undo's
that can be saved, except for memory.  Continuing to issue undo commands
walks you backwards through your edit history.
:BLANKLINE.
Specifying &parm1 undoes the last undo (redo).  Again, there are no
restrictions on this.  However, once you modify the file, you can longer undo the
last undo.
:SEEALSO.
:SEE key=1.u
:SEE key=1.U
:eSEEALSO.
:eFUNC.

:FUNC.UNMAP
:SYNTAX.* UNMAP '"!"' <key>
Removes the mapping of the key &parm2 for &cmdmode
:PERIOD.
If &parm1 is specified, then the key mapping is removed for &tinsmode
:PERIOD.
See the
:KEYWORD.map
command for details on mapping keys.
:BLANKLINE.
There are a number of pre-defined
symbols that are recognized for &parm1.
:PERIOD.
These are described in the Appendix
:HDREF refid='symkey'.
:PERIOD.
:EXAMPLE.unmap CTRL_W
Removes the mapping of CTRL_W for &cmdmode
:PERIOD.
Pressing CTRL_W in command
mode will now do the default action.
:EXAMPLE.unmap! F1
Removes the mapping of F1 for &tinsmode
:PERIOD.
Typing F1 in &tinsmode will now do the default action.
:SEEALSO.
:SEE.map
:SEE.mapbase
:eSEEALSO.
:eFUNC.

:FUNC.VERSION
:SYNTAX.* VERsion
Displays the current version of &edname in the message window.
:SEEALSO.
:SEE key=1.^V
:eSEEALSO.
:eFUNC.

:FUNC.VIEW
:SYNTAX.* VIEw '"!"' <file_name>
Functions the same as the
:KEYWORD.edit
command, except that it causes the file edited to be a "view only"
file (no modification commands work).
:EXAMPLE.view test.c
Edits the file
:ITALICS.test.c
in view-only mode.
:SEEALSO.
:SEE.edit
:SEE.open
:SEE.visual
:eSEEALSO.
:eFUNC.

:FUNC.VISUAL
:SYNTAX.* VIsual <file_name>
Causes &edname to re-enter visual mode (full screen editing mode)
if &edname is in
:KEYWORD pc=1.EX mode
:PERIOD.
If the filename &parm1 is specified,
this functions just like the
:KEYWORD.edit
command.
:EXAMPLE.vi test.c
Return to full screen editing mode (if in
:KEYWORD pc=1.EX mode
:CONT.)
and edit the file
test.c
:EXAMPLE.vi
Return to full screen editing mode (if in
:KEYWORD pc=1.EX mode
:CONT.)
:PERIOD.
:SEEALSO.
:SEE.edit
:SEE.open
:SEE.view
:eSEEALSO.
:eFUNC.

:FUNC.WRITE
:SYNTAX.<line_range> Write '"!"' <file_name>
Writes the specified range of lines &range to the file &parm2.
:PERIOD.
:BLANKLINE.
If no line range is specified, then all lines are written.
:BLANKLINE.
If &parm2 is not specified, then the current file is written.
If &parm2 exists, and is not the name of the file that you are writing,
then the write will fail.
:BLANKLINE.
Specifying &parm1 forces an overwrite of an existing file.
:EXAMPLE.1,10 w! test.c
Write the first 10 lines of the current file to the file
:ITALICS.test.c
:CONT.,
and overwrite
:ITALICS.test.c
if it already exists.
:EXAMPLE.w
Write out the current file.
:SEEALSO.
:SEE.quit
:SEE.wq
:SEE.xit
:SEE key=1.Z
:eSEEALSO.
:eFUNC.

:FUNC.WQ
:SYNTAX. * WQ
Writes current file, and exits.
:SEEALSO.
:SEE.quit
:SEE.write
:SEE.xit
:SEE key=1.Z
:eSEEALSO.
:eFUNC.

:FUNC.YANK
:SYNTAX.<line_range> Yank <buffer>
Yank (make a copy of) the specified line range &range.
:PERIOD.
:BLANKLINE.
If &parm1 is not specified, the text is yanked (copied) into the active
&copybuffer
:PERIOD.
:BLANKLINE.
If &parm1 ('1'-'9', or 'a'-'z') is specified, the text is yanked into
that &copybuffer
:PERIOD.
:BLANKLINE.
The
:KEYWORD.put
command may be used to place the contents of a &copybuffer into the file.
:EXAMPLE.% y
Yanks (copies) all lines into the active &copybuffer
:PERIOD.
:EXAMPLE..,$ y z
Yanks the lines from the current line to the last line in the file
into the &copybuffer
:ITALICS.z
:PERIOD.
:SEEALSO.
:SEE.delete
:SEE.put
:SEE key=1.d
:SEE key=1.y
:SEE key=1.p
:SEE key=1.P
:eSEEALSO.
:eFUNC.

:FUNC.XIT
:SYNTAX.Xit
Exits the current file, saving it if it has been modified.
:SEEALSO.
:SEE.quit
:SEE.write
:SEE.wq
:SEE key=1.Z
:eSEEALSO.
:eFUNC.

:eFNLIST.
