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
.* Description:  Basic usage.
.*
.* Date         By              Reason
.* ----         --              ------
.* 30-jul-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted, figures
.*
.****************************************************************************/
:DL break.
:DT.CTRL_V
:DD.Insert next keystroke directly; do not process as a special character.

:DT.CTRL_Q
:DD.Insert next keystroke directly; do not process as a special character.

:DT.CTRL_O
:DD.Insert the current input string after current line in the edit buffer.

:DT.ALT_O
:DD.Insert the current input string before current line in the edit buffer.

:DT.ALT_L
:DD.Adds the current line in the current edit buffer, from the current
column to the end of the line, to the input string.

:DT.CTRL_E
:DD.Adds the current space delimited word in the current edit buffer to
the input string.

:DT.CTRL_L
:DD.Adds the current line in the current edit buffer to the input string.

:DT.CTRL_R
:DD.Adds the currently selected column range in the current edit buffer to
the input string.

:DT.CTRL_W
:DD.Adds the current word in the current edit buffer to the input string.

:DT.CTRL_INS
:DD.Restores last thing typed in the input window (one level undo).

:DT.UP
:DD.Scroll backwards through the history.

:DT.DOWN
:DD.Scroll forwards through the history.

:DT.ALT_TAB
:DD.Command completion. Looks backwards through the history for
first item starting with what is already entered.  Subsequent
presses of ALT_TAB get the 2nd last matching item, and so on.

:DT.RIGHT
:DD.Move cursor right through input string.

:DT.LEFT
:DD.Move cursor left through input string.

:DT.CTRL_END
:DD.Delete to end of the input string.

:DT.END
:DD.Move to end of the input string.

:DT.HOME
:DD.Move to start of the input string.

:DT.INSERT
:DD.Toggle between insertion and overstrike of text.

:DT.BS
:DD.Backspace in the input string, deleting the previous character.

:DT.DELETE
:DD.Delete current character.

:DT.ENTER
.if '&searching' eq '1' .do begin
:DD.Do the search.
.do end
.el .do begin
:DD.Process the command.
.do end

:DT.ESC
.if '&searching' eq '1' .do begin
:DD.Cancel the search request.
.do end
.el .do begin
:DD.Cancel the command.
.do end

:DT.TAB
:DD.Try to complete the file name based on the current string.
.sp
The first match is completed, and a window with possible
choices is displayed. Subsequent presses of TAB will
scroll forward through the list of possible matches, and
pressing SHIFT_TAB will scroll backwards through the
list of possible matches. Cursor keys may also be used, and so may the
mouse.
.if '&searching' eq '1' .do begin
.figure *depth='2.47' *scale='59' *file='vi024' File Name Completion Window
.do end
.el .do begin
.figure *depth='2.47' *scale='59' *file='vi029' File Name Completion Window
.do end

:eDL.
.se searching=0
