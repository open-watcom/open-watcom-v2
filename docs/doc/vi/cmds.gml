.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Root file of VI documentation.
.*
.* Date         By              Reason
.* ----         --              ------
.* 31-jul-92    Craig Eisler    initial draft
.* 01-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.*
.chap *refid=cmds 'Editor Commands'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
This chapter describes the various editor commands that may be entered.
A command is entered by pressing the colon (':') key while in &cmdmode
:cont.,
or by selecting the
.keyword Enter command
option in the
.keyword File
menu.
.np
To ensure that you are in &cmdmode,
press
.keyword ESC
until the mode indicator says
.millust begin
Mode: command
.millust end
Once you press the ':' key, the command entry window will appear:
.figure *depth='2.47' *scale='59' *file='vi028' Command Entry Window
This position and size of the command entry window are controlled using
the
.keyref commandwindow
windowing command (character mode versions of the editor only).
Commands may be up to 512 bytes in length; the command window scrolls.
.np
There are a special set of commands that may be entered at the command
line for controlling the various windows and menus of &edvi.
:period.
These commands are discussed in the next chapter,
:HDREF refid=winmenu.
:period.
.np
The &cmdline has a command history associated with it; the size
of this command history is controlled using the
.keyref maxclhistory
setting.  As well, the command history is preserved across sessions of
&edvi. if the
.keyref historyfile
parameter is set.
.np
If a command is being executed through a mapped key (see the
.keyref map
and
.keyref mapbase
commands later in this chapter), then it can be useful to keep the command
from being added to the history.  By having a "\h" as the first characters
after the ':' character, the command will not be added to the command
history.
.* ******************************************************************
.section 'Special Keys In The Command Window'
.* ******************************************************************
.np
Once in the command window, a number of keys have special meaning:
:INCLUDE file='speckeys'.
.* ******************************************************************
.section *refid=lineadd 'Line Addresses'
.* ******************************************************************
.np
Some commands take a line address range and/or a line address.
A line address is composed of a line number or a special symbol.
As well, '+' and '-' may be used to add or subtract from the current address;
e.g.:
.*
:DL break.
.*
:DT.5+7
:DD.Indicates line 13

:DT.100-5
:DD.indicates line 95

:DT.99-11+6
:DD.indicates line 94
.*
:eDL.
.*
.np
Special symbols are:
.*
:DL break.
.*
:DT.. (dot)
:DD.Represents the current line number.

:DT.$ (dollar)
:DD.Represents the last line number.

:DT.% (percent)
:DD.Represents the address range 1,$ (the entire file)

:DT.# (pound)
:DD.Represents the current selected region

:DT.'a (front quote)
:DD.Indicates the line with the mark 'a' set; marks 'a' through 'z'
may be used.

:DT./regexp/
:DD.Indicates the first line following the current line that has
the regular expression
.var regexp
:period.
If
.param regexp
is not specified, then the last regular expression search is used.

:DT.?regexp?
:DD.Indicates the first line previous to the current line that has
the regular expression
.param regexp
:period.
If
.param regexp
is not specified, then the last regular expression search is used.
.*
:eDL.
.*
.np
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:period.
.np
If a line address is not specified, then the current line is assumed.
If + or - are the first character, then they are assumed to operate
on the current line number; e.g. specifying +1 takes you forward one
line in the file.
.np
A line address range is two line addresses separated by a comma; these
indicate the start and end of the line address range.
.*
.beglevel
.* ******************************************************************
.section 'Line Address Examples'
.* ******************************************************************
:DL break.
.*
:DT..-5
:DD.5 lines before the current line

:DT.1,5
:DD.lines 1 to 5.

:DT..-10,.+10
:DD.10 lines before the current line to 10 lines past the current line

:DT.$-5
:DD.5 lines before the last line in the file

:DT.%
:DD.all lines

:DT.#
:DD.all lines in the currently selected region

:DT.'a,.
:DD.line with mark 'a' to current line.

:DT./foo/
:DD.the first line after the current line containing 'foo'

:DT.'z+5,$-10
:DD.5 lines past line with mark 'z' to 10 lines before the end of the file.

:DT./foo/+5,/bar/-1
:DD.5 lines past line next line containing 'foo' to
1 line before the line containing 'bar'.
.*
:eDL.
.*
.endlevel
.* ******************************************************************
.section 'Commands'
.* ******************************************************************
.np
:INCLUDE file='cmdintro'.
For example, in its syntax model,
the edit command is specified as "Edit", indicating that the "e", "ed",
"edi", and "edit" are acceptable abbreviations (and that "e" is the shortest
of them).
.np
Some commands are noted as "EX mode
only".  This means that the command
is not available from the normal &cmdline;
it may only be used from an editor
.keyword script
or from
.keyword EX mode
(which is entered by pressing 'Q' in &cmdmode).
.*
.fnlist begin Commands
.*
.begfunc &gt. '(Shift Right)'
.syntx &lt.line_range&gt. &gt.
.begdescr
This command shifts the specified line range &range to the
right
.keyref shiftwidth
spaces, inserting necessary leading tabs if
.keyref realtabs 1
is specified.
.enddescr
.xmplsect begin
.begxmpl 1,. &gt.
Shifts from first line to current line to the right
.keyref shiftwidth
spaces.
.endxmpl
.xmplsect end
.alsosee begin
.seethis &lt.
:cmt. .seethis *key=1 &gt.
:cmt. .seethis *key=1 &lt.
.alsosee end
.endfunc

.begfunc &lt. '(Shift Left)'
.syntx &lt.line_range&gt. &lt.
.begdescr
This command shifts the specified line range &range to the
left
.keyref shiftwidth
spaces.
.enddescr
.xmplsect begin
.begxmpl .,$ &lt.
Shifts entire file
.keyref shiftwidth
spaces to the left.
.endxmpl
.xmplsect end
.alsosee begin
.seethis &gt.
:cmt. .seethis *key=1 &gt.
:cmt. .seethis *key=1 &lt.
.alsosee end
.endfunc

.begfunc ! '(Filter)'
.syntx &lt.line_range&gt. ! &lt.cmd&gt.
.begdescr
If &range is specified, then the lines are run through the specified
system command &parm1 (the command must get its input from standard in and
write its output to standard out) and replaces the lines with the output 
of the command.
.np
If no range is specified, then the system command
&parm1 is run.  If &parm1 is not specified, then an operating
system shell is started.
.np
The global variable %(Sysrc) contains the return code
from the last system command, and %(Syserr) contains
the errno value.
.enddescr
.xmplsect begin
.begxmpl 1,$ ! sort
Takes all lines in the current edit buffer and runs them through the
.param sort
command.  The lines are then replaced with the output of the sort
command.
.endxmpl
.begxmpl ! dir
Executes the system
.param dir
command.  After
.param dir
is finished executing, you are prompted to press a key before returning
to the editor.
.endxmpl
.begxmpl !
Temporarily leaves &edvi. and enters an operating system command shell.
.endxmpl
.xmplsect end
.alsosee begin
.seethis shell
:cmt. .seethis *key=1 !
.alsosee end
.endfunc

.begfunc ABBREV
.syntx * ABbrev &lt.short&gt. &lt.long&gt.
.begdescr
Create an abbreviation of &parm1 for &parm2.
:period.
Whenever &parm1 is typed
as a word during &tinsmode, it is expanded to &parm2.
:period.
.keyref unabbrev
is used to remove the abbreviation.
.enddescr
.xmplsect begin
.begxmpl abbrev wh while(
Whenever
.param wh
is entered as a word, the word
.param while(
is substituted.
.endxmpl
.xmplsect end
.alsosee begin
.seethis unabbrev
.alsosee end
.endfunc

.begfunc ALIAS
.syntx * ALias &lt.alias&gt. &lt.data&gt.
.begdescr
Creates an &cmdline alias of &parm1 for &parm2.
:period.
Whenever &parm1
is typed on the &cmdline, the full command &parm2 is substituted.
.keyref unalias
is used to remove the abbreviation.
.enddescr
.xmplsect begin
.begxmpl alias ai set autoindent
Whenever
.param ai
is entered on the &cmdline, the command
.param set autoindent
is executed.
.endxmpl
.xmplsect end
.alsosee begin
.seethis unalias
.alsosee end
.endfunc

.begfunc APPEND
.syntx &lt.line_num&gt. Append
.begdescr
Appends source lines after line &range.
:period.
Append is terminated when a line
with nothing but a dot ('.') is entered.
.exmode
.enddescr
.alsosee begin
.seethis change
.seethis insert
.alsosee end
.endfunc

.begfunc CASCADE
.syntx * CASCADE
.begdescr
Causes all edit buffer windows to cascade (overlap each other with top
border of each visible).
.enddescr
.alsosee begin
.seethis maximize
.seethis minimize
.seethis movewin
.seethis resize
.seethis size
.seethis tile
.alsosee end
.endfunc

.begfunc CD
.syntx * CD &lt.dir&gt.
.begdescr
Changes current working directory to &parm1.
:period.
If &parm1 is not specified,
then the current directory is displayed in the message window.  &parm1
may be specified with drive and path.
.enddescr
.xmplsect begin
.begxmpl cd c:\tmp
Changes to the \tmp directory of the c drive
.endxmpl
.begxmpl cd
Display the current working directory
.endxmpl
.xmplsect end
.endfunc

.begfunc CHANGE
.syntx &lt.line_range&gt. Change
.begdescr
Deletes the line range &parm1, and replaces the range with inputted
source lines.  The input of text is terminated when a line with
nothing on it but a dot ('.') is entered.
.exmode
.enddescr
.alsosee begin
.seethis append
.seethis insert
.alsosee end
.endfunc

.begfunc COMPILE
.syntx * COMPile '"-a"' '"-A"' &lt.script&gt. &lt.result&gt.
.begdescr
Compiles the editor script &parm3
:period.
.np
If
.param -a
is specified,
all local variables are translated at compile time (rather
than at run time) - this is useful for the
.keyword configuration script
:period.
.np
If
.param -A
is specified, all variables (both local and global)
are translated at compile time.
.np
The file will be compiled
into a file with the same name as the script and the extension ._vi.,
unless &parm4 is specified.
.np
For information on editor
.keyword scripts
:cont.,
see the chapter
:HDREF refid='scripts'.
:period.
.enddescr
.xmplsect begin
.begxmpl comp -a ed.cfg
Compiles the default
.keyword configuration script
:fname.ed.cfg:efname.
and expands all local variables to any values assigned to them in
the script.  The compiled script file
:fname.ed._vi:efname.
is generated.
.np
If any errors occur while compiling, a file with the the same name
as the script and the extension .err
is created.
.endxmpl
.begxmpl comp test.vi test.out
Compiles the script
:fname.test.vi:efname.
and generates the compiled script file
:fname.test.out:efname.
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis load
.seethis source
.alsosee end
.endfunc

.begfunc COMPRESS
.syntx * COMpress
.begdescr
Replaces spaces in the current edit buffer with 
tabs.  Single spaces are not replaced with a tab, and spaces inside
a quoted string are not replaced with a tab.
.enddescr
.endfunc

.begfunc COPY
.syntx &lt.line_range&gt. COpy &lt.line_number&gt.
.begdescr
Copies the specified range of lines &range after the line &parm1.
:period.
.enddescr
.xmplsect begin
.begxmpl % copy $
Copies the entire file and places the lines after the last line
in the file.
.endxmpl
.begxmpl 1,. copy .
Copies all lines from the beginning of the file to the current line
and places the lines after the current line.
.endxmpl
.xmplsect end
.endfunc

.begfunc DATE
.syntx * DAte
.begdescr
Display the current time and date in the message window.
.enddescr
.endfunc

.begfunc DELETE
.syntx &lt.line_range&gt. Delete &lt.buffer&gt.
.begdescr
Deletes the specified line range &range.
:period.
.np
If &parm1 is not specified,
the text is deleted into the active &copybuffer
:period.
.np
If &parm1 ('1'-'9', or 'a'-'z') is specified, the text is deleted into
that &copybuffer
:period.
.np
The
.keyref put
command may be used to place contents of a &copybuffer into the file.
.enddescr
.xmplsect begin
.begxmpl % d
Deletes all lines into the active &copybuffer
:period.
.endxmpl
.begxmpl 1,10 d a
Deletes lines 1 to 10 into the named &copybuffer
.param a
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis move
.seethis put
.seethis yank
:cmt. .seethis *key=1 d
:cmt. .seethis *key=1 p
:cmt. .seethis *key=1 P
:cmt. .seethis *key=1 y
.alsosee end
.endfunc

.begfunc ECHO
.syntx * ECHO &lt.line&gt. &lt.msg&gt.
.begdescr
Echos the message &parm2 on line &parm1 of the message window.
.np
If &parm1 is specified as
.param off
:cont.,
then nothing is echoed into the message window from then on.
.np
If &parm1 is specified as
.param on
:cont.,
messages will start to appear in the message window again.
.np
&parm2 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
.enddescr
.xmplsect begin
.begxmpl echo 1 "hello world"
The words
.param hello world
will appear on line 1 of the message window.
.endxmpl
.begxmpl echo off
Disables output to the message window.
.endxmpl
.begxmpl echo on
Enables output to the message window.
.endxmpl
.begxmpl echo 2 /Line 2 message/
The words
.param Line 2 message
will appear on line 2 of the message window.
.endxmpl
.xmplsect end
.endfunc

.begfunc EDIT
.syntx * Edit '"!"' &lt.files&gt.
.begdescr
Edits the specified files &parm2.
:period.
&parm2 may be a single file or a list of files.
.np
Each file name may contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
.np
If &parm1 is specified, then the current file is discarded before editing
the new file(s).
.np
If a file is already being edited,
then control is simply transferred to the window with that file.
If the file is already being edited and you specified a different path,
then a new copy of the file is read from the disk and is edited, unless
.keyref samefilecheck 1
is set.
If
.keyref samefilecheck 1
is set, then control is transferred to the window with the original copy
of the file.
.np
If you wish to have have multiple views on the same file, the
.keyref open
command is available.
.np
If &parm2 is not specified, then a window containing
a list of files in the current directory is opened.
:INCLUDE file='fsdesc'.
.enddescr
.xmplsect begin
.begxmpl edit test.c
Edits the file test.c.
.endxmpl
.begxmpl edit! test2.c
Discards the current file, and edits test2.c.
.endxmpl
.begxmpl edit test.(c|h)
Edits the file test.c if it exists, and the file test.h if it exists.
.endxmpl
.begxmpl edit ([a-c])*.c
Edits all files in the current directory that start with the letters
a, b or c and have the extension .c.
.endxmpl
.begxmpl edit *
Edits all files in the current directory.
.endxmpl
.begxmpl edit
Gives a file selection display:
.figure *depth='2.47' *scale='59' *file='vi016' File Selection Display
.endxmpl
.xmplsect end
.alsosee begin
.seethis open
.seethis read
.seethis view
.seethis visual
.alsosee end
.endfunc

.begfunc EGREP
.syntx * EGRep &lt.regexp&gt. &lt.files&gt.
.begdescr
Searches the file list &parm2 for the regular expression &parm1.
:period.
.np
&parm1 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
.np
&parm2 may be a single file, a directory, or a list of files.  &parm2 may
contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
.np
If &parm2 is not specified, it defaults to the setting of
.keyref grepdefault
:period.
If a directory is specified for &parm2, then the files in that directory
are searched, based on the setting of
.keyref grepdefault
:period.
.np
When all matches are found, a selection window is presented with
all the files that contained &parm1.
:period.
.np
If you are not using regular
expressions, the
.keyref fgrep
command is much faster.
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:period.
.enddescr
.xmplsect begin
.begxmpl egrep ((if)|(while)) *.c
Searches all files in the current directory ending in the extension .c
for the regular expression
.param ((if)|(while))
:period.
.endxmpl
.begxmpl egrep [a-z]a+
Searches all files specified by
.keyref grepdefault
for the regular expression
.param [a-z]a+
:period.
.endxmpl
.begxmpl egrep [a-z]a+ ..\c
Searches all files in the directory ..\c specified by
.keyref grepdefault
for the regular expression
.param [a-z]a+
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis fgrep
.alsosee end
.endfunc

.begfunc EVAL
.syntx * EVAL &lt.expr&gt.
.begdescr
Evalutes a given mathematical expression, and displays the result on
line 1 of the message window.  For a full explanation of the rules
for formulating the expression &parm1, see the chapter
:HDREF refid='scripts'.
:period.
.enddescr
.xmplsect begin
.begxmpl eval 5+7*3
Evaluates
.param 5+7*3
and displays the result (26) in the message window.
.endxmpl
.begxmpl eval (5+7)*3
Evaluates
.param (5+7)*3
and displays the result (36) in the message window.
.endxmpl
.begxmpl eval ((12+3)*100)/50-3
Evaluates
.param ((12+3)*100)/50-3
and displays the result (27) in the message window.
.endxmpl
.xmplsect end
.endfunc

.begfunc EXECUTE
.syntx * EXECUTE &lt.str&gt.
.begdescr
Execute the string &parm1.
:period.
This causes the editor to behave as if
the string &parm1 was typed at the keyboard.
.np
Special keys in the string are indicated as follows:
:INCLUDE file='spmapkey'
:cmt. .np
:cmt. See
:cmt. .keyword keypush
:cmt. for delayed execution of keystrokes.
.enddescr
.xmplsect begin
.begxmpl execute dd
Acts as if
.param dd
were typed, and the current line gets deleted.
.endxmpl
.begxmpl execute :eval 5*3\n
Acts as if
.param :eval 5*3&lt.ENTER&gt.
was typed, and the number 15 gets displayed in the message window.
.endxmpl
.xmplsect end
.alsosee begin
.seethis keyadd
.seethis map
.seethis mapbase
.alsosee end
.endfunc

.begfunc EXITALL
.syntx * EXITALL
.begdescr
Exits all files.  For each file that has been modified, you are prompted
as to whether you want to save the file or not.
.enddescr
.alsosee begin
.seethis quitall
:cmt. .seethis *key=1 CTRL_C
.alsosee end
.endfunc

.begfunc EXPAND
.syntx * EXPAND
.begdescr
Replaces all tabs in the current edit buffer with spaces.
.enddescr
.endfunc

.begfunc FGREP
.syntx * FGRep '"-c"' '"-u"' &lt.string&gt. &lt.files&gt.
.begdescr
Searches the file list &parm4 for the string &parm3.
:period.
.np
The search is by default case insensitive, unless
.param -c
is specified, which forces the search to be case sensitive.
Specifying
.param -u
causes the setting of
.keyref caseignore 1
to determine whether or not to be case sensitive in the search.
.np
&parm3 may be surrounded by double quotes (") or a forward slash (/)
if it contains any spaces.
.np
&parm4 may be a single file or a list of files.  Each file name may
contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
.np
If &parm4 is not specified, it defaults to the setting of
.keyref grepdefault
:period.
.np
While searching for the item, &edvi. displays a window that shows
all files being searched.
When all matches are found, a selection window is presented with
all the files that contained &parm4.
:period.
.enddescr
.xmplsect begin
.begxmpl fgrep window *.c
Searches all files in the current directory ending in the extension .c
for the string
.param window
:period.
This will produce out similar to the following:
.figure *depth='2.47' *scale='59' *file='vi023' Grep Result display
.endxmpl
.xmplsect end
.alsosee begin
.seethis egrep
.alsosee end
.endfunc

.begfunc FILES
.syntx * Files
.begdescr
Opens a window with the a list of all files current being edited.
Files that have been modified will have a '*' beside them.
A file may be selected from this list either with the keyboard or
with the mouse.
.enddescr
.xmplsect begin
.begxmpl files
Displays following screen
.figure *depth='2.47' *scale='59' *file='vi017' Current file list
.endxmpl
.xmplsect end
.endfunc

.begfunc FLOATMENU
.syntx * FLOATMENU &lt.id&gt. &lt.slen&gt. &lt.x1&gt. &lt.y1&gt.
.begdescr
Activates the floating (popup) menu &parm1 (&parm1 may be 0, 1, 2 or 3).
.np
The floating menus are defined using the &cmdline command
.keyref menu
(See the section
:HDREF refid='menucmd'.
).
.np
&parm3, &parm4 specify the coordinates of the upper left hand corner
of the floating menu.  (0,0) specifies the upper left hand corner
of the screen.
.np
&parm2 defines the length of a string that the menu is around; the
floating menu will try to appear around the string (&parm1 may
be 0).  This is useful if you wish to pop up a menu around some
selected characters on a line, for example.
.np
If &parm2 is non-zero, then the coordinates
is assumed to be the lower right-hand side of the string that the menu
is popping up around.
.enddescr
.xmplsect begin
.begxmpl float 0 0 10 10
Bring up floating menu 0 at location (10,10) on the screen.  There is
no string, so the menu position will simply cycle around (10,10)
in an attempt to position itself.
.endxmpl
.begxmpl float 0 6 50 20
Bring up floating menu 0 at location (50,20) on the screen.  This
is assumed to be on the lower right hand side of a string of length 5
(i.e., it is assumed that the string is on line 19 and starts at column
45). If the menu cannot fit by opening at (50,20), it will try to open
at all other corners of the string.
.endxmpl
.xmplsect end
.alsosee begin
.seethis menu
.seethis input
.seethis get
.alsosee end
.endfunc

.begfunc GENCONFIG
.syntx * GENCONFIG &lt.filename&gt.
.begdescr
Writes the current editor configuration out to the file &parm1.
:period.
If &parm1 is not specified, then the file name "ed.cfg" is assumed.
.enddescr
.endfunc

.begfunc GLOBAL
.syntx &lt.line_range&gt. Global '"!"' /&lt.regexp&gt./ &lt.cmd&gt.
.begdescr
For each line in the range &range that matches the regular expression
.param &lt.regexp&gt.
:cont.,
the editor command &parm3 is executed.  &parm3 may contain replacement
expressions, see the chapter
:HDREF refid='rxchap'.
for more information.
.np
If &parm1 is specified, then the command &parm3 is executed on every line
that does NOT match the regular expression
.param &lt.regexp&gt.
:period.
.np
If &range is not specified, then the
.keyword global
command operates on the entire line range of the current edit buffer.
.enddescr
.xmplsect begin
.begxmpl g/printf/d
Deletes all lines in the current edit buffer that have the word
.param printf
:period.
.endxmpl
.begxmpl g!/[a-c]123/ d
Deletes all lines in the current edit buffer that DO not match the
regular expression
.param [a-c]123
:period.
.endxmpl
.begxmpl g/(abc)*/ execute Iabc\e
For every line that matches the regular expression
.param (abc)*
:cont.,
execute the keystrokes
.param Iabc&lt.ESC&gt.
(this will insert the characters abc at the start of the line).
.endxmpl
.xmplsect end
.alsosee begin
.seethis substitute
.alsosee end
.endfunc

.begfunc HELP
.syntx * HELP &lt.topic&gt.
.begdescr
Starts a view-only edit buffer on help for a specified topic.  Possible
topics are:
:DL break.
.*
:DT.COMmandline
:DD.All &cmdline commands.

:DT.KEYS
:DD.What different keystrokes do in &cmdmode
:period.

:DT.REGularexpressions
:DD.How to use regular expressions.

:DT.SETtings
:DD.Everything that can be modified with the
.keyref set
&cmdline command.

:DT.SCRipts
:DD.&edvi.
.keyword script
guide.

:DT.STARTing
:DD.How to start the editor: switches, required files.
.*
:eDL.
.enddescr
.xmplsect begin
.begxmpl help com
Gives help on &cmdline commands.
.endxmpl
.begxmpl help
Gives list of help topics.
.endxmpl
.xmplsect end
.endfunc

.begfunc INSERT
.syntx &lt.line_number&gt. Insert
.begdescr
Inserts text after a the specified line number &range.
:period.
Insert is terminated when a line with nothing but a '.' is entered.
.exmode
.enddescr
.alsosee begin
.seethis append
.seethis change
.alsosee end
.endfunc

.begfunc JOIN
.syntx &lt.line_range&gt. Join
.begdescr
Joins the lines in the specified range &range into a single line
(lines are concatenated one after another).
.enddescr
.xmplsect begin
.begxmpl .,.+2 join
Joins the current line and the next 2 lines into a single line.
.endxmpl
.xmplsect end
:cmt. .alsosee begin
:cmt. .seethis *key=1 J
:cmt. .alsosee end
.endfunc

.begfunc KEYADD
.syntx * KEYAdd &lt.string&gt.
.begdescr
Adds a set of keystrokes &parm1 to the key buffer just as if they
were typed by the user.  The processing of these keystrokes is deferred
until &edvi. finishes its current processing, and is ready to process
keystrokes again.
This is different than the
.keyref execute
command, which processes the keystrokes immediately.
.np
.keyref Keyadd
is useful in a script,
because it allows keystrokes to be executed after the
script is exited.
This prevents re-entrance of a script that is being executed by a
mapped key in input mode, for example.
.np
Keys are processed in FIFO order. Multiple
.keyref keyadd
commands cause more keys to queue up for processing.
.np
If you need to use one or more special keys (e.g. ENTER, F1, etc) in &parm1,
they may specified as follows:
.np
:INCLUDE file='spmapkey'
.enddescr
.xmplsect begin
.begxmpl keyadd dd
Acts as if
.param dd
were typed, and the current line gets deleted.
.endxmpl
.begxmpl keyadd :eval 5*3\n
Acts as if
.param :eval 5*3&lt.ENTER&gt.
was typed, and the number 15 gets displayed in the message window.
.endxmpl
.xmplsect end
.alsosee begin
.seethis execute
.seethis map
.seethis mapbase
.alsosee end
.endfunc

.begfunc LIST
.syntx &lt.line_range&gt. List
.begdescr
Lists lines in the specified line range.
.exmode
.enddescr
.endfunc

.begfunc LOAD
.syntx * LOAD &lt.script&gt.
.begdescr
Loads a script
into memory for the life of the edit session.
This allows for much faster access to the script,
since the data structures for the script do not have to be built
every time the script is invoked.  This is especially important
for a
.keyword hook script
:period.
.np
For information on editor scripts,
see the chapter
:HDREF refid='scripts'.
:period.
.enddescr
.xmplsect begin
.begxmpl load rdme._vi
Loads the script
:fname.rdme._vi:efname.
and makes it resident.
.endxmpl
.xmplsect end
.alsosee begin
.seethis compile
.seethis source
.alsosee end
.endfunc

.begfunc MAP
.syntx * MAP '"!"' &lt.key&gt. &lt.string&gt.
.begdescr
Tells the editor to run the string of keys &parm3 whenver the
key &parm2 is pressed in &cmdmode
:period.
.np
If &parm1 is specified,
then the string of keys &parm3 is executed whenever &parm2 is pressed
in &tinsmode
:period.
.np
When a mapped key is pressed, it acts is if the characters in &parm3
are being typed at the keyboard.
Recursion does not occur; if a key that is mapped is executed after
it has been executed as a mapped key, then the default behaviour for that
key is used, e.g.:
.millust begin
map a 0a
.millust end
will cause the editor to move to column 1, and then start appending
after the character in column 1.
.np
If you need to specify a special key (e.g. ENTER, F1, etc) in &parm2,
you specify a symbolic name for that key.
There are a number of pre-defined keys
symbols that are recognized when specifying which key is being
mapped/unmapped. These are described in the Appendix
:HDREF refid='symkey'.
:period.
.np
If you need to use one or more special keys (e.g. ENTER, F1, etc) in &parm3,
then you may enter:
.np
:INCLUDE file='spmapkey'
.np
To remove a mapping, use the
.keyref unmap
command.
.enddescr
.xmplsect begin
.begxmpl map K \x:next\n
Whenever K is pressed in &cmdmode, the
.keyref next
command is executed. The command window will not be displayed,
because of the
.param \x
:period.
.endxmpl
.begxmpl map CTRL_T \x:\hda\n
Whenever CTRL_T is pressed in &cmdmode,
the current date is displayed. The command window will not be
displayed, because of the
.param \x
:period.
The command will not be added to the command history, because of the
.param \h
:period.
.endxmpl
.begxmpl map CTRL_W \x:fgrep \&lt.CTRL_W&gt.\n
Whenever CTRL_W is pressed in &cmdmode, an fgrep command, searching for
the current word, is executed.  The
.param \x
keeps the command window from opening.
.param \&lt.CTRL_W&gt.
simulates CTRL_W being pressed, so the current word is inserted into
the &cmdline
:period.
.endxmpl
.begxmpl map! CTRL_W \edwi
Whenever CTRL_W is pressed in &tinsmode, &tinsmode
is exited (\e simulates the ESC key being pressed), the current word is
deleted, and &tinsmode is re-entered.  This has the effect of
deleting the current word in &tinsmode
and appearing to remain in &tinsmode
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis execute
.seethis keyadd
.seethis unmap
.alsosee end
.endfunc

.begfunc MAPBASE
.syntx * MAPBASE &lt.key&gt. &lt.string&gt.
.begdescr
Tells the editor to run the string of keys &parm3 whenver the
key &parm2 is pressed in &cmdmode
:period.
.np
This works the same as the
.keyref map
command, only all characters in &parm2 work as their base meaning; that is,
all key mappings are ignored and the keys have their default
behaviour.
.enddescr
.alsosee begin
.seethis execute
.seethis keyadd
.seethis map
.seethis unmap
.alsosee end
.endfunc

.begfunc MARK
.syntx &lt.line_number&gt. MArk &lt.markname&gt.
.begdescr
Sets the
.keyword text mark
&parm1 on the line &range.
:period.
The mark name is a single letter from
.param a
to
.param z
:period.
This mark may then be referred to on the &cmdline or in &cmdmode
by using a front quote (') before the mark name, e.g.:
.millust begin
'a
.millust end
.enddescr
.xmplsect begin
.begxmpl mark a
Sets the mark
.param a
on the current line.  Typing the command
.param 'a
will return you to that mark.
.endxmpl
.begxmpl 100 mark z
Sets the mark
.param z
on line 100.
.endxmpl
.xmplsect end
:cmt. .alsosee begin
:cmt. .seethis *key=1 m
:cmt. .alsosee end
.endfunc

.begfunc MATCH
.syntx * match //&lt.rx1&gt./&lt.rx2&gt.//
.begdescr
Set what is matched by the '%' &cmdmode command.  Defaults are "{","}"
and "(",")". For example, by pressing the percent key ('%') when
the cursor is on the first open bracket ('(') in the line:
.millust begin
    if( ( i=foo( x ) ) ) return;
.millust end
moves the cursor to the last ')' in the line.
.np
This command allows you to extend what is matched to 
general regular expressions.
.param &lt.rx1&gt.
is the regular expression that opens a match,
.param &lt.rx2&gt.
is the regular expression that closes a match.
.np
Note that in the matching regular expressions, 
.keyref magic 1
is set (special characters automatically
have their meaning, and do not need to be escaped).
.np
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:period.
.enddescr
.xmplsect begin
.begxmpl match /# *if/# *endif/
This adds the matching of all
.param #if and
.param #endif
commands (an arbitrary number of spaces is allowed to occur between
the '#' sign and the
.param if
or
.param endif
words.  If '%' is pressed while over a
.param #if
statement, the cursor is moved to the corresponding
.param #endif
statement.
.endxmpl
.xmplsect end
:cmt. .alsosee begin
:cmt. .seethis *key=1 %
:cmt. .alsosee end
.endfunc

.begfunc MAXIMIZE
.syntx * MAXimize
.begdescr
Maximizes the current edit buffer window.
.enddescr
.alsosee begin
.seethis cascade
.seethis minimize
.seethis movewin
.seethis resize
.seethis size
.seethis tile
.alsosee end
.endfunc

.begfunc MINIMIZE
.syntx * MINimize
.begdescr
Minimizes the current edit buffer window.
.enddescr
.alsosee begin
.seethis cascade
.seethis maximize
.seethis movewin
.seethis resize
.seethis size
.seethis tile
.alsosee end
.endfunc

.begfunc MOVE
.syntx &lt.line_range&gt. Move &lt.line_number&gt.
.begdescr
Deletes the specified range of lines &range and places them
after the line &parm1.
:period.
.enddescr
.xmplsect begin
.begxmpl 1,10 move $
Moves the first 10 lines of the file after the last line in the file.
.endxmpl
.begxmpl 1,. move .+1
Deletes all lines from the beginning of the file to the current line
and places the lines one line after the current line.
.endxmpl
.xmplsect end
.alsosee begin
.seethis delete
.alsosee end
.endfunc

.begfunc MOVEWIN
.syntx * MOVEWin
.begdescr
Enter window movement mode.  The cursor keys are then used to move the
current edit buffer window.
.enddescr
.alsosee begin
.seethis cascade
.seethis maximize
.seethis minimize
.seethis resize
.seethis size
.seethis tile
.alsosee end
.endfunc

.begfunc NEXT
.syntx * Next
.begdescr
Moves to the next file in the list of files being edited.
.enddescr
.alsosee begin
:cmt. .seethis *key=1 F1
:cmt. .seethis *key=1 F2
.seethis prev
.alsosee end
.endfunc

.begfunc OPEN
.syntx * Open &lt.file&gt.
.begdescr
Opens a new window on the specified file. 
If no file is specified,
then a new window is opened on the current edit buffer.
These new
windows are different views on the same edit buffer.
.np
Once multiple views
on an edit buffer are opened, the window border contains a number indicating
which view of the edit buffer is associated with that file:
.figure *depth='2.47' *scale='59' *file='vi030' Two views of the same file
.enddescr
.alsosee begin
.seethis edit
.seethis view
.seethis visual
.alsosee end
.endfunc

.begfunc POP
.syntx * POP
.begdescr
Restores the last pushed file position.
.np
The setting
.keyref maxpush
controls the maximum number of push commands that will be remembered.
.enddescr
.alsosee begin
:cmt. .seethis *key=1 F11
:cmt. .seethis *key=1 F12
.seethis push
.seethis tag
.alsosee end
.endfunc

.begfunc PREV
.syntx * Prev
.begdescr
Moves to the previous file in the list of files being edited.
.enddescr
.alsosee begin
:cmt. .seethis *key=1 F1
:cmt. .seethis *key=1 F2
.seethis next
.alsosee end
.endfunc

.begfunc PUSH
.syntx * PUSH
.begdescr
Saves the current file position.  The next
.keyref pop
command will cause a return to this position.
.np
The setting
.keyref maxpush
controls the maximum number of push commands that will be remembered.
.np
The
.keyref tag
command does an implicit push.
.enddescr
.alsosee begin
:cmt. .seethis *key=1 F11
:cmt. .seethis *key=1 F12
.seethis pop
.seethis tag
.alsosee end
.endfunc

.begfunc PUT
.syntx &lt.line_number&gt. PUt '"!"' &lt.buffer&gt.
.begdescr
Puts (pastes) the &copybuffer
&parm2 ('1'-'9', or 'a'-'z') after the
line &range.
:period.
.np
If &parm2 is not specified, the active &copybuffer is assumed.
.np
If &parm1 is specified, then the lines are put before the
line &range.
:period.
.enddescr
.xmplsect begin
.begxmpl put
Pastes the active &copybuffer after the current line.
.endxmpl
.begxmpl 1 put!
Pastes the active  &copybuffer before the first line in the edit buffer.
.endxmpl
.begxmpl $ put a
Pastes named &copybuffer
.param a
after the last line in the file.
.endxmpl
.begxmpl put! 4
Pastes numbered &copybuffer before the current line.
.endxmpl
.xmplsect end
.alsosee begin
.seethis delete
.seethis yank
:cmt. .seethis *key=1 p
:cmt. .seethis *key=1 P
.alsosee end
.endfunc

.begfunc QUIT
.syntx * Quit '"!"'
.begdescr
Quits the current file.
.keyref Quit
will not quit a modified file, unless &parm1 is specified, in which
case all changes are discarded and the file is exited.
.enddescr
.xmplsect begin
.begxmpl q!
Quits the current file, discarding all modifications since the last write.
.endxmpl
.xmplsect end
.alsosee begin
.seethis write
.seethis wq
.seethis xit
:cmt. .seethis *key=1 Z
.alsosee end
.endfunc

.begfunc QUITALL
.syntx * QUITAll
.begdescr
Exits the editor if no files have been modified.  If files have been modified,
a prompt is displayed asking to you verify that you really want to discard
the modified file(s).  
If you do not respond with a 'y', then the command is cancelled.
.enddescr
.xmplsect begin
.begxmpl quitall
If files have been modified, the following prompt is displayed:
.figure *depth='2.47' *scale='59' *file='vi003' Really Exit prompt
.endxmpl
.xmplsect end
.alsosee begin
.seethis exitall
:cmt. .seethis *key=1 CTRL_C
.alsosee end
.endfunc

.begfunc READ
.syntx &lt.line_number&gt. Read &lt.file_name&gt.
.begdescr
Reads the text from file &parm1 into the current edit buffer.
The lines are placed after line specified by &range.
:period.
If &range is not specified, the current line is assumed.
.np
Line 0 may be specified as &range in order to read a file
in before the first line of the current edit buffer.
.np
Each file name &parm1 may contain file regular expressions, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'
for more information.
.np
If &parm1 is not specified, then a window containing
a list of files in the current directory is opened, from
which a file may be selected.
.np
If the first character of &parm1 is a dollar sign ('$'), then this
indicates that a directory is to be read in.  If nothing follows,
then the current directory is read.  Otherwise, all characters that
follow the dollar sign are treated as a file regular expression, which
is used to read in the directory.
.enddescr
.xmplsect begin
.begxmpl 0 read test.c
Reads the file test.c into the current edit buffer and places the text
before the first line of the file.
.endxmpl
.begxmpl r test.c
Reads the file test.c into the current edit buffer and places the text
after the current line.
.endxmpl
.begxmpl r $
Reads the current directory and places the data after the current line.
.endxmpl
.begxmpl r $*.c
Reads the current directory, matching only files with .c extensions,
and places the data after the current line.
.endxmpl
.begxmpl r $..\c\*.*
Reads the ..\c directory, and places the data after the current line.
.endxmpl
.begxmpl read
Gives a file selection display:
.figure *depth='2.47' *scale='59' *file='vi016' File Selection display
.endxmpl
.xmplsect end
.endfunc

.begfunc RESIZE
.syntx * RESize
.begdescr
Allows resizing of the current edit window with the keyboard.  The
cursor keys are used as follows:
:DL break.
.*
:DT.UP
:DD.move top border up

:DT.DOWN
:DD.move top border down

:DT.LEFT
:DD.move right border left

:DT.RIGHT
:DD.move right border right

:DT.SHIFT_UP
:DD.move bottom border up

:DT.SHIFT_DOWN
:DD.move bottom border down

:DT.SHIFT_LEFT
:DD.move left border left

:DT.SHIFT_RIGHT
:DD.move left border right
.*
:eDL.
.enddescr
.alsosee begin
.seethis cascade
.seethis maximize
.seethis minimize
.seethis movewin
.seethis size
.seethis tile
.alsosee end
.endfunc

.begfunc SET
.syntx * SEt &lt.variable&gt. &lt.value&gt.
.begdescr
Certain variables within &edvi. may be changed after &edvi. is
executing.  &parm2 is assigned to &parm1.
:period.
.np
If &parm1 and
&parm2 are not specified, the a window containing a list
of all boolean values is displayed.
.np
If &parm1 is specified
as a
.param 2
:cont.,
then a window containing all other values is displayed.
.np
From the selection window, a variable may be selected (with ENTER or
double clicking the mouse), and a
new value entered. If the variable was boolean, then pressing
ENTER or double clicking toggles the value.
.np
If a variable is a boolean variable, then it is be set via
.millust begin
set var    - set var to TRUE
set novar  - set var to FALSE
.millust end
.np
Variables other than boolean variables are set via
.millust begin
set var = test  - set var to 'test'
set var  test   - set var to 'test'
.millust end
Note that the '=' operator is optional.
.np
For information on all the different settable options,
see the chapter
:HDREF refid='edset'.
:period.
.enddescr
.endfunc

.begfunc SETCOLOR
.syntx * SETCOLOR &lt.c&gt. &lt.r&gt. &lt.g&gt. &lt.b&gt.
.begdescr
Set the color number &parm1 to have the RGB value &parm2, &parm3, &parm4.
:period.
&parm1 may have a value of 0 to 15.
&parm1, &parm2, and &parm3 may have values from 0 to 63.
.np
This command only has an affect under operating systems where it is possible
to remap the colors in some way (DOS).
.enddescr
.xmplsect begin
.begxmpl setcolor 1 63 0 38
This remaps color number 1 to a pink color.
.endxmpl
.begxmpl setcolor 15 25 40 38
This remaps color number 15 to a pale green color.
.endxmpl
.xmplsect end
.endfunc

.begfunc SHELL
.syntx * SHell
.begdescr
Escapes to an operating system shell.
.enddescr
.alsosee begin
.seethis !
:cmt. .seethis *key=1 !
.alsosee end
.endfunc

.begfunc SIZE
.syntx * SIZE &lt.x1&gt. &lt.y1&gt. &lt.x2&gt. &lt.y2&gt.
.begdescr
Resizes the current edit buffer window to have upper left-hand corner at
(&parm1,&parm2) and lower right-hand corner at (&parm3,&parm4).
.enddescr
.xmplsect begin
.begxmpl SIZE 0 0 10 10
Changes the size of the current edit buffer window to have
upper left-hand corner at (0,0) and lower right-hand corner at (10,10).
.endxmpl
.xmplsect end
.alsosee begin
.seethis cascade
.seethis maximize
.seethis minimize
.seethis movewin
.seethis resize
.seethis tile
.alsosee end
.endfunc

.begfunc SOURCE
.syntx * SOurce &lt.script&gt. &lt.p1&gt. &lt.p2&gt. ... &lt.pn&gt.
.begdescr
Execute &edvi. source script file &parm1.
:period.
Optional parameters &parm2 to
&parm5 may be specified, these are passed to the specified script.
.np
If "." is specified as the script name, the current
file being edited is run as a script.
.np
For information on editor scripts,
see the chapter
:HDREF refid='scripts'.
:period.
.enddescr
.xmplsect begin
.begxmpl source foo.vi abc
Executes the script
.param foo.vi
:cont.,
passing it the parm
.param abc
:period.
.endxmpl
.begxmpl source .
Executes the current edit buffer as a script
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis compile
.seethis load
.alsosee end
.endfunc

.begfunc SUBSTITUTE
.syntx &lt.line_range&gt. Substitute /&lt.regexp&gt./&lt.replexp&gt./&lt.g&gt.&lt.i&gt.
.begdescr
Over the line range &range,
replace each occurrence of regular expression
.param &lt.regexp&gt.
with the replacement expression
.param &lt.replexp&gt.
:period.
.np
Only the first occurrence on each line is replaced, unless
.param &lt.g&gt.
is specified, in which case all occurrences on a line are replaced.
.np
If
.param &lt.i&gt.
is specified, each replacement is verified before it is performed.
.np
See the chapter
:HDREF refid='rxchap'.
for more information on regular expression matching and substitution.
.enddescr
.xmplsect begin
.begxmpl %s/foo/bar/
Changes the first occurrence of
.param foo
to
.param bar
on each line of the entire file.
.endxmpl
.begxmpl 1,.s/(([a-z]bc)*)/Abc\2/g
Changes all occurrences of the regular expression
.param (([a-z]bc)*)
to the substitution expression
.param Abc\2
:period.
The changes are only applied on lines 1 to the current line of the
current edit buffer.
.endxmpl
.begxmpl 'a,'b/^abc//i
Any line that starts with
.param abc
has the
.param abc
changed to the null string.
The user is prompted before each change.  The changes are only applied
from the line containing mark
.param a
to the line containing mark
.param b
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis global
.alsosee end
.endfunc

.begfunc TAG
.syntx * TAG &lt.tagname&gt.
.begdescr
Searches for the tag &parm1.
:period.
Tags are kept in a special file,
which must be located somewhere in your path.
This file is controlled with the
.keyref tagfilename
setting; the default for this setting is
.param tags
:period.
The tags file contains a collection of procedure names and typedefs,
along with the file in which they are located and a search command/line
number with which to exactly locate the tag.
.np
See the appendix
:HDREF refid='ctags'
for more information.
.enddescr
.xmplsect begin
.begxmpl tag MyFunc
Locates the tag
.param MyFunc
in the tags file, edits the source file that contains the function
.param MyFunc
:cont.,
and moves the cursor to the definition of the function in the source file.
.endxmpl
.xmplsect end
:cmt. .alsosee begin
:cmt. .seethis *key=1 ^]
:cmt. .alsosee end
.endfunc

.begfunc TILE
.syntx * TILE '"h"' '|' '"v"' '|' &lt.x&gt. &lt.y&gt.
.begdescr
Tile all current file windows.  The tiling layout is specified
as an &parm5 by &parm6  grid.  The or bars ('|') in the command
syntax indicate that only one of the options may be used.
.np
If no parameters are specified,
.keyref maxwindowtilex
and
.keyref maxwindowtiley
are used (this is the default tile grid).
.np
If &parm1 is specified,
then files are tiled horizontally (as many as will fit).
.np
If &parm3 is specified, then files are tiled vertically (as many as will fit).  
.np
Specifying &parm5 &parm6 overrides the default tile grid. As a special
case, specifying &parm5 &parm6 as 1 1 causes all windows to be restored.
.enddescr
.xmplsect begin
.begxmpl tile 3 5
Tile windows 3 across and 5 high.
.endxmpl
.begxmpl tile 1 1
Untile windows.
.endxmpl
.begxmpl tile h
Tile windows horizontally.
.endxmpl
.begxmpl tile
Tile windows according to default tile grid.
.endxmpl
.xmplsect end
.alsosee begin
.seethis cascade
.seethis maximize
.seethis minimize
.seethis resize
.seethis size
.seethis movewin
.alsosee end
.endfunc

.begfunc UNABBREV
.syntx * UNABbrev &lt.abbrev&gt.
.begdescr
Removes the abbreviation &parm1.
:period.
See the
.keyref abbrev
command for how to set an abbreviation.
.enddescr
.xmplsect begin
.begxmpl unabbrev wh
Remove the abbreviation
.param wh
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis abbrev
.alsosee end
.endfunc

.begfunc UNALIAS
.syntx * UNALias &lt.alias&gt.
.begdescr
Removes the &cmdline alias &parm1.
:period.
See the
.keyref alias
command for how to set a &cmdline alias.
.enddescr
.xmplsect begin
.begxmpl unalias ai
Remove the &cmdline alias
.param ai
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis alias
.alsosee end
.endfunc

.begfunc 'UNDO (command)'
:cmt. .ix 'undo command'
.syntx * Undo '"!"'
.begdescr
Undo the last change.  There is no limit on the number of undo's
that can be saved, except for memory.  Continuing to issue undo commands
walks you backwards through your edit history.
.np
Specifying &parm1 undoes the last undo (redo).  Again, there are no
restrictions on this.  However, once you modify the file, you can longer undo the
last undo.
.enddescr
:cmt. .alsosee begin
:cmt. .seethis *key=1 u
:cmt. .seethis *key=1 U
:cmt. .alsosee end
.endfunc

.begfunc UNMAP
.syntx * UNMAP '"!"' &lt.key&gt.
.begdescr
Removes the mapping of the key &parm2 for &cmdmode
:period.
If &parm1 is specified, then the key mapping is removed for &tinsmode
:period.
See the
.keyref map
command for details on mapping keys.
.np
There are a number of pre-defined
symbols that are recognized for &parm1.
:period.
These are described in the Appendix
:HDREF refid='symkey'.
:period.
.enddescr
.xmplsect begin
.begxmpl unmap CTRL_W
Removes the mapping of CTRL_W for &cmdmode
:period.
Pressing CTRL_W in command
mode will now do the default action.
.endxmpl
.begxmpl unmap! F1
Removes the mapping of F1 for &tinsmode
:period.
Typing F1 in &tinsmode will now do the default action.
.endxmpl
.xmplsect end
.alsosee begin
.seethis map
.seethis mapbase
.alsosee end
.endfunc

.begfunc VERSION
.syntx * VERsion
.begdescr
Displays the current version of &edvi. in the message window.
.enddescr
:cmt. .alsosee begin
:cmt. .seethis *key=1 ^V
:cmt. .alsosee end
.endfunc

.begfunc VIEW
.syntx * VIEw '"!"' &lt.file_name&gt.
.begdescr
Functions the same as the
.keyref edit
command, except that it causes the file edited to be a "view only"
file (no modification commands work).
.enddescr
.xmplsect begin
.begxmpl view test.c
Edits the file
.param test.c
in view-only mode.
.endxmpl
.xmplsect end
.alsosee begin
.seethis edit
.seethis open
.seethis visual
.alsosee end
.endfunc

.begfunc VISUAL
.syntx * VIsual &lt.file_name&gt.
.begdescr
Causes &edvi. to re-enter visual mode (full screen editing mode)
if &edvi. is in
.keyword EX mode
:period.
If the filename &parm1 is specified,
this functions just like the
.keyref edit
command.
.enddescr
.xmplsect begin
.begxmpl vi test.c
Return to full screen editing mode (if in
.keyword EX mode
:cont.)
and edit the file
test.c
.endxmpl
.begxmpl vi
Return to full screen editing mode (if in
.keyword EX mode
:cont.)
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis edit
.seethis open
.seethis view
.alsosee end
.endfunc

.begfunc WRITE
.syntx &lt.line_range&gt. Write '"!"' &lt.file_name&gt.
.begdescr
Writes the specified range of lines &range to the file &parm2.
:period.
.np
If no line range is specified, then all lines are written.
.np
If &parm2 is not specified, then the current file is written.
If &parm2 exists, and is not the name of the file that you are writing,
then the write will fail.
.np
Specifying &parm1 forces an overwrite of an existing file.
.enddescr
.xmplsect begin
.begxmpl 1,10 w! test.c
Write the first 10 lines of the current file to the file
.param test.c
:cont.,
and overwrite
.param test.c
if it already exists.
.endxmpl
.begxmpl w
Write out the current file.
.endxmpl
.xmplsect end
.alsosee begin
.seethis quit
.seethis wq
.seethis xit
:cmt. .seethis *key=1 Z
.alsosee end
.endfunc

.begfunc WQ
.syntx  * WQ
.begdescr
Writes current file, and exits.
.enddescr
.alsosee begin
.seethis quit
.seethis write
.seethis xit
:cmt. .seethis *key=1 Z
.alsosee end
.endfunc

.begfunc YANK
.syntx &lt.line_range&gt. Yank &lt.buffer&gt.
.begdescr
Yank (make a copy of) the specified line range &range.
:period.
.np
If &parm1 is not specified, the text is yanked (copied) into the active
&copybuffer
:period.
.np
If &parm1 ('1'-'9', or 'a'-'z') is specified, the text is yanked into
that &copybuffer
:period.
.np
The
.keyref put
command may be used to place the contents of a &copybuffer into the file.
.enddescr
.xmplsect begin
.begxmpl % y
Yanks (copies) all lines into the active &copybuffer
:period.
.endxmpl
.begxmpl .,$ y z
Yanks the lines from the current line to the last line in the file
into the &copybuffer
.param z
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis delete
.seethis put
:cmt. .seethis *key=1 d
:cmt. .seethis *key=1 y
:cmt. .seethis *key=1 p
:cmt. .seethis *key=1 P
.alsosee end
.endfunc

.begfunc XIT
.syntx Xit
.begdescr
Exits the current file, saving it if it has been modified.
.enddescr
.alsosee begin
.seethis quit
.seethis write
.seethis wq
:cmt. .seethis *key=1 Z
.alsosee end
.endfunc
.*
.fnlist end
