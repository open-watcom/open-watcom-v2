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
.* 28-sep-05    L. Haynes       reformatted, figures, gadget menu invocation
.* 30-apr-06    F. Beythien     some minor corrections
.*
.****************************************************************************/
.chap *refid=basic 'Basic Usage'
.np
Using any editor as powerful as &edvi. takes some practice. This chapter
will lead you through the basics or using &edvi.
.* ******************************************************************
.section 'Starting the &edname.'
.* ******************************************************************
.np
&edvi is invoked by typing the command
.millust begin
vi
.millust end
at the command prompt.  Try typing this.  You will see something similar
to the following:
.figure *depth='2.47' *scale='59' *file='vi001' The &edvi screen
.np
If you are running DOS on a 386 (or higher)
machine, then you can also use the
.keyword protected mode
version of &edvi..  This version of &edvi runs as a native 386
application, which means that it has access to more memory and will do its
processing more quickly. You can start this version by typing
.millust begin
vip
.millust end
at the command prompt.
.np
&edvi also accepts one or more files as an optional parameter.  Each
file that you specify will be edited by &edvi..  For example, typing
.millust begin
vi test.c
.millust end
will edit the file
.mono test.c
:period.
Typing
.millust begin
vi test.c other.c
.millust end
will edit the files
.mono test.c
and
.mono other.c
:period.
Typing
.millust begin
vi *.c
.millust end
will edit all files that have the extension
.mono .c
:period.
.np
:INCLUDE file='exitvi'.
.* ******************************************************************
.section 'What is a Modal Editor?'
.* ******************************************************************
.np
A modal editor is one where keystrokes have different meaning, depending
on what mode you are in.  &edvi has two main modes, &cmdmode and
&tinsmode.
:period.
.np
While in &cmdmode, pressing a key on your keyboard can cause something
different to happen.  For example, pressing the letter D (capital 'd')
deletes all characters from the current cursor position to the end of the
line.
If the same letter 'D' (is pressed while in &tinsmode,
the letter 'D' appears in your text (much as you would expect).
.np
It is easy to tell what mode you are in while using &edvi..
When you are in &cmdmode, the cursor is a thin line.  When
you are in &tinsmode, the cursor is much thicker: if you are inserting
text, the cursor is one half of a full block, and if you are overstriking
text, the cursor is a full block.
.np
The mode indicator on the menu bar at the
top of your screen shows the current mode.
While in &cmdmode, you will see the following:
.millust begin
Mode: command
.millust end
While in &tinsmode, you will see either:
.millust begin
Mode: insert
.millust end
or
.millust begin
Mode: overstrike
.millust end
depending on whether you are inserting or overstriking text.
.np
&edvi is in &cmdmode by default.  There are numerous ways to enter
&tinsmode, the easiest of which is to press the
.param Insert
key on your keyboard.
.np
The
.param ESC
key is one of the more important keys in &edvi.. Pressing
.param ESC
once will stop whatever you are doing and
return you to &cmdmode.
:period.
If you have started a &cmdmode sequence, pressing
.param ESC
will cancel the command and return you to &cmdmode.
:period.
.exercises
Start up &edvi by typing
.millust begin
vi
.millust end
at your command prompt.
.np
You will see a screen that looks like the following:
.figure *depth='2.47' *scale='59' *file='vi001' The &edvi screen
.np
Press the insert (
.param INS
) key.  The cursor will get larger and you will see the mode indicator change
from
.millust begin
Mode: command
.millust end
to
.millust begin
Mode: insert
.millust end
Try typing some characters.  Notice that they appear in the edit window
as you type.  When you are done, press the
.param ESC
key.  The cursor will get smaller and you will see the mode indicator
change from
.millust begin
Mode: insert
.millust end
to
.millust begin
Mode: command
.millust end
.np
:INCLUDE file='exitvi'.
.np
Once you exit, you will be prompted with a message as follows:
.figure *depth='2.47' *scale='59' *file='vi003' The Really Exit prompt
there are still files that have been modified but not saved.
Press 'n' then the
.param ENTER
key to exit without saving your current file.
.* ******************************************************************
.section 'Some Basic Commands'
.* ******************************************************************
.np
To begin editing with &edvi, there are only a few basic commands
that you need to learn.  Once you master these few commands, you
will be able to accomplish all basic editing tasks.
In later chapters, more sophisticated commands will be introduced which
allow advanced tasks to be completed more quickly.
.*
.beglevel
.* ******************************************************************
.section 'Moving Around'
.* ******************************************************************
.np
When you are in &cmdmode, you may move around in your text using
various cursor keys.  The basic keys that you use to move around are:
:INCLUDE file='basicmv'
.np
These same cursor keys may also be used when entering text in &tinsmode.
:period.
.* ******************************************************************
.section *refid=savexit 'Saving and Exiting a File'
.* ******************************************************************
.np
Once you have modified a file, may want to save it and either edit other
files or exit the editor.  When
you exit a modified file, you may want to either discard your changes or
keep them.
.np
You can discard ALL of your files by
selecting the
.keyword Exit
item in the
.keyword File
menu.  Alternatively, you can discard or save the changes on
an individual file basis.
.np
If you wish to save the current file you are editing and leave that
particular edit session, the fastest way to do it is to use the
command 'ZZ'.  Press
.param ESC
to make sure that you are in &cmdmode.
:cont., and then type two capital z's ('ZZ')
in a row.  This saves the file, then exits the edit buffer for the file.
If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edvi will exit.
.np
You may use certain menu items to save or discard changes to your file.
The main menu option
.keyword File
(by clicking on it with the mouse or by pressing
.param ALT_F
:cont.) has options for saving the current file. As well,
each edit window has a menu associated with it that you can access
by pressing
.param ALT_G
:period.
You can also access this menu by clicking on the gadget ('ð') in the top left hand
corner of an edit window.  This menu has a number of choices for dealing with
the file.
.np
If you activate the
.keyword File
menu item, either with the mouse or by pressing
.param ALT_F,
a menu will be displayed as follows:
.figure *depth='2.47' *scale='59' *file='vi004' The File Menu
.np
If you select the
.keyword Save current file
item, &edvi will save the current file you are editing.  You will
remain editing the current file.
.np
If you select the
.keyword Save current file and close
item, &edvi will save the current file you are editing, and
close the edit window.
If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edvi will exit.
.np
If you activate the individual file menu either by clicking on the gadget
at the top-left corner of the edit window
with the mouse or by pressing
.param ALT_G
:cont., then a menu will pop up as follows:
.figure *depth='2.47' *scale='59' *file='vi005' The Individual Edit Window menu
.np
If you select the
.keyword Save current file
item, &edvi will save the current file you are editing.  You will
remain editing the current file.
.np
If you select the
.keyword Save & close
item, &edvi will save the current file you are editing, and
exit that edit buffer.  If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edvi will exit.
.np
If you select the
.keyword Close no save
item, &edvi will discard the current file without saving your
modifications. If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edvi will exit.
.np
If you select the
.keyword Close
item, &edvi will try to discard the current file.  However, if
the file has been modified, then the option will fail.
.* ******************************************************************
.section *refid=intext 'Inputting Text'
.* ******************************************************************
.np
You have already learned that pressing the
.param INS
key while in &cmdmode
puts &edvi into insert mode.  Once you are in insert mode, you
may enter whatever text you like.  You may cursor around and modify
your text in whatever way you choose.  Once you are done, you
may press the
.param ESC
key to return to &cmdmode.
:period.
.np
While you are in &tinsmode, the basic keys that allow you to move through
the text are:
:INCLUDE file='basicmv'
.np
These keys allow you to manipulate the text while in &tinsmode:
.*
:DL break.

:DT.CTRL_DEL (Control-Delete)
:DD.Deletes the current line.

:DT.BS (Backspace key)
:DD.Deletes the character before the cursor, moving the cursor and the
rest of the line to the left one character.

:DT.DEL (Delete key)
:DD.Delete the character under the cursor.  If you are at the end of the line,
.param DEL
has the same effect as pressing
.param BS.

:DT.ENTER
:DD.Start a new line.

:DT.INS
:DD.Toggles between inserting and overstriking text.

:eDL.
.*
.np
Along with the
.param INS
key, there are a number of other keys that you can press in &cmdmode
that will place you in &tinsmode, where you can edit text.
The difference between
all of these commands is where the cursor moves to before you start
inputting text.  The basic set of keys is:
.*
:DL compact.

:DT.a
:DD.Starts appending (inserting) text after the current character.

:DT.A
:DD.Starts appending (inserting) text after the last character on the
current line.

:DT.i
:DD.Starts inserting text at the current cursor position.  This is the
same as pressing the
.param INS
key.

:DT.I
:DD.Starts inserting text before the first non-white space character on
the current line.

:DT.o
:DD.Adds a blank line after the current line, and starts you inserting text
on the new line.

:DT.O
:DD.Adds a blank line before the current line, and starts you inserting text
on the new line.

:DT.R
:DD.Starts overstriking text at the current cursor position.

:DT.INS
:DD.Start inserting text at the current cursor position.

:eDL.
.*
.np
Remember, while in &tinsmode, the following mode indicator will appear
on the menu bar:
.millust begin
Mode: insert
.millust end
or
.millust begin
Mode: overstrike
.millust end
depending on whether you are inserting or overstriking text.
As well, the cursor will change to a larger block cursor
to help you remember that you are are inputting text.
.np
To exit &tinsmode at any time, press the
.param ESC
key.  Your mode indicator will switch to
.millust begin
Mode: command
.millust end
.*
.exercises
:OL.

:LI.Edit a new file (called "test", for example) as follows:
.millust begin
vi test
.millust end
You will see the following screen:
.figure *depth='2.47' *scale='59' *file='vi006' Editing the New File "test"
Note the message window (the bottom two lines of the screen)
contains the message
.millust begin
"test" [new file] line 1 of 1  -- 100% --
.millust end
This message indicates that you are editing a new file called
test, that you are on line 1 of a 1 line file, and that line 1 is
100% of the way into the file.
.np
You may see that message any time by pressing
.param CTRL_G
(hold down the Ctrl key and press the g key).
.np
Enter &tinsmode by pressing the
.param INS
key, and type the following lines:
.millust begin
This is a test line.
This is another test line.
.millust end
When you are done typing these lines, remember to press the
.param ESC
key to return to &cmdmode..
.np
Press
.param CTRL_G
(hold down Ctrl and press the g key).
The message window (the bottom two lines of the screen)
will show the message:
.millust begin
"test" [modified] line 2 of 2  -- 100% --
.millust end
.np
Now, press two capital z's in a row ('ZZ').  This will save the file
and return to the operating system command prompt.
:LI.Edit the file you created in the previous example, by typing
.millust begin
vi test
.millust end
You will see the following screen:
.figure *depth='2.47' *scale='59' *file='vi007' Editing the File "test"
Note the message window (the bottom two lines of the screen)
contains the message
.millust begin
"test" line 1 of 2  -- 50% --
.millust end
This message is different than when you edited "test" for the first time.
The "[new file]" indicator is gone, since you have edited an existing file.
This message indicates to you that you are now editing a file called
test, that you are on line 1 of a 2 line file, and that line 1 is
50% of the way into the file.
.np
Now try to experiment with the different ways of entering insert mode.
:OL.

:LI.Try pressing the 'A'(capital 'a') key.  This will move your cursor to
the end of the line, and will put you into insert mode.  Try adding some
text to this line, and press the
.param ESC
key when you are done.

:LI.Try pressing the 'I' key.  This will move your cursor to the start
of the line, and will put you into insert mode.  Try adding some
text to this line, and press the
.param ESC
key when you are done.

:LI.Try pressing the 'R' key.  This will start you in overstrike mode.
Note that the you have a very large cursor and that the mode indicator says:
.millust begin
Mode: overstrike
.millust end
Try typing some characters.  As you type, what was there already is
replaced by what you type. Press the
.param ESC
key when you are done.

:LI.Try pressing the 'i' key.  This is the same as pressing the
.param INS
key. Now type some text, then press the
.param INS
key.  Note that your cursor turned to full height from half height, and the
mode indicator changed from:
.millust begin
Mode: insert
.millust end
to
.millust begin
Mode: overstrike
.millust end
Now, when you type, you will replace the existing characters.  Try
pressing the
.param INS
key again.  This time, your cursor will turn from full height to half height,
and the mode indicator will change from:
.millust begin
Mode: overstrike
.millust end
to
.millust begin
Mode: insert
.millust end
Remember to press the
.param ESC
key when you are done.

:LI.Try pressing the 'a' key.  This will move the cursor over one
character, and put you in insert mode.  Try cursoring around inside
your text using the arrow keys.  Use the
.param END
key to move to the end of a line, and the
.param HOME
key to move to the start of a line.
Press the
.param ESC
key when you are done.

:eOL.

:INCLUDE file='basicdis'.

:eOL.
.*
.endlevel
.* ******************************************************************
.section *refid=scap 'Cutting and Pasting Text'
.* ******************************************************************
.np
So far, you have learned how to edit a file, input some text, and
save or cancel your changes.  Now you will learn some simple ways of
deleting text and moving it elsewhere.
.*
.beglevel
.* ******************************************************************
.section 'Lines'
.* ******************************************************************
.np
The section
:HDREF refid='intext'.
noted that pressing
.param CTRL_DEL
(control-delete) deleted a line when inputting
text.  This same command can be used while in &cmdmode.
:period.
.np
If you want to delete more than one line at once, you may precede
this delete command with a
.keyword repeat count.
You enter a repeat count
simply by typing numbers before you press
.param CTRL_DEL
:period.
As you type a number in &cmdmode, it will appear in a special window,
as follows:
.figure *depth='2.47' *scale='59' *file='vi008' Repeat Count Display
The number that you are typing is displayed in the repeat count window.
As with all &cmdmode commands, if you decide that you have made a mistake,
just press the
.param ESC
key and the count you have been typing will be cancelled.
.np
So, if you type a repeat count of 5 and then press
.param CTRL_DEL
:cont.,
five lines will be deleted.  If you do not type a repeat count, then
one line will be deleted.
.np
You can also delete a line by pressing the 'd' key twice in a row ('dd').
This has the exact same effect as pressing the
.param CTRL_DEL
key.  You may precede this command by a repeat count as well.  For example,
typing
.millust begin
12dd
.millust end
deletes the next 12 lines in your file.
.np
Once you have deleted some lines, you might want to paste them back in.
If you press
.param SHIFT_INS
(shift insert),
the line(s) you deleted will be pasted in after the current line.  If
you press
.param CTRL_INS
(control insert),
the line(s) you deleted will be pasted in before the current line (which
reverses the action of
.param CTRL_DEL
:cont.)
:period.
.np
There are two other keys for pasting, the letters
'p' and 'P' (small p and capital p).  Small 'p' pastes the line(s) you deleted after
the current line, just like
.param SHIFT_INS
:period.
Capital p ('P') pastes the line(s) you deleted before the current line,
just like
.param CTRL_INS
:period.
.np
If you do not wish to delete some lines, but you do wish to copy them
so you can paste them somewhere else, then you may use the "yank"
command.  If you press the letter 'Y', or type the letters
'yy', you will "yank" (make a copy of) the current line.  Just like
'dd' or
.param CTRL_DEL,
you can precede these commands with a repeat count.  For example, typing
.millust begin
6Y
.millust end
or
.millust begin
6yy
.millust end
will copy 6 lines in your file.
.np
Once you have yanked the lines, you may paste them in, as discussed above.
.* ******************************************************************
.section 'Characters'
.* ******************************************************************
.np
To delete characters while in &cmdmode, you can press the
.param DEL
key or the 'x' key.  The current character under the cursor will be deleted.
You may type a repeat count before you type
.param DEL
or 'x',
and that will delete a number of characters.
For example, typing
.mono 3x
when the cursor is on 'T' in the line
.millust begin
Delete Test.
.millust end
will leave you with
.millust begin
Delete t.
.millust end
and the cursor will be on the 't' just before the period.
.np
If you wish to delete the character before the cursor, you may
press the letter 'X' (capital x).  This will delete the character before
the cursor, and move the cursor back one (i.e. the cursor stays on
the character that it was on before you pressed the 'X').
You may use a repeat count with this command. For example, typing
.mono 3X
when the cursor is on 'T' in the line
.millust begin
Delete Test.
.millust end
will leave you with
.millust begin
DeleTest.
.millust end
and the cursor will still be on the 'T'.
.np
Once you have deleted some characters, you might want to paste them back in.
The keys involved are the exact same keys for pasting lines.  The
difference is that when you paste characters, the characters are
pasted into the current line, whereas when you paste lines, the lines are
pasted above or below the current line.
.np
If you press
.param SHIFT_INS
(shift insert),
the character(s) you deleted will be pasted in after the current
cursor position on the current line.  If you press
.param CTRL_INS
(control insert),
the character(s) you deleted will be pasted in before the current
cursor position on the current line.
.np
There are two other keys for pasting, the letters
.param p and
.param P
(small p and capital p).  Small p pastes the character(s) you deleted after
the current cursor position on the current line, just like
.param SHIFT_INS
:period.
Capital p ('P') pastes the character(s) you deleted before the cursor
position on the current line, just like
.param CTRL_INS
:period.
.*
.exercises
Edit the file "test" created in the previous examples by typing:
.millust begin
vi test
.millust end
You should
see the lines:
.millust begin
This is a test line.
This is another test line.
.millust end
The following examples are meant to be tried in sequence.  Each example
builds on the previous one.
:OL.

:LI.Copy the first line by pressing the 'Y' (capital 'y') while the cursor
is on the first line.

:LI.Now press the 'p' key to paste the line after the current line.  You
should see the lines:
.millust begin
This is a test line.
This is a test line.
This is another test line.
.millust end

:LI.Now, go to the second line and yank 2 lines, by pressing the number
'2' followed by the letters 'yy'.

:LI.Cursor up to the top line, and press the capital p ('P') key to paste
the lines above the current line.  You will see:
.millust begin
This is a test line.
This is another test line.
This is a test line.
This is a test line.
This is another test line.
.millust end

:LI.Now, move to the top line in the file and delete the first 3 lines
by press the number '3' followed by the letters 'dd'.  You will now
have the following lines:
.millust begin
This is a test line.
This is another test line.
.millust end

:LI.Delete the first line by pressing
.param CTRL_DEL
or by typing 'dd'.  You will be left with a single line:
.millust begin
This is another test line.
.millust end

:LI.Paste the line you deleted back in.  If you press the letter 'p'
or
.param SHIFT_INS
:cont.,
the line you deleted will appear after the first line:
.millust begin
This is another test line.
This is a test line.
.millust end

:LI.Paste the line above the current line by pressing capital p ('P')
or
.param CTRL_INS
:period.  The line should appear above the first line:
.millust begin
This is a test line.
This is another test line.
This is a test line.
.millust end

:LI.Delete all 3 lines of text.  Make sure your cursor is on the
first line in the file, and type
.mono 3dd
:period.
This will delete all three lines, and you will have an empty edit buffer.

:LI.Paste the lines back in by typing 'p'.  The three lines will be pasted
in:
.millust begin
This is a test line.
This is another test line.
This is a test line.
.millust end

:LI.Go to the first line in the file, and to the first column
(try pressing
.param CTRL_PAGEUP
).  Then press the 'x' key.  The first line should become:
.millust begin
his is a test line.
.millust end
Your cursor should be on the letter 'h' in column 1.

:LI.Type the letter 'p'.  The 'T' that you deleted will appear after the
'h' in column 1:
.millust begin
hTis is a test line.
.millust end

:LI.Type capital p ('P').  The 'T' that you deleted will appear before the
'i' and after the 'T' you just pasted in.
.millust begin
hTTis is a test line.
.millust end

:LI.Cursor to the first column and type
.mono 6x
:period.
This will delete the first word and the space:
.millust begin
is a test line.
.millust end

:LI.Move the cursor over to the 't' in 'test'.  Press capital x ('X'), and
you will see:
.millust begin
is atest line.
.millust end
The cursor will remain on the 't'.

:LI.Type
.mono 3X
:period.
You will now see
.millust begin
itest line.
.millust end
and the cursor will still be on the 't'.

:eOL.
:INCLUDE file='basicdis'.
.*
.endlevel
.* ******************************************************************
.section *refid=basmenu 'Using the Menus'
.* ******************************************************************
.np
&edvi comes with a default set of menus that have the following items
.millust begin
File  Edit  Position  Window  Options  Help
.millust end
The first letter of each word is highlighted, indicating that that
key is the hot key to activate the menu.
.np
To use a hot key, hold down the
.param ALT
key and press the highlighted key.
This will display the menu.
.np
There is also a menu associated with each edit window.  You can activate
this window by pressing
.param ALT_G
or by clicking on the gadget in the upper left hand corner of
the edit window.
.np
You may activate a menu on the menu bar by pressing the appropriate
hot key or by clicking on the word
with the mouse.  When you press down with the left mouse button,
the menu is activated.  If you keep the button down and move the
mouse from right to left across the menu bar,  the other menus will
activate as the mouse cursor sweeps across the word.
.np
Once a menu is activated, a selection list appears.  An item in the
selection list may be chosen by doing one of the following:
:OL.

:LI.Cursoring up or down to the item and pressing the
.param ENTER
key.

:LI.Typing the hot key for the menu item.

:LI.Clicking on an item with the mouse.

:LI.Dragging the mouse and releasing the mouse button on an item.

:eOL.
.np
Once a menu is activated, it may be cancelled by pressing the
.param ESC
key, or by clicking the mouse somewhere outside the menu.
If you press the cursor right key, the menu to the right of the menu
currently selected will activate.  If you press the cursor left key,
the menu to the left of the menu currently selected will activate.
.np
The following sections describe each of the menus, and how each
menu item is used.
.*
.beglevel
.* ******************************************************************
.section 'Edit Window Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_G
or by clicking on the gadget in the upper left hand corner of an edit
window with the mouse.  Once you have done one of these, the following menu
appears:
.figure *depth='2.47' *scale='59' *file='vi005' The Individual Edit Window menu
.*
:DL break.

:DT.Maximize
:DD.Causes the window to become as large as possible.

:DT.Minimize
:DD.Causes the window to become as small as possible.

:DT.Open another view
:DD.Creates a separate window that is editing the same copy of the
current file.  This is useful if you wish to be able to look at one
part of a file while editing another part

:DT.Save
:DD.Saves the current file.  You remain editing the current file.

:DT.Save and close
:DD.Saves the current file and closes the window.

:DT.Close no save
:DD.Closes the window, discarding the current file and any changes you may have
made.

:DT.Close
:DD.Closes the window.  If the file has been modified, then the close will fail.

:eDL.
.* ******************************************************************
.section 'Window Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_W
or by clicking on word
.keyword Window
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi009' The Window menu
.*
:DL break.

:DT.Tile windows
:DD.Tile all edit windows in a grid pattern, so that each window is displayed
without overlapping any other.

:DT.Cascade windows
:DD.Causes all edit windows to cascade (overlap each other with the top
border of each visible).

:DT.Reset windows
:DD.Resets all edit buffer windows to be full size (the same as maximizing each
window individually).

:eDL.
.* ******************************************************************
.section 'Options Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_O
or by clicking on word
.keyword Options
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi010' The Options menu
.*
:DL break.

:DT.Settings
:DD.This brings up a list of all settings for &edvi..
For more information on settings, see the chapter
:HDREF refid='edset'..

:eDL.
.* ******************************************************************
.section 'File Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_F
or by clicking on the word
.keyword File
in the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi004' The File menu
.*
:DL break.

:DT.Open new file
:DD.Displays a list of all files and directories in the current directory,
along with all drives that are available.  If you pick a file, you will
edit that file.  If you pick a directory, &edvi will
display all the files in that directory.  If you pick a drive, &edvi will
display all files in the current directory on that drive.

:DT.Next file
:DD.Flip to the next file in the list of files that you are editing.

:DT.Read file
:DD.Displays a list of all files and directories in the current directory,
along with all drives that are available.  If you pick a file, that file
will be read into the current edit buffer, after the current line.
If you pick a directory, &edvi will
display all the files in that directory.  If you pick a drive, &edvi will
display all files in the current directory on that drive.

:DT.File list
:DD.When selected, a list of all files that you are editing is displayed.
Any modified files have an asterisk ('*') before their name.
By picking a file from this list, you move to that file.

:DT.Save current file
:DD.&edvi will save the current file you are editing.  You will
remain editing the current file.

:DT.Save current file and close
:DD.&edvi will save the current file you are editing, and
close the edit window. If you are editing any other files, the next
one in the list will become the current one.  If you are not
editing any other files, then &edvi will exit.

:DT.Enter command
:DD.Allows you to enter a &cmdline command.
The &cmdline is discussed in the next chapter,
:HDREF refid='inter'..

:DT.System
:DD.Starts an operating system command shell.  You exit the command shell
by typing 'exit'.

:DT.Exit
:DD.Exits all edit sessions, as long as no files have been modified.

:eDL.
.* ******************************************************************
.section 'Edit Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_E
or by clicking on word
.keyword Edit
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi012' The Edit menu
.*
:DL break.

:DT.Delete region
:DD.Deletes the selected (highlighted) region.

:DT.Copy (yank) region
:DD.Makes a copy of the selected (highlighted) region.

:DT.Paste (put)
:DD.Pastes the last deleted or copied text into the current edit buffer.  The
text is pasted after the current position in the file.

:DT.Insert text
:DD.Causes &edvi to enter insert mode at the current cursor position.

:DT.Overstrike text
:DD.Causes &edvi to enter overstrike mode at the current cursor position.

:DT.Undo
:DD.Undoes the last change that you made to the current edit buffer.  If
you keep selecting this item, you will undo more and more of your
changes.  If you select this item enough times, your file will be
restored to the state when it was first opened or created.

:DT.Redo
:DD.Redoes the last undo that you did in the current edit buffer.  If you
keep selecting this item, you will redo more and more of your undos.
If you select this item enough times, your file will return to the state
when you made your last change.

:eDL.
.* ******************************************************************
.section 'Position Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_P
or by clicking on word
.keyword Position
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi013' The Position menu
.*
:DL break.

:DT.Start of file
:DD.Moves to the start of the current edit buffer.

:DT.End of file
:DD.Moves to the end of the current edit buffer.

:DT.Line number
:DD.Prompts for a specific line number.  Once you enter the number,
you are placed at that line.

:DT.Start of line
:DD.Moves to the start of the current line.

:DT.End of line
:DD.Moves to the end of the current line.

:DT.Search forwards
:DD.Prompts for some search text.  Once you type some text and press
.param ENTER
:cont.,
&edvi searches forwards through the current edit buffer for the text.
If the text is found, it is highlighted and the cursor is placed on the
first character of the text.

:DT.Search backwards
:DD.Prompts for some search text.  Once you type some text and press
.param ENTER
:cont.,
&edvi searches backwards through the current edit buffer for the text.
If the text is found, it is highlighted and the cursor is placed on the
first character of the text.

:DT.Last search
:DD.Repeats the last search that you typed, in the same direction as the
initial search request.

:DT.Reverse last search
:DD.Repeats the last search that you typed, but the searches occurs in the
opposite direction of the initial search request.

:eDL.
.* ******************************************************************
.section 'Help Menu'
.* ******************************************************************
.np
This menu is selected by pressing
.param ALT_H
or by clicking on word
.keyword Help
on the menu bar with the mouse.  Once you have done one of these,
the following menu appears:
.figure *depth='2.47' *scale='59' *file='vi014' The Help menu
.*
:DL break.

:DT.Command line
:DD.Gives help on all &cmdline commands.

:DT.Keystrokes
:DD.Gives help on &cmdmode and &tinsmode.

:DT.Regular expressions
:DD.Gives help on the search and replace abilities of &edvi's
regular expressions.

:DT.Scripts
:DD.Gives help on &edvi's script language.

:DT.Starting up
:DD.Gives help on the various command line parameters for &edvi.
:period.

:eDL.
.*
.endlevel
.*

