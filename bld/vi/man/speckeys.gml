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
:DEFLIST.
:DEFITEM.CTRL_V
insert next keystroke directly; do not process as a special character.
:DEFITEM.CTRL_Q
insert next keystroke directly; do not process as a special character.
:DEFITEM.CTRL_O
Insert the current input string after current line in the edit buffer.
:DEFITEM.ALT_O
Insert the current input string before current line in the edit buffer.
:DEFITEM.CTRL_D
Add the current "dot buffer" to the input string;
this is useful in conjunction with
:KEYWORD.memorize mode
("m.").
Once a sequence has been memorized, then this can be 
used to make a map command
:ILLUST.
map F1 <press CTRL_D>
:eILLUST.
:DEFITEM.ALT_L
Adds the current line in the current edit buffer, from the current
column to the end of the line, to the input string.
:DEFITEM.CTRL_E
Adds the current space delimited word in the current edit buffer to
the input string.
:DEFITEM.CTRL_L
Adds the current line in the current edit buffer to the input string.
:DEFITEM.CTRL_R
Adds the currently selected column range in the current edit buffer to
the input string.
:DEFITEM.CTRL_W
Adds the current word in the current edit buffer to the input string.
:DEFITEM.CTRL_INS
Restores last thing typed in the input window (one level undo).
:DEFITEM.UP
Scroll backwards through the history.
:DEFITEM.DOWN
Scroll forwards through the history.
:DEFITEM.ALT_TAB
Command completion. Looks backwards through the history for
first item starting with what is already entered.  Subsequent
presses of ALT_TAB get the 2nd last matching item, and so on.
:DEFITEM.RIGHT
Move cursor right through input string.
:DEFITEM.LEFT
Move cursor left through input string.
:DEFITEM.CTRL_END
Delete to end of the input string.
:DEFITEM.END
Move to end of the input string.
:DEFITEM.HOME
Move to start of the input string.
:DEFITEM.INSERT
Toggle between insertion and overstrike of text.
:DEFITEM.BS
Backspace in the input string, deleting the previous character.
:DEFITEM.DELETE
Delete current character.
:DEFITEM.ENTER
.if '&searching' eq '1' .do begin
Do the search.
.do end
.el .do begin
Process the command.
.do end
:DEFITEM.ESC
.if '&searching' eq '1' .do begin
Cancel the search request.
.do end
.el .do begin
Cancel the command.
.do end
:DEFITEM.TAB
Try to complete the file name based on the current string.
:ADDLINE.
The first match is completed, and a window with possible
choices is displayed. Subsequent presses of TAB will
scroll forward through the list of possible matches, and
pressing SHIFT_TAB will scroll backwards through the
list of possible matches. Cursor keys may also be used, and so may the
mouse.
.if '&searching' eq '1' .do begin
:PICTURE file='filecs' text='File Name Completion Window'.
.do end
.el .do begin
:PICTURE file='filec' text='File Name Completion Window'.
.do end
:eDEFLIST.
.se searching = 0
