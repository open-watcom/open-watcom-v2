.chap Learning &edname Basics
.*
.np
This chapter will take you through a session with the &editor. that
will cover most of the functions that you will use on a regular basis.
The next chapter describes all of the menu items and options.
.*
.section What You Will Learn
.*
.np
This tutorial covers some of the features of the Editor by having you
edit a sample program, search for and correct some errors, and find
the occurrences of a certain string.
.*
.section Tutorial
.*
.ix 'Editor' 'tutorial'
.ix 'Tutorial' 'Editor'
.np
You have just tried to compile a program that creates a calendar for
Windows. You have discovered that the EndDialog function was typed
incorrectly. You also suspect that you are missing a brace somewhere
because of one of the error messages you received. During this session
with the Editor, you will correct these errors.
.begstep
.step Start the &editor..
.result
Within the &editor., you can open up several files at once and then
work through them in order. When you open files in the &editor. the
last file you opened is the first in the
.us File List
and the first file you opened is the last.
.step Open the file
called
.fi calendar.h.
.result
During your debugging session, you discovered that the EndDialog function
was typed incorrectly as EnDlg in several spots.
You need to find and replace these occurrences.
.step Find the first occurrence of the word
to be sure you have the correct
misspelling.
.result
Choose Find from the Edit menu and enter EnDlg as the search text.
.np
Click on the Find button.
.step Choose Find Next
from the Edit menu to see if each mis-spelling is
the same each time. You will find that it is.
.step Choose Replace
from the Edit menu.
.result
The Replace dialog appears.
.np
Because you want to control which occurrences are changed, turn on the
.us; Prompt on Replace
option.
.np
When you begin the search, the editor finds the first occurrence of the
string and prompts you to change it.
.step Repeat the Replace
until you have changed all occurrences.
.result
One of the errors that you got from the compiler was
.us; Expecting ")" but found ";".
This indicates
that you are missing a matching brace somewhere. You want to go through
your code to find if there are any mismatched braces.
.step Go to the line where the error was reported
and repeat for each brace on that line.
.step Click on the matching brace icon in the toolbar.
.result
This icon is a set of braces with a double-ended arrow in between.
Move to each brace in order and click on the matching brace icon until you
find a brace that does not have a match.
.np
The last task you want to perform while your file is open is find where
you defined WFrameWindow. Use the Fgrep option.
.step
Using the right mouse button, click on the word WFrame and select the fgrep
item from the pop-up menu.
.step Choose Exit
from the File menu to leave the &editor. and return to
the &vip..
.endstep
