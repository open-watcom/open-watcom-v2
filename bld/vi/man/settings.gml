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
.* 30-jul-92	Craig Eisler	initial draft
.*
:set symbol="guionly" value="This option is only valid with the GUI versions of the editor.".
:set symbol="charonly" value="This option is only valid with the character mode versions of the editor.".
:CHAPTER id='edset'.Editor Settings
This chapter describes the various options that may be controlled using
&edname.'s set command.  Options are typically set in the
:KEYWORD.configuration script
:CONT.;
however, options are settable at execution time as well.
:P.
If you know the option you wish to set, you may just issue the
:KEYWORD.set
command directly at the command prompt, e.g.:
:ILLUST.
set nocaseignore
set autosaveinterval=10
:eILLUST.
:P.
A boolean option is set in the following way:
:ILLUST.
set autoindent   - turns on autoindent
set noautoindent - turns off autoindent
:eILLUST.
Short forms may also be used; for example:
:ILLUST.
set ai   - turns on autoindent
set noai - turns off autoindent
:eILLUST.
:P.
A non-boolean option may be set in the following way:
:ILLUST.
set filename=test.c  - sets current filename to 'test.c'
set filename test2.c - sets current filename to 'test2.c'
:eILLUST.
Note that the assignment operator '=' is optional.
:P.
If you do not know the boolean option you wish to set, you may issue the
set command with no option at the command prompt, e.g.:
:ILLUST.
set
:eILLUST.
This will cause a menu of all possible settings to appear.  These
values may be changed by either cursoring to the desired one and pressing
enter, or by double clicking on the desired one with the mouse.  Boolean
settings will toggle between TRUE and FALSE.  Selecting any other setting
will cause a window to pop up, displaying the old
value and prompting you for the new value.  This window may be cancelled
by pressing the ESC key.
:picture file='set' text='&edname Settings Selection List'.
:P.
When you are finished with the settings menus, you may close the window
by pressing the
:HILITE.ESC
key.
:SECTION id='boolset'.Boolean Settings
:SETLIST.
:SETCMD short=ai.autoindent
In text insertion mode, &cmd_long causes the cursor to move to
start of previous line when a new line is started.
In &cmdmode, &cmd_long causes the cursor to go to the first non
white-space when ENTER is pressed.

:SETCMD short=ac.automessageclear
Automatically erases the message window when a key is typed in &cmdmode
:PERIOD.

:SETCMD short=bf.beepflag
&edname normally beeps when an error is encountered.  Setting &nocmd_long
disables the beeping.

:SETCMD short=ci.caseignore
Causes all searches to be case insensitive if set.

:SETCMD short=cv.changelikevi
If set, then the change command behaves like vi, i.e.
if ESC is pressed when no change has been made, the text is deleted.
Normally, pressing ESC cancels the change without deleting the text.

:SETCMD short=cm.cmode
When cmode is set, certain things will happen when you are entering text:
:OL.
:LI.After entering a line ending in '{', the next line will be indented a
:KEYWORD.shiftwidth
further than the current one.
:LI.After entering a line ending in '}', the current line is shifted to match the
indentation of the line with the matching '{'.  The cursor will flash for
a brief instant on the matching '{' if
:KEYWORD.showmatch
is set.
:LI.All lines entered will have trailing white space trimmed off.
:LI."case" and "default" statements are shifted to be aligned with switch
statements.
:eOL.
:ADDLINE.
Each file has its own &cmd_long. setting; so setting &cmd_long in one file
and not in another (during the same editing session) will work.
:ADDLINE.
One thing that is useful is to add the following lines to your read
:KEYWORD.hook script
:CONT.:
:ILLUST.
			      
    if %E == .c
	set cmode
    else
	set nocmode
    endif

:eILLUST.
This will cause cmode to be set if the file has a .c extension, and not
to be set for any other type of file.

:SETCMD short=cs.columninfilestatus
Causes the current column to be added to file status display
(obtained when typing ^G).

:SETCMD short=ct.currentstatus
Enables the display of the current status on the menu bar. The position
on the menu bar is controlled with
:KEYWORD.currentstatuscolumn
:PERIOD.

:SETCMD short=dt.drawtildes
If &cmd_long is true, then the all lines displayed that do not have any
data associated with them will have a tilde ('~') displayed on the line.
If &cmd_long is false, then no tidles will be displayed and the string
:KEYWORD. fileendstring
will be displayed after the last line with data.

:SETCMD short=eb.eightbits
If &cmd_long is set, then all characters are displayed as normal.  If
&nocmd_long is set then non-printable ASCII will be displayed as control
characters.

:SETCMD short=em.escapemessage
Display the current file status whenever the
:HILITE.ESC
key is pressed in &cmdmode
:PERIOD.

:SETCMD short=xm.extendedmemory
If &cmd_long is set, extended memory is used if it is present
(standard extended, EMS, XMS). This option applies to the real-mode
DOS version of &edname only.

:SETCMD short=iz.ignorectrlz
Normally, a ^Z in a file acts as an end-of-file indicator.  Setting
&cmd_long causes &edname to treat ^Z as just another character in
the file.  This option may also be selected using the '-z' option
when invoking &edname.
:PERIOD.

:SETCMD short=it.ignoretagcase
When using the "-t" command line option of &edname, the tag that is matched
is normally case sensitive.  Setting &cmd_long causes the tag matching
to be case insensitive.

:SETCMD short=ma.magic
If &cmd_long is set, then all special characters in a regular expression
are treated as "magic", and must be escaped to be used in an ordinary
fashion.  If &nocmd_long is set, then any special characters in
:KEYWORD. magicstring
are NOT treated as magic characters by the regular expression
handler, and must be escaped to have special meaning.
:BLANKLINE.
Magic characters are:
:MONO.^$.[()|?+*\~@

:SETCMD short=ps.pauseonspawnerr
This option, if set, causes &edname to pause after spawning (running
a system command)
if there was an error, even if the system command was spawned from a script.
Normally, a command spawned from a script
does not pause when control returns to the editor.

:SETCMD short=qu.quiet
When running in quiet mode, &edname does not update the screen.  This
is useful when running a complex script,
so that the activity of the
editor is hidden.  This option may be selected when invoking
&edname by using the '-q' switch, causing &edname to run in a 'batch mode'.

:SETCMD short=qf.quitmovesforward
If this option is set, then when a file is quit, the next file in the list
of files is moved to.  Otherwise, the previous file in the list of files
is moved to.

:SETCMD short=rf.readentirefile
If &cmd_long is set, then the entire file is read into memory when
it is edited.  This is the default setting.  However, if &nocmd_long
is set, then the file is only read into memory as it is needed.  This
option is useful when you only want to look at the first few pages of
a large file.  This option may be selected when invoking &edname by
using the '-n' switch.

:SETCMD short=rc.readonlycheck
This option causes &edname to complain about modifications to
read-only files every time the file is modified.

:SETCMD short=rt.realtabs
If &nocmd_long is set, then tabs are expanded to spaces when the
file is read.

:SETCMD short=rm.regsubmagic
If &nocmd_long is set, then escaped characters have no meaning
in regular expression substitution expressions.

:SETCMD short=sc.samefilecheck
Normally, &edname just warns you if you edit a file twice
(with a different path). If &cmd_long is set, then if you
edit a file that is the same as a file already being edited (only you
specified a different path), then that file will be brought up, rather
than a new copy being read in.

:SETCMD short='sn'.saveconfig
If this option is set, then the editor's configuration (fonts, colors,
settings, etc) is saved when the editor is exited.
:BLANKLINE.
&guionly

:SETCMD short='so'.saveposition
If this option is set, then the editor's position and size is saved
when the editor is exited, and restored the next time the editor
is started.
:BLANKLINE.
&guionly

:SETCMD short=sw.searchwrap
When searching a file, &edname normally wraps around the top or bottom
of the file.   If &nocmd_long is set, then &edname terminates its searches
once it reaches the top or bottom of the file.

:SETCMD short=sm.showmatch
If &cmd_long is set, the &edname briefly moves the cursor to a matching '('
whenever a ')' is typed while entering text.  Also, the matching '{' is
shown if a '}' is typed if
:KEYWORD.cmode
is set.

:SETCMD short=tp.tagprompt
If a more than one instance of a tag is found in the tags file, a
list of choices is displayed if &cmd_long is set.

:SETCMD short=un.undo
Setting &nocmd_long disables &edname.'s undo ability.

:SETCMD short=ve.verbose
If enabled, this option causes &edname to display extra messages
while doing involved processing.

:SETCMD short=ww.wordwrap
If enabled, word movement commands ('w','W','b','B') will wrap
to the next or previous line.

:SETCMD short=ws.wrapbackspace
If this option is set, pressing backspace while on column one of
a line will cause &edname to wrap to the end of the previous line
(while in
:KEYWORD.text insertion mode
:CONT.).

:SETCMD short=wl.writecrlf
Normally, lines are written with carriage return and line feeds at the
end of each line.  If &nocmd_long is set, the lines are written
with only a line feed at the end.
:eSETLIST.

:SUBSECT.Mouse Control
:SETLIST.
:SETCMD short=lm.lefthandmouse
When &cmd_long is set, the right and left mouse buttons are inverted.

:SETCMD short=um.usemouse
This option enables/disables the use of the mouse in &edname..
:eSETLIST.
:eSUBSECT.

:SUBSECT.Window Control
:SETLIST.
:SETCMD short=cl.clock
This enables/disables the clock display.  The position of the clock
is controlled by the
:KEYWORD.clockx
and
:KEYWORD.clocky
set commands.

:SETCMD short=ln.linenumbers
This option turns on the line number window.  This window is
displayed on the left-hand side of the edit window by default, unless
:KEYWORD.linenumsonright
is set.

:SETCMD short=lr.linenumsonright
Setting &cmd_long causes the line number window to appear on the
right-hand side of the edit window.

:SETCMD short=ml.marklonglines
If this option is set, than any line that exceeds the with of the screen
has the last character highlighted.  If
:KEYWORD.endoflinechar
is a non-zero ASCII value, then the last character is displayed as that
ASCII value.

:SETCMD short=me.menus
This option enables/disables the menu bar.

:SETCMD short=ri.repeatinfo
Normally, &edname echos the repeat count in the
:KEYWORD.countwindow
as it is typed.  Setting &nocmd_long disables this feature.

:SETCMD short=sp.spinning
If set, this option enables the busy-spinner.  Whenever the editor is busy,
a spinner will appear.  The position of the spinner is controlled using
the
:KEYWORD.spinx
and the
:KEYWORD.spiny
set commands.

:SETCMD short=si.statusinfo
If set, this option enables the status info window.  This window
contains the current line and column, and is controlled using the
:KEYWORD.statuswindow
window command.

:SETCMD short=tb.toolbar
This option enables/disables the toolbar.
:BLANKLINE.
&guionly

:SETCMD short=wg.windowgadgets
This option enables/disables gadgets on edit session windows.

:eSETLIST.
:eSUBSECT.

:SECTION id='nonbset'.Non-Boolean Settings
:SETLIST.
:SETCMD parm=seconds.autosaveinterval
Sets the number of seconds between autosaves of the current file to the
backup directory. Autosave is disabled if &cmd_parm is 0.  The backup
&cmd_short
directory is defined using the
:KEYWORD.tmpdir
parameter.
    
:SETCMD parm=size.commandcursortype
Sets the size of the cursor when in &cmdmode.
:PERIOD.
Values for &cmd_parm are 0 to 100 (0=full size, 100=thin).

:SETCMD parm=ascii_val.endoflinechar
If
:KEYWORD.marklonglines
is set, and &cmd_parm is non-zero, then the character in the last column
of a line wider than the screen is displayed as the ASCII value &cmd_parm.
:PERIOD.

:SETCMD parm=attr.exitattr
Defines the attribute to be assigned to the screen when &edname is exited.
The attribute is composed of a foreground and a background color,
(16*background+ foreground gives &cmd_parm). The default
is 7 (white text, black background).
:P.
&charonly.

:SETCMD parm=string.fileendstring
If
:KEYWORD.drawtildes
is false, then the character string &cmd_parm
will be displayed after the last line with data.

:SETCMD parm=rexexp.grepdefault
Default files to search when using the
:KEYWORD.fgrep
or
:KEYWORD.egrep
commands.
&cmd_parm is a file matching regular expression, the default is *.(c|h)
For more information, see the section
:HDREF refid='fmrx'.
in the chapter
:HDREF page=no refid='rxchap'

:SETCMD parm=dist.hardtab
This controls the distance between tabs when a file is displayed.  The
default is 8 (4 on QNX).

:SETCMD parm=fname.historyfile
If the history file is defined, your command and search history is
saved across editing sessions in the file &cmd_parm.
:PERIOD.

:SETCMD parm=size.insertcursortype
Sets the size of the cursor when inserting text in &tinsmode
:PERIOD.
Values for &cmd_parm
are 0 to 100 (0=full size, 100=thin).

:SETCMD parm=str.magicstring
If
:KEYWORD.magic
is not set, then the characters specified in &parm1 are NOT treated
as magic characters by the regular expression handler, and must be
escaped to have special meaning.
:BLANKLINE.
Magic characters are:
:MONO.^$.[()|?+*\~@

:SETCMD parm=numcmds.maxclhistory
&edname keeps a history of commands entered at the &cmdline
:PERIOD.
&cmd_parm sets the number of commands kept in the history.

:SETCMD parm=kbytes.maxemsk
Sets the maximum number of kilobytes of EMS memory to be used
by &edname (DOS real-mode version only).
:BLANKLINE.
This option can only be set during editor initialization.

:SETCMD parm=numfiltercmds.maxfilterhistory
&edname keeps a history of the filter commands entered.
&cmd_parm sets the number of filter commands kept in the history.

:SETCMD parm=numsearchcmds.maxfindhistory
&edname keeps a history of search commands entered.  &cmd_parm sets the
number of search commands kept in the history.

:SETCMD parm=maxlne.maxlinelen
This parameter controls the maximum line length allowed by &edname.
:PERIOD.
The default value is 512 bytes.  Any lines longer than &cmd_parm
are broken up into multiple lines.

:SETCMD parm=num.maxpush
Controls the number of pushed file positions that will be remembered.
Once more than &parm1
:KEYWORD.push
or
:KEYWORD.tag
commands have been issued, the first pushed positions are lost.

:SETCMD parm=n.maxswapk
Sets the maximum number of kilobytes of disk space to be used for
temporary storage by &edname
:PERIOD.
:BLANKLINE.
This option can only be set during editor initialization.

:SETCMD parm=kbytes.maxxmsk
Sets the maximum number of kilobytes of XMS memory to be used
by &edname (DOS real-mode version only).
:BLANKLINE.
This option can only be set during editor initialization.

:SETCMD parm=lines.pagelinesexposed
Sets the number of lines of context left exposed when a page up/down
is done.  For example, if &cmd_parm is set to 1, then when a page down
key is pressed, the bottom line of the file will be visible at the
top of the new page.

:SETCMD parm=size.overstrikecursortype
Sets the size of the cursor when in overstriking text in
:KEYWORD.text insertion mode
:PERIOD.
Values for &cmd_parm are 0 to 100 (0=full size, 100=thin).

:SETCMD parm=rdx.radix
Sets the radix (base) of the results of using the
&cmdline command
:KEYWORD.eval
:PERIOD.
The default is base 10.

:SETCMD parm=nsp.shiftwidth
Sets the number of spaces inserted/deleted by the shift operators ('>'
and '<'), ^D and ^T in text insertion mode, and by
:KEYWORD.autoindent
and
:KEYWORD.cmode
when they are indenting.

:SETCMD parm=kbytes.stackk
Sets the size (in kilobytes) of the stack used by &edname.
:PERIOD.
The minimum is 10.  This can be set higher if you plan on using
nested scripts that go deeper than 4 or 5 levels.
:BLANKLINE.
This option can only be set during editor initialization.

:SETCMD parm=sects.statussections
The controls the appearance of the bars in the status window.  A list
of distances (in pixels) is given.  This distance is measured from the
start of the status window.  Each section may have something put in it
via the
:KEYWORD.statusstring
:BLANKLINE.
&guionly.

:SETCMD parm=str.statusstring
This controls what is displayed in the status window.  Any characters
may be in this string.  Additionally, the dollar sign ('$') is a special
character.  It is used in combination with other characters to represent
special values:
:UL.
:LI.$<n>C
current column number.  If <n> is specified (a number), then the column
number will be padded with spaces so that the it occupies <n> characters.
:LI.$D
current date
:LI.$H
current hint text from menus or toolbar (GUI editors only)
:LI.$<n>L
current line number.  If <n> is specified (a number), then the line
number will be padded with spaces so that the it occupies <n> characters.
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
:LI.$>
text will be right-justified (within current block for GUI editors)
:LI.$<
text will be left-justified (within current block for GUI editors)
:eUL.
A number may precede the L or the C, to indicate the amount of
space that the string should occupy; for example,
$6L  will cause the line number to always occupy at least 6 spaces.
:BLANKLINE.
The string may be surrounded by quotes if spaces are being used.
The default status string setting for character mode editors is:
:ILLUST.
set statusstring="L:$6L$nC:$6C"
:eILLUST.
For GUI editors, the default status string setting is:
:ILLUST.
set statusstring = "Ln:$5L$[Col:$3C$[Mode: $M$[$|$T$[$H"
:eILLUST.

:SETCMD parm=string.shellprompt
This setting controls what the prompt at the command line will be
after using the
:KEYWORD.shell
command.

:SETCMD parm=nsp.tabamount
Sets the number of spaces inserted when the tab key is pressed in
a text insertion mode.  If
:KEYWORD.realtabs
is set, these spaces will be changed into tabs, based on the setting of
:KEYWORD.hardtab
:PERIOD.

:SETCMD parm=fname.tagfilename
This defines the file name that &edname is to use to locate tags
in.  The default is the name
:ITALICS.tags
:PERIOD.

:SETCMD parm=dir.tmpdir
This is used to specify the directory where all temporary editor files
are to be created.
:ADDLINE.
This option can only be set during editor initialization.

:SETCMD parm=str.word
This defines the word used by &edname.
:PERIOD.
&cmd_parm is a group of character pairs.
Each pair defines a range; e.g. 09az defines the characters 0 through 9
and a thorough z.
Any character in
the ranges defined by
&cmd_parm is considered part of a word.
:BLANKLINE.
The default for &cmd_long is "__09AZaz".
:BLANKLINE.
The word will be delimited by
white space (spaces or tabs) and all characters not in the ranges
defined by &cmd_parm.
:PERIOD.

:SETCMD parm=margin.wrapmargin
If &cmd_long is set to a non-zero value, then word wrapping is enabled.
As text is entered, the position of the cursor is monitored.  Once the
cursor gets within &cmd_parm characters of the right margin, the current
word is moved to a new line.

:eSETLIST

:SUBSECT.Mouse Control
:SETLIST.

:SETCMD parm=ticks.mousedclickspeed
Sets the number of ticks between the first mouse button depress and
the second mouse button depress for the action to count as a double-click.
A tick is approximately 1/18 of a second.
:BLANKLINE.
&charonly.

:SETCMD parm=ticks.mouserepeatdelay
Sets the number of ticks between when a mouse button is depressed and
when the button starts to "repeat".
A tick is approximately 1/18 of a second.
:BLANKLINE.
&charonly.

:SETCMD parm=speed.mousespeed
Sets the speed of the mouse. &cmd_parm may be in the range 0-31
(0 is the fastest, 31 is the slowest).
:BLANKLINE.
&charonly.

:SETCMD parm=str.wordalt
This defines the alternate word used when double clicking the mouse.
&cmd_parm is defined in the same fashion as for the
:KEYWORD.word
setting.
:BLANKLINE.
The default for &cmd_long is "::..\\__09AZaz".
:eSETLIST.
:eSUBSECT.

:SUBSECT.Window Control
:SETLIST.
:SETCMD parm=height.buttonheight
Sets the height (in pixels) of the tools on the toolbar.
:BLANKLINE.
&guionly.

:SETCMD parm=height.buttonwidth
Sets the width (in pixels) of the tools on the toolbar.
:BLANKLINE.
&guionly.

:SETCMD parm=x.clockx
Sets the x-coordinate of where the clock is to be displayed, if
:KEYWORD.clock
is enabled.
:BLANKLINE.
&charonly.

:SETCMD parm=y.clocky
Sets the y-coordinate of where the clock is to be displayed, if
:KEYWORD.clock
is enabled.
:BLANKLINE.
&charonly.

:SETCMD parm=col.currentstatuscolumn
Controls which column current status information is displayed in on the
the menu bar, if
:KEYWORD.currentstatus
is enabled.
:BLANKLINE.
&charonly.

:SETCMD parm=ticks.cursorblinkrate
Controls the speed at which the cursor blinks at.
:BLANKLINE.
&guionly.

:SETCMD parm=str.gadgetstring
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
:BLANKLINE.
&charonly.

:SETCMD parm=clr.inactivewindowcolor
Sets the foreground color of an edit window border when it is inactive
(not the current edit window).  &cmd_parm may be 0-15, or one of
the previously defined color keywords.
:BLANKLINE.
&charonly.

:SETCMD parm=n.maxtilecolors
Controls the number of tile colors for tiled windows (when using the
:KEYWORD.tile
command).
:BLANKLINE.
&charonly.

:SETCMD parm=x.maxwindowtilex
Defines the maximum number of windows that may be tiled together
in the x direction when using the
:KEYWORD.tile
command.
:BLANKLINE.
&charonly.

:SETCMD parm=y.maxwindowtiley
Defines the maximum number of windows that may be tiled together
in the y direction when using the
:KEYWORD.tile
command.
:BLANKLINE.
&charonly.

:SETCMD parm=attr.movecolor
Controls the color attributes of an edit window border when the
window is being moved (either by using the mouse or by
using the
:KEYWORD.movewin
command).
The attribute is composed of a foreground and a background color,
(16*background+ foreground gives &cmd_parm).
:BLANKLINE.
&charonly.

:SETCMD parm=attr.resizecolor
Controls the color attributes of an edit window border when the
window is being resized (either by using the mouse or by
using the
:KEYWORD.resize
command).
The attribute is composed of a foreground and a background color,
(16*background+ foreground gives &cmd_parm).
:BLANKLINE.
&charonly.

:SETCMD parm=x.spinx
Sets the x-coordinate of where the busy spinner is displayed, if
:KEYWORD.spinning
is enabled.
:BLANKLINE.
&charonly.

:SETCMD parm=y.spiny
Sets the y-coordinate of where the busy spinner is displayed, if
:KEYWORD.spinning
is enabled.
:BLANKLINE.
&charonly.

:SETCMD parm='n fg bg'.tilecolor
Sets tile area
:CMDPARM.<n>
to have the foreground color
:CMDPARM.<fg>
and the background color
:CMDPARM.<bg>
:PERIOD.
:BLANKLINE
The tile area
:CMDPARM.<n>
must be in the range 1 to
:KEYWORD.maxtilecolors
:PERIOD.
:BLANKLINE
The colors may be in the range 0-15, or one of
the previously defined color keywords.
:BLANKLINE.
&charonly.

:eSETLIST.
