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
.* 11-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id='inter'.Intermediate Usage
This chapter discusses a number of the commonly used features of &edname.
:PERIOD.
The knowledge of the information in the chapter
:HDREF page=no refid='basic'.
is assumed.
:SECTION.The Command Line
&edname has a powerful set of commands that are entered in a special
command window. These commands
are referred to as &cmdline commands.  You can activate the command window
in two ways:
:OL.
:LI.Select the
:HILITE.Enter command
item under the
:HILITE.Control
menu.
:LI.Press the colon (':') key when in &cmdmode
:PERIOD.
Remember to press the
:HILITE.ESC
key to ensure that you are in &cmdmode
before pressing ':'.
:eOL.
Once you have done one of the previous things, the following window will
appear on your screen:
:PICTURE file='cmdenter' text='Command Entry Window'.
You may enter a command in this window (for example: quit).  If you wish
to cancel the command that you are typing, just press the
:HILITE.ESC
key and the window will disappear.
:P.
You may cursor back and forth in the command window, and use the
backspace and delete keys to change mistakes.  Once you press
:HILITE.ENTER
:CONT.,
the command will be processed.
:P.
If you cursor up, you will go through a list of commands
that you have entered at the command window (newest to oldest).  This
is your command
:ITALICS.history
:PERIOD.
Cursoring
down will take you through the history from oldest to newest.  This is
very useful if you have typed a complicated command and did not get it
quite right or if you just wish to execute the command again.
:P.
The chapter
:HDREF refid='cmds'.
describes the &cmdline in more detail.
:SUBSECT.Line Numbers
Some &cmdline commands accept a line address or a line range as a
parameter.
For example, when specifying the
:KEYWORD.write
command, you may specify
:ILLUST.
 :write
:eILLUST.
or you may specify
:ILLUST.
 :1,10 write
:eILLUST.
:P.
A line address is a number or a sum of numbers.  As well,
some special symbols may be used:
:DEFLIST.
:DEFITEM.. (dot)
Represents the current line number.
:DEFITEM.$ (dollar)
Represents the last line number.
:DEFITEM.'a (front quote)
Indicates the line with the mark 'a' set; marks 'a' through 'z'
may be used.  Marks are discussed later in this chapter.
:eDEFLIST.
:P.
A line range is two line addresses separated by a comma.
:P.
Some examples of line addresses and line ranges are:
:ILLUST.
 .+5     - five lines past the current line.

:bILLUST.
 'a      - the line with mark a.

:bILLUST.
 $       - the last line

:bILLUST.
 1,5     - lines 1 to 5

:bILLUST.
 .,$     - current line in file to end line of file

:bILLUST.
 .-3,100 - the line 3 before the current to line 100
:eILLUST.
Line addresses are discussed in greater detail in the section
:HDREF refid='lineadd'.
:PERIOD.
:P.
If you just enter a line address on its own on the &cmdline, then
you will go directly to that line.
:eSUBSECT.

:EXERCISES.
:OL.
:LI.Start up &edname, and try selecting the
:HILITE.Control
menu, and then selecting the
:HILITE.Enter command
item.  Notice how the window pops up.  Try typing and cursoring around.
When you are done, press the
:HILITE.ESC
key to cancel the command.
:LI.Make sure that you are in &cmdmode, then press the colon (':') key.
Once again, the command window pops up.
Try typing and cursoring around. When you are done, press the
:HILITE.ESC
key to cancel the command.
:LI.Add 10 lines to your file.  Then press the colon (':') key and
enter the number 5.  You will go to line 5.  Try entering different
numbers and see what happens.   If you enter a line number that
does not exist, you will see the message:
:ILLUST.
No such line
:eILLUST.
:LI.Now that you have a number of commands entered, try cursoring up
and down in the command window.  You will see all the commands that
you have typed.
:eOL.
:eEXERCISES.

:SECTION.Getting a File for Editing
In the chapter
:HDREF refid='basic' page=no.
:CONT.,
you saw that you could edit a file by either specifying the name on the
command line when invoking &edname, or by selecting the
:HILITE.File
menu and picking the
:HILITE.Edit new file
option.
:P.
The general way to edit a new file is to use the &cmdline command
:KEYWORD.edit
:PERIOD.
To enter this command, make sure that you are in &cmdmode and press
the colon (':') key.  Once the command prompt is displayed, then
simply type
:KEYWORD.edit
(optionally followed by a file name or a list of files), and then press
:HILITE.ENTER
:PERIOD.
:P.
If you do not specify a file, then a directory listing is displayed.
:INCLUDE file='fsdesc'.
:P.
You may also specify one or more files after the
:KEYWORD.edit
command. 
If a file you specify is the same as one already being edited,
then control is simply transferred to the window with that file.
:EXERCISES.
:OL.
:LI.Start up &edname without any files specified. Then try entering the
&cmdline command
:KEYWORD.edit
without any parameters (remember to press the colon (':') key first).
You will see a screen similar to the following:
:PICTURE file='filesel' text='File Selection Display'.
Try changing to other directories or drives
with this list.  When you are done, press the
:HILITE.ESC
key to cancel the selection list.
:LI.Try entering the following &cmdline
:ILLUST.
 :e afile
:eILLUST.
This will cause &edname to start editing a new file called "afile".
:LI.Try entering the command
:ILLUST.
 :e bfile cfile
:eILLUST.
This will cause &edname to edit two new files, one named "bfile"
and one named "cfile".
:LI.Enter the command
:ILLUST.
 :e afile
:eILLUST.
This returns you to the first file ("afile") that you were already editing.
:LI.Press
:HILITE.CTRL_C
:CONT., and &edname will quit all the files that you have started editing.
:eOL.
:eEXERCISES.

:SECTION.Moving Between Files
There are a number of ways to move between files that you are editing.
As you have seen in the previous section,  you can move to a file
that you are already editing by using the &cmdline command
:KEYWORD.edit
and specifying the name of the file you wish to move to.
:P.
You may press the
:HILITE.F1
key while in &cmdmode or &tinsmode
:PERIOD.
This function key moves you to the next file in the list of files that
you are editing.
:P.
You may press the
:HILITE.F2
key while in &cmdmode or &tinsmode
:PERIOD.
This function key moves you to the previous file in the list of files that
you are editing.
:P.
The previous two function keys that you may use also have &cmdline
equivalents.
The &cmdline command
:KEYWORD.next
moves you to the next file in the list of files that you are editing.
:P.
The &cmdline command
:KEYWORD.prev
moves you to the previous file in the list of files that you are editing.
:P.
It is also possible to display a list of all files that you are currently
editing.  You may press the
:HILITE.F7
key in either &cmdmode or &tinsmode, or enter the &cmdline command
:KEYWORD.files
:PERIOD.
Doing any one of these things will cause a list of all files currently
being edited to appear.  An asterisk ('*') will precede files that
have been modified.
From this list, you may go to one of the files,
quit one of the files, or save one of the files then quit it.
:EXERCISES.
:OL.
:LI.Start up &edname in the following way:
:ILLUST.
vi a b c
:eILLUST.
This will cause you to edit three new files.
Now, press the
:HILITE.F7
key.  The following will appear:
:PICTURE file='files' text='Current File List'.
Select the file
:HILITE.b
:PERIOD.
That file will become the current file being edited.
:LI.Type the &cmdline
:ILLUST.
 :files
:eILLUST.
You will see the same result as you saw in the previous example.
Press the
:HILITE.ESC
key to cancel this display.
:LI.Press the
:HILITE.F1
key several times.  You will rotate through the three files that you
are editing.
:LI.Press the
:HILITE.F2
key several times.  You will rotate through the three files that you
are editing, but in the opposite order than when you were pressing the
:HILITE.F1
key.
:LI.Use the &cmdline commands
:KEYWORD.next
and
:KEYWORD.prev
to move through the files.  These commands behave the same as
pressing
:HILITE.F1
and
:HILITE.F2
:PERIOD.
:LI.Press
:HILITE.CTRL_C
:CONT., and &edname will quit all the files that you have started editing.
:eOL.
:eEXERCISES.

:SECTION id='maiaf'.Moving Around in a File
You have already learned to use the cursor keys to move around through
a file.  When you are in &cmdmode, there are a number of keys that
also cause movement through the file. Many of these keys may be
preceded with a repeat count.  You enter the repeat count
by typing a number (which will be echoed in a special window on the
screen).
:P.
Once you have entered the repeat count, you may cancel it
by pressing the
:HILITE.ESC
key, or you may follow it with a movement command.  For example, if
you type:
:ILLUST.
3<Down Arrow Key>
:eILLUST.
you will move down three lines instead of one.
:P.
The basic &cmdmode movement commands are:
:INCLUDE file='basicmv'.
:P.
There are additional commands that move you around the file which do not
require your fingers to move off the home row of your keyboard.
For a touch typist, this is a great advantage.
For a list of all of the movement commands, see the section
:HDREF refid='mvement'
in the chapter
:HDREF page=no refid='modes'
:PERIOD.
The following list of movement commands move you around on the
current line:
:DEFLIST.
:DEFITEM.$ (dollar sign)
Move to the end of the current line.
:DEFITEM.0 (zero)
Move to the start of the current line.
:DEFITEM.b
Move backwards to the previous word on the current line. If preceded
with a repeat count, you move back that many words.
:DEFITEM.h
Move right through the text.
If preceded with a repeat count, then you move right that many characters.
:DEFITEM.l
Move left through the text.
If preceded with a repeat count, then you move left that many characters.
:DEFITEM.w
Move forward to the next word on the current line. If preceded
with a repeat count, you move forward that many words.
:DEFITEM.B
Move backwards to the previous whitespace delimited word on the current line.
If preceded with a repeat count, you move back that many words.
:DEFITEM.W
Move forward to the next whitespace delimited word on the current line.
If preceded with a repeat count, you move forward that many words.
:eDEFLIST.
:P.
The following list of movement commands move you to other lines:
:DEFLIST.
:DEFITEM.CTRL_B
Move back one page in the text.
If preceded with a repeat count, you will move back that many pages.
:DEFITEM.CTRL_D
Move down one half page in the text.
If preceded with a repeat count, then you move forward that many lines.
As well, any future
:HILITE.CTRL_D
or
:HILITE.CTRL_U
commands issued will move that many lines, instead of a half page.
:DEFITEM.CTRL_F
Move forward one page in the text.
If preceded with a repeat count, you will move forward that many pages.
:DEFITEM.CTRL_U
Move up one half page in the text.
If preceded with a repeat count, then you move backwards that many lines.
As well, any future
:HILITE.CTRL_U
or
:HILITE.CTRL_D
commands issued will move that many lines, instead of a half page.
:DEFITEM.j
Move down through the text.
If preceded with a repeat count, then you move down that many lines.
:DEFITEM.k
Move up through the text.
If preceded with a repeat count, then you move up that many lines.
:DEFITEM.G
Moves to the last line in the file.  If preceded with a repeat
count, then you move to that line in the file.
:DEFITEM.H
Move to the top of the current edit window.
If preceded with a repeat count, you move that many lines from the
top of the edit window.
:DEFITEM.L
Move to the bottom of the current edit window.
If preceded with a repeat count, you move that many lines from the
bottom of the edit window.
:DEFITEM.M
Move to the middle of the current edit window.
:eDEFLIST.
:EXERCISES.
:OL.
:LI.Edit a new file, "atest".  Once you have edited this file, add the line:
:ILLUST.
This is a test line.
:eILLUST.
Once you have done this, copy this line by pressing 'Y'. Press 'p' to paste
in the copy.  Press 'p' 28 more times, so that you create a file with
30 lines in it (all just like the first line).
:LI.So that we can more easily see the results, type the following &cmdline:
:ILLUST.
 :%s/^/\# /
:eILLUST.
This is a substitution command.  It will replace the start of each of your
lines with the line number.  We will learn about the substitution command
in the next chapter.  When you are done, you should see a screen similar to
the following:
:PICTURE file='atest' text='"ATEST" File Contents '.
:LI.Press the 'G' key.  You will move to the last line of the file.
:LI.Type the following:
:ILLUST.
15G
:eILLUST.
This will move you to line 15.
:LI.Try using
:HILITE.CTRL_F
and
:HILITE.CTRL_B
:PERIOD.
Notice that they behave just like
:HILITE.PAGEUP
and
:HILITE.PAGEDOWN
:PERIOD.
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
:ILLUST.
3H
:eILLUST.
will move your cursor to the 3rd line from the top of the edit window.
:LI.Press the 'M' key.  The cursor will move to the middle of edit window.
:LI.Press the '$' key.  The cursor will move to the end of the current line.
:LI.Press the '0' (zero) key.  The cursor will move to the start of the current line.
:LI.Press
:HILITE.CTRL_D
:PERIOD.
You will move down half a page
:LI.Type the number '2' and then press CTRL_D.  Notice that you
only move down 2 lines.
:LI.Press
:HILITE.CTRL_D
:PERIOD.
You will move down 2 lines.
:LI.Press
:HILITE.CTRL_U
:PERIOD.
You will move up 2 lines.
:LI.Press 'ZZ' to save the file.  This file will be used in later exercises.
:eOL.
:eEXERCISES.

:SECTION.Saving and Exiting a File Revisited
We have already seen in the section
:HDREF refid='savexit'
a number of ways to save and exit your files.  These methods included
typing 'ZZ' and using the menus.
:P.
There are a number of different &cmdline commands that can be used for
saving and/or quitting your files.
:OL.
:LI.:KEYWORD.quit
("!") or :CMDPARM.q
("!")
:LI.:KEYWORD.quitall
:LI.:KEYWORD.write
("!") or :CMDPARM.w
("!")
:LI.:KEYWORD.wq
:LI.:KEYWORD.xit
:EOL.
The
:KEYWORD.quit
command is used to exit a file without saving it.  If the file has
been modified, the command will fail and the message:
:ILLUST.
File modified - use :q! to force
:eILLUST.
will be displayed.  To quit a modified file, the exclamation point ('!')
is used:
:ILLUST.
 :quit!
:eILLUST.
or
:ILLUST.
 :q!
:eILLUST.
This discards the contents of the current edit buffer.
:P.
To quit every file that you are editing, the
:KEYWORD.quitall
command is used.  If no files have been modified, then you will immediately
exit &edname.
:PERIOD.
If files have been modified, you will be asked to
verify whether or not you really want to exit the editor.
:P.
The
:KEYWORD.write
command is used to write the current file.
If you specify  a file name, the edit buffer will be written to a file
with that name.
:ILLUST.
 :write new.txt
:eILLUST.
writes out a new file with the name
:ITALICS new.txt
:PERIOD.
:P.
If the file name you specify already exists, you will see the message:
:ILLUST.
File exists - use w! to force
:eILLUST.
To overwrite an existing file, use the exclamation point ('!'):
:ILLUST.
write! new.txt
:eILLUST.
or
:ILLUST.
w! new.txt
:eILLUST.
:P.
If you are specifying a new file name, you may also specify a line range
to write to that new file. Some examples are:
:ILLUST
 :1,100 write new.txt       - write the first 100 lines to "new.txt".
 :50 write a.txt            - write line 50 to "a.txt"
:eILLUST.
:P.
The
:KEYWORD.wq
(write and quit) and the
:KEYWORD.xit
(exit) commands both do the same thing.  They write out the current file
if it has been modified, and then exit the file.  This is the exact
same as typing 'ZZ' in &cmdmode
:PERIOD.
:EXERCISES.
:OL.
:LI.Edit a file as follows:
:ILLUST.
vi abc
:eILLUST.
Add the lines:
:ILLUST.
Line 1.
Line 2.
Line 3.
:eILLUST.

:LI.Enter the &cmdline command
:KEYWORD.quit
(remember to press the colon (':') key to bring up the command window).
You will see the message:
:ILLUST.
File modified - use :q! to force
:eILLUST.
Press
:HILITE.CTRL_G
:PERIOD.
The message window will indicate the following:
:ILLUST.
"abc" [modified] line 3 of 3  -- 100% --
:eILLUST.
As you can see, the file has been modified, so you are not
allowed to quit.
:P.
:LI.Enter the &cmdline command
:KEYWORD.write
:PERIOD.
You will see the message:
:ILLUST.
"abc" 3 lines, 27 bytes
:eILLUST.
This indicates that the file has been written.  Now press
:HILITE.CTRL_G
:CONT.,
and you will see:
:ILLUST.
"abc" line 3 of 3  -- 100% --
:eILLUST.
Notice that the file no longer is marked as modified once it is
written.
:LI.Try the
&cmdline command
:KEYWORD.quit
again.  This time, you will be able to quit the file, since
the file has been written, and is no longer marked as modified.
:LI.Re-edit the file "abc".  Enter command:
:ILLUST.
 :1,2 w def
:eILLUST.
This will write out a new file called "def".
Now quit &edname.
:PERIOD.
:LI.Edit the file "def".  Notice that it contains the lines
:ILLUST.
Line 1.
Line 2.
:eILLUST.
These are the first two lines of "abc", that you wrote to this file.
Try entering the command:
:ILLUST.
 :write abc
:eILLUST.
You will see the message
:ILLUST.
File exists - use w! to force
:eILLUST.
Since "abc" already exists, you are not allowed to overwrite it, unless
you specify the exclamation point, as follows:
:ILLUST.
 :write! abc
:eILLUST.
:LI.Re-edit the file "abc".  Delete the last line.  Press
:HILITE.CTRL_G
:CONT.,
and you will see that the file is modified.  Now, enter the
&cmdline command (remembering to press ':'):
:ILLUST
 :q!
:eILLUST.
You will exit the file, even though it has been modified.
:LI.Re-edit the file "abc", and delete the last line.  Enter the
&cmdline command
:KEYWORD.xit.
This will save the file and exit it,
and because you are not editing any other files, you will exit &edname
:PERIOD.
You could also use the command
:KEYWORD.wq
to do the same thing.  Both of these commands do the same thing as
pressing 'ZZ' while in &cmdmode
:PERIOD.
:LI.Start up &edname as follows:
:ILLUST.
vi abc def
:eILLUST.
This will edit two files, "abc" and "def". Enter the &cmdline command
:KEYWORD.quitall
and you will exit &edname
:PERIOD.
:LI.Repeat the previous example, but add a line to one of the two files.
Now enter the &cmdline command
:KEYWORD.quitall
:PERIOD.
In this case, you will be prompted with
:ILLUST.
Files are modified, really exit?
:eILLUST.
Reply with a 'y', and you will exit &edname, even though files are modified.
:eOL.
:eEXERCISES.

:SECTION.Using the Mouse
You may use the mouse for many things.  You may select text, relocate
the cursor, resize a window, move a window, use the scroll bar, or
use the menus.
:P.
Text selection will be discussed in the next section.  Using menus with
the mouse was discussed in the previous chapter, in the section
:HDREF refid='basmenu'.
:PERIOD.
:P.
By simply moving your mouse cursor to a location in an edit window
and clicking the left mouse button, the cursor will move to that
position.
:P.
By moving your mouse to the top border of an edit window and pressing
down the left mouse button, you can move the window around
by moving your mouse.  When you release the button, the window
will move to the new position.
:P.
By moving your mouse to the bottom right hand corner of an edit window
(to the vertical two-headed arrow)
and pressing down the left mouse button, you can resize the window by
moving your mouse. When you release the left button, the window
will be redrawn in its new size.
:P.
Edit windows have scroll bars which indicate the position in the file and
allow you to position to different portions of the file.  The scroll thumb
(the solid block on the scroll bar) indicates the relative location
of your current cursor position in the file.  If the scroll thumb is
at the top, then you are on the top line of the file.  If it is at
the bottom, then you at the end of the file.
:P.
By left-clicking on the single
arrows at the top or the bottom of the scroll bar, the edit window
will scroll up or down a single line.  If you hold the left mouse button
down, then the window will scroll continuously.
:P.
If you click the left mouse button in the scrollbar region between the
thumb and the top arrow,
you will move up a page in the file you are editing.  If you click
the left mouse button below the scroll thumb, you will move down a page
in the file you are editing.  If you hold the left mouse button down,
then you will page continuously.
:P.
By pressing and holding down the left mouse button on the scroll
thumb, you can set the edit position yourself.  As you drag the scroll thumb
up and down, the edit window will be redrawn to show you the corresponding
portion of your file.

:SECTION.Selecting Text
&edname has the ability to highlight (select) text, either on an individual
line or a group of lines, and then do various actions on the highlighted
(selected) text.
:P.
You may select text with either the keyboard or the mouse.  The
keyboard interface is as follows:
:INCLUDE file='textsel.gml'
:P.
Once text selection has been started, then any movement command may
be used to expand or shrink the selected region.  Multiple line selections
always select complete lines.  A portion of a single line can be
selected (i.e. a word) by growing the selection left or right.  A portion
of a line is called a column region and a multiple line selection is
called a line region.
:P.
You can select text with the mouse by holding down the right or left mouse
button, and moving the mouse up and down or left and right.
When using the right button, a selection menu will appear after the mouse
button is released , from which you choose what
you wish to do with the selected text.
:P.
If you highlight a region by holding down the left button and moving
the mouse, then releasing the button has no effect
(the region simply remains highlighted).  This region may then be operated
on from the &cmdline, using different &cmdmode commands, or by right-clicking
the mouse in the selected region.
:P.
If you click the right mouse button (right-click) while the mouse cursor
is in a highlighted (selected) set of lines, then a selection menu
for the lines appears.  If you right-click in a selected group of characters
on a single line (a column region) then a selection menu for that column
region appears.  The two menus are different.  You may also bring up
these menus by pressing the underscore
('_') key.
:P.
If you click the right mouse button (right-click) in an unselected region,
the current word will be highlighted and the selection menu will appear.
This can also be accomplished by pressing the underscore ('_') key.
:P.
If you double click the left mouse button, the current word will be
highlighted and the selection menu will appear.  However, the word selected here
is slightly different than the word selected by clicking the right mouse
button.  This word is defined to include the characters '.', ':' and '\', so
that double clicking the left mouse button on a file name will select the
entire path.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Click the left mouse button when the mouse cursor is on the top line,
and drag your mouse down until the first 10 lines are selected.
If you do not have a mouse, then press the shift key and cursor down
until the first 10 lines are selected.
:P.
Now, click the right mouse button somewhere inside the selected
region.  If you do not have a mouse, press the underscore ('_') key. A
menu will appear.
The selected region and the menu will appear as follows:
:PICTURE file='mlsel' text='Selected Lines Menu'.
From this menu, you can either delete or yank (copy) the lines.  You
may cancel the menu by pressing
:HILITE.ESC
(the region remains highlighted). You can cancel the selected region by
pressing
:HILITE.ESC
again.
:P.
If you click your left mouse button somewhere outside the selected region,
both the menu and the selected region will be cancelled.

:LI.Make sure the selected region is cancelled (press
:HILITE.ESC
until it is gone).
Now press the right mouse button on the word "This" in the first line of
the file.  If you do not have a mouse, then position the cursor somewhere
in the word "This" and press the underscore ('_') key.  You will see the following
menu appear:
:PICTURE file='mcsel' text='Selected Columns Menu'.
You may do a number of things from the popup menu:
:DEFLIST.
:DEFITEM.Open
Open (edit) the file indicated by the highlighted (selected) text.  The
selected text is treated like a file name, and an edit session for that file is
started.  The file will be given the name of the highlighted text.
:DEFITEM.Change
Change the selected word.
:DEFITEM.Delete
Delete the selected word.
:DEFITEM.Yank
Yank (copy) the selected word.
:DEFITEM.Fgrep
Search the current directory for any files containing the selected word.
See the &cmdline command
:KEYWORD.fgrep
in the chapter
:HDREF refid='cmds'.
for more information.
:DEFITEM.Tag
Search your tags file for the selected word.
See the &cmdline command
:KEYWORD.tag
in the chapter
:HDREF refid='cmds'.
for more information.
:eDEFLIST.
:LI.Make sure the selected region is cancelled.  Then add the following
line to the start of the file (use the &cmdmode key capital o ('O')
to open a line above the first line):
:ILLUST.
#include <c:\h\test.h>
:eILLUST.
Now, try right mouse clicking on parts of the file name.  Notice how
only individual pieces of the file name are selected.  Now, try double
clicking the left mouse button somewhere on the file name.  The
menu from the previous example will appear, selecting the entire file name
as shown below:
:PICTURE file='mcdcsel' text='Double Click Selection'.
:LI.Try using
:HILITE.CTRL_R
to start a selection and then move around in your file.  Use the
:HILITE.ESC
key to cancel your selection.
:LI.Try using the shifted cursor keys to select lines.  Use the
:HILITE.ESC
key to cancel your selection.

:eOL.
:eEXERCISES.

:SECTION.Joining Text
&edname has the ability to join two lines together.  If you press the
letter 'J' (capital 'j') while in &cmdmode, then the next line will join to the
end of the current line.  All white space except for a single space
will be removed. For example, typing 'J' while on the first line in these
two lines:
:ILLUST.
This is a line.
      This is another line.
:eILLUSt.
produces the line:
:ILLUST.
This is a line. This is another line.
:eILLUST.
If you precede 'J' with a repeat count, then that many lines after the current
line will be joined to the current line.  For example, typing
:ILLUST.
4J
:eILLUST.
while on the first line of:
:ILLUST.
Line 1.
   Line 2.
   Line 3.
Line 4.
    Line 5.
    Line 6.
:eILLUST.
will produce the result:
:ILLUST.
Line 1. Line 2. Line 3. Line 4. Line 5.
    Line 6.
:eILLUST.
:P.
There is also a &cmdline command called
:KEYWORD.join
that is used to join lines of text together.  This command is used
as follows:
:ILLUST.
<linerange> join
:eILLUST.
The lines in the specified range
:HILITE.<linerange>
are joined together.  If a single line number is specified, then the
line after that line is joined to the specified line.  If no line number
is specified, then the line after the current line is joined to the current
line.  For example, the command:
:ILLUST.
 :1,5 join
:eILLUST.
will cause the lines 1 through 5 of the file to be joined into a single
line.
:SECTION.Marks
:INCLUDE file='markinfo'.
:P.
A mark is useful in that you do not have to remember a specific line
number, you just need to remember the letter that you picked for the
mark name.   You can then return to the line or even the exact position
on the line easily.
:P.
You may set a mark by pressing the letter 'm' (in &cmdmode) and pressing one
of the letters from 'a' to 'z'.  For example, if you type
:ILLUST
ma
:eILLUST
you will set the mark 'a' at the current position in the file, and
you will see the following message appear:
:ILLUST.
Mark 'a' set
:eILLUST.
:P.
You can set a mark with the &cmdline command
:KEYWORD.mark
:PERIOD.
The syntax of the command is
:ILLUST.
<line> mark <letter>
:eILLUST.
You specify which line the mark is to be set on with
:HILITE.<line>
:PERIOD.
If no line is specified, the current line is assumed.  You specify
the mark id ('a'-'z') with
:HILITE.<letter>
:PERIOD.
:P.
For example, the following &cmdline commands may be used to set marks:
:ILLUST.
 mark a   - sets the mark 'a' on the current line.
 5 mark b - sets the mark 'b' on line 5.
:eILLUST.
:P.
Once you have set a mark, you may return to the mark by pressing either
the front quote (apostrophy) (') or the back quote (`), followed by the
letter of the mark you wish to return to.  Using the back quote causes you
to return to the row and column position of the mark.
Using the front quote (apostrophy) causes you to return to the line with the
mark (the cursor moves to the first column on the line).
:P.
For example, after setting the mark 'a', you can return to it by typing:
:ILLUST.
 'a   - return to the line with the mark 'a'.
 `a   - return to the exact position  with the mark 'a'.
:eILLUST.
:P.
Marks are useful when you need to go searching a file for something,
but you want to be able to return to a specific position.  They are
also useful when deleting and copying text (see the section
:HDREF refid='dcapt'.
later on in this chapter).
:P.
For more information on marks, see the section
:HDREF refid='marks'
in the chapter
:HDREF page=no refid='modes'
:PERIOD.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Cursor to the letter 'i' in the word 'is' on the first line, and type
:ILLUST.
ma
:eILLUST.
This will set the mark 'a' at that position.
:LI.Page down twice.  Now, type
:ILLUST.
`a
:eILLUST.
You will be moved to the 'i' in the word 'is' on the first line.
:LI.Page down twice.  Now, type
:ILLUST.
'a
:eILLUST.
You will be moved to the first column of the first line.
:LI.Go to the bottom of the file.  Now, enter the &cmdline command
:KEYWORD.mark
as follows (remember to press colon (':') to bring up the command window)

:ILLUST.
 :mark z
:eILLUST.
:LI.Go to the top of the file (using CTRL_PAGEUP), and enter the
&cmdline
:ILLUST.
 :'z
:eILLUST.
This will return you to the last line of the file
:LI.Now, type the following &cmdline
:ILLUST
 :'a
:eILLUST.
This will take you back to line 1, where you set the mark 'a'.
:eOL.
:eEXERCISES.

:SECTION id='stext'.Searching for Text
You can search a file for a string in either a forwards or backwards
direction.
By using the &cmdmode key '/', you are prompted for a string to
search for in the forwards direction.  By using the &cmdmode key '?',
you are prompted for a string to search for in the backwards direction.
:P.
When the string is found, your cursor is moved to the first character
of the string, and the string is highlighted.
:P.
If the string (for example, "abc") is not found, you will see the message:
:ILLUST.
String 'abc' not found
:eILLUST.
:P.
When you press '/' (forward search), the following window will appear:
:PICTURE file='srchent' text='Search String Entry Window'.
This window behaves just like the command window:
you may cursor back and forth in the command window, and use the
backspace and delete keys to change mistakes.  Once you press
:HILITE.ENTER
:CONT.,
the command will be processed.  If you press
:HILITE.ESC
:CONT.,
the search in cancelled.
:P.
If you press '?' (backwards search), you will be able to enter a backwards
search string.
:P.
If you press 'n', &edname will take you to the next occurrence of the
last search string, searching in the same direction as the last
search command.
:P.
If you press 'N', &edname will take you to the next occurrence of the
last search string, only it will search in the opposite direction as
the last search command.
:P.
:INCLUDE file='rxexp'.
:P.
The section
:HDREF refid='srching'.
in the chapter
:HDREF page=no refid='modes'.
describes the searching in more detail.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
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
:eEXERCISES.

:SECTION id='dcapt'.Deleting, Copying, and Pasting Text
There are two useful commands in &edname for deleting and
copying text.  In &cmdmode, you press the 'd' key to start the delete
sub-mode.  You press the 'y' key to start the yank (copy) sub-mode.  Each
of these sub-modes is indicated on the mode indicator on the menu bar:
:ILLUST.
Mode: delete
:eILLUST
or
:ILLUST.
Mode: yank
:eILLUST
:P.
Once you have entered the sub-mode, you can then specify one of the following
operations:
:INCLUDE file='operlist'.
Some examples are:
:ILLUST.
dw     -  delete a word
yr     -  yank (copy) highlighted region
d/text - delete up to the word "text".
:eILLUST.
:P.
These commands can be preceded by an optional repeat count.  This repeat
count specifies the number of times that the command will be executed.
If the repeat count is not specified, the command is executed once.
For example:
:ILLUST.
3dw - delete 3 words
2yy - copy 2 lines
:eILLUST.
:P.
A &copybuffer can be specified between the repeat count and the command.
The buffer is identified by preceding it with double quotes (").  For
example:
:ILLUST.
3"ayy - copy 3 lines into buffer 'a'.
"zdd  - delete 3 lines into buffer 'z'.
:eILLUST.
The optional &copybuffer is a place where deleted or yanked text resides.  If
you do not specify a buffer, then the active &copybuffer is assumed.
There are 9 numbered buffers (1-9) that may be selected as the active
buffer (buffer 1 is the default active buffer).  When text enters the
active &copybuffer, the old contents of the active buffer spills into the
next buffer.  The contents of each buffer spills into the next, with the end
buffer (9) losing its contents.
:P.
The active copy buffer may be selected by pressing
:HILITE.CTRL_F1
through
:HILITE.CTRL_F9
in &cmdmode.
:PERIOD.
When you do this, a message appears showing you which buffer
has been selected, how many lines/characters are in the buffer, and the first
line of the buffer.
:P.
There are also 26 named buffers, 'a' through 'z'.  The contents of
these buffers is constant over the life of your editing session.  They
retain their contents until you update them.
:P.
For more information on copy buffers, see the section
:HDREF refid='cpybuff' in the chapter
:HDREF page=no refid='modes'
:PERIOD.
:P.
There is also a
:KEYWORD.delete
&cmdline command and a
:KEYWORD.yank
&cmdline command for deleting and yanking text.  These commands only
operate on line ranges.  Their syntax is:
:ILLUST.
 <range> delete <"?>
 <range> yank <"?>
:eILLUST.
The
:HILITE.<"?>
indicates a &copybuffer,
and
:HILITE.<range>
indicates a line range.  If no line range is specified, then the
current line is assumed.  Some examples are:
:ILLUST.
 :1,5 delete        - delete lines 1 to 5
 :1,$ yank "a       - copy all lines into buffer 'a'
 :d                 - deletes the current line
:eILLUST.
:P.
In the section
:HDREF refid='scap'.
in the previous chapter, you learned about the small 'p' and the
capital 'p' ('P') &cmdmode commands to put (paste) copied or deleted
text into the edit buffer.  Remember, small 'p' is used to paste after the
current cursor position, and capital 'p' ('P') is use to paste before
the current cursor position.
:P.
If what you deleted or yanked was a sequence of characters on a single line,
then these characters are inserted into the current line when you paste.
If you deleted or yanked whole lines, then those lines are inserted
around the current line when you paste.
:P.
The put (paste) &cmdline commands accept a &copybuffer as the buffer
to paste out of.  The default is the active &copybuffer, but any
buffer can be specified. Once again, 
:HILITE.<"?>
indicates a &copybuffer:
:ILLUST.
 <"?>p
 <"?>P
:eILLUST.
:P.
As well, there is a &cmdline command for pasting text: the
:KEYWORD.put
command. The syntax is:
:ILLUST.
 <line> put <!> <"?>
:eILLUST.
If the line
:HILITE.<line>
is specified, then the buffer is pasted around the specified line instead
of the current line.
:P.
If the exclamation mark is specified, then contents of the &copybuffer
are pasted before the specified line.  Otherwise, the contents of the
&copybuffer are pasted after the specified line.
:P.
The
:HILITE.<"?>
is an optional &copybuffer.
:PERIOD.
If it is not specified, then the active &copybuffer is used.  The double
quotes (") must be specified.
:P.
Deleting text is discussed in more detail in the section
:HDREF refid='deltext'.
in the chapter
:HDREF page=no refid='modes'.
:PERIOD.
:P.
Copying text is discussed in more detail in the section
:HDREF refid='cpytext'.
in the chapter
:HDREF page=no refid='modes'.
:PERIOD.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Press 'dd'.  This will delete the first line of the file.
:LI.Press 'dw'. This will delete the first word of the line.
:LI.Press 'd$'.  This will delete to the end of the line.
:LI.Press 'dj'.  This will delete the current line and the next line.
:LI.Press '2dw'.  This will delete the first two words of the current line:
:ILLUST.
04 This is a test line.
:eILLUST.
leaving you with
:ILLUST.
is a test line.
:eILLUST.
:LI.Press '2w'. This will move you to the letter 't' in "test".  Press
capital p ('P').
The 2 words you deleted will be inserted before the 't':
:ILLUST.
is a 04 This test line.
:eILLUST.
:LI.Type the following &cmdmode keys:
:ILLUST.
"add
:eILLUST.
You will see the message:
:ILLUST.
1 lines deleted into buffer a
:eILLUST.
:LI.Press 'p'.  Note that you inserted the two words you deleted before
into the current line.
:LI.Type:
:ILLUST.
"ap
:eILLUST.
This will insert the contents of buffer 'a' (line line you deleted) after
the current line.
:LI.Type the following:
:ILLUST.
2"byy
:eILLUST.
This will copy the next two lines into buffer "b".  You will see the message
:ILLUST.
2 lines yanked into buffer b
:eILLUST.
:LI.Type:
:ILLUST.
"bP
:eILLUST.
This will insert the two lines you yanked before the current line.
:LI.Type
:ILLUST.
3"zyw
:eILLUST.
This will yank three words into &copybuffer 'z'.
:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.

:SECTION.Changing Text
You could change text by deleting the text and then entering insert mode.
However, &edname provides a special method for doing both of these
things at once.  By pressing the 'c' key in &cmdmode, you enter the
change sub-mode.  You will see the mode line indicate:
:ILLUST.
Mode: change
:eILLUST.
If you are changing characters on a line, the characters will be highlighted.
If you press the
:HILITE.ESC
key, the change will be cancelled.
Once you type a character, the characters will be deleted and you will
enter &tinsmode.
:PERIOD.
:P.
If you are changing whole lines, the lines are deleted and you enter
&tinsmode.
:PERIOD.
:P.
Once you have entered the change sub-mode, you can then specify one of the
following operations:
:INCLUDE file='operlist'.
Some examples are:
:ILLUST.
cw - change current word
c$ - change to the end of the current line
:eILLUST.
:P.
You can also specify an optional repeat count before the change command.
This will cause the change command to be repeated that many times.
For example:
:ILLUST.
2cw - change two words
3cc - change three lines
:eILLUST.
:P.
The changing of text is discussed in greater detail in the section
:HDREF refid='cmchg'.
of the chapter
:HDREF page=no refid='modes'
:PERIOD.

:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Type 'cw'.  This first word "01" will be highlighted.  Press
:HILITE.ESC
:PERIOD.
The word will return to normal, and nothing will happen.
:LI.Type 'cw' again.  This time, type the following:
:ILLUST.
This is new text.
:eILLUST.
and press the
:HILITE.ESC
key to exit &tinsmode
:PERIOD.
Your original line:
:ILLUST.
01 This is a test line.
:eILLUST.
will now be:
:ILLUST.
This is new text. This is a test line.
:eILLUST.
:LI.Type '2cc'.  The first two lines will be deleted, and you will enter
text insertion mode.  Type the following:
:ILLUST.
This is more new text.
:eILLUST.
:LI.Return to &cmdmode by pressing the
:HILITE.ESC
key. Cursor to the letter 'i' in the word "is" and type 'c$'.  This
will highlight
from the letter 'i' to the end of line.  Enter the
text:
:ILLUST.
This is more new text.
:eILLUST.

:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.

:SECTION.Undo and Redo
&edname has an unlimited undo capacity (constrained only by memory and
disk space).  Every change that you make
to a file is remembered, and can be undone in the reverse order that
you made the changes.
:P.
A change can be undone by pressing the letter 'u' while in
&cmdmode
:PERIOD.
You can undo further changes by pressing 'u' repeatedly.
:P.
If you undo a change you wanted to keep, you can
:ITALICS.redo
it by pressing capital 'u' ('U'). Each time
you press 'U', an undo is re-done.  Once you undo changes, you cannot
redo them after you modify the file.
:P.
The &cmdline command for undoing changes has the following syntax:
:ILLUST.
undo 
:eILLUST.
If you specify an exclamation point ('!') after the
:KEYWORD.undo
keyword, then you will do a redo
instead of an undo:
:ILLUST.
undo!
:eILLUST.
:P.
See the section
:HDREF refid='cmundo'
in the chapter
:HDREF page=no refid='modes'
for more information.

:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Delete the first line, delete the second line, then delete the third
line (one at a time).  Then, press 'u' in command mode.  The third
line will come back.
:LI.Press 'u' again.  The second line will come back.
:LI.Enter the &cmdline command:
:ILLUST.
 :undo
:eILLUST.
The first line will come back.
:LI.Enter the &cmdline command
:ILLUST.
 :undo
:eILLUST.
again.  This undo command cannot do anything, since you have already
undo all the changes to your file.  You will see the message
:ILLUST.
No more undos
:eILLUST.
appear.
:LI.Press the capital u ('U') key in &cmdmode.
:PERIOD.
The first line will now disappear, as you are undoing your undo.
:LI.Press 'U' again.  The second line will disappear.
:LI.Enter the &cmdline command (remember to press ':'):
:ILLUST.
undo!
:eILLUST.
The third line will disappear.
:LI.Enter the &cmdline command:
:ILLUST.
undo!
:eILLUST.
You will see the message:
:ILLUST.
No more undos
:eILLUST.
:eOL.
:eEXERCISES.

:SECTION.Repeating Edit Operations
When you enter a &cmdmode command to modify your file
you can re-execute the sequence by pressing the dot ('.') key.
:P.
For example, suppose you entered the &cmdmode command:
:ILLUST.
3dd
:eILLUST.
to delete 3 lines.  If you press '.' after this, you will delete another
3 lines.
:P.
There are two useful extension to this as well.  &edname has concepts
known as
:KEYWORD.memorize mode
and
:KEYWORD.alternate memorize mode
:PERIOD.
You enter
:KEYWORD.memorize mode
by pressing the letter 'm' (also used to set a mark) followed by a dot ('.').
You will then see the message
:ILLUST.
Memorize Mode started
:eILLUST.
From this point forward, every key that you type is memorized.  When you
are done memorizing, you can then press the dot ('.') key while in &cmdmode,
and this ends
:KEYWORD.memorize mode
:PERIOD.
You will see the message:
:ILLUST.
Memorize Mode ended
:eILLUST.
:P.
Once you have memorized a key sequence, you can re-execute it by pressing dot ('.').
This will cause &edname to behave as if all the keys you memorized were
being typed by you again.
:P.
The memorized sequence will be lost the next time you
change the text other than with '.'.  To memorize a sequence that will
always be remembered, you can use
:KEYWORD.alternate memorize mode
:PERIOD.
This mode is used the same way as
:KEYWORD.memorize mode
:CONT.,
only you use an equals sign ('=') instead of a dot ('.').
:EXERCISES.
:OL.
:LI.Edit the file "atest" from the Exercises section of
:HDREF refid='maiaf'
:PERIOD.
Delete the first two lines by typing the &cmdmode command:
:ILLUST.
2dd
:eILLUST.
Now press the dot ('.') key.  Two more lines will be deleted.
:LI.Move to the first column of the first line.  Type the &cmdmode command:
:ILLUST.
m.
:eILLUST.
This will start you in memorize mode. Now, type:
:ILLUST.
dwjdwj.
:eILLUST.
This deletes the word on the first line, goes down to the next line,
deletes another word on the next line, and goes down one more line.
The final dot ('.') terminates memorize mode.
:LI.Now, press dot ('.').  This will delete the first word of the next
two lines.
:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.
