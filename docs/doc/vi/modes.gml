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
.* 01-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=modes 'Modes'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
The &edname is a modal editor.  When you are in &cmdmode
(the default mode), there are a number of valid keys that may be pressed.
To ensure that you are in &cmdmode,
press the
.param ESC
key until the mode indicator
on the menu bar displays:
.millust begin
Mode: command
.millust end
.np
When in &tinsmode, text may be entered.  There are two aspects to
&tinsmode.:
.keyword insert
and
.keyword overstrike
:period.
.keyword Text insertion mode
is entered via a number of different commands from &cmdmode,
and is indicated by a
larger cursor, along with a mode line indication.  The
types of cursor are controlled with the
.keyref commandcursortype
:cont.,
.keyref insertcursortype
:cont.,
and
.keyref overstrikecursortype
settings.
.np
When &edvi is in a &tinsmode.,
the mode indicator on the menu bar displays one of:
.millust begin
Mode: insert
.millust end
or
.millust begin
Mode: overstrike
.millust end
.* ******************************************************************
.section *refid=timode 'Text Insertion Mode'
.* ******************************************************************
.np
When in &tinsmode (either inserting or overstriking), you
may enter text and freely cursor about through the file.  When you
are finished adding text, the
.param ESC
key returns you to &cmdmode
:period.
.np
It should be remembered that an undo applies to
changes caused by commands; so all changes made while in &tinsmode
are part of a single undo record. For more information on
undos, see the section
:HDREF refid='cmundo'.
later on in this chapter.
.np
The following keys, when pressed in &cmdmode, place you into &tinsmode:
.*
:DL break.
.*
:DT.a
:DD.Starts appending (inserting) text after the current character in
the edit buffer.

:DT.A
:DD.Starts appending (inserting) text after the last character on the
current line in the edit buffer.

:DT.C
:DD.Changes text from the current position to the end of the current line.
Deletes the text, and enters &tinsmode
:period.

:DT.&lt.n&gt.c&lt.oper&gt.
:DD.Change command.  Deletes the text in the range specified by
.param &lt.oper&gt.
:cont., and enters &tinsmode
:period.

:DT.g
:DD.Starts inserting or overstriking text at the current cursor position,
depending on how you were adding text the last time you were in &tinsmode
:period.

:DT.i
:DD.Starts inserting text at the current cursor position.

:DT.I
:DD.Starts inserting text before the first non-white space character on
the current line.

:DT.o
:DD.Opens a line after the current line, and enters &tinsmode
:period.

:DT.O
:DD.Opens a line before the current line, and enters &tinsmode
:period.

:DT.R
:DD.Starts overstriking text at the current character in the edit buffer.

:DT.&lt.n&gt.s
:DD.Substitute
.param &lt.n&gt.
characters.  The first
.param &lt.n&gt.
characters from the current cursor position are deleted, and &tinsmode
is entered.

:DT.&lt.n&gt.S
:DD.Substitute lines of text.
.param &lt.n&gt.
lines from the current line forward are deleted, and &tinsmode is entered.

:DT.INS
:DD.Start inserting text at the current cursor position.
.*
:eDL.
.*
.beglevel
.* ******************************************************************
.section 'Special Keys'
.* ******************************************************************
.np
While in &tinsmode, certain keys do special things.  These keys are:
.*
:DL break.
.*
:DT.Arrow Keys
:DD.:DL break.
:DT.Up
:DD.Cursor up through the text.
:DT.DOWN
:DD.Cursor down through the text.
:DT.LEFT
:DD.Cursor left through the text.
:DT.RIGHT
:DD.Cursor right through the text.
:eDL.

:DT.Text Selection Keys
:DD.:INCLUDE file='textsel'.

:DT.CTRL_PAGEUP
:DD.Moves to the first non-white space character on the first line
of the current edit buffer.

:DT.CTRL_PAGEDOWN
:DD.Moves to the last character on the last line
of the current edit buffer.

:DT.SHIFT_DEL
:DD.Deletes the currently selected region into the active &copybuffer
:period.

:DT.SHIFT_INS
:DD.Pastes the active &copybuffer
into the text after the current position.

:DT.SHIFT_TAB
:DD.Move back to the previous tab stop, deleting the characters before the
cursor.

:DT.CTRL_DEL (ctrl-delete)
:DD.Delete the current line into the active &copybuffer
:period.

:DT.CTRL_INS (ctrl-insert)
:DD.Pastes the active &copybuffer into the text before the current position.

:DT.CTRL_D
:DD.Move backwards
.keyref shiftwidth
spaces, deleting the characters before the cursor. A
.keyref shiftwidth
is a number that you may set, its default value is 4.

:DT.CTRL_T
:DD.Insert
.keyref shiftwidth
spaces.  If
.keyref realtabs 1
is set, then once
.keyref tabamount
spaces are inserted, the spaces are replaced with a tab character.

:DT.CTRL_V
:DD.The next key typed is inserted directly, without any interpretation.

:DT.CTRL_Q
:DD.The next key typed is inserted directly, without any interpretation.

:DT.BS
:DD.Backspace one on the current line, deleting the character before the
cursor.

:DT.DEL
:DD.Delete the character under the cursor.  If you are at the end of the line,
.keyword DEL
has the same effect as pressing
.keyword BS
:period.

:DT.ENTER
:DD.Start a new line.

:DT.END
:DD.Move to the end of the current line.

:DT.HOME
:DD.Move to the start of the current line.

:DT.INS
:DD.Toggles between insert and overstrike mode.

:DT.PAGEUP
:DD.Move up one page in the text.

:DT.PAGEDOWN
:DD.Move up down one page in the text.

:DT.TAB
:DD.Move forward to the next tab stop.  If
.keyref realtabs 1
is set, a tab character is inserted into the file. Otherwise, spaces
are inserted.
.*
:eDL.
.*
.endlevel
.* ******************************************************************
.section 'Command Mode'
.* ******************************************************************
.np
The following &cmdmode command descriptions show items within
angle brackets (&lt.&gt.).
The angle brackets are there to indicate items that you may supply.
You are not required to type the brackets.  For example, &lt.n&gt. simply
means that in the corresponding place in the command you can enter
a number.
.np
Many commands may be preceded with a repeat count, which is
indicated by a
.param &lt.n&gt.
before a command.
The number is not required; if it is not supplied, it is usually assumed
that a 1 was entered for the repeat count. As long as the setting
.keyref repeatinfo
is enabled, the number that is typed appears in a special window called the
.keyref countwindow
:period.
.np
Other commands may be preceded with a &copybuffer
name, which is indicated with a
.param &lt."?&gt.
:period.
If you do not want the result of the operation to be copied into the
active buffer, then an alternate buffer may be specified.
The double quotes (")
are required (this indicates that an alternate buffer is being specified),
and then a buffer '1'-'9' or 'a'-'z' is specified. See the section
:HDREF refid='cpybuff'.
for more information.
.*
.beglevel
.* ******************************************************************
.section *refid=mvement 'Movement'
.* ******************************************************************
.np
The following are &cmdmode commands that cause movement in the
current edit buffer.
.*
.keylist begin Movement
.*
.begkey "|" "&lt.n&gt.|" '(or bar)'
Move to the column number specified by
.param &lt.n&gt.
:period.
.np
.xmplsect begin
.begxmpl |
Move to column 1 of the current line.
.endxmpl
.begxmpl 15|
Move to column 15 of the current line.
.endxmpl
.xmplsect end
.endkey

.begkey ` `&lt.?&gt. (back quote)
Moves to the mark position (line and column) specified by
.param &lt.?&gt.
See the section
:HDREF refid='marks'.
for more information.
.seecmd mark
.np
.xmplsect begin
.begxmpl `a
Move to the line and column with mark
.param a
:period.
.endxmpl
.xmplsect end
.endkey

.begkey "'" "'&lt.?&gt." (front quote)
Move to the start of line with the mark
.param &lt.?&gt.
:period.
See the section
:HDREF refid='marks'.
for more information.
.seecmd mark
.np
.xmplsect begin
.begxmpl 'z
Move to the start of the line with mark
.param z
:period.
.endxmpl
.xmplsect end
.endkey

.begkey '%' '%' (percent)
Moves to matching brace or other defined match string.
Defaults are "{","}"
and "(",")". For example, by pressing
.param %
while on the first opening brace ('(') on the line:
.millust begin
if( ( i=foo( x ) ) ) return;
.millust end
moves the cursor to the last closing brace (')') on the line.
It is possible to set arbitrary pairs of match strings using the
&cmdline command
.keyref match
:period.
.endkey

.begkey '$' '$' (dollar)
Moves the cursor to the last character on the current line.
.endkey

.begkey '&caret.' '&caret.' (caret)
Moves the cursor to the first non-whitespace character on the current line.
.endkey

.dc cw OFF
.begkey ; ; (semi-colon)
.dc cw ;
Repeats the last
.keyword f
:cont.,
.keyword F
:cont.,
.keyword t
or
.keyword T
movement commands.
.endkey

.begkey ',' ',' (comma)
Repeats the last
.keyword f
:cont.,
.keyword F
:cont.,
.keyword t
or
.keyword T
movement commands, but the search is done in the opposite direction.
.np
If the last movement command was an
.keyword F
then an
.keyword f
movement command is executed.
If the last movement command was a
.keyword t
then a
.keyword T
movement command is executed.
.np
Similarly, if the last movement
command was an
.keyword f
then an
.keyword F
movement command is executed.
If the last movement command was a
.keyword t
then a
.keyword T
movement command is executed.
.endkey

.begkey - &lt.n&gt.- (dash)
Moves the cursor to the start of the previous line.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved up
.param &lt.n&gt.
lines.
.endkey

.begkey + &lt.n&gt.+ (plus)
Moves the cursor to the start of the next line.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved down
.param &lt.n&gt.
lines.
.endkey

.begkey '0' '0'
Moves the cursor the first character of the current line.
.endkey

.begkey CTRL_PAGEUP CTRL_PAGEUP
Moves to the first non-white space character on the first line
of the current edit buffer.
.endkey

.begkey CTRL_PAGEDOWN CTRL_PAGEDOWN
Moves to the last character on the last line
of the current edit buffer.
.endkey

.begkey DOWN &lt.n&gt.DOWN
Move the cursor down one line.
.param &lt.n&gt.
is specified, the cursor moves down
.param &lt.n&gt.
lines.
.endkey

.begkey END END
Moves the cursor to the last character on the current line.
.endkey

.begkey ENTER &lt.n&gt.ENTER
Moves the cursor to the start of the next line.  If a repeat count
.param &lt.n&gt.
is specified, then the cursor is moved down
.param &lt.n&gt.
lines.
.endkey

.begkey HOME HOME
Moves the cursor the first character of the current line.
.endkey

.begkey LEFT &lt.n&gt.LEFT
Move the cursor left one character.  If
.param &lt.n&gt.
is specified, the cursor moves left
.param &lt.n&gt.
characters.
.endkey

.begkey PAGEDOWN &lt.n&gt.PAGEDOWN
Moves forwards one page.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved ahead
.param &lt.n&gt.
pages.
The number of lines of context maintained
is controlled by the
.keyref pagelinesexposed
setting.
.endkey

.begkey PAGEUP &lt.n&gt.PAGEUP
Moves backwards one page.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved back
.param &lt.n&gt.
pages.
The number of lines of context maintained
is controlled by the
.keyref pagelinesexposed
setting.
.endkey

.begkey RIGHT &lt.n&gt.RIGHT
Move the cursor right one character.  If
.param &lt.n&gt.
is specified, the cursor moves right
.param &lt.n&gt.
characters.
.endkey

.begkey SHIFT_TAB &lt.n&gt.SHIFT_TAB
Moves the cursor left by
.keyref tabamount
characters. A repeat count
.param &lt.n&gt.
multiplies this.
.endkey

.begkey TAB &lt.n&gt.TAB
Moves the cursor right by
.keyref tabamount
characters. A repeat count
.param &lt.n&gt.
multiplies this.
.endkey

.begkey UP &lt.n&gt.UP
Move the cursor up one line.
.param &lt.n&gt.
is specified, the cursor moves up
.param &lt.n&gt.
lines.
.endkey

.begkey CTRL_B &lt.n&gt.CTRL_B
Moves backwards one page.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved back
.param &lt.n&gt.
pages.
The number of lines of context maintained
is controlled by the
.keyref pagelinesexposed
setting.
.endkey

.begkey CTRL_D &lt.n&gt.CTRL_D
Move down a certain number of lines.  The default is to move down
half a page.  If the repeat count
.param &lt.n&gt.
is specified, then that becomes the number of lines moved from then on.
Also see the
.param CTRL_U
key.
.endkey

.begkey CTRL_E &lt.n&gt.CTRL_E
Expose the line below the last line in the current edit window,
leaving the cursor on the same line if possible.  If a repeat
count
.param &lt.n&gt.
is specified, then that many lines are exposed.
.endkey

.begkey CTRL_F &lt.n&gt.CTRL_F
Moves forwards one page.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved ahead
.param &lt.n&gt.
pages.
The number of lines of context maintained
is controlled by the
.keyref pagelinesexposed
setting.
.endkey

.begkey CTRL_N &lt.n&gt.CTRL_N
Move the cursor to the next line.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved down
.param &lt.n&gt.
lines.
.endkey

.begkey CTRL_P &lt.n&gt.CTRL_P
Move the cursor to the previous line.  If a repeat count
.param &lt.n&gt.
is specified, then you are moved up
.param &lt.n&gt.
lines.
.endkey

.begkey CTRL_U &lt.n&gt.CTRL_U
Move up a certain number of lines.  The default is to move up
half a page.  If the repeat count
.param &lt.n&gt.
is specified, then that becomes the number of lines moved from then on.
Also see the
.keyword CTRL_D
key.
.endkey

.begkey CTRL_Y &lt.n&gt.CTRL_Y
Expose the line above the first line in the current edit window,
leaving the cursor on the same line if possible.  If a repeat
count
.param &lt.n&gt.
is specified, then that many lines are exposed.
.endkey

.begkey B &lt.n&gt.B
Moves the cursor backwards to the start of previous space delimited word
on the current line.
.np
.xmplsect begin
.begxmpl B
If the cursor was on the right parenthesis (')') of
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the
.param f
in
.param foo
:period.
.endxmpl
.begxmpl 2B
If the cursor was on the right parenthesis (')') of
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the
.param =
sign.
.endxmpl
.xmplsect end
.endkey

.begkey b &lt.n&gt.b
Moves the cursor backwards to the start of the previous word on the
current line.
:INCLUDE file='worddef'.
.np
.xmplsect begin
.begxmpl b
If the cursor was on the right parenthesis (')') of
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the letter
.param a
in
.param abc
:period.
.endxmpl
.begxmpl 2b
If the cursor was on the right parenthesis (')') of
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to left parenthesis 
.param (
:period.
.endxmpl
.xmplsect end
.endkey

.begkey E &lt.n&gt.E
Moves the cursor to the end of the next space delimited word on the
current line.
.np
.xmplsect begin
.begxmpl E
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the right parenthesis
.param )
:period.
.endxmpl
.begxmpl 2E
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the 
.param +
sign.
.endxmpl
.xmplsect end
.endkey

.begkey e &lt.n&gt.e
Moves the cursor to the end of the next word on the current line.
:INCLUDE file='worddef'.
.np
.xmplsect begin
.begxmpl e
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the second letter
.param o
in
.param foo
:period.
.endxmpl
.begxmpl 2e
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the left parenthesis
.param (
:period.
.endxmpl
.xmplsect end
.endkey

.begkey F &lt.n&gt.F&lt.?&gt.
Moves the cursor backwards from its current position to the character
.param &lt.?&gt.
on the current line.  If a repeat count
.param &lt.n&gt.
is specified, then the nth occurrence of the character
.param &lt.?&gt.
is moved to.
.np
.xmplsect begin
.begxmpl F+
If the cursor is on the semi-colon (';') in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the
.param +
sign.
.endxmpl
.begxmpl 2Fo
If the cursor is on the semi-colon (';') in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the first
.param o
in
.param foo
:period.
.endxmpl
.xmplsect end
.endkey

.begkey f &lt.n&gt.f&lt.?&gt.
Moves the cursor forwards from its current position to the character
.param &lt.?&gt.
on the current line.  If a repeat count
.param &lt.n&gt.
is specified, then the
.param nth
occurrence of the character
.param &lt.?&gt.
is moved to.
.np
.xmplsect begin
.begxmpl f+
If the cursor is on the character
.param x
in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the
.param +
sign.
.endxmpl
.begxmpl 2fo
If the cursor is on the character
.param x
in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the second
.param o
in
.param foo
:period.
.endxmpl
.xmplsect end
.endkey

.begkey G &lt.n&gt.G
Goes to the line specified by the repeat count
.param &lt.n&gt.
:period.
If no repeat count is specified, you move the the last line in the
current edit buffer.
.np
.xmplsect begin
.begxmpl 100G
Moves to line 100 in the current edit buffer.
.endxmpl
.begxmpl G
Moves to the last line in the current edit buffer.
.endxmpl
.xmplsect end
.endkey

.begkey h &lt.n&gt.h
Move the cursor left one character.  If
.param &lt.n&gt.
is specified, the cursor moves left
.param &lt.n&gt.
characters.
.endkey

.begkey H &lt.n&gt.H
Moves to the line at the top of the current file window.  If
a repeat count is specified, then you are moved to that line relative
to the top of the current file window.
.np
.xmplsect begin
.begxmpl 2H
Moves to the second line from the top of the current file window.
.endxmpl
.begxmpl H
Moves to the line at the top of the current file window.
.endxmpl
.xmplsect end
.endkey

.begkey j &lt.n&gt.j
Move the cursor down one line.
.param &lt.n&gt.
is specified, the cursor moves down
.param &lt.n&gt.
lines.
.endkey

.begkey k &lt.n&gt.k
Move the cursor up one line.
.param &lt.n&gt.
is specified, the cursor moves up
.param &lt.n&gt.
lines.
.endkey

.begkey L &lt.n&gt.L
Moves to the line at the bottom of the current file window.  If
a repeat count is specified, then you are moved to that line relative
from the bottom of the current file window.
.np
.xmplsect begin
.begxmpl 2L
Moves to the second line from the bottom of the current file window.
.endxmpl
.begxmpl L
Moves to the line at the bottom of the current file window.
.endxmpl
.xmplsect end
.endkey

.begkey l &lt.n&gt.l
Move the cursor right one character.  If
.param &lt.n&gt.
is specified, the cursor moves right
.param &lt.n&gt.
characters.
.endkey

.begkey M M
Moves the cursor to the line in the middle of the current file window.
.endkey

.begkey T &lt.n&gt.T&lt.?&gt.
Moves the cursor backwards from its current position to the character
after the character
.param &lt.?&gt.
on the current line.  If a repeat count
.param &lt.n&gt.
is specified, then the the character after the nth
occurrence of the character
.param &lt.?&gt.
is moved to.
.np
.xmplsect begin
.begxmpl T+
If the cursor is on the semi-colon (';') in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the space after the
.param +
sign.
.endxmpl
.begxmpl 2To
If the cursor is on the semi-colon (';') in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the second
.param o
in
.param foo
:period.
.endxmpl
.xmplsect end
.endkey

.begkey t &lt.n&gt.t&lt.?&gt.
Moves the cursor forwards from its current position to the character before
the character
.param &lt.?&gt.
on the current line.  If a repeat count
.param &lt.n&gt.
is specified, then the the character before the nth
occurrence of the character
.param &lt.?&gt.
is moved to.
.np
.xmplsect begin
.begxmpl t+
If the cursor is on the character
.param x
in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the space before
.param +
sign.
.endxmpl
.begxmpl 2to
If the cursor is on the character
.param x
in
.millust begin
x = foo(abc) + 3;
.millust end
The the cursor is moved to the first
.param o
in
.param foo
:period.
.endxmpl
.xmplsect end
.endkey

.begkey W &lt.n&gt.W
Moves the cursor forward to the start of the next space delimited word
on the current line.
.np
.xmplsect begin
.begxmpl W
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the 
.param +
sign.
.endxmpl
.begxmpl 2W
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the number
.param 3
:period.
.endxmpl
.xmplsect end
.endkey

.begkey w &lt.n&gt.w
Moves the cursor forward to the start of the next word on the current
line.
:INCLUDE file='worddef'.
.np
.xmplsect begin
.begxmpl w
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the left parenthesis
.param (
:period.
.endxmpl
.begxmpl 2w
If the cursor was on the letter
.param f
in
.millust begin
x = foo(abc) + 3;
.millust end
then the cursor moves to the letter
.param a
in
.param abc
:period.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section *refid=cmundo 'Undoing Changes'
.* ******************************************************************
.np
&edvi keeps an undo history of all changes made to an edit buffer.
There is no limit on the number of undos, as long as there is enough
memory to save the undo information.  If there is not enough memory
to save undo information for the current action, then the oldest undo
information is removed until enough memory has been released.
.np
There is also an undo-undo (redo) history:  as you issue undo commands,
the information to redo the undo is kept.  However, once you modify the
file other than by doing an undo, the redo history is lost.
.np
As you issue undo commands, a message indicating how many undos are
remaining.  The message could look like:

.millust begin
16 items left on undo stack
.millust end

This lets you know how many undos it would take to restore the
edit buffer to its original condition.  Once there are no more undos,
you will see the message:

.millust begin
undo stack is empty
.millust end

Once you undo all changes,
then the file changes state from modified to unmodified.  However,
if some undo changes have had to be discarded because of low memory,
the file will still be in a modified state.
.np
The keystrokes for doing undo and redo are:
.*
.keylist begin 'Undoing Changes'
.*
.begkey u u
Undo last change.
.endkey

.begkey U U
Redo last undo.
.endkey
.*
.keylist end
.*
.np
.seecmd 'undo (command)'
.* ******************************************************************
.section *refid=marks 'Marks'
.* ******************************************************************
.np
:INCLUDE file='markinfo'.
.np
Mark commands are:
.*
.keylist begin Marks
.*
.begkey m m&lt.?&gt.
Allows the setting of mark
.param &lt.?&gt.
:period.
.np
If
.param &lt.?&gt.
is an exclamation mark ('!')
instead of a letter, it clears all marks on the current line.
.np
If
.param &lt.?&gt.
is a dot ('.')
instead of a letter, it puts &edvi in
.keyword memorize mode
:period.
All characters typed are memorized until another dot ('.')
is pressed.  The memorized keystrokes may be repeated by pressing
a dot ('.').
See the dot ('.') &cmdmode command later in this chapter.
.np
If
.param &lt.?&gt.
is an equals sign ('=')
instead of a letter, it puts &edvi in alternate
.keyword memorize mode
:period.
All characters typed are memorized until another equals sign ('=')
is pressed.  The memorized keystrokes may be repeated by pressing
an equals sign ('=').
See the equals sign ('=') &cmdmode command later in this chapter.
.seecmd mark
.np
.xmplsect begin
.begxmpl ma
Sets the mark a at the current cursor position
.endxmpl
.begxmpl m.
Enter memorize mode
.endxmpl
.begxmpl m!
Clear any marks set on the current line.
.endxmpl
.xmplsect end
.endkey

.begkey "'" "'&lt.?&gt." (front quote)
Move to the start of the line with the mark
.param &lt.?&gt.
:period.
.np
.xmplsect begin
.begxmpl 'a
Moves to the first column of the line with mark
.param a
:period.
.endxmpl
.begxmpl ''
Moves to the first column of line of the last position before the last
non-linear movement command was issued.
.endxmpl
.xmplsect end
.endkey

.begkey '`' '`&lt.?&gt.' (back quote)
Move to the position in the edit buffer with the mark
.param &lt.?&gt.
:period.
.np
.xmplsect begin
.begxmpl `a
Moves to the column and line with mark
.param a
:period.
.endxmpl
.begxmpl ``
Moves to the last position before the last
non-linear movement command was issued.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.*
.np
Pressing
.param ``
and
.param ''
take you to the last position you were at before you used a
non-linear movement command (', `, ?, /, G, n, and N commands).
So, if you are at line 5 column 10 and type
.param /foo
:cont.,
pressing
.param ``
will first move
you back to line 5 column 10. Pressing
.param ``
again will move you to the occurrence of
foo, since the previous `` command was a non-linear movement command.
.* ******************************************************************
.section *refid=cpybuff 'Copy Buffers'
.* ******************************************************************
.np
A &copybuffer is a buffer where copied or deleted data is kept.
There are a number of these buffers available.  There are 9 default
buffers that text is placed into when it is deleted/yanked (see
the &cmdline commands
.keyref delete
and
.keyref yank
:cont.,
along with the sections
:HDREF refid='deltext'
and
:HDREF refid='cpytext'
later in this chapter).
These buffers are numbered 1 through 9, and any of these buffers may
be the active &copybuffer
:period.
.np
The active &copybuffer may be selected using function keys. 
CTRL_F1 through CTRL_F9 select buffers 1 through 9 respectively.
When a buffer is selected,
information about its contents is displayed in the message window.
This buffer becomes the active &copybuffer
:period.
All yanked/deleted text is copied into this buffer.
.np
When text is yanked/deleted into the active &copybuffer,
the contents of the the buffers are cascaded forward from the active
buffer into the next one, with the last numbered &copybuffer
losing its contents. Any buffers that are before the active
&copybuffer have their contents preserved.  For example, if buffer 3
is the active buffer, then a deletion will cascade buffer 3 to buffer 4,
buffer 4 to buffer 5, and so on, with the contents of buffer 9 being lost.
Buffers 1 and 2 remain untouched, and buffer 3 gets a copy of the deleted text.
.np
There are several &cmdmode commands that add text to buffers; they
are
.*
:DL break.
.*
:DT.&lt."?&gt.&lt.n&gt.d&lt.oper&gt.
:DD.Deletes text in various ways.

:DT.&lt.n&gt.DEL
:DD.Deletes the character at the current cursor position.

:DT.D
:DD.Deletes text from the current cursor position to the end of the current line.

:DT.&lt.n&gt.x
:DD.Deletes the character at the current cursor position.

:DT.&lt.n&gt.X
:DD.Deletes the character before the current cursor position.

:DT.&lt."?&gt.&lt.n&gt.y&lt.oper&gt.
:DD.Yanks (copies) text in various ways.

:DT.&lt.n&gt.Y
:DD.Yanks (copies) the current line.
.*
:eDL.
.*
There is more information on these &cmdmode
commands later in this chapter.
.np
Text may be yanked/deleted into a specific &copybuffer
by typing "[1-9] before
the appropriate command. As well, there are 26 named buffers that may
be used, 'a'-'z'.  When text is yanked or deleted into a named buffer,
it remains there for the life of the editing session (or until replaced).
.np
To retrieve the contents of a buffer, use:
.*
.keylist begin 'Copy Buffers'
.*
.begkey SHIFT_INS &lt."?&gt.SHIFT_INS
Puts (pastes) the contents of the active &copybuffer
after the cursor position in the current edit buffer. 
.seecmd put
.np
.xmplsect begin
.begxmpl "aSHIFT_INS
Copy the data in the named buffer
.param a
after the current position in the file.
.endxmpl
.begxmpl SHIFT_INS
Copy the data in the active buffer
after the current position in the file.
.endxmpl
.xmplsect end
.endkey

.begkey p &lt."?&gt.p
Puts (pastes) the contents of the active &copybuffer
after the cursor position in the current edit buffer.
.seecmd put
.np
.xmplsect begin
.begxmpl p
Copies the data in the active buffer after the current position in the file.
.endxmpl
.begxmpl "5p
Copies the data in the numbered buffer
.param 5
after the current position in the file.
.endxmpl
.xmplsect end
.endkey

.begkey P &lt."?&gt.P
Puts (pastes) the contents of the active &copybuffer
before the cursor position in the current edit buffer.
.seecmd put
.np
.xmplsect begin
.begxmpl "aP
Copy the data in the named buffer
.param a
before the current position in the file.
.endxmpl
.begxmpl P
Copy the data in the active buffer
before the current position in the file.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.*
.np
Without a
.param "?
prefix, these commands retrieve the contents of the active buffer.
.np
The contents of a &copybuffer
may be executed, as if the contents were typed from the
keyboard. See the
.keyword @
&cmdmode command later in this chapter.
.* ******************************************************************
.section *refid=srching 'Searching'
.* ******************************************************************
.np
The following &cmdmode commands are used for searching for text:
.*
.keylist begin Searching
.*
.begkey '/' '/' (forward slash)
Enter a regular expression to search for from current position forwards.
.endkey

.begkey ? ? (question mark)
Enter a regular expression to search for from current position backwards.
.endkey

.begkey n n
Repeat last search command, in the direction of the last search.
.endkey

.begkey N N
Repeat last search command, in the opposite direction of the last search.
.endkey
.*
.keylist end
.*
.np
For more information on regular expressions, see
:HDREF refid='rxchap'.
:period.
.np
Once you press the
.keyword /
or the
.keyword ?
keys, a search string entry window will appear:
.figure *depth='2.47' *scale='59' *file='vi022' Search String Entry Window
This position and size of the search string entry window are controlled
using
the
.keyref commandwindow
windowing command.
Search strings may be up to 512 bytes in length; the search string window
scrolls.
.np
The search string window has a history associated with it; the size
of this search string history is controlled using the
.keyref maxfindhistory
setting.  As well, the search string history is preserved across sessions of
&edvi if the
.keyref historyfile
parameter is set.
.np
If the first letter of a search string is a CTRL_A (entered by typing
a CTRL_V followed by a CTRL_A)
then that search string will not be added to the search string history.
.*
.beglevel
.* ******************************************************************
.section 'Special Keys In The Search String Window'
.* ******************************************************************
.np
Once in the search string window, a number of keys have special meaning:
.se searching=1
:INCLUDE file='speckeys'.
.*
.endlevel
.* ******************************************************************
.section 'Inserting Text'
.* ******************************************************************
.np
The following commands cause &edvi to go from &cmdmode
directly into &tinsmode:
.*
.keylist begin 'Inserting Text'
.*
.begkey INS INS
Starts inserting text before the current character in the edit buffer.
.endkey

.begkey a a
Starts appending (inserting) text after the current character in
the edit buffer.
.endkey

.begkey A A
Starts appending (inserting) text after the last character on the
current line in the edit buffer.
.endkey

:INCLUDE file='gkey'.

.begkey i i
Starts inserting text before the current character in the edit buffer.
.endkey

.begkey I I
Starts inserting text before the first non-white space character in the
edit buffer.
.endkey

.begkey o o
Opens a line after the current line, and enters &tinsmode
:period.
.endkey

.begkey O O
Opens a line before the current line, and enters &tinsmode
:period.
.endkey
.*
.keylist end
.*
Once you are in &tinsmode, you can toggle back and forth between
insert and overstrike using the
.keyword INS
key.  You exit &tinsmode by pressing the
.keyword ESC
key. See the previous section,
:HDREF refid='timode'.
:cont.,
for more information on manipulating text in &tinsmode
:period.
.* ******************************************************************
.section 'Replacing Text'
.* ******************************************************************
.np
The following commands are used to replace text:
.*
.keylist begin 'Replacing Text'
.*
:INCLUDE file='gkey'.

.begkey R R
Starts overstriking text at the current character in the edit buffer.
Once you are overstriking text, you can toggle back and forth between
overstrike and insert using the
.keyword INS
key.   You exit &tinsmode by pressing the
.keyword ESC
key. See the previous section,
:HDREF refid='timode'.
:cont.,
for more information on manipulating text in &tinsmode
:period.
.endkey

.begkey r &lt.n&gt.r&lt.?&gt.
Replaces the current character with the next character typed,
.param &lt.?&gt.
:period.
If a repeat count is specified, then the next
.param &lt.n&gt.
characters are replaced with the character
.param &lt.?&gt.
:period.
.np
.xmplsect begin
.begxmpl ra
Replaces the current character with the letter
.param a
:period.
.endxmpl
.begxmpl 10rZ
Replaces the next 10 characters with the letter
.param Z
:period.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section *refid=deltext 'Deleting Text'
.* ******************************************************************
.np
The commands in this section are for deleting text in
an edit buffer.  All deleted text is copied into a &copybuffer
for later use, see the section
:HDREF refid='cpybuff'.
:period.
.*
.keylist begin 'Deleting Text'
.*
.begkey D &lt."?&gt.D
Deletes the characters from the current position to the end of line.
.np
.xmplsect begin
.begxmpl "aD
Deletes characters from current position to the end of line into the named
buffer
.param a
:period.
.endxmpl
.xmplsect end
.endkey

.begkey X &lt.n&gt.&lt."?&gt.X
Delete the character before the current cursor position.
.np
.xmplsect begin
.begxmpl X
Delete the previous character.
.endxmpl
.begxmpl 10X
Delete the 10 previous characters.
.endxmpl
.begxmpl "z5X
Delete the 5 previous characters into the named buffer
.param z
:period.
.endxmpl
.xmplsect end
.endkey

.begkey x &lt.n&gt.&lt."?&gt.x
Delete the character at the current cursor position.
.np
.xmplsect begin
.begxmpl x
Delete the current character.
.endxmpl
.begxmpl 3x
Delete the next 3 characters.
.endxmpl
.begxmpl "217x
Delete the next 17 characters into the numbered buffer
.param 2
:period.
.endxmpl
.xmplsect end
.endkey

.begkey DEL &lt.n&gt.&lt."?&gt.DEL
Delete the character at the current cursor position. This behaves
the same as the &cmdmode command
.keyword x
:period.
.np
.xmplsect begin
.begxmpl DEL
Delete the current character.
.endxmpl
.begxmpl 12DEL
Delete the next 12 characters.
.endxmpl
.begxmpl "a5DEL
Delete the next 5 characters into the named buffer
.param a
:period.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.*
.np
.se plural=0
:INCLUDE file='oper'.
.*
.keylist begin 'Deleting Text'
.*
.begkey d &lt.n&gt.&lt."?&gt.d&lt.oper&gt.
Delete text from the current position in the file to the
position specified by
.param &lt.oper&gt.
:period.
A copy of the text is placed into the specified &copybuffer
.param &lt."?&gt.
:period.
If no buffer is specified, then the text is placed into the active
buffer.
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be deleted.
.np
.param &lt.oper&gt.
may be specified as
.param d
:cont.,
which causes a single line to be deleted.
.seecmd delete
.np
.xmplsect begin
.begxmpl dr
Deletes the current selected (highlighted) region in the edit buffer.
A copy is placed into the active &copybuffer
:period.
.endxmpl
.begxmpl "zdd
Deletes the current line. A copy is placed into the named &copybuffer
.param z
:period.
.endxmpl
.begxmpl 95dd
Deletes 95 lines, starting at the current. A copy of the lines
is placed into the active buffer.
.endxmpl
.begxmpl "cdfa
Deletes the characters from the current column up to and including
the first
.param a
on the current line.  A copy of the text is placed
in the named buffer
.param c
:period.
.endxmpl
.begxmpl "5d'a
Deletes the lines from the current line to the line with mark
.param m
into the numbered buffer
.param 5
:period.
.endxmpl
.begxmpl dG
Deletes all lines from the current line to the end of the current edit
buffer.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section *refid=cpytext 'Copying Text'
.* ******************************************************************
.np
This section describes commands that are for yanking (copying) text.
This text is placed into a &copybuffer, see the section
:HDREF refid='cpybuff'.
for more information.
.*
.keylist begin 'Copying Text'
.*
.begkey Y &lt.n&gt.Y
Yank (copy) the current line.  If a repeat count
.param &lt.n&gt.
is specified, then
.param &lt.n&gt.
lines are copied.
.np
This command is the same as typing
.param yy
:period.
.endkey
.*
.keylist end
.*
.se plural=0
.np
:INCLUDE file='oper'.
.*
.keylist begin 'Copying Text'
.*
.begkey y '&lt.n&gt.&lt."?&gt.y&lt.oper&gt.'
Yanks (copies) text from the current position in the file to the
position specified by
.param &lt.oper&gt.
:period.
Text is placed into the specified &copybuffer
.param &lt."?&gt.
:period.
If no buffer is specified, then the text is placed into the active
buffer.
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be copied.
.np
.param &lt.oper&gt.
may be specified as
.param y
:cont.,
which causes a single line to be yanked.
.seecmd delete
.np
.xmplsect begin
.begxmpl yy
Yanks (copies) the current line into the active &copybuffer
:period.
.endxmpl
.begxmpl 10yy
Copies 10 lines, starting at the current, into the active buffer.
.endxmpl
.begxmpl y$
Copies the characters from the current column to the end of the
current line into
the active buffer.
.endxmpl
.begxmpl "ay'm
Yanks the lines from the current line to the line with mark
.param m
into the named buffer
.param a
:period.
.endxmpl
.begxmpl y/foo
Copies:
:UL.
:LI.the part of the current line from the current position to the
end of the line
:LI.all lines between the current line and the first line
containing the string
.param foo
:LI the part of the line containing
.param foo
from the start of the line to the first letter in the string
.param foo
:eUL.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section *refid=cmchg 'Changing Text'
.* ******************************************************************
.np
The following commands are for changing text.
If a range of lines is being changed, the lines are deleted and &edvi
enters &tinsmode
:period.
.np
If the change is taking place on the single line, the range of
characters being changed is highlighted, and the last character in the
range is indicated with a dollar sign ('$').
If the
.keyword ESC
key is pressed, and
.keyref changelikevi 1
is not set, then the change command is cancelled. If
.keyref changelikevi 1
is set, then the highlighted area is deleted.  If anything other than the
.keyword ESC
key is pressed, the highlighted area is deleted and &edvi enters &tinsmode
:period.
.*
.keylist begin 'Changing Text'
.*
.begkey C C
This command changes the characters on the current line from the current
character to the end of the line.  The character range is highlighted, and
once a character is typed, the highlighted text is
deleted, and &tinsmode
is entered.
.np
This command is the same as typing
.param c$
:period.
.endkey

.begkey S &lt.n&gt.S
This command substitutes the current line with text.  The text on the current
line is deleted, and &tinsmode is entered.  If a repeat count
.param &lt.n&gt.
is specified, then
.param &lt.n&gt.
lines are deleted.
.np
This command is the same as typing
.keyword cc
:period.
.endkey

.begkey s &lt.n&gt.s
This command substitutes the current character with text.  If
.param &lt.n&gt.
is specified, then
.param &lt.n&gt.
characters are substituted.
.np
This command is the same as typing
.param cl
:cont.,
.param cRIGHT
:cont.,
or
.param cSPACE
:period.
.endkey
.*
.keylist end
.*
.np
.se plural = 0
:INCLUDE file='oper'
.*
.keylist begin 'Changing Text'
.*
.begkey c &lt.n&gt.c&lt.oper&gt.
Change text from the current position in the file to the
position specified by
.param &lt.oper&gt.
:period.
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be changed.
.np
.param &lt.oper&gt.
may be specified as
.keyword c
:cont.,
which causes a single line to be changed.
.np
.xmplsect begin
.begxmpl cr
Changes the current selected (highlighted) region in the edit buffer.
.endxmpl
.begxmpl cc
Change the current line. The current line is deleted, and &tinsmode
is entered.
.endxmpl
.begxmpl 95cc
Changes 95 lines, starting at the current. The lines are deleted,
and &tinsmode is entered.
.endxmpl
.begxmpl cw
Change the current word.  The current word is highlighted, and once
a character other than
.param ESC
is typed, the word is deleted and &tinsmode is entered.
.endxmpl
.begxmpl c$
Changes from the current column to the end of the
current line. 
The column range is highlighted, and once a character other than
.param ESC
is typed, the column range is deleted and &tinsmode is entered.
.endxmpl
.begxmpl 2cfa
Changes from the current column to the second letter a on the current line.
The column range is highlighted, and once a character other than
.param ESC
is typed, the column range is deleted and &tinsmode is entered.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section 'Shifting Text'
.* ******************************************************************
.np
The following commands are used to shift lines to the right or left,
inserting or deleting leading whitespace. 
.se plural = 1
:INCLUDE file='oper'.
.*
.keylist begin 'Shifting Text'
.*
.begkey &gt. &lt.n&gt.&gt.&lt.oper&gt. (right angle bracket)
This is the shift right command. It shifts the specified lines to the right
.keyref shiftwidth
spaces, inserting necessary leading tabs if
.keyref realtabs 1
is specified.
.np
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be shifted to the right.
.np
.param &lt.oper&gt.
may be specified as
.param &gt.
:cont.,
which causes a single line to be shifted to the right.
.seecmd &gt.
.np
.xmplsect begin
.begxmpl 10&gt.&gt.
Shifts the current line and the next 9 lines to the right
.keyref shiftwidth
spaces.
.endxmpl
.begxmpl &gt.'a
Shifts all lines from the current line to the line with mark
.param a
to the right
.keyref shiftwidth
spaces.
.endxmpl
.begxmpl &gt.r
Shifts all lines in the selected (highlighted) region to the right
.keyref shiftwidth
spaces.
.endxmpl
.xmplsect end
.endkey

.begkey &lt. &lt.n&gt.&lt.&lt.oper&gt. (left angle bracket)
This is the shift left command. It shifts the specified lines to the left
.keyref shiftwidth
spaces.
.np
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be shifted to the left
.keyref shiftwidth
spaces.
.np
.param &lt.oper&gt.
may be specified as
.param &lt.
:cont.,
which causes a single line to be shifted to the left
.keyref shiftwidth
spaces.
.seecmd &lt.
.np
.xmplsect begin
.begxmpl &lt.&lt.
Shifts the current line to the left
.keyref shiftwidth
spaces.
.endxmpl
.begxmpl &lt.G
Shifts all lines from the current line to the last line in the
edit buffer to the left
.keyref shiftwidth
spaces.
.endxmpl
.begxmpl 10&lt.j
Shifts the current line and the next 10 lines to the left
.keyref shiftwidth
spaces.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section 'Case Toggling'
.* ******************************************************************
.np
The case toggle &cmdmode command switches upper case letters to
lower case, and lower case letters to upper case.
 An example of its behaviour is changing the
line
.millust begin
This Is A Line Of Text.
.millust end
to
.millust begin
tHIS iS a lINE oF tEXT.
.millust end
.np
.se plural = 0
:INCLUDE file='oper'.
.*
.keylist begin 'Case Toggling'
.*
.begkey &tilde. &lt.n&gt.&tilde.&lt.oper&gt.  (tilde)
This is the case toggle command.
This command only works if the
.keyword togglecaselikevi
setting is not turned on.  If
.keyword togglecaselikevi
is set, then pressing
.param &tilde.
only changes the case of the current character, and advances the
cursor to the next character.
.np
However, if
.keyword togglecaselikevi
is not set, then this command toggles the case of the characters
over the range specified by
.param &lt.oper&gt.
.np
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be case toggled.
.np
.param &lt.oper&gt.
may be specified as
.param &tilde.
:cont.,
which causes a single line to be have its case toggled.
.np
.xmplsect begin
.begxmpl &tilde.r
Toggles the case of the currently selected (highlighted) range.
.endxmpl
.begxmpl &tilde.$
Toggles the case of all characters from the current column of the
current line to the last character of the current line.
.endxmpl
.begxmpl &tilde.w
Toggles the case of the current word.
.endxmpl
.begxmpl 10&tilde.&tilde.
Toggles the case of the current line and the 9 lines following.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section 'Filters'
.* ******************************************************************
.np
The &cmdmode filter command has the same functionality as the
&cmdline command
.keyword filter
:period.
.se plural = 0
:INCLUDE file='oper'.
.*
.keylist begin Filters
.*
.begkey ! &lt.n&gt.!&lt.oper&gt. (exclamation mark)
Filter text. The line range specified by
.param &lt.oper&gt.
are run through a specified filter.
A repeat count
.param &lt.n&gt.
may precede the command, this causes
.param &lt.n&gt.
units of the
.param &lt.oper&gt.
command to be run through the filter.
.np
.param &lt.oper&gt.
may be specified as
.param !
:cont.,
which causes a single line to be run through the filter.
.np
The lines specified are run through a specified
system command (filter).  Once the filter range has been specified, a window
is displayed, prompting for a system command:
.figure *depth='2.47' *scale='59' *file='vi025' Filter System Command Prompt
The system command must take input from standard in and put
its output to standard out. The lines specified are replaced with the output
of the the command.
.np
There is a filter command history in the filter command prompt, the
size of which is controlled with the
.keyref maxfilterhistory
setting.
.seecmd !
.np
.xmplsect begin
.begxmpl !r
Runs the current selected (highlighted) region in the edit buffer
through a specified filter.
.endxmpl
.begxmpl !!
Runs the current line through a specified filter
.endxmpl
.begxmpl !G
Runs all lines from the current line to the end of the edit buffer
through a specified filter.
.endxmpl
.xmplsect end
.endkey
.*
.keylist end
.* ******************************************************************
.section 'Text Selection'
.* ******************************************************************
.np
Text may be selected with the mouse. However, if you do not wish
to use the mouse, there is a keyboard interface to allow you to
accomplish the same thing.
.np
Once text selection has been started, any movement command adds to
the selected region.   The selected region may be cleared, and
text selection ended, by
pressing the
.param ESC
key.
.np
A selected region is highlighted by exchanging the foreground and
background the colors of the line.  A selected region could look
as follows:
.figure *depth='2.47' *scale='59' *file='vi026' Selected Text Region
.np
The following are the &cmdmode commands for selecting text and
manipulating selected text.
.*
.keylist begin 'Text Selection'
.*
.begkey CTRL_R CTRL_R
Starts text selection, if no text is selected.  The current character is highlighted.
.np
If region is already selected, then cancel the selected region.
.endkey

.begkey SHIFT_UP &lt.n&gt.SHIFT_UP
Starts selection (if not already started) and moves up to the previous
line.  The previous line and the current line are selected.
If a repeat count
.param &lt.n&gt.
is specified, then the cursor moves up
.param &lt.n&gt.
lines, and all the lines between the starting and ending
position are selected.
.endkey

.begkey SHIFT_DOWN &lt.n&gt.SHIFT_DOWN
Starts selection (if not already started) and moves down to the next line.
The next line and the current line are selected.
If a repeat count
.param &lt.n&gt.
is specified, then the cursor moves down
.param &lt.n&gt.
lines, and all the lines between the starting and ending
position are selected.
.endkey

.begkey SHIFT_LEFT &lt.n&gt.SHIFT_LEFT
Starts selection (if not already started) and move left to the previous
character.  The current character and the previous character are selected.
If a repeat count
.param &lt.n&gt.
is specified, then the cursor moves left
.param &lt.n&gt.
characters, and all the characters between the starting and ending
position are selected.
.endkey

.begkey SHIFT_RIGHT &lt.n&gt.SHIFT_RIGHT
Starts selection (if not already started) and move right to the next
character.  The current character and the next character are selected.
If a repeat count
.param &lt.n&gt.
is specified, then the cursor moves right
.param &lt.n&gt.
characters, and all the characters between the starting and ending
position are selected.
.endkey

.begkey SHIFT_DEL &lt."?&gt.SHIFT_DEL
Deletes the currently selected region.  If the &copybuffer
.param &lt."?&gt.
is specified, the region is copied into that buffer,
otherwise the data is copied into the active buffer.
.endkey

.begkey _ _ (underscore)
Simulates the right mouse being clicked at the current cursor position.
If a region is not selected, then the current word will be selected.
The word is defined using the &cmdline command
.keyref word
:period.
.endkey
.*
.keylist end
.* ******************************************************************
.section 'Miscellaneous Keys'
.* ******************************************************************
.keylist begin 'Miscellaneous Keys'
.*
.begkey CTRL_C CTRL_C
Exits the editor if no files have been modified.  If files have been modified,
a prompt is displayed asking you to verify that you really want to discard
the modified file(s).  
If you do not respond with a 'y', then the command is cancelled.
.seecmd quitall
.endkey

.begkey CTRL_G CTRL_G
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
.millust begin
"test.c" [modified] line 5 of 100  -- 5% --
.np
"..\c\file.c" [read only] line 100 of 100  -- 100% --
.np
"\autoexec.bat" line 1 of 100  -- 1% --
.millust end
.endkey

.begkey CTRL_L CTRL_L
Redraws the current screen.
.endkey

.begkey CTRL_V CTRL_V
Displays the current version of &edvi in the message window.
.seecmd version
.endkey

.begkey CTRL_X CTRL_X
Displays the hex value (and the decimal value) of the current
character.  A sample of the output in the message window is:
.millust begin
Char 'e': 0x65 (101)
.millust end
.endkey

.begkey CTRL_] CTRL_]  (control right square bracket)
Go to the tag given by the current word.  The word is defined using the
&cmdline command
.keyref word
:period.
.seecmd tag
.np
See the appendix
:HDREF refid='ctags'.
for more information.
.endkey

.begkey ALT_W ALT_W
Activates the current edit window's menu.  This menu is defined
using the
.keyword windowgadgetmenu
.keyref menu
:period.
See the chapter
:HDREF refid='winmenu'.
for more information on setting this menu.
.endkey

.begkey ALT_X ALT_X
Insert a character, at the current cursor position.  When
.param ALT_X
is pressed, a prompt is displayed:
.figure *depth='2.47' *scale='59' *file='vi027' Character Insertion Prompt
Enter either a decimal or a hex number.  That character will
be inserted directly into the edit buffer.
.endkey

.begkey : : (colon)
Allows entry of a &cmdline
:period.
See the chapter
:HDREF refid='cmds'
for full details on &cmdline commands.
.endkey

.begkey @ @&lt.?&gt. (at sign)
This command executes the &copybuffer
.param &lt.?&gt.
:period.
&edvi behaves as if the contents
of the buffer were being typed at the keyboard.
.endkey

.begkey J &lt.n&gt.J
Joins the next line to the current line.  If a repeat count
.param &lt.n&gt.
is specified, the next
.param &lt.n&gt.
lines are joined to the current line.
(lines are concatenated one after another).
.seecmd join
.np
.xmplsect begin
.begxmpl J
Joins the next line to the current line.
.endxmpl
.begxmpl 3J
Joins the next 3 lines to the current line.
.endxmpl
.xmplsect end
.endkey

.begkey Q Q
Enters
.keyword EX mode
:period.
.keyword EX mode
is a line-oriented mode of
&edvi.
:period.
To exit
.keyword EX mode
:cont.,
use the
.keyref visual
command.
.endkey

.begkey Z Z&lt.Z&gt.
Used when you are finished with the current edit buffer.
If the current edit buffer has been modified, it is saved.
.np
.xmplsect begin
.begxmpl ZZ
Finished with current edit buffer.
.endxmpl
.xmplsect end
.endkey

.begkey z &lt.n&gt.z&lt.?&gt.
Reorients the current screen position.  The current line moves
as follows, depending on the value of
.param &lt.?&gt.
:cont.:
:DL break.
:DT.ENTER
:DD.Moves the current line to the top of the screen.
:DT.. (dot)
:DD.Moves the current line to the center of the screen.
:DT.- (dash)
:DD.Moves the current line to the bottom of the screen.
:eDL.
If a repeat count
.param &lt.n&gt.
is specified, then
.param &lt.n&gt.
is made the current line.
.np
.xmplsect begin
.begxmpl z-
Move the current line to the bottom of the screen.
.endxmpl
.begxmpl 100zENTER
Makes line 100 the current line, and puts line 100 at the
top of the screen.
.endxmpl
.begxmpl 25z.
Makes line 25 the current line, and puts line 25 at the
center of the screen.
.endxmpl
.xmplsect end
.endkey

.begkey F1 F1
Move forward through the file list to the next file.
.seecmd next
.endkey

.begkey F2 F2
Move backwards through the file list to the previous file.
.seecmd prev
.endkey

.begkey F11 F11
Push the current file and position.  If you press
.param F12
:cont.,
you will be restored to this position.  These positions are stacked
up, up to a maximum of
.keyref maxpush
:period.
.seecmd push
.endkey

.begkey F12 F12
Restore the last pushed file and position.
.seecmd pop
.endkey

.begkey . . (dot)
Repeat the last &cmdmode command that changed text in
the edit buffer. It is also possible to memorize more than
just one command for '.' by using
.keyword memorize mode
:cont.:
:OL.
:LI.Type "
.param m.
:cont.".
(&edvi enters memorize mode).
:LI.Enter keystrokes.
:LI.Type "
.param .
:cont."
:eOL.
Now, whenever you press dot ('.'),
all the entered keystrokes will be executed.
.endkey

.begkey = = (equals sign)
Performs the last alternate memorized command sequence.  The
.keyword alternate memorize mode
is used as follows:
:OL.
:LI.Type "
.param m=
:cont.".
(&edvi enters
.keyword alternate memorize mode
:cont.).
:LI.Enter keystrokes.
:LI.Type "
.param =
:cont."
:eOL.
Now, whenever you press the equals sign ('='),
all the entered keystrokes will be executed as if you typed them
again from the keyboard.
.np
This memorized keystroke sequence will last until you memorize
another, unlike using "
.param m.
:cont.".
.endkey

.begkey ALT_M ALT_M
Display current memory state.  Shows the total amount of memory, the
amount of memory for use by &edvi, and how much extended memory and/or
disk space is available.
.endkey
.*
.keylist end
.*
.endlevel
