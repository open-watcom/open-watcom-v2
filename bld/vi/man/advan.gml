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
.* 18-aug-92	Craig Eisler	initial draft
.*
:CHAPTER.Advanced Usage
:SECTION.The Substitute Command
The &cmdline command
:KEYWORD.substitute
is a powerful mechanism for changing text in your file.  For every match
of a string, the string can be replaced with something else.
:P.
:INCLUDE file='rxexp'.
:P.
The syntax of the
:KEYWORD.substitute
command is:
:ILLUST.
 <linerange> substitute /<string>/<subs>/"g""i"
:eILLUST.
:P.
The line range
:HILITE.<linerange>
specifies the lines that the
:KEYWORD.substitute
command is to work on.  If no line range is specified, then the
current line is assumed.
:P.
The first occurrence of the search string
:HILITE.<string>
on each line is replaced with the string
:HILITE.<subs>
:PERIOD.
:P.
If the letter 'g' is specified at the end, then every occurrence of
the search string
:HILITE.<string>
on each line is replaced with the string
:HILITE.<subs>
:PERIOD.
:P.
If the letter 'i' is specified at the end, then the substitution is
interactive.  You will be prompted before each replacement to verify
that you want it to be done.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:PERIOD.
Enter the &cmdline command (remember to press colon (':') to bring up the
command window):
:ILLUST.
 :1,$s/This/This This/
:eILLUST.
All lines will have the word "This" replaced with "This This".
:LI.Enter the &cmdline command:
:ILLUST.
 :1,$s/This/Change/
:eILLUST.
Notice that only the first occurrence of the word "This" was replaced.
:LI.Type the letter 'u' in &cmdmode to undo the change you just made.
Then enter the &cmdline command:
:ILLUST.
 :1,$s/This/Change/g
:eILLUST.
Now all of the occurrences of "This" were replaced.

:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.

:SECTION.The Global Command
The &cmdline command
:KEYWORD.global
is a method of executing a command on every line that has (or does not have)
a specific string.
:P.
:INCLUDE file='rxexp'.
:P.
The syntax of the
:KEYWORD.global
command is:
:ILLUST.
 <linerange> global "!" /<string>/ <cmd>
:eILLUST.
:P.
The line range
:HILITE.<linerange>
specifies the lines that the
:KEYWORD.global
command is to work on.  If no line range
is specified, then all the lines in the file is assumed.
:P.
The &cmdline command
:HILITE.<cmd>
will be executed on every line that has the string
:HILITE.<string>
:PERIOD.
:P.
If the exclamation mark ('!') is used, then the &cmdline command
:HILITE.<cmd>
will be executed on every line that does not have the string
:HILITE.<string>
:PERIOD.
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:PERIOD.
Enter the &cmdline command (remember to press colon (':') to bring up the
command window):
:ILLUST.
 :g/This/delete
:eILLUST.
Each line that contains the word "This" on it will have the
:KEYWORD.delete
command executed on it.
:LI.Press the 'u' key to undo the previous global command, then enter the
&cmdline command:
:ILLUST.
 :g/1/delete
:eILLUST.
All lines with the character '1' in them will be deleted.
:LI.Press the 'u' key to undo the previous global command, then enter the
&cmdline command:
:ILLUST.
 :g!/1/delete
:eILLUST.
All lines without the character '1' in them will be deleted.
:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.

:SECTION.Searching Files For Text
There are &cmdline commands provided to search through all files for
some text.  These commands differ from the search commands outlined
in the section
:HDREF refid='stext'.
in the previous chapter in that they search files in directories rather
than files that are being edited. These file search commands are
:KEYWORD.fgrep
and
:KEYWORD.egrep
:PERIOD.
:P.
The
:KEYWORD.fgrep
command is a fast search for a specified string.  The syntax of this command
is:
:ILLUST
fgrep "-c" "-i" <string> <files>.
:eILLUST.
The file list
:HILITE.<files>
is searched for the string
:HILITE.<string>
:PERIOD. The file list may contain wild cards.
:P.
If you wish to have spaces in your string, then you can enclose the
string in either double quotes ('"') or forward slashes ('/').
:P.
If
:HILITE.-c
is specified, then the search is made case sensitive.  If
:HILITE.-i
is specified, then the search is made case insensitive.  If neither of
these options is specified, then case sensitivity is determined by the
current
:KEYWORD.caseignore
setting (for more information about
:KEYWORD.caseignore
:CONT.,see the section
:HDREF refid='boolset'
in the chapter
:HDREF page=no refid='edset'.
:CONT.).
:P.
The
:KEYWORD.egrep
command searches for regular expressions, and is slower than the
:KEYWORD.fgrep
command.  The syntax of this command is:
:ILLUST
egrep <regexp> <files>.
:eILLUST.
The file list
:HILITE.<files>
is searched for the regular expression
:HILITE.<regexp>
:PERIOD. The file list may contain wild cards.
:P.
If you wish to have spaces in your regular expression, then you can enclose
the string in either double quotes ('"') or forward slashes ('/').
:P.
For example, entering the following command:
:ILLUST.
:fgrep window *.c
:eILLUST.
searches all files in the current directory ending in the extension .c
for the string
:ITALICS.window
:PERIOD.
While searching for the item, &edname displays a window that shows
all files being searched. 
:P.
When all matches are found, a selection window is presented with
all the files that contained the search string.
:P.
The search display appears as follows:
:PICTURE file='grepsrch' text='Grep Working Display'.
The selection window appears as follows:
:PICTURE file='grepwin' text='Grep Result Display'.

:SECTION.Mapping Keys
A powerful feature in &edname is the ability to change the meaning of
any key in &cmdmode or &tinsmode
:PERIOD.
Using this feature, you can configure the editor any way which suits your
needs.
:P.
The &cmdline commands
:KEYWORD.map
and
:KEYWORD.mapbase
are used to remap the definition of a key.  The syntax of these commands
is as follows:
:ILLUST.
map "!" <key> <string>
mapbase <key> <string>
:eILLUST.
:P.
Both commands remap the specified key
:HILITE.<key>
to execute the characters in
:HILITE.<string>
:PERIOD.
Whenever the specified key is pressed while in &cmdmode, it is equivalent
to typing the characters in
:HILITE.<string>
:PERIOD.
:P.
In the
:KEYWORD.map
command, specifying the exclamation point ('!') causes the map to be
for &tinsmode, rather than &cmdmode
:PERIOD.
:P.
It is possible for
:HILITE.<string>
to contain keys that are mapped themselves.  If you want to do a key
mapping that is in terms of the base definitions of the keys, then you
should use the
:KEYWORD.mapbase
command.
:P.
To remove the mapping of a key, the &cmdline command
:KEYWORD.unmap
is used.  The syntax of this command is:
:ILLUST.
unmap <key>
:eILLUST.
:P.
If you need to specify a special key (e.g. ENTER, F1, etc) for
:HILITE.<key>
you specify a symbolic name for it.
There are a number of pre-defined key
symbols that are recognized when specifying which key is being
mapped or unmapped. These are described in the Appendix
:HDREF refid='symkey'.
:PERIOD.
:P.
If you need to use one or more special keys (e.g. ENTER, F1, etc) in
:HILITE.<string>
then you may enter:
:P.
:INCLUDE file='spmapkey'
:EXERCISES.
:OL.
:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:PERIOD.
Enter the following &cmdline command:
:ILLUST.
 :map F5 dd
:eILLUST.
Now, whenever you press
:HILITE.F5
:CONT.,
a line will be deleted.
Try pressing
:HILITE.F5
a few times to verify this.
:LI.Enter the &cmdline command
:ILLUST.
 :unmap F5
:eILLUST.
and try pressing F5 again.  It will no longer delete lines.
:LI.Enter the &cmdline command
:ILLUST.
 :map F6 :date\n
:eILLUST.
Press
:HILITE.F6
:PERIOD.
The time and date will be displayed in the message window.
You will notice that the command window flashes as you push
:HILITE.F6
:PERIOD.
:LI.
:ILLUST.
 :map F6 \x:date\n
:eILLUST.
Press
:HILITE.F6
:PERIOD.
The time and date will be displayed in the message window.
You will notice that the command window no longer flashes.
:LI. Bring up the command window ( press ':') and press cursor up.  You will
notice that the date commands are in your history.  Now, enter the &cmdline
command:
:ILLUST.
 :map F6 \x:\hdate\n
:eILLUST.
Press
:HILITE.F6
a few times.  Bring up the command window again and cursor up.  You
will see that the date commands did not get added to the history.
:LI.Enter the &cmdline command:
:ILLUST.
 :map! F6 \edwi
:eILLUST.
Whenever you press
:HILITE.F6
in &tinsmode, you will exit insert mode (the \e is like pressing the
:HILITE.ESC
key), delete the current word, and then re-enter insert mode.
Try entering &tinsmode and pressing
:HILITE.F6
:PERIOD.
:LI.Exit &tinsmode and press
:HILITE.F6
:PERIOD.
You will notice that the date is still displayed, since the &tinsmode
and &cmdmode mappings for
:HILITE.F6
are different.
:INCLUDE file='exerquit'.
:eOL.
:eEXERCISES.
