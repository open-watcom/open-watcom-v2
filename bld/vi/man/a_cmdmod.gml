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
.* 03-aug-92	Craig Eisler	initial draft
.*
:CHAPTER.Command Mode Key Summary
The following is a list of all possible keys that may be pressed in
command mode, and their default behaviour.
:P.
Commands preceded with a
:ITALICS.<n>
take a repeat count.
:P.
Commands preceded with a
:ITALICS.<"?>
accept a copy buffer name.
:P.
Commands that accept mark letter ('a'-'z') have a
:ITALICS.<?>
in their definition.
:P.
.se plural = 1
:INCLUDE file='oper'.

:P.
:DEFLIST.
:DEFITEM.<n>CTRL_B
Move up one page.

:DEFITEM.<n>CTRL_D
Move down a half page.

:DEFITEM.<n>CTRL_E
Expose the line below the bottom line on the screen.

:DEFITEM.<n>CTRL_F
Move down one page.

:DEFITEM.CTRL_G
Display current file status.

:DEFITEM.CTRL_L
Redraw the screen.

:DEFITEM.<n>CTRL_N
Move to next line.

:DEFITEM.<n>CTRL_P
Move to previous line.

:DEFITEM.CTRL_R
Start text selection.

:DEFITEM.<n>CTRL_U
Move up a half page.

:DEFITEM.CTRL_V
Display current version.

:DEFITEM.CTRL_X
Display hex value of character under cursor.

:DEFITEM.<n>CTRL_Y
Expose the line before the top line on the screen.

:DEFITEM.CTRL_]
Go to the tag indicated by the current word.

:DEFITEM.<n>SPACE
Move the cursor right one character.

:DEFITEM.<n>!<oper>
Run lines through a filter.

:DEFITEM.$
Move to the end of the current line.

:DEFITEM.%
Find the matching brace or other defined match item.

:DEFITEM.'<?>
Go to the mark
:ITALICS.?
:PERIOD.

:DEFITEM.<n>+
Go to the start of the next line.

:DEFITEM.,
Perform the last f,t,F or T command, in the opposite direction.

:DEFITEM.<n>-
Go to the start of the previous line.

:DEFITEM..
Repeat last command mode sequence that changed the edit buffer.

:DEFITEM./
Search forward for a string.

:DEFITEM.0
Move to the first character on the current line.

:DEFITEM.:
Enter a command line command.

:DEFITEM.';'
Perform the last f,t,F or T command.

:DEFITEM.<n><<oper>
Shift lines to the left.

:DEFITEM.<n>><oper>
Shift lines to the right.

:DEFITEM.?
Search backwards for a string

:DEFITEM.@
Execute a copy buffer as if it were typed at the keyboard.

:DEFITEM.A
Start inserting text at the end of the current line.

:DEFITEM.<n>B
Move back to the start of the previous space delimited word.

:DEFITEM.C
Change characters from the current column to the end of the current line.

:DEFITEM.D
Delete characters from the current column to the end of the current line.

:DEFITEM.<n>E
Move forwards to the end of the next space delimited word.

:DEFITEM.F<n><?>
Move backwards to a specified character.

:DEFITEM.<n>G
Go to a specified line number.

:DEFITEM.<n>H
Go to the top of the current edit window.

:DEFITEM.I
Start inserting text at the first non-whitespace character on the
current line.

:DEFITEM.<n>J
Join the next line to the current line.

:DEFITEM.<n>L
Go to the bottom of the current edit window.

:DEFITEM.M
Go to the middle of the current edit window.

:DEFITEM.N
Repeat the last find command, only search in the opposite direction.

:DEFITEM.O
Open a blank line above the current line, and enter &tinsmode
:PERIOD.

:DEFITEM.<"?>P
Insert a copy buffer before the current position.

:DEFITEM.Q
Enter
:KEYWORD.EX mode
:PERIOD.

:DEFITEM.R
Enter text overstrike mode.

:DEFITEM.<n>S
Substitute lines with other text.

:DEFITEM.T<?>
Move backwards to the character after a specified character.

:DEFITEM.U
Re-do (undoes the last undo).

:DEFITEM.<n>W
Move forward to the start of the next space delimited word.

:DEFITEM.<n>X
Delete the character before the current character.

:DEFITEM.<n>Y
Yank (copy) lines.

:DEFITEM.Z
If followed by another
:ITALICS.Z
:CONT.,
saves the current file (if it has been modified) and quits the file.

:DEFITEM.^
Move to the first non-whitespace character on the line.

:DEFITEM._
Simulate right-mouse button press.

:DEFITEM.`<?>
Go to the line and column with the mark
:ITALICS.<?>
:PERIOD.

:DEFITEM.a
Start inserting text at the character after the current cursor position.

:DEFITEM.<n>b
Move backwards to the start of the previous word.

:DEFITEM.<n>c<oper>
Change text.

:DEFITEM.<"?><n>d<oper>
Delete text.

:DEFITEM.<n>e
Move to the end of the current word.

:DEFITEM.<n>f<?>
Move forward to the character
:ITALICS.<?>
on the current line.

:DEFITEM.<n>h
Move left.

:DEFITEM.i
Start inserting text at the current cursor position.

:DEFITEM.<n>j
Move down one line.

:DEFITEM.<n>k
Move up one line.

:DEFITEM.<n>l
Move right.

:DEFITEM.m<?>
Set the mark
:ITALICS.<?>
:PERIOD.

:DEFITEM.n
Repeat the last find command.

:DEFITEM.o
Open a new line after the current line, and start inserting text.

:DEFITEM.<"?>p
Insert a copy buffer at the current position in the edit buffer.

:DEFITEM.<n>r
Replace the current character.

:DEFITEM.<n>s
Substitute the current character with text.

:DEFITEM.<n>t<?>
Move up to the character before the character
:ITALICS.<?>
on the current line.

:DEFITEM.u
Undo the last change.

:DEFITEM.<n>w
Move to the start of the next word.

:DEFITEM.<n>x
Delete the character at the cursor.

:DEFITEM.<n><"?>y<oper>
Yank (copy) text.

:DEFITEM.<n>z<?>
Reposition the current line.

:DEFITEM.<n>|
Move to the specified column.

:DEFITEM.<n>~~<oper>
Toggle the case of text.

:DEFITEM.F1
Move to the next file in the list of files being edited.

:DEFITEM.F2
Move back to the previous file in the list of files being edited.

:DEFITEM.<n>UP
Move up one line.

:DEFITEM.<n>DOWN
Move down one line.

:DEFITEM.<n>LEFT
Move left to the previous character.

:DEFITEM.<n>RIGHT
Move right to the next character.

:DEFITEM.<n>PAGEUP
Move up one page.

:DEFITEM.<n>PAGEDOWN
Move down one page.

:DEFITEM.INS
Start inserting text at the current cursor position.

:DEFITEM.<"?><n>DEL
Delete the character at the current cursor position.

:DEFITEM.<n>BS
Move left to the previous character.

:DEFITEM.<n>SHIFT_TAB
Move left by a tab amount.

:DEFITEM.<n>ENTER
Move to the start of the next line.

:DEFITEM.<n>TAB
Move right by a tab amount.

:DEFITEM.HOME
Move to the start of the current line.

:DEFITEM.END
Move to the end of the current line.

:DEFITEM.CTRL_F1
Make copy buffer 1 active.

:DEFITEM.CTRL_F2
Make copy buffer 2 active.

:DEFITEM.CTRL_F3
Make copy buffer 3 active.

:DEFITEM.CTRL_F4
Make copy buffer 4 active.

:DEFITEM.CTRL_F5
Make copy buffer 5 active.

:DEFITEM.CTRL_F6
Make copy buffer 6 active.

:DEFITEM.CTRL_F7
Make copy buffer 7 active.

:DEFITEM.CTRL_F8
Make copy buffer 8 active.

:DEFITEM.CTRL_F9
Make copy buffer 9 active.

:DEFITEM.ALT_M
Display current memory state.

:DEFITEM.ALT_W
Activate the current edit window menu.

:DEFITEM.ALT_X
Insert a keystroke by entering the ASCII value.

:DEFITEM.CTRL_PAGEUP
Move to the first character in the current edit buffer.

:DEFITEM.CTRL_PAGEDOWN
Move to the last character in the current edit buffer.

:DEFITEM.<n>SHIFT_UP
Start selection, and move up.

:DEFITEM.<n>SHIFT_DOWN
Start selection, and move down.

:DEFITEM.<n>SHIFT_LEFT
Start selection, and move left.

:DEFITEM.<n>SHIFT_RIGHT
Start selection, and move right.

:DEFITEM.<"?>SHIFT_DEL
Delete selected region.

:DEFITEM.<"?>SHIFT_INS
Insert a copy buffer at the current position in the edit buffer.

:eDEFLIST.
