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
.* Description:  Command mode key summary.
.*
.* Date         By              Reason
.* ----         --              ------
.* 03-aug-92    Craig Eisler    initial draft
.* 03-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap 'Command Mode Key Summary'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
The following is a list of all possible keys that may be pressed in
command mode, and their default behaviour.
.np
Commands preceded with a
.param <n>
take a repeat count.
.np
Commands preceded with a
.param <"?>
accept a copy buffer name.
.np
Commands that accept mark letter ('a'-'z') have a
.param <?>
in their definition.

.np
.se plural = 1
:INCLUDE file='oper'.
.*
:DL break.
.*
:DT.<n>CTRL_B
:DD.Move up one page.

:DT.<n>CTRL_D
:DD.Move down a half page.

:DT.<n>CTRL_E
:DD.Expose the line below the bottom line on the screen.

:DT.<n>CTRL_F
:DD.Move down one page.

:DT.CTRL_G
:DD.Display current file status.

:DT.CTRL_L
:DD.Redraw the screen.

:DT.<n>CTRL_N
:DD.Move to next line.

:DT.<n>CTRL_P
:DD.Move to previous line.

:DT.CTRL_R
:DD.Start text selection.

:DT.<n>CTRL_U
:DD.Move up a half page.

:DT.CTRL_V
:DD.Display current version.

:DT.CTRL_X
:DD.Display hex value of character under cursor.

:DT.<n>CTRL_Y
:DD.Expose the line before the top line on the screen.

:DT.CTRL_]
:DD.Go to the tag indicated by the current word.

:DT.<n>SPACE
:DD.Move the cursor right one character.

:DT.<n>!<oper>
:DD.Run lines through a filter.

:DT.$
:DD.Move to the end of the current line.

:DT.%
:DD.Find the matching brace or other defined match item.

:DT.'<?>
:DD.Go to the mark
.param ?
:period.

:DT.<n>+
:DD.Go to the start of the next line.

:DT.,
:DD.Perform the last f,t,F or T command, in the opposite direction.

:DT.<n>-
:DD.Go to the start of the previous line.

:DT..
:DD.Repeat last command mode sequence that changed the edit buffer.

:DT./
:DD.Search forward for a string.

:DT.0
:DD.Move to the first character on the current line.

:DT.:
:DD.Enter a command line command.

.dc cw OFF
:DT.;
:DD.Perform the last f,t,F or T command.
.dc cw ;

:DT.<n><<oper>
:DD.Shift lines to the left.

:DT.<n>><oper>
:DD.Shift lines to the right.

:DT.?
:DD.Search backwards for a string

:DT.@
:DD.Execute a copy buffer as if it were typed at the keyboard.

:DT.A
:DD.Start inserting text at the end of the current line.

:DT.<n>B
:DD.Move back to the start of the previous space delimited word.

:DT.C
:DD.Change characters from the current column to the end of the current line.

:DT.D
:DD.Delete characters from the current column to the end of the current line.

:DT.<n>E
:DD.Move forwards to the end of the next space delimited word.

:DT.F<n><?>
:DD.Move backwards to a specified character.

:DT.<n>G
:DD.Go to a specified line number.

:DT.<n>H
:DD.Go to the top of the current edit window.

:DT.I
:DD.Start inserting text at the first non-whitespace character on the
current line.

:DT.<n>J
:DD.Join the next line to the current line.

:DT.<n>L
:DD.Go to the bottom of the current edit window.

:DT.M
:DD.Go to the middle of the current edit window.

:DT.N
:DD.Repeat the last find command, only search in the opposite direction.

:DT.O
:DD.Open a blank line above the current line, and enter &tinsmode
:period.

:DT.<"?>P
:DD.Insert a copy buffer before the current position.

:DT.Q
:DD.Enter
.keyword EX mode
:period.

:DT.R
:DD.Enter text overstrike mode.

:DT.<n>S
:DD.Substitute lines with other text.

:DT.T<?>
:DD.Move backwards to the character after a specified character.

:DT.U
:DD.Re-do (undoes the last undo).

:DT.<n>W
:DD.Move forward to the start of the next space delimited word.

:DT.<n>X
:DD.Delete the character before the current character.

:DT.<n>Y
:DD.Yank (copy) lines.

:DT.Z
:DD.If followed by another
.param Z
:cont.,
saves the current file (if it has been modified) and quits the file.

:DT.&caret.
:DD.Move to the first non-whitespace character on the line.

:DT._
:DD.Simulate right-mouse button press.

:DT.`<?>
:DD.Go to the line and column with the mark
.param <?>
:period.

:DT.a
:DD.Start inserting text at the character after the current cursor position.

:DT.<n>b
:DD.Move backwards to the start of the previous word.

:DT.<n>c<oper>
:DD.Change text.

:DT.<"?><n>d<oper>
:DD.Delete text.

:DT.<n>e
:DD.Move to the end of the current word.

:DT.<n>f<?>
:DD.Move forward to the character
.param <?>
on the current line.

:DT.<n>h
:DD.Move left.

:DT.i
:DD.Start inserting text at the current cursor position.

:DT.<n>j
:DD.Move down one line.

:DT.<n>k
:DD.Move up one line.

:DT.<n>l
:DD.Move right.

:DT.m<?>
:DD.Set the mark
.param <?>
:period.

:DT.n
:DD.Repeat the last find command.

:DT.o
:DD.Open a new line after the current line, and start inserting text.

:DT.<"?>p
:DD.Insert a copy buffer at the current position in the edit buffer.

:DT.<n>r
:DD.Replace the current character.

:DT.<n>s
:DD.Substitute the current character with text.

:DT.<n>t<?>
:DD.Move up to the character before the character
.param <?>
on the current line.

:DT.u
:DD.Undo the last change.

:DT.<n>w
:DD.Move to the start of the next word.

:DT.<n>x
:DD.Delete the character at the cursor.

:DT.<n><"?>y<oper>
:DD.Yank (copy) text.

:DT.<n>z<?>
:DD.Reposition the current line.

:DT.<n>|
:DD.Move to the specified column.

:DT.<n>&tilde.<oper>
:DD.Toggle the case of text.

:DT.F1
:DD.Move to the next file in the list of files being edited.

:DT.F2
:DD.Move back to the previous file in the list of files being edited.

:DT.<n>UP
:DD.Move up one line.

:DT.<n>DOWN
:DD.Move down one line.

:DT.<n>LEFT
:DD.Move left to the previous character.

:DT.<n>RIGHT
:DD.Move right to the next character.

:DT.<n>PAGEUP
:DD.Move up one page.

:DT.<n>PAGEDOWN
:DD.Move down one page.

:DT.INS
:DD.Start inserting text at the current cursor position.

:DT.<"?><n>DEL
:DD.Delete the character at the current cursor position.

:DT.<n>BS
:DD.Move left to the previous character.

:DT.<n>SHIFT_TAB
:DD.Move left by a tab amount.

:DT.<n>ENTER
:DD.Move to the start of the next line.

:DT.<n>TAB
:DD.Move right by a tab amount.

:DT.HOME
:DD.Move to the start of the current line.

:DT.END
:DD.Move to the end of the current line.

:DT.CTRL_F1
:DD.Make copy buffer 1 active.

:DT.CTRL_F2
:DD.Make copy buffer 2 active.

:DT.CTRL_F3
:DD.Make copy buffer 3 active.

:DT.CTRL_F4
:DD.Make copy buffer 4 active.

:DT.CTRL_F5
:DD.Make copy buffer 5 active.

:DT.CTRL_F6
:DD.Make copy buffer 6 active.

:DT.CTRL_F7
:DD.Make copy buffer 7 active.

:DT.CTRL_F8
:DD.Make copy buffer 8 active.

:DT.CTRL_F9
:DD.Make copy buffer 9 active.

:DT.ALT_M
:DD.Display current memory state.

:DT.ALT_W
:DD.Activate the current edit window menu.

:DT.ALT_X
:DD.Insert a keystroke by entering the ASCII value.

:DT.CTRL_PAGEUP
:DD.Move to the first character in the current edit buffer.

:DT.CTRL_PAGEDOWN
:DD.Move to the last character in the current edit buffer.

:DT.<n>SHIFT_UP
:DD.Start selection, and move up.

:DT.<n>SHIFT_DOWN
:DD.Start selection, and move down.

:DT.<n>SHIFT_LEFT
:DD.Start selection, and move left.

:DT.<n>SHIFT_RIGHT
:DD.Start selection, and move right.

:DT.<"?>SHIFT_DEL
:DD.Delete selected region.

:DT.<"?>SHIFT_INS
:DD.Insert a copy buffer at the current position in the edit buffer.
.*
:eDL.
.*

