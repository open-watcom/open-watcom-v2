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
.* 30-jul-92    Craig Eisler    initial draft
.* 02-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
:set symbol="guionly" value="This option is only valid with the GUI versions of the editor.".
:set symbol="charonly" value="This option is only valid with the character mode versions of the editor.".

.chap *refid=edset 'Editor Settings'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
This chapter describes the various options that may be controlled using
the &edname.'s 
.keyref set
command.  Options are typically set in the
.keyword configuration script
:cont.;
however, options are settable at execution time as well.
.np
If you know the option you wish to set, you may just issue the
.keyref set
command directly at the command prompt, e.g.:
.millust begin
set nocaseignore
set autosaveinterval=10
.millust end
.np
A boolean option is set in the following way:
.millust begin
set autoindent   - turns on autoindent
set noautoindent - turns off autoindent
.millust end
Short forms may also be used; for example:
.millust begin
set ai   - turns on autoindent
set noai - turns off autoindent
.millust end
.np
A non-boolean option may be set in the following way:
.millust begin
set filename=test.c  - sets current filename to 'test.c'
set filename test2.c - sets current filename to 'test2.c'
.millust end
Note that the assignment operator '=' is optional.
.np
If you do not know the boolean option you wish to set, you may issue the
set command with no option at the command prompt, e.g.:
.millust begin
set
.millust end
This will cause a menu of all possible settings to appear.  These
values may be changed by either cursoring to the desired one and pressing
enter, or by double clicking on the desired one with the mouse.  Boolean
settings will toggle between TRUE and FALSE.  Selecting any other setting
will cause a window to pop up, displaying the old
value and prompting you for the new value.  This window may be cancelled
by pressing the ESC key.
.figure *depth='2.47' *scale='59' *file='vi031' '&edvi Settings Selection list'
.np
When you are finished with the settings menus, you may close the window
by pressing the
.keyword ESC
key.
.* ******************************************************************
.section *refid=boolset 'Boolean Settings'
.* ******************************************************************
.np
The section contains descriptions of the boolean settings.
.*
.fnlist begin 'Boolean Settings'
.*
.setcmd *short=ai autoindent
.setsyntx
.begdescr
In text insertion mode, &cmd_long causes the cursor to move to
start of previous line when a new line is started.
In &cmdmode, &cmd_long causes the cursor to go to the first non
white-space when ENTER is pressed.
.enddescr

.setcmd *short=ac automessageclear
.setsyntx
.begdescr
Automatically erases the message window when a key is typed in &cmdmode.
:period.
.enddescr

.setcmd *short=bf beepflag
.setsyntx
.begdescr
&edvi normally beeps when an error is encountered.  Setting &nocmd_long
disables the beeping.
.enddescr

.setcmd *short=ci caseignore
.setsyntx
.begdescr
Causes all searches to be case insensitive if set.
.enddescr

.setcmd *short=cv changelikevi
.setsyntx
.begdescr
If set, then the change command behaves like UNIX &edvi., i.e.
if ESC is pressed when no change has been made, the text is deleted.
Normally, pressing ESC cancels the change without deleting the text.
.enddescr

.setcmd *short=cm cmode
.setsyntx
.begdescr
When cmode is set, certain things will happen when you are entering text:
:UL.
:LI.After entering a line ending in '{', the next line will be indented a
.keyref shiftwidth
further than the current one.
:LI.After entering a line ending in '}', the current line is shifted to match the
indentation of the line with the matching '{'.  The cursor will flash for
a brief instant on the matching '{' if
.keyref showmatch 1
is set.
:LI.All lines entered will have trailing white space trimmed off.
:LI."case" and "default" statements are shifted to be aligned with switch
statements.
:eUL.
.np
Each file has its own &cmd_long. setting; so setting &cmd_long in one file
and not in another (during the same editing session) will work.
.np
One thing that is useful is to add the following lines to your read
.keyword hook script
:cont.:
.millust begin
    if %E == .c
        set cmode
    else
        set nocmode
    endif
.millust end
This will cause cmode to be set if the file has a .c extension, and not
to be set for any other type of file.
.enddescr

.setcmd *short=cs columninfilestatus
.setsyntx
.begdescr
Causes the current column to be added to file status display
(obtained when typing CTRL_G).
.enddescr

.setcmd *short=ct currentstatus
.setsyntx
.begdescr
Enables the display of the current status on the menu bar. The position
on the menu bar is controlled with
.keyref currentstatuscolumn
:period.
.enddescr

.setcmd *short=dt drawtildes
.setsyntx
.begdescr
If &cmd_long is true, then the all lines displayed that do not have any
data associated with them will have a tilde ('&tilde.') displayed on the line.
If &cmd_long is false, then no tidles will be displayed and the string
.keyref  fileendstring
will be displayed after the last line with data.
.enddescr

.setcmd *short=eb eightbits
.setsyntx
.begdescr
If &cmd_long is set, then all characters are displayed as normal.  If
&nocmd_long is set then non-printable ASCII will be displayed as control
characters.
.enddescr

.setcmd *short=em escapemessage
.setsyntx
.begdescr
Display the current file status whenever the
.keyword ESC
key is pressed in &cmdmode.
:period.
.enddescr

.setcmd *short=xm extendedmemory
.setsyntx
.begdescr
If &cmd_long is set, extended memory is used if it is present
(standard extended, EMS, XMS). This option applies to the real-mode
DOS version of &edvi only.
.enddescr

.setcmd *short=iz ignorectrlz
.setsyntx
.begdescr
Normally, a CTRL_Z in a file acts as an end-of-file indicator.  Setting
&cmd_long causes &edvi to treat CTRL_Z as just another character in
the file.  This option may also be selected using the '-z' option
when invoking &edvi.
:period.
.enddescr

.setcmd *short=it ignoretagcase
.setsyntx
.begdescr
When using the "-t" command line option of &edvi, the tag that is matched
is normally case sensitive.  Setting &cmd_long causes the tag matching
to be case insensitive.
.enddescr

.setcmd *short=ma magic
.setsyntx
.begdescr
If &cmd_long is set, then all special characters in a regular expression
are treated as "magic", and must be escaped to be used in an ordinary
fashion.  If &nocmd_long is set, then any special characters in
.keyref  magicstring
are NOT treated as magic characters by the regular expression
handler, and must be escaped to have special meaning.
.np
Magic characters are:
.mono ^$.[()|?+*\~@
.enddescr

.setcmd *short=ps pauseonspawnerr
.setsyntx
.begdescr
This option, if set, causes &edvi to pause after spawning (running
a system command)
if there was an error, even if the system command was spawned from a script.
Normally, a command spawned from a script
does not pause when control returns to the editor.
.enddescr

.setcmd *short=qu quiet
.setsyntx
.begdescr
When running in quiet mode, &edvi does not update the screen.  This
is useful when running a complex script,
so that the activity of the
editor is hidden.  This option may be selected when invoking
&edvi by using the '-q' switch, causing &edvi to run in a 'batch mode'.
.enddescr

.setcmd *short=qf quitmovesforward
.setsyntx
.begdescr
If this option is set, then when a file is quit, the next file in the list
of files is moved to.  Otherwise, the previous file in the list of files
is moved to.
.enddescr

.setcmd *short=rf readentirefile
.setsyntx
.begdescr
If &cmd_long is set, then the entire file is read into memory when
it is edited.  This is the default setting.  However, if &nocmd_long
is set, then the file is only read into memory as it is needed.  This
option is useful when you only want to look at the first few pages of
a large file.  This option may be selected when invoking &edvi by
using the '-n' switch.
.enddescr

.setcmd *short=rc readonlycheck
.setsyntx
.begdescr
This option causes &edvi to complain about modifications to
read-only files every time the file is modified.
.enddescr

.setcmd *short=rt realtabs
.setsyntx
.begdescr
If &nocmd_long is set, then tabs are expanded to spaces when the
file is read.
.enddescr

.setcmd *short=rm regsubmagic
.setsyntx
.begdescr
If &nocmd_long is set, then escaped characters have no meaning
in regular expression substitution expressions.
.enddescr

.setcmd *short=sc samefilecheck
.setsyntx
.begdescr
Normally, &edvi just warns you if you edit a file twice
(with a different path). If &cmd_long is set, then if you
edit a file that is the same as a file already being edited (only you
specified a different path), then that file will be brought up, rather
than a new copy being read in.
.enddescr

.setcmd *short='sn' saveconfig
.setsyntx
.begdescr
If this option is set, then the editor's configuration (fonts, colors,
settings, etc) is saved when the editor is exited.
.np
&guionly
.enddescr

.setcmd *short='so' saveposition
.setsyntx
.begdescr
If this option is set, then the editor's position and size is saved
when the editor is exited, and restored the next time the editor
is started.
.np
&guionly
.enddescr

.setcmd *short=sw searchwrap
.setsyntx
.begdescr
When searching a file, &edvi normally wraps around the top or bottom
of the file.   If &nocmd_long is set, then &edvi terminates its searches
once it reaches the top or bottom of the file.
.enddescr

.setcmd *short=sm showmatch
.setsyntx
.begdescr
If &cmd_long is set, the &edvi briefly moves the cursor to a matching '('
whenever a ')' is typed while entering text.  Also, the matching '{' is
shown if a '}' is typed if
.keyref cmode 1
is set.
.enddescr

.setcmd *short=tp tagprompt
.setsyntx
.begdescr
If a more than one instance of a tag is found in the tags file, a
list of choices is displayed if &cmd_long is set.
.enddescr

.setcmd *short=un undo
.ix 'undo setting'
.setsyntx
.begdescr
Setting &nocmd_long disables &edvi.'s undo ability.
.enddescr

.setcmd *short=ve verbose
.setsyntx
.begdescr
If enabled, this option causes &edvi to display extra messages
while doing involved processing.
.enddescr

.setcmd *short=ww wordwrap
.setsyntx
.begdescr
If enabled, word movement commands ('w','W','b','B') will wrap
to the next or previous line.
.enddescr

.setcmd *short=ws wrapbackspace
.setsyntx
.begdescr
If this option is set, pressing backspace while on column one of
a line will cause &edvi to wrap to the end of the previous line
(while in
.keyword text insertion mode
:cont.).
.enddescr

.setcmd *short=wl writecrlf
.setsyntx
.begdescr
Normally, lines are written with carriage return and line feeds at the
end of each line.  If &nocmd_long is set, the lines are written
with only a line feed at the end.
.enddescr
.*
.fnlist end
.*
.beglevel
.* ******************************************************************
.section 'Mouse Control Booleans'
.* ******************************************************************
.np
The section contains descriptions of the boolean settings affecting the mouse.
.*
.fnlist begin 'Boolean Mouse Settings'
.*
.setcmd *short=lm lefthandmouse
.setsyntx
.begdescr
When &cmd_long is set, the right and left mouse buttons are inverted.
.enddescr

.setcmd *short=um usemouse
.setsyntx
.begdescr
This option enables/disables the use of the mouse in &edvi.
:period.
.enddescr
.*
.fnlist end
.* ******************************************************************
.section 'Window Control Booleans'
.* ******************************************************************
.np
The section contains descriptions of the boolean settings affecting windows.
.*
.fnlist begin 'Boolean Window Settings'
.*
.setcmd *short=cl clock
.setsyntx
.begdescr
This enables/disables the clock display.  The position of the clock
is controlled by the
.keyref clockx
and
.keyref clocky
set commands.
.enddescr

.setcmd *short=ln linenumbers
.setsyntx
.begdescr
This option turns on the line number window.  This window is
displayed on the left-hand side of the edit window by default, unless
.keyref linenumsonright
is set.
.enddescr

.setcmd *short=lr linenumsonright
.setsyntx
.begdescr
Setting &cmd_long causes the line number window to appear on the
right-hand side of the edit window.
.enddescr

.setcmd *short=ml marklonglines
.setsyntx
.begdescr
If this option is set, then any line that exceeds the with of the screen
has the last character highlighted.  If
.keyref endoflinechar
is a non-zero ASCII value, then the last character is displayed as that
ASCII value.
.enddescr

.setcmd *short=me menus
.setsyntx
.begdescr
This option enables/disables the menu bar.
.enddescr

.setcmd *short=ri repeatinfo
.setsyntx
.begdescr
Normally, &edvi echos the repeat count in the
.keyref countwindow
as it is typed.  Setting &nocmd_long disables this feature.
.enddescr

.setcmd *short=sp spinning
.setsyntx
.begdescr
If set, this option enables the busy-spinner.  Whenever the editor is busy,
a spinner will appear.  The position of the spinner is controlled using
the
.keyref spinx
and the
.keyref spiny
set commands.
.enddescr

.setcmd *short=si statusinfo
.setsyntx
.begdescr
If set, this option enables the status info window.  This window
contains the current line and column, and is controlled using the
.keyref statuswindow
window command.
.enddescr

.setcmd *short=tb toolbar
.setsyntx
.begdescr
This option enables/disables the toolbar.
.np
&guionly
.enddescr

.setcmd *short=wg windowgadgets
.setsyntx
.begdescr
This option enables/disables gadgets on edit session windows.
.enddescr
.*
.fnlist end
.*
.endlevel
.* ******************************************************************
.section *refid=nonbset 'Non-Boolean Settings'
.* ******************************************************************
.np
The section contains descriptions of the non-boolean settings.
.*
.fnlist begin 'Non-Boolean Settings'
.*
.setcmd *parm=seconds autosaveinterval
.setsyntx
.begdescr
Sets the number of seconds between autosaves of the current file to the
backup directory. Autosave is disabled if &cmd_parm is 0.  The backup
&cmd_short
directory is defined using the
.keyref tmpdir
parameter.
.enddescr
    
.setcmd *parm=size commandcursortype
.setsyntx
.begdescr
Sets the size of the cursor when in &cmdmode.
:period.
Values for &cmd_parm are 0 to 100 (0=full size, 100=thin).
.enddescr

.setcmd *parm=ascii_val endoflinechar
.setsyntx
.begdescr
If
.keyref marklonglines
is set, and &cmd_parm is non-zero, then the character in the last column
of a line wider than the screen is displayed as the ASCII value &cmd_parm.
:period.
.enddescr

.setcmd *parm=attr exitattr
.setsyntx
.begdescr
Defines the attribute to be assigned to the screen when &edvi is exited.
The attribute is composed of a foreground and a background color,
(16 * background + foreground gives &cmd_parm). The default
is 7 (white text, black background).
.np
&charonly.
.enddescr

.setcmd *parm=string fileendstring
.setsyntx
.begdescr
If
.keyref drawtildes 1
is false, then the character string &cmd_parm
will be displayed after the last line with data.
.enddescr

.setcmd *parm=rexexp grepdefault
.setsyntx
.begdescr
Default files to search when using the
.keyref fgrep
or
.keyref egrep
commands.
&cmd_parm is a file matching regular expression, the default is *.(c|h)
For more information, see the section
:HDREF refid='fmrx'.
:period.
.enddescr

.setcmd *parm=dist hardtab
.setsyntx
.begdescr
This controls the distance between tabs when a file is displayed.  The
default is 8 (4 on QNX).
.enddescr

.setcmd *parm=fname historyfile
.setsyntx
.begdescr
If the history file is defined, your command and search history is
saved across editing sessions in the file &cmd_parm.
:period.
.enddescr

.setcmd *parm=size insertcursortype
.setsyntx
.begdescr
Sets the size of the cursor when inserting text in &tinsmode
:period.
Values for &cmd_parm
are 0 to 100 (0=full size, 100=thin).
.enddescr

.setcmd *parm=str magicstring
.setsyntx
.begdescr
If
.keyref magic 1
is not set, then the characters specified in &cmd_parm are NOT treated
as magic characters by the regular expression handler, and must be
escaped to have special meaning.
.np
Magic characters are:
.mono ^$.[()|?+*\~@
.enddescr

.setcmd *parm=numcmds maxclhistory
.setsyntx
.begdescr
&edvi keeps a history of commands entered at the &cmdline.
:period.
&cmd_parm sets the number of commands kept in the history.
.enddescr

.setcmd *parm=kbytes maxemsk
.setsyntx
.begdescr
Sets the maximum number of kilobytes of EMS memory to be used
by &edvi (DOS real-mode version only).
.np
This option can only be set during editor initialization.
.enddescr

.setcmd *parm=numfiltercmds maxfilterhistory
.setsyntx
.begdescr
&edvi keeps a history of the filter commands entered.
&cmd_parm sets the number of filter commands kept in the history.
.enddescr

.setcmd *parm=numsearchcmds maxfindhistory
.setsyntx
.begdescr
&edvi keeps a history of search commands entered.  &cmd_parm sets the
number of search commands kept in the history.
.enddescr

.setcmd *parm=maxlne maxlinelen
.setsyntx
.begdescr
This parameter controls the maximum line length allowed by &edvi.
:period.
The default value is 512 bytes.  Any lines longer than &cmd_parm
are broken up into multiple lines.
.enddescr

.setcmd *parm=num maxpush
.setsyntx
.begdescr
Controls the number of pushed file positions that will be remembered.
Once more than &cmd_parm
.keyref push
or
.keyref tag
commands have been issued, the first pushed positions are lost.
.enddescr

.setcmd *parm=n maxswapk
.setsyntx
.begdescr
Sets the maximum number of kilobytes of disk space to be used for
temporary storage by &edvi.
:period.
.np
This option can only be set during editor initialization.
.enddescr

.setcmd *parm=kbytes maxxmsk
.setsyntx
.begdescr
Sets the maximum number of kilobytes of XMS memory to be used
by &edvi (DOS real-mode version only).
.np
This option can only be set during editor initialization.
.enddescr

.setcmd *parm=lines pagelinesexposed
.setsyntx
.begdescr
Sets the number of lines of context left exposed when a page up/down
is done.  For example, if &cmd_parm is set to 1, then when a page down
key is pressed, the bottom line of the file will be visible at the
top of the new page.
.enddescr

.setcmd *parm=size overstrikecursortype
.setsyntx
.begdescr
Sets the size of the cursor when in overstriking text in
.keyword text insertion mode
:period.
Values for &cmd_parm are 0 to 100 (0=full size, 100=thin).
.enddescr

.setcmd *parm=rdx radix
.setsyntx
.begdescr
Sets the radix (base) of the results of using the
&cmdline command
.keyref eval
:period.
The default is base 10.
.enddescr

.setcmd *parm=nsp shiftwidth
.setsyntx
.begdescr
Sets the number of spaces inserted/deleted by the shift operators ('&gt.'
and '&lt.'), CTRL_D and CTRL_T in text insertion mode, and by
.keyref autoindent 1
and
.keyref cmode 1
when they are indenting.
.enddescr

.setcmd *parm=kbytes stackk
.setsyntx
.begdescr
Sets the size (in kilobytes) of the stack used by &edvi.
:period.
The minimum is 10.  This can be set higher if you plan on using
nested scripts that go deeper than 4 or 5 levels.
.np
This option can only be set during editor initialization.
.enddescr

.setcmd *parm=sects statussections
.setsyntx
.begdescr
The controls the appearance of the bars in the status window.  A list
of distances (in pixels) is given.  This distance is measured from the
start of the status window.  Each section may have something put in it
via the
.keyref statusstring
.np
&guionly.
.enddescr

.setcmd *parm=str statusstring
.setsyntx
.begdescr
This controls what is displayed in the status window.  Any characters
may be in this string.  Additionally, the dollar sign ('$') is a special
character.  It is used in combination with other characters to represent
special values:
:UL.
:LI.$&lt.n&gt.C
current column number.  If &lt.n&gt. is specified (a number), then the column
number will be padded with spaces so that the it occupies &lt.n&gt. characters.
:LI.$D
current date
:LI.$H
current hint text from menus or toolbar (GUI editors only)
:LI.$&lt.n&gt.L
current line number.  If &lt.n&gt. is specified (a number), then the line
number will be padded with spaces so that the it occupies &lt.n&gt. characters.
:LI.$M
current mode the editor is in
:LI.$T
current time
:LI.$n
skip to next line in status window (character mode editors only)
:LI.$$
replaced with a '$'
:LI.$c
replaced with a comma ','
:LI.$[
skip to next block in the status window (GUI editors only)
:LI.$|
text will be centered (within current block for GUI editors)
:LI.$&gt.
text will be right-justified (within current block for GUI editors)
:LI.$&lt.
text will be left-justified (within current block for GUI editors)
:eUL.
A number may precede the L or the C, to indicate the amount of
space that the string should occupy; for example,
$6L  will cause the line number to always occupy at least 6 spaces.
.np
The string may be surrounded by quotes if spaces are being used.
The default status string setting for character mode editors is:
.millust begin
set statusstring="L:$6L$nC:$6C"
.millust end
For GUI editors, the default status string setting is:
.millust begin
set statusstring = "Ln:$5L$[Col:$3C$[Mode: $M$[$|$T$[$H"
.millust end
.enddescr

.setcmd *parm=string shellprompt
.setsyntx
.begdescr
This setting controls what the prompt at the command line will be
after using the
.keyref shell
command.
.enddescr

.setcmd *parm=nsp tabamount
.setsyntx
.begdescr
Sets the number of spaces inserted when the tab key is pressed in
a text insertion mode.  If
.keyref realtabs 1
is set, these spaces will be changed into tabs, based on the setting of
.keyref hardtab
:period.
.enddescr

.setcmd *parm=fname tagfilename
.setsyntx
.begdescr
This defines the file name that &edvi is to use to locate tags
in.  The default is the name
.us tags
:period.
.enddescr

.setcmd *parm=dir tmpdir
.setsyntx
.begdescr
This is used to specify the directory where all temporary editor files
are to be created.
.np
This option can only be set during editor initialization.
.enddescr

.setcmd *parm=str word
.setsyntx
.begdescr
This defines the word used by &edvi.
:period.
&cmd_parm is a group of character pairs.
Each pair defines a range; e.g. 09az defines the characters 0 through 9
and a thorough z.
Any character in
the ranges defined by
&cmd_parm is considered part of a word.
.np
The default for &cmd_long is "__09AZaz".
.np
The word will be delimited by
white space (spaces or tabs) and all characters not in the ranges
defined by &cmd_parm.
:period.
.enddescr

.setcmd *parm=margin wrapmargin
.setsyntx
.begdescr
If &cmd_long is set to a non-zero value, then word wrapping is enabled.
As text is entered, the position of the cursor is monitored.  Once the
cursor gets within &cmd_parm characters of the right margin, the current
word is moved to a new line.
.enddescr
.*
.fnlist end
.*
.beglevel
.* ******************************************************************
.section 'Mouse Control Values'
.* ******************************************************************
.np
The section contains descriptions of the non-boolean settings affecting the mouse.
.*
.fnlist begin 'Non-Boolean Mouse Settings'
.*
.setcmd *parm=ticks mousedclickspeed
.setsyntx
.begdescr
Sets the number of ticks between the first mouse button depress and
the second mouse button depress for the action to count as a double-click.
A tick is approximately 1/18 of a second.
.np
&charonly.
.enddescr

.setcmd *parm=ticks mouserepeatdelay
.setsyntx
.begdescr
Sets the number of ticks between when a mouse button is depressed and
when the button starts to "repeat".
A tick is approximately 1/18 of a second.
.np
&charonly.
.enddescr

.setcmd *parm=speed mousespeed
.setsyntx
.begdescr
Sets the speed of the mouse. &cmd_parm may be in the range 0-31
(0 is the fastest, 31 is the slowest).
.np
&charonly.
.enddescr

.setcmd *parm=str wordalt
.setsyntx
.begdescr
This defines the alternate word used when double clicking the mouse.
&cmd_parm is defined in the same fashion as for the
.keyref word
setting.
.np
The default for &cmd_long is "::..\\__09AZaz".
.enddescr
.*
.fnlist end
.* ******************************************************************
.section 'Window Control Values'
.* ******************************************************************
.np
The section contains descriptions of the non-boolean settings affecting windows.
.*
.*
.fnlist begin 'Non-Boolean Window Settings'
.*
.setcmd *parm=height buttonheight
.setsyntx
.begdescr
Sets the height (in pixels) of the tools on the toolbar.
.np
&guionly.
.enddescr

.setcmd *parm=width buttonwidth
.setsyntx
.begdescr
Sets the width (in pixels) of the tools on the toolbar.
.np
&guionly.
.enddescr

.setcmd *parm=x clockx
.setsyntx
.begdescr
Sets the x-coordinate of where the clock is to be displayed, if
.keyref clock
is enabled.
.np
&charonly.
.enddescr

.setcmd *parm=y clocky
.setsyntx
.begdescr
Sets the y-coordinate of where the clock is to be displayed, if
.keyref clock
is enabled.
.np
&charonly.
.enddescr

.setcmd *parm=col currentstatuscolumn
.setsyntx
.begdescr
Controls which column current status information is displayed in on the
the menu bar, if
.keyref currentstatus 1
is enabled.
.np
&charonly.
.enddescr

.setcmd *parm=ticks cursorblinkrate
.setsyntx
.begdescr
Controls the speed at which the cursor blinks at.
.np
&guionly.
.enddescr

.setcmd *parm=str gadgetstring
.setsyntx
.begdescr
This string controls the characters that are used to draw the gadgets
on the border. The characters in the string are used as follows:
:OL compact.
:LI.top left corner of edit window
:LI.top right corner of edit window
:LI.bottom left corner of edit window
:LI.bottom right corner of edit window
:LI.left side of edit window
:LI.top and bottom of edit window
:LI.left side of file name on top of border of edit window
:LI.right side of file name on top of border of edit window
:LI.cursor up gadget on scroll bar
:LI.cursor down gadget on scroll bar
:LI.right side of edit window (scroll bar area)
:LI.scroll thumb
:eOL.
.np
&charonly.
.enddescr

.setcmd *parm=clr inactivewindowcolor
.setsyntx
.begdescr
Sets the foreground color of an edit window border when it is inactive
(not the current edit window).  &cmd_parm may be 0-15, or one of
the previously defined color keywords.
.np
&charonly.
.enddescr

.setcmd *parm=n maxtilecolors
.setsyntx
.begdescr
Controls the number of tile colors for tiled windows (when using the
.keyref tile
command).
.np
&charonly.
.enddescr

.setcmd *parm=x maxwindowtilex
.setsyntx
.begdescr
Defines the maximum number of windows that may be tiled together
in the x direction when using the
.keyref tile
command.
.np
&charonly.
.enddescr

.setcmd *parm=y maxwindowtiley
.setsyntx
.begdescr
Defines the maximum number of windows that may be tiled together
in the y direction when using the
.keyref tile
command.
.np
&charonly.
.enddescr

.setcmd *parm=attr movecolor
.setsyntx
.begdescr
Controls the color attributes of an edit window border when the
window is being moved (either by using the mouse or by
using the
.keyref movewin
command).
The attribute is composed of a foreground and a background color,
(16 * background + foreground gives &cmd_parm.).
.np
&charonly.
.enddescr

.setcmd *parm=attr resizecolor
.setsyntx
.begdescr
Controls the color attributes of an edit window border when the
window is being resized (either by using the mouse or by
using the
.keyref resize
command).
The attribute is composed of a foreground and a background color,
(16 * background + foreground gives &cmd_parm.).
.np
&charonly.
.enddescr

.setcmd *parm=x spinx
.setsyntx
.begdescr
Sets the x-coordinate of where the busy spinner is displayed, if
.keyref spinning
is enabled.
.np
&charonly.
.enddescr

.setcmd *parm=y spiny
.setsyntx
.begdescr
Sets the y-coordinate of where the busy spinner is displayed, if
.keyref spinning
is enabled.
.np
&charonly.
.enddescr

.setcmd *parm='n fg bg' tilecolor
.setsyntx
.begdescr
Sets tile area
.param &lt.n&gt.
to have the foreground color
.param &lt.fg&gt.
and the background color
.param &lt.bg&gt.
:period.
.np
The tile area
.param &lt.n&gt.
must be in the range 1 to
.keyref maxtilecolors
:period.
.np
The colors may be in the range 0-15, or one of
the previously defined color keywords.
.np
&charonly.
.enddescr
.*
.fnlist end
.*
.endlevel
