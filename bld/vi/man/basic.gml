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
.* 10-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id='basic'.Basic Usage
:SECTION.Invoking &edname
&edname is invoked by typing the command
:ILLUST.
vi
:eILLUST.
at the command prompt.  Try typing this.  You will see something similar
to the following:
:PICTURE file='vistart' text='&edname Display'.
:P.
If you are running DOS on a 386 (or higher)
machine, then you can also use the
:ITALICS.protected mode
version of &edname..  This version of &edname runs as a native 386
application, which means that it has access to more memory and will do its
processing more quickly. You can start this version by typing
:ILLUST.
vip
:eILLUST.
at the command prompt.
:P.
&edname also accepts one or more files as an optional parameter.  Each
file that you specify will be edited by &edname..  For example, typing
:ILLUST.
vi test.c
:eILLUST.
will edit the file
:ITALICS.test.c
:PERIOD.
Typing
:ILLUST.
vi test.c other.c
:eILLUST.
will edit the files
:ITALICS.test.c
and
:ITALICS.other.c
:PERIOD.
Typing
:ILLUST.
vi *.c
:eILLUST.
will edit all files that have the extension
:ITALICS..c
:PERIOD.
:P.
:INCLUDE file='exitvi'.

:SECTION.What is a Modal Editor?
A modal editor is one where keystrokes have different meaning, depending
on what mode you are in.  &edname has two main modes, &cmdmode and
&tinsmode.
:PERIOD.
:P.
While in &cmdmode, pressing a key on your keyboard can cause something
different to happen.  For example, pressing the letter D (capital 'd')
deletes all characters from the current cursor position to the end of the
line.
If the same letter 'D' (is pressed while in &tinsmode,
the letter 'D' appears in your text (much as you would expect).
:P.
It is easy to tell what mode you are in while using &edname.. 
When you are in &cmdmode, the cursor is a thin line.  When
you are in &tinsmode, the cursor is much thicker: if you are inserting
text, the cursor is one half of a full block, and if you are overstriking
text, the cursor is a full block.
:P.
The mode indicator on the menu bar at the
top of your screen shows the current mode.
While in &cmdmode, you will see the following:
:ILLUST.
Mode: command
:eILLUST.
While in &tinsmode, you will see either:
:ILLUST.
Mode: insert
:eILLUST.
or
:ILLUST.
Mode: overstrike
:eILLUST.
depending on whether you are inserting or overstriking text.
:P.
&edname is in &cmdmode by default.  There are numerous ways to enter
&tinsmode, the easiest of which is to press the
:HILITE.Insert
key on your keyboard.
:P.
The
:HILITE.ESC
key is one of the more important keys in &edname.. Pressing
:HILITE.ESC
once will stop whatever you are doing and
return you to &cmdmode.
:PERIOD.
If you have started a &cmdmode sequence, pressing
:HILITE.ESC
will cancel the command and return you to &cmdmode.
:PERIOD.
:EXERCISES.
Start up &edname by typing
:ILLUST.
vi
:eILLUST.
at your command prompt.
:P.
You will see a screen that looks like the following:
:PICTURE file='vistart' text='&edname Display'.
:P.
Press the insert (
:HILITE.INS
) key.  The cursor will get larger and you will see the mode indicator change
from
:ILLUST.
Mode: command
:eILLUST.
to
:ILLUST.
Mode: insert
:eILLUST.
Try typing some characters.  Notice that they appear in the edit window
as you type.  When you are done, press the
:HILITE.ESC
key.  The cursor will get smaller and you will see the mode indicator
change from
:ILLUST.
Mode: insert
:eILLUST.
to
:ILLUST.
Mode: command
:eILLUST.
:P.
:INCLUDE file='exitvi'.
:P.
Once you exit, you will be prompted with a message as follows:
:PICTURE file='exit' text='Really Exit Prompt'.
This lets you know that you are attempting to leave &edname while
there are still files that have been modified but not saved.
Press 'n' then the
:HILITE.ENTER
key to exit without saving your current file.
:eEXERCISES.

:SECTION.Some Basic Commands
To begin editing with &edname, there are only a few basic commands
that you need to learn.  Once you master these few commands, you
will be able to accomplish all basic editing tasks.
In later chapters, more sophisticated commands will be introduced which
allow advanced tasks to be completed more quickly.

:SUBSECT.Moving Around
When you are in &cmdmode, you may move around in your text using
various cursor keys.  The basic keys that you use to move around are:
:INCLUDE file='basicmv'
:P.
These same cursor keys may also be used when entering text in &tinsmode
:PERIOD.
:eSUBSECT.

:SUBSECT id='savexit'.Saving and Exiting a File
Once you have modified a file, may want to save it and either edit other
files or exit the editor.  When
you exit a modified file, you may want to either discard your changes or
keep them.
:P.
You can discard ALL of your files by
either pressing
:HILITE.CTRL_C
or selecting the
:HILITE.Exit
item in the
:HILITE.Control
menu.  Alternatively, you can discard or save the changes on
an individual file basis.
:P.
If you wish to save the current file you are editing and leave that
particular edit session, the fastest way to do it is to use the
command 'ZZ'.  Press
:HILITE.ESC
to make sure that you are in &cmdmode
:CONT., and then type two capital z's ('ZZ')
in a row.  This saves the file, then exits the edit buffer for the file.
If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edname will exit.
:P.
You may use certain menu items to save or discard changes to your file.
The main menu option
:HILITE.File
(by clicking on it with the mouse or by pressing
:HILITE.ALT_F
:CONT.)
has options for saving the current file. As well,
each edit window has a menu associated with it that you can access
by pressing
:HILITE.ALT_W
:PERIOD.
You can also access this menu by clicking on the gadget ('ð') in the top left hand
corner of an edit window.  This menu has a number of choices for dealing with
the file.
:P.
If you activate the
:HILITE.File
menu item, either with the mouse or by pressing
:HILITE.ALT_F,
a menu will be displayed as follows:
:PICTURE file='filemenu' text='File Menu'.
:P.
If you select the
:HILITE.Save current file
item, &edname will save the current file you are editing.  You will
remain editing the current file.
:P.
If you select the
:HILITE.Save current file and close
item, &edname will save the current file you are editing, and
close the edit window.
If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edname will exit.
:P.
If you activate the individual file menu either by clicking on the gadget
at the top-left corner of the edit window
with the mouse or by pressing
:HILITE.ALT_W
:CONT.,
then a menu will pop up as follows:
:PICTURE file='gadmenu' text='Individual Edit Window Menu'.
:P.
If you select the
:HILITE.Save current file
item, &edname will save the current file you are editing.  You will
remain editing the current file.
:P.
If you select the
:HILITE.Save & close
item, &edname will save the current file you are editing, and
exit that edit buffer.  If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edname will exit.
:P.
If you select the
:HILITE.Close no save
item, &edname will discard the current file without saving your
modifications. If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edname will exit.
:P.
If you select the
:HILITE.Close
item, &edname will try to discard the current file.  However, if
the file has been modified, then the option will fail.
:P.
:eSUBSECT.

:SUBSECT id='intext'.Inputting Text
You have already learned that pressing the
:HILITE.INS
key while in &cmdmode
puts &edname into insert mode.  Once you are in insert mode, you
may enter whatever text you like.  You may cursor around and modify
your text in whatever way you choose.  Once you are done, you
may press the
:HILITE.ESC
key to return to &cmdmode.
:PERIOD.
:P.
While you are in &tinsmode, the basic keys that allow you to move through
the text are:
:INCLUDE file='basicmv'
:P.
These keys allow you to manipulate the text while in &tinsmode:
:DEFLIST.
:DEFITEM.CTRL_DEL (Control-Delete)
Deletes the current line.

:DEFITEM.BS (Backspace key)
Deletes the character before the cursor, moving the cursor and the
rest of the line to the left one character.

:DEFITEM.DEL (Delete key)
Delete the character under the cursor.  If you are at the end of the line,
:ITALICS.DEL
has the same effect as pressing
:ITALICS.BS
:PERIOD.

:DEFITEM.ENTER
Start a new line.

:DEFITEM.INS
Toggles between inserting and overstriking text.

:eDEFLIST.
:P.
Along with the
:HILITE.INS
key, there are a number of other keys that you can press in &cmdmode
that will place you in &tinsmode, where you can edit text.
:PERIOD.
The difference between
all of these commands is where the cursor moves to before you start
inputting text.  The basic set of keys is:
:DEFLIST.
:DEFITEM.a
Starts appending (inserting) text after the current character. 
:DEFITEM.A
Starts appending (inserting) text after the last character on the
current line.
:DEFITEM.i
Starts inserting text at the current cursor position.  This is the
same as pressing the
:HILITE.INS
key.
:DEFITEM.I
Starts inserting text before the first non-white space character on
the current line.
:DEFITEM.o
Adds a blank line after the current line, and starts you inserting text
on the new line.
:DEFITEM.O
Adds a blank line before the current line, and starts you inserting text
on the new line.
:DEFITEM.R
Starts overstriking text at the current cursor position.
:DEFITEM.INS
Start inserting text at the current cursor position.
:eDEFLIST.
:P.
Remember, while in &tinsmode, the following mode indicator will appear
on the menu bar:
:ILLUST.
Mode: insert
:eILLUST.
or
:ILLUST.
Mode: overstrike
:eILLUST.
depending on whether you are inserting or overstriking text.
As well, the cursor will change to a larger block cursor
to help you remember that you are are inputting text.
:P.
To exit &tinsmode at any time, press the
:HILITE.ESC
key.  Your mode indicator will switch to
:ILLUST.
Mode: command
:eILLUST.
:eSUBSECT.

:EXERCISES.
:OL.
:LI.Edit a new file (called "test", for example) as follows:
:ILLUST.
vi test
:eILLUST.
You will see the following screen:
:PICTURE file='testex' text='Editing the New File "test"'.
Note the message window (the bottom two lines of the screen)
contains the message
:ILLUST.
"test" [new file] line 1 of 1  -- 100% --
:eILLUSt.
This message indicates that you are editing a new file called
test, that you are on line 1 of a 1 line file, and that line 1 is
100% of the way into the file.
:P.
You may see that message any time by pressing
:HILITE.CTRL_G
(hold down the Ctrl key and press the g key).
:P.
Enter &tinsmode by pressing the
:HILITE.INS
key, and type the following lines:
:ILLUST.
This is a test line.
This is another test line.
:eILLUST.
When you are done typing these lines, remember to press the
:HILITE.ESC
key to return to &cmdmode
:PERIOD.
:P.
Press
:HILITE.CTRL_G
(hold down Ctrl and press the g key).
The message window (the bottom two lines of the screen)
will show the message:
:ILLUST.
"test" [modified] line 2 of 2  -- 100% --
:eILLUST.
:P.
Now, press two capital z's in a row ('ZZ').  This will save the file
and return to the operating system command prompt.
:LI.Edit the file you created in the previous example, by typing
:ILLUST.
vi test
:eILLUST.
You will see the following screen:
:PICTURE file='testex2' text='Editing The File "test"'.
Note the message window (the bottom two lines of the screen)
contains the message
:ILLUST.
"test" line 1 of 2  -- 50% --
:eILLUSt.
This message is different than when you edited "test" for the first time.
The "[new file]" indicator is gone, since you have edited an existing file.
This message indicates to you that you are now editing a file called
test, that you are on line 1 of a 2 line file, and that line 1 is
50% of the way into the file.
:P.
Now try to experiment with the different ways of entering insert mode.
:OL.
:LI.Try pressing the 'A'(capital 'a') key.  This will move your cursor to
the end of the line, and will put you into insert mode.  Try adding some
text to this line, and press the
:HILITE.ESC
key when you are done.
:LI.Try pressing the 'I' key.  This will move your cursor to the start
of the line, and will put you into insert mode.  Try adding some
text to this line, and press the
:HILITE.ESC
key when you are done.
:LI.Try pressing the 'R' key.  This will start you in overstrike mode.
Note that the you have a very large cursor and that the mode indicator says:
:ILLUST.
Mode: overstrike
:eILLUST.
Try typing some characters.  As you type, what was there already is
replaced by what you type. Press the
:HILITE.ESC
key when you are done.
:LI.Try pressing the 'i' key.  This is the same as pressing the
:HILITE.INS
key. Now type some text, then press the
:HILITE.INS
key.  Note that your cursor turned to full height from half height, and the
mode indicator changed from:
:ILLUST.
Mode: insert
:eILLUST.
to
:ILLUST.
Mode: overstrike
:eILLUST.
Now, when you type, you will replace the existing characters.  Try
pressing the
:HILITE.INS
key again.  This time, your cursor will turn from full height to half height,
and the mode indicator will change from:
:ILLUST.
Mode: overstrike
:eILLUST.
to
:ILLUST.
Mode: insert
:eILLUST.
Remember to press the
:HILITE.ESC
key when you are done.
:LI.Try pressing the 'a' key.  This will move the cursor over one
character, and put you in insert mode.  Try cursoring around inside
your text using the arrow keys.  Use the
:HILITE.END
key to move to the end of a line, and the
:HILITE.HOME
key to move to the start of a line.
Press the
:HILITE.ESC
key when you are done.
:eOL.
:INCLUDE file='basicdis'.
:eOL.
:eEXERCISES.
:SUBSECT id='scap'.Cutting and Pasting

:SUBSECT.Lines
So far, you have learned how to edit a file, input some text, and
save or cancel your changes.  Now you will learn some simple ways of
deleting text and moving it elsewhere.
:P.
The section
:HDREF refid='intext'.
noted that pressing
:HILITE.CTRL_DEL
(control-delete) deleted a line when inputting
text.  This same command can be used while in &cmdmode
:PERIOD.
:P.
If you want to delete more than one line at once, you may precede
this delete command with a
:KEYWORD.repeat count.
You enter a 
:KEYWORD.repeat count
simply by typing numbers before you press
:HILITE.CTRL_DEL
:PERIOD.
As you type a number in &cmdmode, it will appear in a special window,
as follows:
:PICTURE file='repcnt' text='Repeat Count Display'.
The number that you are typing is displayed in the repeat count window.
As with all &cmdmode commands, if you decide that you have made a mistake,
just press the
:HILITE.ESC
key and the count you have been typing will be cancelled.
:P.
So, if you type a repeat count of 5 and then press
:HILITE.CTRL_DEL
:CONT.,
five lines will be deleted.  If you do not type a repeat count, then
one line will be deleted.
:P.
You can also delete a line by pressing the 'd' key twice in a row ('dd').
This has the exact same effect as pressing the
:HILITE.CTRL_DEL
key.  You may precede this command by a repeat count as well.  For example,
typing
:ILLUST.
12dd
:eILLUST.
deletes the next 12 lines in your file.
:P.
Once you have deleted some lines, you might want to paste them back in.
If you press
:HILITE.SHIFT_INS
(shift insert),
the line(s) you deleted will be pasted in after the current line.  If
you press
:HILITE.CTRL_INS
(control insert),
the line(s) you deleted will be pasted in before the current line (which
reverses the action of
:HILITE.CTRL_DEL
:CONT.)
:PERIOD.
:P.
There are two other keys for pasting, the letters
'p' and 'P' (small p and capital p).  Small 'p' pastes the line(s) you deleted after
the current line, just like
:HILITE.SHIFT_INS
:PERIOD.
Capital p ('P') pastes the line(s) you deleted before the current line,
just like
:HILITE.CTRL_INS
:PERIOD.
:P.
If you do not wish to delete some lines, but you do wish to copy them
so you can paste them somewhere else, then you may use the "yank"
command.  If you press the letter 'Y', or type the letters
'yy', you will "yank" (make a copy of) the current line.  Just like
'dd' or
:HILITE.CTRL_DEL,
you can precede these commands with a repeat count.  For example, typing
:ILLUST.
6Y
:eILLUST.
or
:ILLUST.
6yy
:eILLUST.
:eSUBSECT.
will copy 6 lines in your file.
:P.
Once you have yanked the lines, you may paste them in, as discussed above.
:SUBSECT.Characters
To delete characters while in &cmdmode, you can press the
:HILITE.DEL
key or the 'x' key.  The current character under the cursor will be deleted.
You may type a repeat count before you type
:HILITE.DEL
or 'x',
and that will delete a number of characters.
For example, typing
:MONO.3x
when the cursor is on 'T' in the line
:ILLUST.
Delete Test.
:eILLUST.
will leave you with
:ILLUST
Delete t.
:eILLUST.
and the cursor will be on the 't' just before the period.
:P.
If you wish to delete the character before the cursor, you may
press the letter 'X' (capital x).  This will delete the character before
the cursor, and move the cursor back one (i.e. the cursor stays on
the character that it was on before you pressed the 'X').
You may use a repeat count with this command. For example, typing
:MONO.3X
when the cursor is on 'T' in the line
:ILLUST.
Delete Test.
:eILLUST.
will leave you with
:ILLUST
DeleTest.
:eILLUST.
and the cursor will still be on the 'T'.
:P.
Once you have deleted some characters, you might want to paste them back in.
The keys involved are the exact same keys for pasting lines.  The
difference is that when you paste characters, the characters are
pasted into the current line, whereas when you paste lines, the lines are
pasted above or below the current line.
:P.
If you press
:HILITE.SHIFT_INS
(shift insert),
the character(s) you deleted will be pasted in after the current
cursor position on the current line.  If you press
:HILITE.CTRL_INS
(control insert),
the character(s) you deleted will be pasted in before the current
cursor position on the current line.
:P.
There are two other keys for pasting, the letters
:HILITE.p and
:HILITE.P
(small p and capital p).  Small p pastes the character(s) you deleted after
the current cursor position on the current line, just like
:HILITE.SHIFT_INS
:PERIOD.
Capital p ('P') pastes the character(s) you deleted before the cursor
position on the current line,
just like
:HILITE.CTRL_INS
:PERIOD.
:eSUBSECT.
:EXERCISES.
Edit the file "test" created in the previous examples by typing:
:ILLUST.
vi test
:eILLUST.
You should
see the lines:
:ILLUST.
This is a test line.
This is another test line.
:eILLUST.
The following examples are meant to be tried in sequence.  Each example
builds on the previous one.
:OL.
:LI.Copy the first line by pressing the 'Y' (capital 'y') while the cursor
is on the first line.
:LI.Now press the 'p' key to paste the line after the current line.  You
should see the lines:
:ILLUST.
This is a test line.
This is a test line.
This is another test line.
:eILLUST.
:LI.Now, go to the second line and yank 2 lines, by pressing the number
'2' followed by the letters 'yy'.
:LI.Cursor up to the top line, and press the capital p ('P') key to paste
the lines above the current line.  You will see:
:ILLUST.
This is a test line.
This is another test line.
This is a test line.
This is a test line.
This is another test line.
:eILLUST.
:LI.Now, move to the top line in the file and delete the first 3 lines
by press the number '3' followed by the letters 'dd'.  You will now
have the following lines:
:ILLUST.
This is a test line.
This is another test line.
:eILLUST.
:LI.Delete the first line by pressing
:HILITE.CTRL_DEL
or by typing 'dd'.  You will be left with a single line:
:ILLUST.
This is another test line.
:eILLUST.
:LI.Paste the line you deleted back in.  If you press the letter 'p'
or
:HILITE.SHIFT_INS
:CONT.,
the line you deleted will appear after the first line:
:ILLUST.
This is another test line.
This is a test line.
:eILLUST.
:LI.Paste the line above the current line by pressing capital p ('P')
or
:HILITE.CTRL_INS
:PERIOD.  The line should appear above the first line:
:ILLUST.
This is a test line.
This is another test line.
This is a test line.
:eILLUST.
:LI.Delete all 3 lines of text.  Make sure your cursor is on the
first line in the file, and type
:MONO.3dd
:PERIOD.
This will delete all three lines, and you will have an empty edit buffer.
:LI.Paste the lines back in by typing 'p'.  The three lines will be pasted
in:
:ILLUST.
This is a test line.
This is another test line.
This is a test line.
:eILLUST.
:LI.Go to the first line in the file, and to the first column
(try pressing
:HILITE.CTRL_PAGEUP
).  Then press the 'x' key.  The first line should become:
:ILLUST.
his is a test line.
:eILLUST.
Your cursor should be on the letter 'h' in column 1.
:LI.Type the letter 'p'.  The 'T' that you deleted will appear after the
'h' in column 1:
:ILLUST.
hTis is a test line.
:eILLUST.
:LI.Type capital p ('P').  The 'T' that you deleted will appear before the
'i' and after the 'T' you just pasted in.
:ILLUST.
hTTis is a test line.
:eILLUST.
:LI.Cursor to the first column and type
:MONO.6x
:PERIOD.
This will delete the first word and the space:
:ILLUST.
is a test line.
:eILLUST.
:LI.Move the cursor over to the 't' in 'test'.  Press capital x ('X'), and
you will see:
:ILLUST.
is atest line.
:eILLUST.
The cursor will remain on the 't'.
:LI.Type
:MONO.3X
:PERIOD.
You will now see
:ILLUST.
itest line.
:eILLUST.
and the cursor will still be on the 't'.
:eOL.
:INCLUDE file='basicdis'.

:eEXERCISES.
:eSUBSECT.

:SECTION id='basmenu'.Using the Menus
&edname comes with a default set of menus that have the following items
:ILLUST
Control  File  Edit  Position  Help
:eILLUST.
The first letter of each word is highlighted, indicating that that
key is the hot key to activate the menu.
:P.
To use a hot key, hold down the
:HILITE.ALT
key and press the highlighted key.
This will display the menu.
:P.
There is also a menu associated with each edit window.  You can activate
this window by pressing
:HILITE.ALT_W
or by clicking on the gadget in the upper left hand corner of
the edit window.
:P.
You may activate a menu on the menu bar by pressing the appropriate
hot key or by clicking on the word
with the mouse.  When you press down with the left mouse button,
the menu is activated.  If you keep the button down and move the
mouse from right to left across the menu bar,  the other menus will
activate as the mouse cursor sweeps across the word.
:P.
Once a menu is activated, a selection list appears.  An item in the
selection list may be chosen by doing one of the following:
:OL.
:LI.Cursoring up or down to the item and pressing the
:HILITE.ENTER
key.
:LI.Typing the hot key for the menu item.
:LI.Clicking on an item with the mouse.
:LI.Dragging the mouse and releasing the mouse button on an item.
:eOL.
:P.
Once a menu is activated, it may be cancelled by pressing the
:HILITE.ESC
key, or by clicking the mouse somewhere outside the menu.
If you press the cursor right key, the menu to the right of the menu
currently selected will activate.  If you press the cursor left key,
the menu to the left of the menu currently selected will activate.
:P.
The following sections describe each of the menus, and how each
menu item is used.
:SUBSECT.Edit Window Menu
This menu is selected by pressing
:HILITE.ALT_W
or by clicking on the gadget in the upper left hand corner of an edit
window with the mouse.  Once you have done one of these, the following menu
appears:
:PICTURE file='gadmenu' text='Individual Edit Window Menu'.
:DEFLIST.
:DEFITEM.Maximize
Causes the window to become as large as possible.
:DEFITEM.Minimize
Causes the window to become as small as possible.
:DEFITEM.Open another view
Creates a separate window that is editing the same copy of the
current file.  This is useful if you wish to be able to look at one
part of a file while editing another part
:DEFITEM.Save
Saves the current file.  You remain editing the current file.
:DEFITEM.Save and close
Saves the current file and closes the window.
:DEFITEM.Close no save
Closes the window, discarding the current file and any changes you may have
made.
:DEFITEM.Close
Closes the window.  If the file has been modified, then the close will fail.
:eDEFLIST.
:eSUBSECT.
:NEWPAGE.

:SUBSECT.Control Menu
This menu is selected by pressing
:HILITE.ALT_C
or by clicking on word
:HILITE.Control
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
:PICTURE file='ctrlmenu' text='Control Menu'.
:DEFLIST.
:DEFITEM.Tile windows
Tile all edit windows in a grid pattern, so that each window is displayed
without overlapping any other.
:DEFITEM.Cascade windows
Causes all edit windows to cascade (overlap each other with the top
border of each visible).
:DEFITEM.Reset windows
Resets all edit buffer windows to be full size (the same as maximizing each
window individually).
:DEFITEM.Settings
This brings up a list of all settings for &edname.
:PERIOD.
For more information on settings, see the chapter
:HDREF refid='edset'.
:PERIOD.
:DEFITEM.Enter command
Allows you to enter a &cmdline command.
The &cmdline is discussed in the next chapter,
:HDREF refid='inter'.
:PERIOD.
:DEFITEM.System
Starts an operating system command shell.  You exit the command shell
by typing 'exit'.
:DEFITEM.Exit
Exits all edit sessions, as long as no files have been modified.
:eDEFLIST.
:eSUBSECT.

:SUBSECT.File Menu
This menu is selected by pressing
:HILITE.ALT_F
or by clicking on the word
:HILITE.File
in the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
:PICTURE file='filemenu' text='File Menu'.
:DEFLIST.
:DEFITEM.File list
When selected, a list of all files that you are editing is displayed.
Any modified files have an asterisk ('*') before their name.
By picking a file from this list, you move to that file.
:DEFITEM.Next file
Flip to the next file in the list of files that you are editing.
:DEFITEM.Edit new file
Displays a list of all files and directories in the current directory,
along with all drives that are available.  If you pick a file, you will
edit that file.  If you pick a directory, &edname will
display all the files in that directory.  If you pick a drive, &edname will
display all files in the current directory on that drive.
:DEFITEM.Read file
Displays a list of all files and directories in the current directory,
along with all drives that are available.  If you pick a file, that file
will be read into the current edit buffer, after the current drive.
If you pick a directory, &edname will
display all the files in that directory.  If you pick a drive, &edname will
display all files in the current directory on that drive.
:DEFITEM.Save current file
&edname will save the current file you are editing.  You will
remain editing the current file.
:DEFITEM.Save current file and close
&edname will save the current file you are editing, and
close the edit window. If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edname will exit.
:eDEFLIST.
:eSUBSECT.

:SUBSECT.Edit Menu
This menu is selected by pressing
:HILITE.ALT_E
or by clicking on word
:HILITE.Edit
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
:PICTURE file='editmenu' text='Edit Menu'.
:DEFLIST.
:DEFITEM.Start region
Starts a selection region in the current edit buffer.  Any movement
commands that you type from then on will select text.
:DEFITEM.Delete region
Deletes the selected (highlighted) region.
:DEFITEM.Copy (yank) region
Makes a copy of the selected (highlighted) region.
:DEFITEM.Paste (put)
Pastes the last deleted or copied text into the current edit buffer.  The
text is pasted after the current position in the file.
:DEFITEM.Insert text
Causes &edname to enter insert mode at the current cursor position.
:DEFITEM.Overstrike text
Causes &edname to enter overstrike mode at the current cursor position.
:DEFITEM.Undo
Undoes the last change that you made to the current edit buffer.  If
you keep selecting this item, you will undo more and more of your
changes.  If you select this item enough times, your file will be
restored to the state when it was first opened or created.
:DEFITEM.Redo
Redoes the last undo that you did in the current edit buffer.  If you
keep selecting this item, you will redo more and more of your undos.
If you select this item enough times, your file will return to the state
when you made your last change.
:eDEFLIST.
:eSUBSECT.

:SUBSECT.Position Menu
This menu is selected by pressing
:HILITE.ALT_P
or by clicking on word
:HILITE.Position
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
:PICTURE file='posmenu' text='Position Menu'.
:DEFLIST.
:DEFITEM.Start of file
Moves to the start of the current edit buffer.
:DEFITEM.End of file
Moves to the end of the current edit buffer.
:DEFITEM.Line number
Prompts for a specific line number.  Once you enter the number,
you are placed at that line.
:DEFITEM.Start of line
Moves to the start of the current line.
:DEFITEM.End of line
Moves to the end of the current line.
:DEFITEM.Search forwards
Prompts for some search text.  Once you type some text and press
:HILITE.ENTER
:CONT.,
&edname searches forwards through the current edit buffer for the text.
If the text is found, it is highlighted and the cursor is placed on the
first character of the text.
:DEFITEM.Search backwards
Prompts for some search text.  Once you type some text and press
:HILITE.ENTER
:CONT.,
&edname searches backwards through the current edit buffer for the text.
If the text is found, it is highlighted and the cursor is placed on the
first character of the text.
:DEFITEM.Last search
Repeats the last search that you typed, in the same direction as the
initial search request.
:DEFITEM.Reverse last search
Repeats the last search that you typed, but the searches occurs in the
opposite direction of the initial search request.
:eDEFLIST.
:eSUBSECT.

:NEWPAGE.

:SUBSECT.Help Menu
This menu is selected by pressing
:HILITE.ALT_H
or by clicking on word
:HILITE.Help
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
:PICTURE file='helpmenu' text='Help Menu'.
:DEFLIST.
:DEFITEM.Command line
Gives help on all &cmdline commands.
:DEFITEM.Keystrokes
Gives help on &cmdmode and &tinsmode.
:DEFITEM.Regular expressions
Gives help on the search and replace abilities of &edname's
regular expressions.
:DEFITEM.Scripts
Gives help on &edname's script language.
:DEFITEM.Starting up
Gives help on the various command line parameters for &edname
:PERIOD.
:eDEFLIST.

:eSUBSECT.
