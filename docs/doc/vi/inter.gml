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
.* 10-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted, figures
.* 30-apr-06    F. Beythien     minor updates, typing errors
.*
.****************************************************************************/
.chap *refid=inter 'Intermediate Usage'
.*
.np
This chapter discusses a number of the commonly used features of the &edname.
:period.
The knowledge of the information in the chapter
:HDREF page=no refid='basic'.
is assumed.
.* ******************************************************************
.section 'The Command Line'
.* ******************************************************************
.np
The &edname has a powerful set of commands that are entered in a special
command window. These commands
are referred to as &cmdline commands.  You can activate the command window
in two ways:
.*
:OL.

:LI.Select the
.keyword Enter command
item under the
.keyword File
menu.

:LI.Press the colon (':') key when in &cmdmode
:period.
Remember to press the
.param ESC
key to ensure that you are in &cmdmode
before pressing ':'.

:eOL.
.*
Once you have done one of the previous things, the following window will
appear on your screen:
.figure *depth='2.47' *scale='59' *file='vi015' The Command Entry window
You may enter a command in this window (for example: quit).  If you wish
to cancel the command that you are typing, just press the
.param ESC
key and the window will disappear.
.np
You may cursor back and forth in the command window, and use the
backspace and delete keys to change mistakes.  Once you press
.param ENTER
:cont.,
the command will be processed.
.np
If you cursor up, you will go through a list of commands
that you have entered at the command window (newest to oldest).  This
is your command
.keyword history
:period.
Cursoring
down will take you through the history from oldest to newest.  This is
very useful if you have typed a complicated command and did not get it
quite right or if you just wish to execute the command again.
.np
The chapter
:HDREF refid='cmds'.
describes the &cmdline in more detail.
.*
.beglevel
.* ******************************************************************
.section 'Line Numbers'
.* ******************************************************************
.np
Some &cmdline commands accept a line address or a line range as a
parameter.
For example, when specifying the
.keyref write
command, you may specify
.millust begin
:write
.millust end
or you may specify
.millust begin
:1,10 write
.millust end
.np
A line address is a number or a sum of numbers.  As well,
some special symbols may be used:
.*
:DL break.

:DT.. (dot)
:DD.Represents the current line number.

:DT.$ (dollar)
:DD.Represents the last line number.

:DT.'a (front quote)
:DD.Indicates the line with the mark 'a' set; marks 'a' through 'z'
may be used.  Marks are discussed later in this chapter.

:eDL.
.*
.np
A line range is two line addresses separated by a comma.
.np
Some examples of line addresses and line ranges are:
.millust begin
 .+5     - five lines past the current line.

 'a      - the line with mark a.

 $       - the last line

 1,5     - lines 1 to 5

 .,$     - current line in file to end line of file

 .-3,100 - the line 3 before the current to line 100
.millust end
Line addresses are discussed in greater detail in the section
:HDREF refid='lineadd'.
:period.
.np
If you just enter a line address on its own on the &cmdline, then
you will go directly to that line.
.*
.exercises
:OL.

:LI.Start up &edvi, and try selecting the
.keyword File
menu, and then selecting the
.keyword Enter command
item.  Notice how the window pops up.  Try typing and cursoring around.
When you are done, press the
.param ESC
key to cancel the command.

:LI.Make sure that you are in &cmdmode, then press the colon (':') key.
Once again, the command window pops up.
Try typing and cursoring around. When you are done, press the
.param ESC
key to cancel the command.

:LI.Add 10 lines to your file.  Then press the colon (':') key and
enter the number 5.  You will go to line 5.  Try entering different
numbers and see what happens.   If you enter a line number that
does not exist, you will see the message:
.millust begin
No such line
.millust end

:LI.Now that you have a number of commands entered, try cursoring up
and down in the command window.  You will see all the commands that
you have typed.

:eOL.
.*
.endlevel
.* ******************************************************************
.section 'Getting a File for Editing'
.* ******************************************************************
.np
In the chapter
:HDREF refid='basic' page=no.
:cont.,
you saw that you could edit a file by either specifying the name on the
command line when invoking &edvi, or by selecting the
.keyword File
menu and picking the
.keyword Edit new file
option.
.np
The general way to edit a new file is to use the &cmdline command
.keyref edit
:period.
To enter this command, make sure that you are in &cmdmode and press
the colon (':') key.  Once the command prompt is displayed, then
simply type
.keyref edit
(optionally followed by a file name or a list of files), and then press
.param ENTER
:period.
.np
If you do not specify a file, then a directory listing is displayed.
:INCLUDE file='fsdesc'.
.np
You may also specify one or more files after the
.keyref edit
command.
If a file you specify is the same as one already being edited,
then control is simply transferred to the window with that file.
.*
.exercises
:OL.

:LI.Start up &edvi without any files specified. Then try entering the
&cmdline command
.keyref edit
without any parameters (remember to press the colon (':') key first).
You will see a screen similar to the following:
.figure *depth='2.47' *scale='59' *file='vi016' File Selection display
Try changing to other directories or drives
with this list.  When you are done, press the
.param ESC
key to cancel the selection list.

:LI.Try entering the following &cmdline
.millust begin
:e afile
.millust end
This will cause &edvi to start editing a new file called "afile".

:LI.Try entering the command
.millust begin
:e bfile cfile
.millust end
This will cause &edvi to edit two new files, one named "bfile"
and one named "cfile".

:LI.Enter the command
.millust begin
:e afile
.millust end
This returns you to the first file ("afile") that you were already editing.

:LI.Press
.param CTRL_C
:cont., and &edvi will quit all the files that you have started editing.

:eOL.
.* ******************************************************************
.section 'Moving Between Files'
.* ******************************************************************
.np
There are a number of ways to move between files that you are editing.
As you have seen in the previous section,  you can move to a file
that you are already editing by using the &cmdline command
.keyref edit
and specifying the name of the file you wish to move to.
.np
You may press the
.param F1
key while in &cmdmode or &tinsmode
:period.
This function key moves you to the next file in the list of files that
you are editing.
.np
You may press the
.param F2
key while in &cmdmode or &tinsmode
:period.
This function key moves you to the previous file in the list of files that
you are editing.
.np
The previous two function keys that you may use also have &cmdline
equivalents.
The &cmdline command
.keyref next
moves you to the next file in the list of files that you are editing.
.np
The &cmdline command
.keyref prev
moves you to the previous file in the list of files that you are editing.
.np
It is also possible to display a list of all files that you are currently
editing.  You may press the
.param F7
key in either &cmdmode or &tinsmode, or enter the &cmdline command
.keyref files
:period.
Doing any one of these things will cause a list of all files currently
being edited to appear.  An asterisk ('*') will precede files that
have been modified.
From this list, you may go to one of the files,
quit one of the files, or save one of the files then quit it.
.*
.exercises
:OL.

:LI.Start up &edvi in the following way:
.millust begin
vi a b c
.millust end
This will cause you to edit three new files.
Now, press the
.param F7
key.  The following will appear:
.figure *depth='2.47' *scale='59' *file='vi017' Current File List
Select the file
.param b
:period.
That file will become the current file being edited.

:LI.Type the &cmdline
.millust begin
:files
.millust end
You will see the same result as you saw in the previous example.
Press the
.param ESC
key to cancel this display.

:LI.Press the
.param F1
key several times.  You will rotate through the three files that you
are editing.

:LI.Press the
.param F2
key several times.  You will rotate through the three files that you
are editing, but in the opposite order than when you were pressing the
.param F1
key.

:LI.Use the &cmdline commands
.keyref next
and
.keyref prev
to move through the files.  These commands behave the same as
pressing
.param F1
and
.param F2
:period.

:eOL.
.* ******************************************************************
.section *refid=maiaf 'Moving Around in a File'
.* ******************************************************************
.np
You have already learned to use the cursor keys to move around through
a file.  When you are in &cmdmode, there are a number of keys that
also cause movement through the file. Many of these keys may be
preceded with a repeat count.  You enter the repeat count
by typing a number (which will be echoed in a special window on the
screen).
.np
Once you have entered the repeat count, you may cancel it
by pressing the
.param ESC
key, or you may follow it with a movement command.  For example, if
you type:
.millust begin
3&lt.Down Arrow Key&gt.
.millust end
you will move down three lines instead of one.
.np
The basic &cmdmode movement commands are:
:INCLUDE file='basicmv'.
.np
There are additional commands that move you around the file which do not
require your fingers to move off the home row of your keyboard.
For a touch typist, this is a great advantage.
For a list of all of the movement commands, see the section
:HDREF refid='mvement'
in the chapter
:HDREF page=no refid='modes'
:period.
The following list of movement commands move you around on the
current line:
.*
:DL break.

:DT.$ (dollar sign)
:DD.Move to the end of the current line.

:DT.0 (zero)
:DD.Move to the start of the current line.

:DT.b
:DD.Move backwards to the previous word on the current line. If preceded
with a repeat count, you move back that many words.

:DT.h
:DD.Move right through the text.
If preceded with a repeat count, then you move right that many characters.

:DT.l
:DD.Move left through the text.
If preceded with a repeat count, then you move left that many characters.

:DT.w
:DD.Move forward to the next word on the current line. If preceded
with a repeat count, you move forward that many words.

:DT.B
:DD.Move backwards to the previous whitespace delimited word on the current line.
If preceded with a repeat count, you move back that many words.

:DT.W
:DD.Move forward to the next whitespace delimited word on the current line.
If preceded with a repeat count, you move forward that many words.

:eDL.
.*
.np
The following list of movement commands move you to other lines:
.*
:DL break.

:DT.CTRL_B
:DD.Move back one page in the text.
If preceded with a repeat count, you will move back that many pages.

:DT.CTRL_D
:DD.Move down one half page in the text.
If preceded with a repeat count, then you move forward that many lines.
As well, any future
.param CTRL_D
or
.param CTRL_U
commands issued will move that many lines, instead of a half page.

:DT.CTRL_F
:DD.Move forward one page in the text.
If preceded with a repeat count, you will move forward that many pages.

:DT.CTRL_U
:DD.Move up one half page in the text.
If preceded with a repeat count, then you move backwards that many lines.
As well, any future
.param CTRL_U
or
.param CTRL_D
commands issued will move that many lines, instead of a half page.

:DT.j
:DD.Move down through the text.
If preceded with a repeat count, then you move down that many lines.

:DT.k
:DD.Move up through the text.
If preceded with a repeat count, then you move up that many lines.

:DT.G
:DD.Moves to the last line in the file.  If preceded with a repeat
count, then you move to that line in the file.

:DT.H
:DD.Move to the top of the current edit window.
If preceded with a repeat count, you move that many lines from the
top of the edit window.

:DT.L
:DD.Move to the bottom of the current edit window.
If preceded with a repeat count, you move that many lines from the
bottom of the edit window.

:DT.M
:DD.Move to the middle of the current edit window.

:eDL.
.*
.exercises
:OL.

:LI.Edit a new file, "atest".  Once you have edited this file, add the line:
.millust begin
This is a test line.
.millust end
Once you have done this, copy this line by pressing 'Y'. Press 'p' to paste
in the copy.  Press 'p' 28 more times, so that you create a file with
30 lines in it (all just like the first line).

:LI.So that we can more easily see the results, type the following &cmdline:
.millust begin
:%s/^/\# /
.millust end
This is a substitution command.  It will replace the start of each of your
lines with the line number.  We will learn about the substitution command
in the next chapter.  When you are done, you should see a screen similar to
the following:
.figure *depth='2.47' *scale='59' *file='vi018' "ATEST" File Contents

:LI.Press the 'G' key.  You will move to the last line of the file.

:LI.Type the following:
.millust begin
15G
.millust end
This will move you to line 15.

:LI.Try using
.param CTRL_F
and
.param CTRL_B
:period.
Notice that they behave just like
.param PAGEUP
and
.param PAGEDOWN
:period.

:LI.Try using 'w' and 'b' to move forward and backwards through words
in the file.

:LI.Try using 'j' and 'k' to cursor up and down in the file.

:LI.Try using 'l' and 'h' to cursor left and right in the file.

:LI.Press the 'H' key.  The cursor will move to the top line
in the edit window.

:LI.Press the 'L' key.  The cursor will move to the bottom line
in the edit window.

:LI.Try typing some numbers before pressing the 'H' and 'L' keys.  For
example, typing
.millust begin
3H
.millust end
will move your cursor to the 3rd line from the top of the edit window.

:LI.Press the 'M' key.  The cursor will move to the middle of edit window.

:LI.Press the '$' key.  The cursor will move to the end of the current line.

:LI.Press the '0' (zero) key.  The cursor will move to the start of the current line.

:LI.Press
.param CTRL_D
:period.
You will move down half a page

:LI.Type the number '2' and then press CTRL_D.  Notice that you
only move down 2 lines.

:LI.Press
.param CTRL_D
:period.
You will move down 2 lines.

:LI.Press
.param CTRL_U
:period.
You will move up 2 lines.

:LI.Press 'ZZ' to save the file.  This file will be used in later exercises.

:eOL.
.* ******************************************************************
.section 'Saving and Exiting a File Revisited'
.* ******************************************************************
.np
We have already seen in the section
:HDREF refid='savexit'
a number of ways to save and exit your files.  These methods included
typing 'ZZ' and using the menus.
.np
There are a number of different &cmdline commands that can be used for
saving and/or quitting your files.
:OL.

:LI.
.keyref quit
("!") or
.param q
("!")

:LI.
.keyref quitall

:LI.
.keyref write
("!") or
.param w
("!")

:LI.
.keyref wq

:LI.
.keyref xit

:eOL.
The
.keyref quit
command is used to exit a file without saving it.  If the file has
been modified, the command will fail and the message:
.millust begin
File modified - use :q! to force
.millust end
will be displayed.  To quit a modified file, the exclamation point ('!')
is used:
.millust begin
:quit!
.millust end
or
.millust begin
:q!
.millust end
This discards the contents of the current edit buffer.
.np
To quit every file that you are editing, the
.keyref quitall
command is used.  If no files have been modified, then you will immediately
exit &edvi.
:period.
If files have been modified, you will be asked to
verify whether or not you really want to exit the editor.
.np
The
.keyref write
command is used to write the current file.
If you specify  a file name, the edit buffer will be written to a file
with that name.
.millust begin
:write new.txt
.millust end
writes out a new file with the name
:fname.new.txt:efname.
:period.
.np
If the file name you specify already exists, you will see the message:
.millust begin
File exists - use w! to force
.millust end
To overwrite an existing file, use the exclamation point ('!'):
.millust begin
write! new.txt
.millust end
or
.millust begin
w! new.txt
.millust end
.np
If you are specifying a new file name, you may also specify a line range
to write to that new file. Some examples are:
.millust begin
:1,100 write new.txt       - write the first 100 lines to "new.txt".
:50 write a.txt            - write line 50 to "a.txt"
.millust end
.np
The
.keyref wq
(write and quit) and the
.keyref xit
(exit) commands both do the same thing.  They write out the current file
if it has been modified, and then exit the file.  This is the exact
same as typing 'ZZ' in &cmdmode
:period.
.*
.exercises
:OL.

:LI.Edit a file as follows:
.millust begin
vi abc
.millust end
Add the lines:
.millust begin
Line 1.
Line 2.
Line 3.
.millust end

:LI.Enter the &cmdline command
.keyref quit
(remember to press the colon (':') key to bring up the command window).
You will see the message:
.millust begin
File modified - use :q! to force
.millust end
Press
.param CTRL_G
:period.
The message window will indicate the following:
.millust begin
"abc" [modified] line 3 of 3  -- 100% --
.millust end
As you can see, the file has been modified, so you are not
allowed to quit.
.np

:LI.Enter the &cmdline command
.keyref write
:period.
You will see the message:
.millust begin
"abc" 3 lines, 27 bytes
.millust end
This indicates that the file has been written.  Now press
.param CTRL_G
:cont.,
and you will see:
.millust begin
"abc" line 3 of 3  -- 100% --
.millust end
Notice that the file no longer is marked as modified once it is
written.

:LI.Try the
&cmdline command
.keyref quit
again.  This time, you will be able to quit the file, since
the file has been written, and is no longer marked as modified.

:LI.Re-edit the file "abc".  Enter command:
.millust begin
:1,2 w def
.millust end
This will write out a new file called "def".
Now quit &edvi.
:period.

:LI.Edit the file "def".  Notice that it contains the lines
.millust begin
Line 1.
Line 2.
.millust end
These are the first two lines of "abc", that you wrote to this file.
Try entering the command:
.millust begin
:write abc
.millust end
You will see the message
.millust begin
File exists - use w! to force
.millust end
Since "abc" already exists, you are not allowed to overwrite it, unless
you specify the exclamation point, as follows:
.millust begin
:write! abc
.millust end

:LI.Re-edit the file "abc".  Delete the last line.  Press
.param CTRL_G
:cont.,
and you will see that the file is modified.  Now, enter the
&cmdline command (remembering to press ':'):
.millust begin
:q!
.millust end
You will exit the file, even though it has been modified.

:LI.Re-edit the file "abc", and delete the last line.  Enter the
&cmdline command
.keyref xit
:period.
This will save the file and exit it,
and because you are not editing any other files, you will exit &edvi
:period.
You could also use the command
.keyref wq
to do the same thing.  Both of these commands do the same thing as
pressing 'ZZ' while in &cmdmode
:period.

:LI.Start up &edvi as follows:
.millust begin
vi abc def
.millust end
This will edit two files, "abc" and "def". Enter the &cmdline command
.keyref quitall
and you will exit &edvi
:period.

:LI.Repeat the previous example, but add a line to one of the two files.
Now enter the &cmdline command
.keyref quitall
:period.
In this case, you will be prompted with
.millust begin
Files are modified, really exit?
.millust end
Reply with a 'y', and you will exit &edvi, even though files are modified.

:eOL.
.* ******************************************************************
.section 'Using the Mouse'
.* ******************************************************************
.np
You may use the mouse for many things.  You may select text, relocate
the cursor, resize a window, move a window, use the scroll bar, or
use the menus.
.np
Text selection will be discussed in the next section.  Using menus with
the mouse was discussed in the previous chapter, in the section
:HDREF refid='basmenu'.
:period.
.np
By simply moving your mouse cursor to a location in an edit window
and clicking the left mouse button, the cursor will move to that
position.
.np
By moving your mouse to the top border of an edit window and pressing
down the left mouse button, you can move the window around
by moving your mouse.  When you release the button, the window
will move to the new position.
.np
By moving your mouse to the bottom right hand corner of an edit window
(to the vertical two-headed arrow)
and pressing down the left mouse button, you can resize the window by
moving your mouse. When you release the left button, the window
will be redrawn in its new size.
.np
Edit windows have scroll bars which indicate the position in the file and
allow you to position to different portions of the file.  The scroll thumb
(the solid block on the scroll bar) indicates the relative location
of your current cursor position in the file.  If the scroll thumb is
at the top, then you are on the top line of the file.  If it is at
the bottom, then you at the end of the file.
.np
By left-clicking on the single
arrows at the top or the bottom of the scroll bar, the edit window
will scroll up or down a single line.  If you hold the left mouse button
down, then the window will scroll continuously.
.np
If you click the left mouse button in the scrollbar region between the
thumb and the top arrow,
you will move up a page in the file you are editing.  If you click
the left mouse button below the scroll thumb, you will move down a page
in the file you are editing.  If you hold the left mouse button down,
then you will page continuously.
.np
By pressing and holding down the left mouse button on the scroll
thumb, you can set the edit position yourself.  As you drag the scroll thumb
up and down, the edit window will be redrawn to show you the corresponding
portion of your file.
.* ******************************************************************
.section 'Selecting Text'
.* ******************************************************************
.np
&edvi has the ability to highlight (select) text, either on an individual
line or a group of lines, and then do various actions on the highlighted
(selected) text.
.np
You may select text with either the keyboard or the mouse.  The
keyboard interface is as follows:
:INCLUDE file='textsel.gml'
.np
Once text selection has been started, then any movement command may
be used to expand or shrink the selected region.  Multiple line selections
always select complete lines.  A portion of a single line can be
selected (i.e. a word) by growing the selection left or right.  A portion
of a line is called a column region and a multiple line selection is
called a line region.
.np
You can select text with the mouse by holding down the right or left mouse
button, and moving the mouse up and down or left and right.
When using the right button, a selection menu will appear after the mouse
button is released , from which you choose what
you wish to do with the selected text.
.np
If you highlight a region by holding down the left button and moving
the mouse, then releasing the button has no effect
(the region simply remains highlighted).  This region may then be operated
on from the &cmdline, using different &cmdmode commands, or by right-clicking
the mouse in the selected region.
.np
If you click the right mouse button (right-click) while the mouse cursor
is in a highlighted (selected) set of lines, then a selection menu
for the lines appears.  If you right-click in a selected group of characters
on a single line (a column region) then a selection menu for that column
region appears.  The two menus are different.  You may also bring up
these menus by pressing the underscore
('_') key.
.np
If you click the right mouse button (right-click) in an unselected region,
the current word will be highlighted and the selection menu will appear.
This can also be accomplished by pressing the underscore ('_') key.
.np
If you double click the left mouse button, the current word will be
highlighted and the selection menu will appear.  However, the word selected here
is slightly different than the word selected by clicking the right mouse
button.  This word is defined to include the characters '.', ':' and '\', so
that double clicking the left mouse button on a file name will select the
entire path.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Click the left mouse button when the mouse cursor is on the top line,
and drag your mouse down until the first 10 lines are selected.
If you do not have a mouse, then press the shift key and cursor down
until the first 10 lines are selected.
.np
Now, click the right mouse button somewhere inside the selected
region.  If you do not have a mouse, press the underscore ('_') key. A
menu will appear.
The selected region and the menu will appear as follows:
.figure *depth='2.47' *scale='59' *file='vi019' Selected Lines menu
From this menu, you can either delete or yank (copy) the lines.  You
may cancel the menu by pressing
.param ESC
(the region remains highlighted). You can cancel the selected region by
pressing
.param ESC
again.
.np
If you click your left mouse button somewhere outside the selected region,
both the menu and the selected region will be cancelled.

:LI.Make sure the selected region is cancelled (press
.param ESC
until it is gone).
Now press the right mouse button on the word "This" in the first line of
the file.  If you do not have a mouse, then position the cursor somewhere
in the word "This" and press the underscore ('_') key.  You will see the following
menu appear:
.figure *depth='2.47' *scale='59' *file='vi020' Selected Columns menu
You may do a number of things from the popup menu:
.*
:DL break.

:DT.Open
:DD.Open (edit) the file indicated by the highlighted (selected) text.  The
selected text is treated like a file name, and an edit session for that file is
started.  The file will be given the name of the highlighted text.

:DT.Change
:DD.Change the selected word.

:DT.Delete
:DD.Delete the selected word.

:DT.Yank
:DD.Yank (copy) the selected word.

:DT.Fgrep
:DD.Search the current directory for any files containing the selected word.
See the &cmdline command
.keyref fgrep
in the chapter
:HDREF refid='cmds'.
for more information.

:DT.Tag
:DD.Search your tags file for the selected word.
See the &cmdline command
.keyref tag
in the chapter
:HDREF refid='cmds'.
for more information.

:eDL.
.*
:LI.Make sure the selected region is cancelled.  Then add the following
line to the start of the file (use the &cmdmode key capital o ('O')
to open a line above the first line):
.millust begin
#include &lt.c:\h\test.h&gt.
.millust end
Now, try right mouse clicking on parts of the file name.  Notice how
only individual pieces of the file name are selected.  Now, try double
clicking the left mouse button somewhere on the file name.  The
menu from the previous example will appear, selecting the entire file name
as shown below:
.figure *depth='2.47' *scale='59' *file='vi021' Double Click Selection

:LI.Try using
.param CTRL_R
to start a selection and then move around in your file.  Use the
.param ESC
key to cancel your selection.

:LI.Try using the shifted cursor keys to select lines.  Use the
.param ESC
key to cancel your selection.

:eOL.
.* ******************************************************************
.section 'Joining Text'
.* ******************************************************************
.np
&edvi has the ability to join two lines together.  If you press the
letter 'J' (capital 'j') while in &cmdmode, then the next line will join to the
end of the current line.  All white space except for a single space
will be removed. For example, typing 'J' while on the first line in these
two lines:
.millust begin
This is a line.
      This is another line.
.millust end
produces the line:
.millust begin
This is a line. This is another line.
.millust end
If you precede 'J' with a repeat count, then that many lines after the current
line will be joined to the current line.  For example, typing
.millust begin
4J
.millust end
while on the first line of:
.millust begin
Line 1.
   Line 2.
   Line 3.
Line 4.
    Line 5.
    Line 6.
.millust end
will produce the result:
.millust begin
Line 1. Line 2. Line 3. Line 4. Line 5.
    Line 6.
.millust end
.np
There is also a &cmdline command called
.keyref join
that is used to join lines of text together.  This command is used
as follows:
.millust begin
&lt.linerange&gt. join
.millust end
The lines in the specified range
.param &lt.linerange&gt.
are joined together.  If a single line number is specified, then the
line after that line is joined to the specified line.  If no line number
is specified, then the line after the current line is joined to the current
line.  For example, the command:
.millust begin
:1,5 join
.millust end
will cause the lines 1 through 5 of the file to be joined into a single
line.
.* ******************************************************************
.section 'Using Marks'
.* ******************************************************************
.np
:INCLUDE file='markinfo'.
.np
A mark is useful in that you do not have to remember a specific line
number, you just need to remember the letter that you picked for the
mark name.   You can then return to the line or even the exact position
on the line easily.
.np
You may set a mark by pressing the letter 'm' (in &cmdmode) and pressing one
of the letters from 'a' to 'z'.  For example, if you type
.millust begin
ma
.millust end
you will set the mark 'a' at the current position in the file, and
you will see the following message appear:
.millust begin
Mark 'a' set
.millust end
.np
You can set a mark with the &cmdline command
.keyref mark
:period.
The syntax of the command is
.millust begin
&lt.line&gt. mark &lt.letter&gt.
.millust end
You specify which line the mark is to be set on with
.param &lt.line&gt.
:period.
If no line is specified, the current line is assumed.  You specify
the mark id ('a'-'z') with
.param &lt.letter&gt.
:period.
.np
For example, the following &cmdline commands may be used to set marks:
.millust begin
mark a   - sets the mark 'a' on the current line.
5 mark b - sets the mark 'b' on line 5.
.millust end
.np
Once you have set a mark, you may return to the mark by pressing either
the front quote (apostrophy) (') or the back quote (`), followed by the
letter of the mark you wish to return to.  Using the back quote causes you
to return to the row and column position of the mark.
Using the front quote (apostrophy) causes you to return to the line with the
mark (the cursor moves to the first column on the line).
.np
For example, after setting the mark 'a', you can return to it by typing:
.millust begin
'a   - return to the line with the mark 'a'.
`a   - return to the exact position with the mark 'a'.
.millust end
.np
Marks are useful when you need to go searching a file for something,
but you want to be able to return to a specific position.  They are
also useful when deleting and copying text (see the section
:HDREF refid='dcapt'.
later on in this chapter).
.np
For more information on marks, see the section
:HDREF refid='marks'
in the chapter
:HDREF page=no refid='modes'
:period.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Cursor to the letter 'i' in the word 'is' on the first line, and type
.millust begin
ma
.millust end
This will set the mark 'a' at that position.

:LI.Page down twice.  Now, type
.millust begin
`a
.millust end
You will be moved to the 'i' in the word 'is' on the first line.

:LI.Page down twice.  Now, type
.millust begin
'a
.millust end
You will be moved to the first column of the first line.

:LI.Go to the bottom of the file.  Now, enter the &cmdline command
.keyref mark
as follows (remember to press colon (':') to bring up the command window)
.millust begin
:mark z
.millust end

:LI.Go to the top of the file (using CTRL_PAGEUP), and enter the
&cmdline
.millust begin
:'z
.millust end
This will return you to the last line of the file

:LI.Now, type the following &cmdline
.millust begin
:'a
.millust end
This will take you back to line 1, where you set the mark 'a'.

:eOL.
.* ******************************************************************
.section *refid=stext 'Searching for Text'
.* ******************************************************************
.np
You can search a file for a string in either a forwards or backwards
direction.
By using the &cmdmode key '/', you are prompted for a string to
search for in the forwards direction.  By using the &cmdmode key '?',
you are prompted for a string to search for in the backwards direction.
.np
When the string is found, your cursor is moved to the first character
of the string, and the string is highlighted.
.np
If the string (for example, "abc") is not found, you will see the message:
.millust begin
String 'abc' not found
.millust end
.np
When you press '/' (forward search), the following window will appear:
.figure *depth='2.47' *scale='59' *file='vi022' Search String Entry Window
This window behaves just like the command window:
you may cursor back and forth in the command window, and use the
backspace and delete keys to change mistakes.  Once you press
.param ENTER
:cont.,
the command will be processed.  If you press
.param ESC
:cont.,
the search in cancelled.
.np
If you press '?' (backwards search), you will be able to enter a backwards
search string.
.np
If you press 'n', &edvi will take you to the next occurrence of the
last search string, searching in the same direction as the last
search command.
.np
If you press 'N', &edvi will take you to the next occurrence of the
last search string, only it will search in the opposite direction as
the last search command.
.np
:INCLUDE file='rxexp'.
.np
The section
:HDREF refid='srching'.
in the chapter
:HDREF page=no refid='modes'.
describes the searching in more detail.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Press the forward slash ('/') key, and enter the string "this".
The word "This" on the first line will be highlighted, and your cursor
will be on the 'T'. (notice that the search is case insensitive).

:LI.Now press, the 'n' key.  You will move to the word "This" on the
second line.

:LI.Press 'n' two more times.  You will now be on the word "This" on
the fourth line.

:LI.Press 'N'.  You will move to the word "This" on the third line.

:LI.Press the question mark ('?') key, and enter the string "1".  You
will move to the '1' on the first line.

:LI.Press 'n'.  The search will wrap around to the end of the file,
and search backwards until the '1' on line 21 is encountered.

:LI.Press 'n'.  You will move to the '1' on line 19.

:LI.Press 'N'.  You will move back to the '1' on line 21.

:eOL.
.* ******************************************************************
.section *refid=dcapt 'Deleting, Copying, and Pasting Text'
.* ******************************************************************
.np
There are two useful commands in &edvi for deleting and
copying text.  In &cmdmode, you press the 'd' key to start the delete
sub-mode.  You press the 'y' key to start the yank (copy) sub-mode.  Each
of these sub-modes is indicated on the mode indicator on the menu bar:
.millust begin
Mode: delete
.millust end
or
.millust begin
Mode: yank
.millust end
.np
Once you have entered the sub-mode, you can then specify one of the following
operations:
:INCLUDE file='operlist'.
.np
Some examples are:
.millust begin
dw     -  delete a word
yr     -  yank (copy) highlighted region
d/text - delete up to the word "text".
.millust end
.np
These commands can be preceded by an optional repeat count.  This repeat
count specifies the number of times that the command will be executed.
If the repeat count is not specified, the command is executed once.
For example:
.millust begin
3dw - delete 3 words
2yy - copy 2 lines
.millust end
.np
A &copybuffer can be specified between the repeat count and the command.
The buffer is identified by preceding it with double quotes (").  For
example:
.millust begin
3"ayy - copy 3 lines into buffer 'a'.
"zdd  - delete 3 lines into buffer 'z'.
.millust end
The optional &copybuffer is a place where deleted or yanked text resides.  If
you do not specify a buffer, then the active &copybuffer is assumed.
There are 9 numbered buffers (1-9) that may be selected as the active
buffer (buffer 1 is the default active buffer).  When text enters the
active &copybuffer, the old contents of the active buffer spills into the
next buffer.  The contents of each buffer spills into the next, with the end
buffer (9) losing its contents.
.np
The active copy buffer may be selected by pressing
.param CTRL_F1
through
.param CTRL_F9
in &cmdmode.
:period.
When you do this, a message appears showing you which buffer
has been selected, how many lines/characters are in the buffer, and the first
line of the buffer.
.np
There are also 26 named buffers, 'a' through 'z'.  The contents of
these buffers is constant over the life of your editing session.  They
retain their contents until you update them.
.np
For more information on copy buffers, see the section
:HDREF refid='cpybuff'
in the chapter
:HDREF page=no refid='modes'
:period.
.np
There is also a
.keyref delete
&cmdline command and a
.keyref yank
&cmdline command for deleting and yanking text.  These commands only
operate on line ranges.  Their syntax is:
.millust begin
&lt.range&gt. delete &lt."?&gt.
&lt.range&gt. yank &lt."?&gt.
.millust end
The
.param &lt."?&gt.
indicates a &copybuffer,
and
.param &lt.range&gt.
indicates a line range.  If no line range is specified, then the
current line is assumed.  Some examples are:
.millust begin
:1,5 delete        - delete lines 1 to 5
:1,$ yank "a       - copy all lines into buffer 'a'
:d                 - deletes the current line
.millust end
.np
In the section
:HDREF refid='scap'.
in the previous chapter, you learned about the small 'p' and the
capital 'p' ('P') &cmdmode commands to put (paste) copied or deleted
text into the edit buffer.  Remember, small 'p' is used to paste after the
current cursor position, and capital 'p' ('P') is use to paste before
the current cursor position.
.np
If what you deleted or yanked was a sequence of characters on a single line,
then these characters are inserted into the current line when you paste.
If you deleted or yanked whole lines, then those lines are inserted
around the current line when you paste.
.np
The put (paste) &cmdline commands accept a &copybuffer as the buffer
to paste out of.  The default is the active &copybuffer, but any
buffer can be specified. Once again,
.param &lt."?&gt.
indicates a &copybuffer:
.millust begin
&lt."?&gt.p
&lt."?&gt.P
.millust end
.np
As well, there is a &cmdline command for pasting text: the
.keyref put
command. The syntax is:
.millust begin
&lt.line&gt. put &lt.!&gt. &lt."?&gt.
.millust end
If the line
.param &lt.line&gt.
is specified, then the buffer is pasted around the specified line instead
of the current line.
.np
If the exclamation mark is specified, then contents of the &copybuffer
are pasted before the specified line.  Otherwise, the contents of the
&copybuffer are pasted after the specified line.
.np
The
.param &lt."?&gt.
is an optional &copybuffer.
:period.
If it is not specified, then the active &copybuffer is used.  The double
quotes (") must be specified.
.np
Deleting text is discussed in more detail in the section
:HDREF refid='deltext'.
in the chapter
:HDREF page=no refid='modes'.
:period.
.np
Copying text is discussed in more detail in the section
:HDREF refid='cpytext'.
in the chapter
:HDREF page=no refid='modes'.
:period.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Press 'dd'.  This will delete the first line of the file.

:LI.Press 'dw'. This will delete the first word of the line.

:LI.Press 'd$'.  This will delete to the end of the line.

:LI.Press 'dj'.  This will delete the current line and the next line.

:LI.Press '2dw'.  This will delete the first two words of the current line:
.millust begin
04 This is a test line.
.millust end
leaving you with
.millust begin
is a test line.
.millust end

:LI.Press '2w'. This will move you to the letter 't' in "test".  Press
capital p ('P').
The 2 words you deleted will be inserted before the 't':
.millust begin
is a 04 This test line.
.millust end

:LI.Type the following &cmdmode keys:
.millust begin
"add
.millust end
You will see the message:
.millust begin
1 lines deleted into buffer a
.millust end

:LI.Press 'p'.  Note that you inserted the two words you deleted before
into the current line.

:LI.Type:
.millust begin
"ap
.millust end
This will insert the contents of buffer 'a' (line line you deleted) after
the current line.

:LI.Type the following:
.millust begin
2"byy
.millust end
This will copy the next two lines into buffer "b".  You will see the message
.millust begin
2 lines yanked into buffer b
.millust end

:LI.Type:
.millust begin
"bP
.millust end
This will insert the two lines you yanked before the current line.

:LI.Type
.millust begin
3"zyw
.millust end
This will yank three words into &copybuffer 'z'.
:INCLUDE file='exerquit'.

:eOL.
.* ******************************************************************
.section 'Altering Text'
.* ******************************************************************
.np
You could change text by deleting the text and then entering insert mode.
However, &edvi provides a special method for doing both of these
things at once.  By pressing the 'c' key in &cmdmode, you enter the
change sub-mode.  You will see the mode line indicate:
.millust begin
Mode: change
.millust end
If you are changing characters on a line, the characters will be highlighted.
If you press the
.param ESC
key, the change will be cancelled.
Once you type a character, the characters will be deleted and you will
enter &tinsmode.
:period.
.np
If you are changing whole lines, the lines are deleted and you enter
&tinsmode.
:period.
.np
Once you have entered the change sub-mode, you can then specify one of the
following operations:
:INCLUDE file='operlist'.
Some examples are:
.millust begin
cw - change current word
c$ - change to the end of the current line
.millust end
.np
You can also specify an optional repeat count before the change command.
This will cause the change command to be repeated that many times.
For example:
.millust begin
2cw - change two words
3cc - change three lines
.millust end
.np
The changing of text is discussed in greater detail in the section
:HDREF refid='cmchg'.
of the chapter
:HDREF page=no refid='modes'
:period.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Type 'cw'.  This first word "01" will be highlighted.  Press
.param ESC
:period.
The word will return to normal, and nothing will happen.

:LI.Type 'cw' again.  This time, type the following:
.millust begin
This is new text.
.millust end
and press the
.param ESC
key to exit &tinsmode
:period.
Your original line:
.millust begin
01 This is a test line.
.millust end
will now be:
.millust begin
This is new text. This is a test line.
.millust end

:LI.Type '2cc'.  The first two lines will be deleted, and you will enter
text insertion mode.  Type the following:
.millust begin
This is more new text.
.millust end

:LI.Return to &cmdmode by pressing the
.param ESC
key. Cursor to the letter 'i' in the word "is" and type 'c$'.  This
will highlight
from the letter 'i' to the end of line.  Enter the
text:
.millust begin
This is more new text.
.millust end

:INCLUDE file='exerquit'.

:eOL.
.* ******************************************************************
.section 'Undo and Redo'
.* ******************************************************************
.np
&edvi has an unlimited undo capacity (constrained only by memory and
disk space).  Every change that you make
to a file is remembered, and can be undone in the reverse order that
you made the changes.
.np
A change can be undone by pressing the letter 'u' while in
&cmdmode
:period.
You can undo further changes by pressing 'u' repeatedly.
.np
If you undo a change you wanted to keep, you can
.keyword redo
it by pressing capital 'u' ('U'). Each time
you press 'U', an undo is re-done.  Once you undo changes, you cannot
redo them after you modify the file.
.np
The &cmdline command for undoing changes has the following syntax:
.millust begin
undo
.millust end
If you specify an exclamation point ('!') after the
.keyref undo
keyword, then you will do a redo
instead of an undo:
.millust begin
undo!
.millust end
.np
See the section
:HDREF refid='cmundo'
in the chapter
:HDREF page=no refid='modes'
for more information.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:period.
Delete the first line, delete the second line, then delete the third
line (one at a time).  Then, press 'u' in command mode.  The third
line will come back.

:LI.Press 'u' again.  The second line will come back.

:LI.Enter the &cmdline command:
.millust begin
:undo
.millust end
The first line will come back.

:LI.Enter the &cmdline command
.millust begin
:undo
.millust end
again.  This undo command cannot do anything, since you have already
undo all the changes to your file.  You will see the message
.millust begin
No more undos
.millust end
appear.

:LI.Press the capital u ('U') key in &cmdmode.
:period.
The first line will now disappear, as you are undoing your undo.

:LI.Press 'U' again.  The second line will disappear.

:LI.Enter the &cmdline command (remember to press ':'):
.millust begin
undo!
.millust end
The third line will disappear.

:LI.Enter the &cmdline command:
.millust begin
undo!
.millust end
You will see the message:
.millust begin
No more undos
.millust end

:eOL.
.* ******************************************************************
.section 'Repeating Edit Operations'
.* ******************************************************************
.np
When you enter a &cmdmode command to modify your file
you can re-execute the sequence by pressing the dot ('.') key.
.np
For example, suppose you entered the &cmdmode command:
.millust begin
3dd
.millust end
to delete 3 lines.  If you press '.' after this, you will delete another
3 lines.
.np
There are two useful extension to this as well.  &edvi has concepts
known as
.keyword memorize mode
and
.keyword alternate memorize mode
:period.
You enter memorize mode
by pressing the letter 'm' (also used to set a mark) followed by a dot ('.').
You will then see the message
.millust begin
Memorize Mode started
.millust end
From this point forward, every key that you type is memorized.  When you
are done memorizing, you can then press the dot ('.') key while in &cmdmode,
and this ends memorize mode.
You will see the message:
.millust begin
Memorize Mode ended
.millust end
.np
Once you have memorized a key sequence, you can re-execute it by pressing dot ('.').
This will cause &edvi to behave as if all the keys you memorized were
being typed by you again.
.np
The memorized sequence will be lost the next time you
change the text other than with '.'.  To memorize a sequence that will
always be remembered, you can use
.keyword alternate memorize mode
:period.
This mode is used the same way as memorize mode,
only you use an equals sign ('=') instead of a dot ('.').
.*
.exercises
:OL.

:LI.Edit the file "atest" from the Exercises section of
:HDREF refid='maiaf'
:period.
Delete the first two lines by typing the &cmdmode command:
.millust begin
2dd
.millust end
Now press the dot ('.') key.  Two more lines will be deleted.

:LI.Move to the first column of the first line.  Type the &cmdmode command:
.millust begin
m.
.millust end
This will start you in memorize mode. Now, type:
.millust begin
dwjdwj.
.millust end
This deletes the word on the first line, goes down to the next line,
deletes another word on the next line, and goes down one more line.
The final dot ('.') terminates memorize mode.

:LI.Now, press dot ('.').  This will delete the first word of the next
two lines.

:INCLUDE file='exerquit'.

:eOL.

