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
.* 01-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id=modes.Modes
:CMT :SECTION.Introduction
&edname is a modal editor.  When you are in &cmdmode
(the default mode), there are a number of valid keys that may be pressed.
To ensure that you are in &cmdmode,
press the
:HILITE.ESC
key until the mode indicator
on the menu bar displays:
:ILLUST.
Mode: command
:eILLUST.
:P.
When in &tinsmode, text may be entered.  There are two aspects to
&tinsmode,
:ITALICS.insert
and
:ITALICS.overstrike
:PERIOD.
:KEYWORD.Text insertion mode
is entered via a number of different commands from &cmdmode,
and is indicated by a
larger cursor, along with a mode line indication.  The
types of cursor are controlled with the
:KEYWORD.commandcursortype
:CONT.,
:KEYWORD.insertcursortype
:CONT.,
and
:KEYWORD.overstrikecursortype
settings.
:P.
When &edname is in a &tinsmode,
the mode indicator on the menu bar displays one of:
:ILLUST.
Mode: insert
:eILLUST.
or
:ILLUST.
Mode: overstrike
:eILLUST.

.*
.*** Text insertion mode
.*
:SECTION id='timode'.Text Insertion Mode
When in &tinsmode (either inserting or overstriking), you
may enter text and freely cursor about through the file.  When you
are finished adding text, the
:HILITE.ESC
key returns you to &cmdmode
:PERIOD.
:P.
It should be remembered that an undo applies to
changes caused by commands; so all changes made while in &tinsmode
are part of a single undo record. For more information on
undos, see the section
:HDREF refid='cmundo'.
later on in this chapter.
:P.
The following keys, when pressed in &cmdmode, place you into &tinsmode:
:DEFLIST.
:DEFITEM.a
Starts appending (inserting) text after the current character in
the edit buffer.
:DEFITEM.A
Starts appending (inserting) text after the last character on the
current line in the edit buffer.
:DEFITEM.C
Changes text from the current position to the end of the current line.
Deletes the text, and enters &tinsmode
:PERIOD.
:DEFITEM.<n>c<oper>
Change command.  Deletes the text in the range specified by
:ITALICS.<oper>
:CONT., and enters &tinsmode
:PERIOD.
:DEFITEM.g
Starts inserting or overstriking text at the current cursor position,
depending on how you were adding text the last time you were in &tinsmode
:PERIOD.
:DEFITEM.i
Starts inserting text at the current cursor position.
:DEFITEM.I
Starts inserting text before the first non-white space character on
the current line.
:DEFITEM.o
Opens a line after the current line, and enters &tinsmode
:PERIOD.
:DEFITEM.O
Opens a line before the current line, and enters &tinsmode
:PERIOD.
:DEFITEM.R
Starts overstriking text at the current character in the edit buffer.
:DEFITEM.<n>s
Substitute
:ITALICS.<n>
characters.  The first
:ITALICS.<n>
characters from the current cursor position are deleted, and &tinsmode
is entered.
:DEFITEM.<n>S
Substitute lines of text.
:ITALICS.<n>
lines from the current line forward are deleted, and &tinsmode is entered.
:DEFITEM.INS
Start inserting text at the current cursor position.
:eDEFLIST.
:P.

:SUBSECT.Special Keys
While in &tinsmode, certain keys do special things.  These keys are:
:DEFLIST.
:DEFITEM.Arrow Keys
:DEFLIST.
:DEFITEM.UP
Cursor up through the text.
:DEFITEM.DOWN
Cursor down through the text.
:DEFITEM.LEFT
Cursor left through the text.
:DEFITEM.RIGHT
Cursor right through the text.
:eDEFLIST.

:DEFITEM.Text Selection Keys
:INCLUDE file='textsel'.

:DEFITEM.CTRL_PAGEUP
Moves to the first non-white space character on the first line
of the current edit buffer.

:DEFITEM.CTRL_PAGEDOWN
Moves to the last character on the last line
of the current edit buffer.

:DEFITEM.SHIFT_DEL
Deletes the currently selected region into the active &copybuffer
:PERIOD.

:DEFITEM.SHIFT_INS
Pastes the active &copybuffer
into the text after the current position.

:DEFITEM.SHIFT_TAB
Move back to the previous tab stop, deleting the characters before the
cursor.

:DEFITEM.CTRL_DEL (ctrl-delete)
Delete the current line into the active &copybuffer
:PERIOD.

:DEFITEM.CTRL_INS (ctrl-insert)
Pastes the active &copybuffer into the text before the current position.

:DEFITEM.CTRL_D
Move backwards
:KEYWORD.shiftwidth
spaces, deleting the characters before the cursor. A
:KEYWORD.shiftwidth
is a number that you may set, its default value is 4.

:DEFITEM.CTRL_T
Insert
:KEYWORD.shiftwidth
spaces.  If
:KEYWORD.realtabs
is set, then once
:KEYWORD.tabamount
spaces are inserted, the spaces are replaced with a tab character.

:DEFITEM.CTRL_V
The next key typed is inserted directly, without any interpretation.

:DEFITEM.CTRL_Q
The next key typed is inserted directly, without any interpretation.

:DEFITEM.BS
Backspace one on the current line, deleting the character before the
cursor.

:DEFITEM.DEL
Delete the character under the cursor.  If you are at the end of the line,
:ITALICS.DEL
has the same effect as pressing
:ITALICS.BS
:PERIOD.

:DEFITEM.ENTER
Start a new line.

:DEFITEM.END
Move to the end of the current line.

:DEFITEM.HOME
Move to the start of the current line.

:DEFITEM.INS
Toggles between insert and overstrike mode.

:DEFITEM.PAGEUP
Move up one page in the text.

:DEFITEM.PAGEDOWN
Move up down one page in the text.

:DEFITEM.TAB
Move forward to the next tab stop.  If
:KEYWORD.realtabs
is set, a tab character is inserted into the file. Otherwise, spaces
are inserted.

:eDEFLIST.
:eSUBSECT.

.*
.*** Command mode
.*
:SECTION.Command Mode
The following &cmdmode command descriptions show items within
angle brackets (<>).
The angle brackets are there to indicate items that you may supply.
You are not required to type the brackets.  For example, <n> simply
means that in the corresponding place in the command you can enter
a number.
:P.
Many commands may be preceded with a repeat count, which is
indicated by a
:ITALICS.<n>
before a command.
The number is not required; if it is not supplied, it is usually assumed
that a 1 was entered for the repeat count. As long as the setting
:KEYWORD.repeatinfo
is enabled, the number that is typed appears in a special window called the
:KEYWORD.countwindow
:PERIOD.
:P.
Other commands may be preceded with a &copybuffer
name, which is indicated with a
:ITALICS.<"?>
:PERIOD.
If you do not want the result of the operation to be copied into the
active buffer, then an alternate buffer may be specified.
The double quotes (")
are required (this indicates that an alternate buffer is being specified),
and then a buffer '1'-'9' or 'a'-'z' is specified. See the section
:HDREF refid='cpybuff'.
for more information.

:CMSECT id='mvement'.Movement

The following are &cmdmode commands that cause movement in the
current edit buffer.
:KEYLIST.
:KEY."|" "<n>|" (or bar)
Move to the column number specified by
:ITALICS.<n>
:PERIOD.
:EXAMPLE.|
Move to column 1 of the current line.
:EXAMPLE.15|
Move to column 15 of the current line.

:KEY.` `<?> (back quote)
Moves to the mark position (line and column) specified by
:ITALICS.<?>
See the section
:HDREF refid='marks'.
for more information.
:SEECMD.mark
:EXAMPLE.`a
Move to the line and column with mark
:ITALICS.a
:PERIOD.

:KEY."'" "'<?>" (front quote)
Move to the start of line with the mark
:ITALICS.<?>
:PERIOD.
See the section
:HDREF refid='marks'.
for more information.
:SEECMD.mark
:EXAMPLE.'z
Move to the start of the line with mark
:ITALICS.z
:PERIOD.

:KEY.'%' '%' (percent)
Moves to matching brace or other defined match string.
Defaults are "{","}"
and "(",")". For example, by pressing
:ITALICS.%
while on the first opening brace ('(') on the line:
:ILLUST.
    if( ( i=foo( x ) ) ) return;
:eILLUST.
moves the cursor to the last closing brace (')') on the line.
It is possible to set arbitrary pairs of match strings using the
&cmdline command
:KEYWORD.match
:PERIOD.

:KEY.'$' '$' (dollar)
Moves the cursor to the last character on the current line.

:KEY.'^' '^' (caret)
Moves the cursor to the first non-whitespace character on the current line.

:KEY.';' ';' (semi-colon)
Repeats the last
:HILITE.f
:CONT.,
:HILITE.F
:CONT.,
:HILITE.t
or
:HILITE.T
movement commands.

:KEY.',' ',' (comma)
Repeats the last
:HILITE.f
:CONT.,
:HILITE.F
:CONT.,
:HILITE.t
or
:HILITE.T
movement commands, but the search is done in the opposite direction.
:P.
If the last movement command was an
:HILITE.F
then an
:HILITE.f
movement command is executed.
If the last movement command was a
:HILITE.t
then a
:HILITE.T
movement command is executed.
:P.
Similarly, if the last movement
command was an
:HILITE.f
then an
:HILITE.F
movement command is executed.
If the last movement command was a
:HILITE.t
then a
:HILITE.T
movement command is executed.

:KEY.- <n>- (dash)
Moves the cursor to the start of the previous line.  If a repeat count
:ITALICS.<n>
is specified, then you are moved up
:ITALICS.<n>
lines.

:KEY.+ <n>+ (plus)
Moves the cursor to the start of the next line.  If a repeat count
:ITALICS.<n>
is specified, then you are moved down
:ITALICS.<n>
lines.

:KEY.0 0
Moves the cursor the first character of the current line.

:KEY.CTRL_PAGEUP CTRL_PAGEUP
Moves to the first non-white space character on the first line
of the current edit buffer.

:KEY.CTRL_PAGEDOWN CTRL_PAGEDOWN
Moves to the last character on the last line
of the current edit buffer.

:KEY.DOWN <n>DOWN
Move the cursor down one line.
:ITALICS.<n>
is specified, the cursor moves down
:ITALICS.<n>
lines.

:KEY.END END
Moves the cursor to the last character on the current line.

:KEY.ENTER <n>ENTER
Moves the cursor to the start of the next line.  If a repeat count
:ITALICS.<n>
is specified, then the cursor is moved down
:ITALICS.<n>
lines.

:KEY.HOME HOME
Moves the cursor the first character of the current line.

:KEY.LEFT <n>LEFT
Move the cursor left one character.  If
:ITALICS.<n>
is specified, the cursor moves left
:ITALICS.<n>
characters.

:KEY.PAGEDOWN <n>PAGEDOWN
Moves forwards one page.  If a repeat count
:ITALICS.<n>
is specified, then you are moved ahead
:ITALICS.<n>
pages.
The number of lines of context maintained
is controlled by the
:KEYWORD.pagelinesexposed
setting.

:KEY.PAGEUP <n>PAGEUP
Moves backwards one page.  If a repeat count
:ITALICS.<n>
is specified, then you are moved back
:ITALICS.<n>
pages.
The number of lines of context maintained
is controlled by the
:KEYWORD.pagelinesexposed
setting.

:KEY.RIGHT <n>RIGHT
Move the cursor right one character.  If
:ITALICS.<n>
is specified, the cursor moves right
:ITALICS.<n>
characters.

:KEY.SHIFT_TAB <n>SHIFT_TAB
Moves the cursor left by
:KEYWORD.tabamount
characters. A repeat count
:ITALICS.<n>
multiplies this.

:KEY.TAB <n>TAB
Moves the cursor right by
:KEYWORD.tabamount
characters. A repeat count
:ITALICS.<n>
multiplies this.

:KEY.UP <n>UP
Move the cursor up one line.
:ITALICS.<n>
is specified, the cursor moves up
:ITALICS.<n>
lines.

:KEY.CTRL_B <n>CTRL_B
Moves backwards one page.  If a repeat count
:ITALICS.<n>
is specified, then you are moved back
:ITALICS.<n>
pages.
The number of lines of context maintained
is controlled by the
:KEYWORD.pagelinesexposed
setting.

:KEY.CTRL_D <n>CTRL_D
Move down a certain number of lines.  The default is to move down
half a page.  If the repeat count
:ITALICS.<n>
is specified, then that becomes the number of lines moved from then on.
Also see the
:HILITE.CTRL_U
key.

:KEY.CTRL_E <n>CTRL_E
Expose the line below the last line in the current edit window,
leaving the cursor on the same line if possible.  If a repeat
count
:ITALICS.<n>
is specified, then that many lines are exposed.

:KEY.CTRL_F <n>CTRL_F
Moves forwards one page.  If a repeat count
:ITALICS.<n>
is specified, then you are moved ahead
:ITALICS.<n>
pages.
The number of lines of context maintained
is controlled by the
:KEYWORD.pagelinesexposed
setting.

:KEY.CTRL_N <n>CTRL_N
Move the cursor to the next line.  If a repeat count
:ITALICS.<n>
is specified, then you are moved down
:ITALICS.<n>
lines.

:KEY.CTRL_P <n>CTRL_P
Move the cursor to the previous line.  If a repeat count
:ITALICS.<n>
is specified, then you are moved up
:ITALICS.<n>
lines.

:KEY.CTRL_U <n>CTRL_U
Move up a certain number of lines.  The default is to move up
half a page.  If the repeat count
:ITALICS.<n>
is specified, then that becomes the number of lines moved from then on.
Also see the
:HILITE.CTRL_D
key.

:KEY.CTRL_Y <n>CTRL_Y
Expose the line above the first line in the current edit window,
leaving the cursor on the same line if possible.  If a repeat
count
:ITALICS.<n>
is specified, then that many lines are exposed.

:KEY.B <n>B
Moves the cursor backwards to the start of previous space delimited word
on the current line.
:EXAMPLE.B
If the cursor was on the right parenthesis (')') of
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the
:ITALICS.f
in
:ITALICS.foo
:PERIOD.
:EXAMPLE.2B
If the cursor was on the right parenthesis (')') of
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the
:ITALICS.=
sign.

:KEY.b <n>b
Moves the cursor backwards to the start of the previous word on the
current line.
:INCLUDE file='worddef'.
:EXAMPLE.b
If the cursor was on the right parenthesis (')') of
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the letter
:ITALICS.a
in
:ITALICS.abc
:PERIOD.

:EXAMPLE.2b
If the cursor was on the right parenthesis (')') of
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to left parenthesis 
:ITALICS.(
:PERIOD.

:KEY.E <n>E
Moves the cursor to the end of the next space delimited word on the
current line.
:EXAMPLE.E
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the right parenthesis
:ITALICS.)
:PERIOD.
:EXAMPLE.2E
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the 
:ITALICS.+
sign.

:KEY.e <n>e
Moves the cursor to the end of the next word on the current line.
:INCLUDE file='worddef'.
:EXAMPLE.e
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the second letter
:ITALICS.o
in
:ITALICS.foo
:PERIOD.
:EXAMPLE.2e
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the left parenthesis
:ITALICS.(
:PERIOD.

:KEY.F <n>F<?>
Moves the cursor backwards from its current position to the character
:ITALICS.<?>
on the current line.  If a repeat count
:ITALICS.<n>
is specified, then the nth occurrence of the character
:ITALICS.<?>
is moved to.
:EXAMPLE.F+
If the cursor is on the semi-colon (';') in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the
:ITALICS.+
sign.
:EXAMPLE.2Fo
If the cursor is on the semi-colon (';') in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the first
:ITALICS.o
in
:ITALICS.foo
:PERIOD.

:KEY.f <n>f<?>
Moves the cursor forwards from its current position to the character
:ITALICS.<?>
on the current line.  If a repeat count
:ITALICS.<n>
is specified, then the
:ITALICS.nth
occurrence of the character
:ITALICS.<?>
is moved to.
:EXAMPLE.f+
If the cursor is on the character
:ITALICS.x
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the
:ITALICS.+
sign.
:EXAMPLE.2fo
If the cursor is on the character
:ITALICS.x
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the second
:ITALICS.o
in
:ITALICS.foo
:PERIOD.

:KEY.G <n>G
Goes to the line specified by the repeat count
:ITALICS.<n>
:PERIOD.
If no repeat count is specified, you move the the last line in the
current edit buffer.
:EXAMPLE.100G
Moves to line 100 in the current edit buffer.
:EXAMPLE.G
Moves to the last line in the current edit buffer.

:KEY.h <n>h
Move the cursor left one character.  If
:ITALICS.<n>
is specified, the cursor moves left
:ITALICS.<n>
characters.

:KEY.H <n>H
Moves to the line at the top of the current file window.  If
a repeat count is specified, then you are moved to that line relative
to the top of the current file window.
:EXAMPLE.2H
Moves to the second line from the top of the current file window.
:EXAMPLE.H
Moves to the line at the top of the current file window.

:KEY.j <n>j
Move the cursor down one line.
:ITALICS.<n>
is specified, the cursor moves down
:ITALICS.<n>
lines.

:KEY.k <n>k
Move the cursor up one line.
:ITALICS.<n>
is specified, the cursor moves up
:ITALICS.<n>
lines.

:KEY.L <n>L
Moves to the line at the bottom of the current file window.  If
a repeat count is specified, then you are moved to that line relative
from the bottom of the current file window.
:EXAMPLE.2L
Moves to the second line from the bottom of the current file window.
:EXAMPLE.L
Moves to the line at the bottom of the current file window.

:KEY.l <n>l
Move the cursor right one character.  If
:ITALICS.<n>
is specified, the cursor moves right
:ITALICS.<n>
characters.

:KEY.M M
Moves the cursor to the line in the middle of the current file window.

:KEY.T <n>T<?>
Moves the cursor backwards from its current position to the character
after the character
:ITALICS.<?>
on the current line.  If a repeat count
:ITALICS.<n>
is specified, then the the character after the nth
occurrence of the character
:ITALICS.<?>
is moved to.
:EXAMPLE.T+
If the cursor is on the semi-colon (';') in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the space after the
:ITALICS.+
sign.
:EXAMPLE.2To
If the cursor is on the semi-colon (';') in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the second
:ITALICS.o
in
:ITALICS.foo
:PERIOD.

:KEY.t <n>t<?>
Moves the cursor forwards from its current position to the character before
the character
:ITALICS.<?>
on the current line.  If a repeat count
:ITALICS.<n>
is specified, then the the character before the nth
occurrence of the character
:ITALICS.<?>
is moved to.
:EXAMPLE.t+
If the cursor is on the character
:ITALICS.x
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the space before
:ITALICS.+
sign.
:EXAMPLE.2to
If the cursor is on the character
:ITALICS.x
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
The the cursor is moved to the first
:ITALICS.o
in
:ITALICS.foo
:PERIOD.

:KEY.W <n>W
Moves the cursor forward to the start of the next space delimited word
on the current line.
:EXAMPLE.W
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the 
:ITALICS.+
sign.
:EXAMPLE.2W
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the number
:ITALICS.3
:PERIOD.

:KEY.w <n>w
Moves the cursor forward to the start of the next word on the current
line.
:INCLUDE file='worddef'.
:EXAMPLE.w
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the left parenthesis
:ITALICS.(
:PERIOD.
:EXAMPLE.2w
If the cursor was on the letter
:ITALICS.f
in
:ILLUST.
x = foo(abc) + 3;
:eILLUST.
then the cursor moves to the letter
:ITALICS.a
in
:ITALICS.abc
:PERIOD.

:eKEYLIST.
:eCMSECT.

:CMSECT id=cmundo.Undoing Changes

&edname keeps an undo history of all changes made to an edit buffer.
There is no limit on the number of undos, as long as there is enough
memory to save the undo information.  If there is not enough memory
to save undo information for the current action, then the oldest undo
information is removed until enough memory has been released.
:P.
There is also an undo-undo (redo) history:  as you issue undo commands,
the information to redo the undo is kept.  However, once you modify the
file other than by doing an undo, the redo history is lost.
:P.
As you issue undo commands, a message indicating how many undos are
remaining.  The message could look like:
:ILLUST.
16 items left on undo stack
:eILLUST.
This lets you know how many undos it would take to restore the
edit buffer to its original condition.  Once there are no more undos,
you will see the message:
:ILLUST.
undo stack is empty
:eILLUST.
Once you undo all changes,
then the file changes state from modified to unmodified.  However,
if some undo changes have had to be discarded because of low memory,
the file will still be in a modified state.
:P.
The keystrokes for doing undo and redo are:
:KEYLIST.
:KEY.u u
Undo last change.
:KEY.U U
Redo last undo.
:eKEYLIST.
:SEECMD.undo

:eCMSECT.

:CMSECT id='marks'.Marks
:INCLUDE file='markinfo'.
:P.
Mark commands are:
:KEYLIST.
:KEY.m m<?>
Allows the setting of mark
:ITALICS.<?>
:PERIOD.
:BLANKLINE.
If
:ITALICS.<?>
is an exclamation mark ('!')
instead of a letter, it clears all marks on the current line.
:BLANKLINE.
If
:ITALICS.<?>
is a dot ('.')
instead of a letter, it puts &edname in
:KEYWORD.memorize mode
:PERIOD.
All characters typed are memorized until another dot ('.')
is pressed.  The memorized keystrokes may be repeated by pressing
a dot ('.').
See the dot ('.') &cmdmode command later in this chapter.
:BLANKLINE
If
:ITALICS.<?>
is an equals sign ('=')
instead of a letter, it puts &edname in alternate
:KEYWORD.memorize mode
:PERIOD.
All characters typed are memorized until another equals sign ('=')
is pressed.  The memorized keystrokes may be repeated by pressing
an equals sign ('=').
See the equals sign ('=') &cmdmode command later in this chapter.
:SEECMD.mark
:EXAMPLE.ma
Sets the mark a at the current cursor position
:EXAMPLE.m.
Enter memorize mode
:EXAMPLE.m!
Clear any marks set on the current line.

:KEY."'" "'<?>" (front quote)
Move to the start of the line with the mark
:ITALICS.<?>
:PERIOD.
:EXAMPLE.'a
Moves to the first column of the line with mark
:ITALICS.a
:PERIOD.
:EXAMPLE.''
Moves to the first column of line of the last position before the last
non-linear movement command was issued.
:KEY.'`' '`<?>' (back quote)
Move to the position in the edit buffer with the mark
:ITALICS.<?>
:PERIOD.
:EXAMPLE.`a
Moves to the column and line with mark
:ITALICS.a
:PERIOD.
:EXAMPLE.``
Moves to the last position before the last
non-linear movement command was issued.
:eKEYLIST.
:P.
Pressing
:ITALICS.``
and
:ITALICS.''
take you to the last position you were at before you used a
non-linear movement command (',`,?,/,G, n, and N commands).
So, if you are at line 5 column 10 and type
:ITALICS./foo
:CONT.,
pressing
:ITALICS.``
will first move
you back to line 5 column 10. Pressing
:ITALICS.``
again will move you to the occurrence of
foo, since the previous `` command was a non-linear movement command.

:eCMSECT.

:CMSECT id='cpybuff'.Copy Buffers

A  &copybuffer is a buffer where copied or deleted data is kept.
There are a number of these buffers available.  There are 9 default
buffers that text is placed into when it is deleted/yanked (see
the &cmdline commands
:KEYWORD.delete
and
:KEYWORD.yank
:CONT.,
along with the sections
:HDREF refid='deltext'
and
:HDREF refid='cpytext'
later in this chapter).
These buffers are numbered 1 through 9, and any of these buffers may
be the active &copybuffer
:PERIOD.
:P.
The active &copybuffer may be selected using function keys. 
CTRL_F1 through CTRL_F9 select buffers 1 through 9 respectively.
When a buffer is selected,
information about its contents is displayed in the message window.
This buffer becomes the active &copybuffer
:PERIOD.
All yanked/deleted text is copied into this buffer.
:P.
When text is yanked/deleted into the active &copybuffer,
the contents of the the buffers are cascaded forward from the active
buffer into the next one, with the last numbered &copybuffer
losing its contents. Any buffers that are before the active
&copybuffer have their contents preserved.  For example, if buffer 3
is the active buffer, then a deletion will cascade buffer 3 to buffer 4,
buffer 4 to buffer 5, and so on, with the contents of buffer 9 being lost.
Buffers 1 and 2 remain untouched, and buffer 3 gets a copy of the deleted text.
:P.
There are several &cmdmode commands that add text to buffers; they
are
:DEFLIST.
:DEFITEM.<"?><n>d<oper>
Deletes text in various ways.
:DEFITEM.<n>DEL
Deletes the character at the current cursor position.
:DEFITEM.D
Deletes text from the current cursor position to the end of the current line.
:DEFITEM.<n>x
Deletes the character at the current cursor position.
:DEFITEM.<n>X
Deletes the character before the current cursor position.
:DEFITEM.<"?><n>y<oper>
Yanks (copies) text in various ways.
:DEFITEM.<n>Y
Yanks (copies) the current line.
:eDEFLIST.
There is more information on these &cmdmode
commands later in this chapter.
:P.
Text may be yanked/deleted into a specific &copybuffer
by typing "[1-9] before
the appropriate command. As well, there are 26 named buffers that may
be used, 'a'-'z'.  When text is yanked or deleted into a named buffer,
it remains there for the life of the editing session (or until replaced).
:P.
To retrieve the contents of a buffer, use:
:KEYLIST.
:KEY.SHIFT_INS <"?>SHIFT_INS
Puts (pastes) the contents of the active &copybuffer
after the cursor position in the current edit buffer. 
:SEECMD.put
:EXAMPLE."aSHIFT_INS
Copy the data in the named buffer
:ITALICS.a
after the current position in the file.
:EXAMPLE.SHIFT_INS
Copy the data in the active buffer
after the current position in the file.

:KEY.p <"?>p
Puts (pastes) the contents of the active &copybuffer
after the cursor position in the current edit buffer.
:SEECMD.put
:EXAMPLE.p
Copies the data in the active buffer after the current position in the file.
:EXAMPLE."5p
Copies the data in the numbered buffer
:ITALICS.5
after the current position in the file.

:KEY.P <"?>P
Puts (pastes) the contents of the active &copybuffer
before the cursor position in the current edit buffer.
:SEECMD.put
:P.
:EXAMPLE."aP
Copy the data in the named buffer
:ITALICS.a
before the current position in the file.
:EXAMPLE.P
Copy the data in the active buffer
before the current position in the file.
:eKEYLIST.
Without a
:ITALICS."?
prefix, these commands retrieve the contents of the active buffer.
:P.
The contents of a &copybuffer
may be executed, as if the contents were typed from the
keyboard. See the
:HILITE.@
&cmdmode command later in this chapter.

:eCMSECT.

:CMSECT id='srching'.Searching

The following &cmdmode commands are used for searching for text:
:KEYLIST.
:KEY.'/' '/' (forward slash)
Enter a regular expression to search for from current position forwards.
:KEY.? ? (question mark)
Enter a regular expression to search for from current position backwards.
:KEY.n n
Repeat last search command, in the direction of the last search.
:KEY.N N
Repeat last search command, in the opposite direction of the last search.
:eKEYLIST.
For more information on regular expressions, see
:HDREF refid='rxchap'.
:PERIOD.
:P.
Once you press the
:HILITE./
or the
:HILITE.?
keys, a search string entry window will appear:
:picture file='srchent' text='Search String Entry Window'.
This position and size of the search string entry window are controlled
using
the
:KEYWORD.commandwindow
windowing command.
Search strings may be up to 512 bytes in length; the search string window
scrolls.
:P.
The search string window has a history associated with it; the size
of this search string history is controlled using the
:KEYWORD.maxfindhistory
setting.  As well, the search string history is preserved across sessions of
&edname if the
:KEYWORD.historyfile
parameter is set.
:P.
If the first letter of a search string is a CTRL_A (entered by typing
a CTRL_V followed by a CTRL_A)
then that search string will not be added to the search string history.
:SUBSECT.Special Keys In The Search String Window
Once in the search string window, a number of keys have special meaning:
.se searching = 1
:INCLUDE file='speckeys'.
:eSUBSECT.
:eCMSECT

:CMSECT.Inserting Text

The following commands cause &edname to go from &cmdmode
directly into &tinsmode:
:KEYLIST.
:KEY.INS INS
Starts inserting text before the current character in the edit buffer.
:KEY.a a
Starts appending (inserting) text after the current character in
the edit buffer.
:KEY.A A
Starts appending (inserting) text after the last character on the
current line in the edit buffer.
:PERIOD.

:INCLUDE file='gkey'.

:KEY.i i
Starts inserting text before the current character in the edit buffer.
:KEY.I I
Starts inserting text before the first non-white space character in the
edit buffer.
:KEY.o o
Opens a line after the current line, and enters &tinsmode
:PERIOD.
:KEY.O O
Opens a line before the current line, and enters &tinsmode
:PERIOD.
:eKEYLIST.
Once you are in &tinsmode, you can toggle back and forth between
insert and overstrike using the
:HILITE.INS
key.  You exit &tinsmode by pressing the
:HILITE.ESC
key. See the previous section,
:HDREF refid='timode'.
:CONT.,
for more information on manipulating text in &tinsmode
:PERIOD.

:eCMSECT.

:CMSECT.Replacing Text

The following commands are used to replace text:
:KEYLIST.

:INCLUDE file='gkey'.

:KEY.R R
Starts overstriking text at the current character in the edit buffer.
Once you are overstriking text, you can toggle back and forth between
overstrike and insert using the
:HILITE.INS
key.   You exit &tinsmode by pressing the
:HILITE.ESC
key. See the previous section,
:HDREF refid='timode'.
:CONT.,
for more information on manipulating text in &tinsmode
:PERIOD.
:KEY.r <n>r<?>
Replaces the current character with the next character typed,
:ITALICS.<?>
:PERIOD.
If a repeat count is specified, then the next
:ITALICS.<n>
characters are replaced with the character
:ITALICS.<?>
:PERIOD.
:EXAMPLE.ra
Replaces the current character with the letter
:ITALICS.a
:PERIOD.
:EXAMPLE.10rZ
Replaces the next 10 characters with the letter
:ITALICS.Z
:PERIOD.
:eKEYLIST.

:eCMSECT.

:CMSECT id='deltext'.Deleting Text

The commands in this section are for deleting text in
an edit buffer.  All deleted text is copied into a &copybuffer
for later use, see the section
:HDREF refid='cpybuff'.
:PERIOD.
:KEYLIST.
:KEY.D <"?>D
Deletes the characters from the current position to the end of line.
:EXAMPLE."aD
Deletes characters from current position to the end of line into the named
buffer
:ITALICS.a
:PERIOD.
:KEY.X <n><"?>X
Delete the character before the current cursor position.
:EXAMPLE.X
Delete the previous character.
:EXAMPLE.10X
Delete the 10 previous characters.
:EXAMPLE."z5X
Delete the 5 previous characters into the named buffer
:ITALICS.z
:PERIOD.

:KEY.x <n><"?>x
Delete the character at the current cursor position.
:EXAMPLE.x
Delete the current character.
:EXAMPLE.3x
Delete the next 3 characters.
:EXAMPLE."217x
Delete the next 17 characters into the numbered buffer
:ITALICS.2
:PERIOD.

:KEY.DEL <n><"?>DEL
Delete the character at the current cursor position. This behaves
the same as the &cmdmode command
:HILITE.x
:PERIOD.
:EXAMPLE.DEL
Delete the current character.
:EXAMPLE.12DEL
Delete the next 12 characters.
:EXAMPLE."a5DEL
Delete the next 5 characters into the named buffer
:ITALICS.a
:PERIOD.
:eKEYLIST.

:P.
.se plural=0
:INCLUDE file='oper'.
:KEYLIST.
:KEY.d <n><"?>d<oper>
Delete text from the current position in the file to the
position specified by
:ITALICS.<oper>
:PERIOD.
A copy of the text is placed into the specified &copybuffer
:ITALICS.<"?>
:PERIOD.
If no buffer is specified, then the text is placed into the active
buffer.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be deleted.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.d
:CONT.,
which causes a single line to be deleted.
:SEECMD.delete
:EXAMPLE.dr
Deletes the current selected (highlighted) region in the edit buffer.
A copy is placed into the active &copybuffer
:PERIOD.
:EXAMPLE."zdd
Deletes the current line. A copy is placed into the named &copybuffer
:ITALICS.z
:PERIOD.
:EXAMPLE.95dd
Deletes 95 lines, starting at the current. A copy of the lines
is placed into the active buffer.
:EXAMPLE."cdfa
Deletes the characters from the current column up to and including
the first
:ITALICS.a
on the current line.  A copy of the text is placed
in the named buffer
:ITALICS.c
:PERIOD.
:EXAMPLE."5d'a
Deletes the lines from the current line to the line with mark
:ITALICS.m
into the numbered buffer
:ITALICS.5
:PERIOD.
:EXAMPLE.dG
Deletes all lines from the current line to the end of the current edit
buffer.
:eKEYLIST.

:eCMSECT.

:CMSECT id='cpytext'.Copying Text

This section describes commands that are for yanking (copying) text.
This text is placed into a &copybuffer, see the section
:HDREF refid='cpybuff'.
for more information.

:KEYLIST.
:KEY.Y <n>Y
Yank (copy) the current line.  If a repeat count
:ITALICS.<n>
is specified, then
:ITALICS.<n>
lines are copied.
:BLANKLINE.
This command is the same as typing
:HILITE.yy
:PERIOD.
:eKEYLIST.

.se plural = 0
:INCLUDE file='oper'.
:KEYLIST.
:KEY.y <n><"?>y<oper>
Yanks (copies) text from the current position in the file to the
position specified by
:ITALICS.<oper>
:PERIOD.
Text is placed into the specified &copybuffer
:ITALICS.<"?>
:PERIOD.
If no buffer is specified, then the text is placed into the active
buffer.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be copied.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.y
:CONT.,
which causes a single line to be yanked.
:SEECMD.delete
:EXAMPLE.yy
Yanks (copies) the current line into the active &copybuffer
:PERIOD.
:EXAMPLE.10yy
Copies 10 lines, starting at the current, into the active buffer.
:EXAMPLE.y$
Copies the characters from the current column to the end of the
current line into
the active buffer.
:EXAMPLE."ay'm
Yanks the lines from the current line to the line with mark
:ITALICS.m
into the named buffer
:ITALICS.a
:PERIOD.
:EXAMPLE.y/foo
Copies:
:UL compact.
:LI.the part of the current line from the current position to the
end of the line
:LI.all lines between the current line and the first line
containing the string
:ITALICS.foo
:LI the part of the line containing
:ITALICS.foo
from the start of the line to the first letter in the string
:ITALICS.foo
:eUL.

:eKEYLIST.

:eCMSECT.

:CMSECT id='cmchg'.Changing Text

The following commands are for changing text.
If a range of lines is being changed, the lines are deleted and &edname
enters &tinsmode
:PERIOD.
:BLANKLINE.
If the change is taking place on the single line, the range of
characters being changed is highlighted, and the last character in the
range is indicated with a dollar sign ('$').
If the
:ITALICS.ESC
key is pressed, and
:KEYWORD.changelikevi
is not set, then the change command is cancelled. If
:KEYWORD.changelikevi
is set, then the highlighted area is deleted.  If anything other than the
:ITALICS.ESC
key is pressed, the highlighted area is deleted and &edname enters &tinsmode
:PERIOD.

:KEYLIST.
:KEY.C C
This command changes the characters on the current line from the current
character to the end of the line.  The character range is highlighted, and
once a character is typed, the highlighted text is
deleted, and &tinsmode
is entered.
:BLANKLINE.
This command is the same as typing
:HILITE.c$
:PERIOD.

:KEY.S <n>S
This command substitutes the current line with text.  The text on the current
line is deleted, and &tinsmode is entered.  If a repeat count
:ITALICS.<n>
is specified, then
:ITALICS.<n>
lines are deleted.
:BLANKLINE.
This command is the same as typing
:HILITE.cc
:PERIOD.

:KEY.s <n>s
This command substitutes the current character with text.  If
:ITALICS.<n>
is specified, then
:ITALICS.<n>
characters are substituted.
:BLANKLINE.
This command is the same as typing
:HILITE.cl
:CONT.,
:HILITE.cRIGHT
:CONT.,
or
:HILITE.cSPACE
:PERIOD.
:eKEYLIST.

.se plural = 0
:INCLUDE file='oper'
:KEYLIST.
:KEY.c <n>c<oper>
Change text from the current position in the file to the
position specified by
:ITALICS.<oper>
:PERIOD.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be changed.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.c
:CONT.,
which causes a single line to be changed.
:EXAMPLE.cr
Changes the current selected (highlighted) region in the edit buffer.
:EXAMPLE.cc
Change the current line. The current line is deleted, and &tinsmode
is entered.
:EXAMPLE.95cc
Changes 95 lines, starting at the current. The lines are deleted,
and &tinsmode is entered.
:EXAMPLE.cw
Change the current word.  The current word is highlighted, and once
a character other than
:ITALICS.ESC
is typed, the word is deleted and &tinsmode is entered.
:EXAMPLE.c$
Changes from the current column to the end of the
current line. 
The column range is highlighted, and once a character other than
:ITALICS.ESC
is typed, the column range is deleted and &tinsmode is entered.
:EXAMPLE.2cfa
Changes from the current column to the second letter a on the current line.
The column range is highlighted, and once a character other than
:ITALICS.ESC
is typed, the column range is deleted and &tinsmode is entered.
:eKEYLIST.

:eCMSECT.

:CMSECT.Shifting Text

The following commands are used to shift lines to the right or left,
inserting or deleting leading whitespace. 
.se plural = 1
:INCLUDE file='oper'.

:KEYLIST.
:KEY.> <n>><oper> (right angle bracket)
This is the shift right command. It shifts the specified lines to the right
:KEYWORD.shiftwidth
spaces, inserting necessary leading tabs if
:KEYWORD.realtabs
is specified.
:BLANKLINE.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be shifted to the right.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.>
:CONT.,
which causes a single line to be shifted to the right.
:SEECMD.>
:EXAMPLE.10>>
Shifts the current line and the next 9 lines to the right
:KEYWORD.shiftwidth
spaces.
:EXAMPLE.>'a
Shifts all lines from the current line to the line with mark
:ITALICS.a
to the right
:KEYWORD.shiftwidth
spaces.
:EXAMPLE.>r
Shifts all lines in the selected (highlighted) region to the right
:KEYWORD.shiftwidth
spaces.

:KEY.< <n><<oper> (left angle bracket)
This is the shift left command. It shifts the specified lines to the left
:KEYWORD.shiftwidth
spaces.
:BLANKLINE.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be shifted to the left
:KEYWORD.shiftwidth
spaces.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.<
:CONT.,
which causes a single line to be shifted to the left
:KEYWORD.shiftwidth
spaces.
:SEECMD.<
:EXAMPLE.<<
Shifts the current line to the left
:KEYWORD.shiftwidth
spaces.
:EXAMPLE.<G
Shifts all lines from the current line to the last line in the
edit buffer to the left
:KEYWORD.shiftwidth
spaces.
:EXAMPLE.10<j
Shifts the current line and the next 10 lines to the left
:KEYWORD.shiftwidth
spaces.
:eKEYLIST.
:eCMSECT.


:CMSECT.Case Toggling

The case toggle &cmdmode command switches upper case letters to
lower case, and lower case letters to upper case.
 An example of its behaviour is changing the
line
:ILLUST.
This Is A Line Of Text.
:eILLUST.
to
:ILLUST.
tHIS iS a lINE oF tEXT.
:eILLUST.
.se plural = 0
:INCLUDE file='oper'.

:KEYLIST.
:KEY.~~ <n>~~<oper>  (tilde)
This is the case toggle command.
This command only works if the
:KEYWORD.togglecaselikevi
setting is not turned on.  If
:KEYWORD.togglecaselikevi
is set, then pressing
:HILITE.~~
only changes the case of the current character, and advances the
cursor to the next character.
:BLANKLINE.
However, if
:KEYWORD.togglecaselikevi
is not set, then this command toggles the case of the characters
over the range specified by
:ITALICS.<oper>
:BLANKLINE.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be case toggled.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.~~
:CONT.,
which causes a single line to be have its case toggled.
:EXAMPLE.~~r
Toggles the case of the currently selected (highlighted) range.
:EXAMPLE.~~$
Toggles the case of all characters from the current column of the
current line to the last character of the current line.
:EXAMPLE.~~w
Toggles the case of the current word.
:EXAMPLE.10~~~~
Toggles the case of the current line and the 9 lines following.
:eKEYLIST.

:eCMSECT.

:CMSECT.Filters

The &cmdmode filter command has the same functionality as the
&cmdline command
:KEYWORD.filter
:PERIOD.
.se plural = 0
:INCLUDE file='oper'.
:KEYLIST.
:KEY.! <n>!<oper> (exclamation mark)
Filter text. The line range specified by
:ITALICS.<oper>
are run through a specified filter.
A repeat count
:ITALICS.<n>
may precede the command, this causes
:ITALICS.<n>
units of the
:ITALICS.<oper>
command to be run through the filter.
:BLANKLINE.
:ITALICS.<oper>
may be specified as
:HILITE.!
:CONT.,
which causes a single line to be run through the filter.
:BLANKLINE.
The lines specified are run through a specified
system command (filter).  Once the filter range has been specified, a window
is displayed, prompting for a system command:
:picture file='filter' text='Filter System Command Prompt'.
The system command must take input from standard in and put
its output to standard out. The lines specified are replaced with the output
of the the command.
:BLANKLINE.
There is a filter command history in the filter command prompt, the
size of which is controlled with the
:KEYWORD.maxfilterhistory
setting.
:SEECMD.!
:EXAMPLE.!r
Runs the current selected (highlighted) region in the edit buffer
through a specified filter.
:EXAMPLE.!!
Runs the current line through a specified filter
:EXAMPLE.!G
Runs all lines from the current line to the end of the edit buffer
through a specified filter.
:eKEYLIST.

:eCMSECT.

:CMSECT.Text Selection
Text may be selected with the mouse. However, if you do not wish
to use the mouse, there is a keyboard interface to allow you to
accomplish the same thing.
:P.
Once text selection has been started, any movement command adds to
the selected region.   The selected region may be cleared, and
text selection ended, by
pressing the
:ITALICS.ESC
key.
:BLANKLINE.
A selected region is highlighted by exchanging the foreground and
background the colors of the line.  A selected region could look
as follows:
:picture file='selrgn' text='Selected Text Region'.
:P.
The following are the &cmdmode commands for selecting text and
manipulating selected text.

:KEYLIST.
:KEY.CTRL_R CTRL_R
Starts text selection, if no text is selected.  The current character is highlighted.
:ADDLINE.
If region is already selected, then cancel the selected region.

:KEY.SHIFT_UP <n>SHIFT_UP
Starts selection (if not already started) and moves up to the previous
line.  The previous line and the current line are selected.
If a repeat count
:ITALICS.<n>
is specified, then the cursor moves up
:ITALICS.<n>
lines, and all the lines between the starting and ending
position are selected.

:KEY.SHIFT_DOWN <n>SHIFT_DOWN
Starts selection (if not already started) and moves down to the next line.
The next line and the current line are selected.
If a repeat count
:ITALICS.<n>
is specified, then the cursor moves down
:ITALICS.<n>
lines, and all the lines between the starting and ending
position are selected.

:KEY.SHIFT_LEFT <n>SHIFT_LEFT
Starts selection (if not already started) and move left to the previous
character.  The current character and the previous character are selected.
If a repeat count
:ITALICS.<n>
is specified, then the cursor moves left
:ITALICS.<n>
characters, and all the characters between the starting and ending
position are selected.

:KEY.SHIFT_RIGHT <n>SHIFT_RIGHT
Starts selection (if not already started) and move right to the next
character.  The current character and the next character are selected.
If a repeat count
:ITALICS.<n>
is specified, then the cursor moves right
:ITALICS.<n>
characters, and all the characters between the starting and ending
position are selected.

:KEY.SHIFT_DEL <"?>SHIFT_DEL
Deletes the currently selected region.  If the &copybuffer
:ITALICS.<"?>
is specified, the region is copied into that buffer,
otherwise the data is copied into the active buffer.

:KEY._ _ (underscore)
Simulates the right mouse being clicked at the current cursor position.
If a region is not selected, then the current word will be selected.
The word is defined using the &cmdline command
:KEYWORD.word
:PERIOD.
:eKEYLIST.

:eCMSECT.

:CMSECT.Miscellaneous Keys

:KEYLIST.
:KEY.CTRL_C CTRL_C
Exits the editor if no files have been modified.  If files have been modified,
a prompt is displayed asking you to verify that you really want to discard
the modified file(s).  
If you do not respond with a 'y', then the command is cancelled.
:SEECMD.quitall

:KEY.CTRL_G CTRL_G
Display information about the current file in the edit window.  The
information includes:
:UL compact.
:LI.the file name.
:LI.a special indicator if the file is read-only.
:LI.a special indicator if the file is view-only.
:LI.a special indicator if the file has been modified.
:LI.the current line number, and the last line number.
:LI.The percentage of the way through the file.
:eUL.
Some sample results are:
:ILLUST.
"test.c" [modified] line 5 of 100  -- 5% --
:BLANKLINE.
"..\c\file.c" [read only] line 100 of 100  -- 100% --
:BLANKLINE.
"\autoexec.bat" line 1 of 100  -- 1% --
:eILLUST.

:KEY.CTRL_L CTRL_L
Redraws the current screen.

:KEY.CTRL_V CTRL_V
Displays the current version of &edname in the message window.
:SEECMD.version

:KEY.CTRL_X CTRL_X
Displays the hex value (and the decimal value) of the current
character.  A sample of the output in the message window is:
:ILLUST.
Char 'e': 0x65 (101)
:eILLUST.

:KEY.CTRL_] CTRL_]  (control right square bracket)
Go to the tag given by the current word.  The word is defined using the
&cmdline command
:KEYWORD.word
:PERIOD.
:SEECMD.tag
:BLANKLINE.
See the appendix
:HDREF refid='ctags'.
for more information.

:KEY.ALT_W ALT_W
Activates the current edit window's menu.  This menu is defined
using the
:ITALICS.windowgadgetmenu
:KEYWORD.menu
:PERIOD.
See the chapter
:HDREF refid='winmenu'.
for more information on setting this menu.

:KEY.ALT_X ALT_X
Insert a character, at the current cursor position.  When
:ITALICS.ALT_X
is pressed, a prompt is displayed:
:picture file='altx' text='Character Insertion Prompt'.
Enter either a decimal or a hex number.  That character will
be inserted directly into the edit buffer.

:KEY.: : (colon)
Allows entry of a &cmdline
:PERIOD.
See the chapter
:HDREF refid='cmds'
for full details on &cmdline commands.

:KEY.@ @<?> (at sign)
This command executes the &copybuffer
:ITALICS.<?>
:PERIOD.
&edname behaves as if the contents
of the buffer were being typed at the keyboard.


:KEY.J <n>J
Joins the next line to the current line.  If a repeat count
:ITALICS.<n>
is specified, the next
:ITALICS.<n>
lines are joined to the current line.
(lines are concatenated one after another).
:SEECMD.join
:EXAMPLE.J
Joins the next line to the current line.
:EXAMPLE.3J
Joins the next 3 lines to the current line.

:KEY.Q Q
Enters
:KEYWORD pc=1.EX mode
:PERIOD.
:KEYWORD pc=1.EX mode
is a line-oriented mode of
&edname.
:PERIOD.
To exit
:KEYWORD pc=1.EX mode
:CONT.,
use the
:KEYWORD.visual
command.

:KEY.Z Z<Z>
Used when you are finished with the current edit buffer.
If the current edit buffer has been modified, it is saved.
:EXAMPLE.ZZ
Finished with current edit buffer.

:KEY.z <n>z<?>
Reorients the current screen position.  The current line moves
as follows, depending on the value of
:ITALICS.<?>
:CONT.:
:DEFLIST.
:DEFITEM.ENTER
Moves the current line to the top of the screen.
:DEFITEM.. (dot)
Moves the current line to the center of the screen.
:DEFITEM.- (dash)
Moves the current line to the bottom of the screen.
:eDEFLIST.
If a repeat count
:ITALICS.<n>
is specified, then
:ITALICS.<n>
is made the current line.
:EXAMPLE.z-
Move the current line to the bottom of the screen.
:EXAMPLE.100zENTER
Makes line 100 the current line, and puts line 100 at the
top of the screen.
:EXAMPLE.25z.
Makes line 25 the current line, and puts line 25 at the
center of the screen.

:KEY.F1 F1
Move forward through the file list to the next file.
:SEECMD.next

:KEY.F2 F2
Move backwards through the file list to the previous file.
:SEECMD.prev

:KEY.F11 F11
Push the current file and position.  If you press
:HILITE.F12
:CONT.,
you will be restored to this position.  These positions are stacked
up, up to a maximum of
:KEYWORD.maxpush
:PERIOD.
:SEECMD.push

:KEY.F12 F12
Restore the last pushed file and position.
:SEECMD.pop

:KEY.. . (dot)
Repeat the last &cmdmode command that changed text in
the edit buffer. It is also possible to memorize more than
just one command for '.' by using
:KEYWORD.memorize mode
:CONT.:
:OL.
:LI.Type "
:HILITE.m.
:CONT.".
(&edname enters memorize mode).
:LI.Enter keystrokes.
:LI.Type "
:HILITE..
:CONT."
:eOL.
Now, whenever you press dot ('.'),
all the entered keystrokes will be executed.

:KEY.= = (equals sign)
Performs the last alternate memorized command sequence.  The
:KEYWORD.alternate memorize mode
is used as follows:
:OL.
:LI.Type "
:HILITE.m=
:CONT.".
(&edname enters
:KEYWORD.alternate memorize mode
:CONT.).
:LI.Enter keystrokes.
:LI.Type "
:HILITE.=
:CONT."
:eOL.
Now, whenever you press the equals sign ('='),
all the entered keystrokes will be executed as if you typed them
again from the keyboard.
:BLANKLINE.
This memorized keystroke sequence will last until you memorize
another, unlike using "
:HILITE.m.
:CONT.".

:KEY.ALT_M ALT_M
Display current memory state.  Shows the total amount of memory, the
amount of memory for use by &edname, and how much extended memory and/or
disk space is available.

:eKEYLIST.

:eCMSECT.
