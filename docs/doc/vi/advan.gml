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
.* 18-aug-92    Craig Eisler    initial draft
.* 28-sep-05    L. Haynes       reformatted, figures
.*
.****************************************************************************/
.chap Advanced Usage
.np
This chapter discusses a number of the less commonly used features of the &edname.
:period.
The knowledge of the information in the chapter
:HDREF page=no refid='basic'.
and 
:HDREF page=no refid='inter'.
is assumed.
.*
.section 'The Substitute Command'
.*
.np
The &cmdline command
.keyref substitute
is a powerful mechanism for changing text in your file.  For every match
of a string, the string can be replaced with something else.
.np
:INCLUDE file='rxexp'.
.np
The syntax of the
.keyref substitute
command is:
.millust begin
&lt.linerange&gt. substitute /&lt.string&gt./&lt.subs&gt./"g""i"
.millust end
.np
The line range
.param &lt.linerange&gt.
specifies the lines that the
.keyref substitute
command is to work on.  If no line range is specified, then the
current line is assumed.
.np
The first occurrence of the search string
.param &lt.string&gt.
on each line is replaced with the string
.param &lt.subs&gt.
:period.
.np
If the letter 'g' is specified at the end, then every occurrence of
the search string
.param &lt.string&gt.
on each line is replaced with the string
.param &lt.subs&gt.
:period.
.np
If the letter 'i' is specified at the end, then the substitution is
interactive.  You will be prompted before each replacement to verify
that you want it to be done.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:period.
Enter the &cmdline command (remember to press colon (':') to bring up the
command window):
.millust begin
:1,$s/This/This This/
.millust end
All lines will have the word "This" replaced with "This This".

:LI.Enter the &cmdline command:
.millust begin
:1,$s/This/Change/
.millust end
Notice that only the first occurrence of the word "This" was replaced.

:LI.Type the letter 'u' in &cmdmode to undo the change you just made.
Then enter the &cmdline command:
.millust begin
:1,$s/This/Change/g
.millust end
Now all of the occurrences of "This" were replaced.

:INCLUDE file='exerquit'.

:eOL.
.* ******************************************************************
.section 'The Global Command'
.* ******************************************************************
.np
The &cmdline command
.keyref global
is a method of executing a command on every line that has (or does not have)
a specific string.
.np
:INCLUDE file='rxexp'.
.np
The syntax of the
.keyref global
command is:
.millust begin
&lt.linerange&gt. global "!" /&lt.string&gt./ &lt.cmd&gt.
.millust end
.np
The line range
.param &lt.linerange&gt.
specifies the lines that the
.keyref global
command is to work on.  If no line range
is specified, then all the lines in the file is assumed.
.np
The &cmdline command
.param &lt.cmd&gt.
will be executed on every line that has the string
.param &lt.string&gt.
:period.
.np
If the exclamation mark ('!') is used, then the &cmdline command
.param &lt.cmd&gt.
will be executed on every line that does not have the string
.param &lt.string&gt.
:period.
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:period.
Enter the &cmdline command (remember to press colon (':') to bring up the
command window):
.millust begin
:g/This/delete
.millust end
Each line that contains the word "This" on it will have the
.keyref delete
command executed on it.
:LI.Press the 'u' key to undo the previous global command, then enter the
&cmdline command:
.millust begin
:g/1/delete
.millust end
All lines with the character '1' in them will be deleted.

:LI.Press the 'u' key to undo the previous global command, then enter the
&cmdline command:
.millust begin
:g!/1/delete
.millust end
All lines without the character '1' in them will be deleted.

:INCLUDE file='exerquit'.

:eOL.
.* ******************************************************************
.section 'Searching Files For Text'
.* ******************************************************************
.np
There are &cmdline commands provided to search through all files for
some text.  These commands differ from the search commands outlined
in the section
:HDREF refid='stext'.
in the previous chapter in that they search files in directories rather
than files that are being edited. These file search commands are
.keyref fgrep
and
.keyref egrep
:period.
.np
The
.keyref fgrep
command is a fast search for a specified string.  The syntax of this command
is:
.millust begin
fgrep "-c" "-i" &lt.string&gt. &lt.files&gt..
.millust end
The file list
.param &lt.files&gt.
is searched for the string
.param &lt.string&gt.
:period. The file list may contain wild cards.
.np
If you wish to have spaces in your string, then you can enclose the
string in either double quotes ('"') or forward slashes ('/').
.np
If
.param -c
is specified, then the search is made case sensitive.  If
.param -i
is specified, then the search is made case insensitive.  If neither of
these options is specified, then case sensitivity is determined by the
current
.keyref caseignore 1
setting (for more information about
.keyref caseignore 1
:cont.,see the section
:HDREF refid='boolset'
in the chapter
:HDREF page=no refid='edset'.
:cont.).
.np
The
.keyref egrep
command searches for regular expressions, and is slower than the
.keyref fgrep
command.  The syntax of this command is:
.millust begin
egrep &lt.regexp&gt. &lt.files&gt..
.millust end
The file list
.param &lt.files&gt.
is searched for the regular expression
.param &lt.regexp&gt.
:period. The file list may contain wild cards.
.np
If you wish to have spaces in your regular expression, then you can enclose
the string in either double quotes ('"') or forward slashes ('/').
.np
For example, entering the following command:
.millust begin
:fgrep window *.c
.millust end
searches all files in the current directory ending in the extension .c
for the string
.param window
:period.
While searching for the item, &edname displays a window that shows
all files being searched. 
.np
When all matches are found, a selection window is presented with
all the files that contained the search string.
.np
The selection window appears as follows:
.figure *depth='2.47' *scale='59' *file='vi023' FGrep result display
.* ******************************************************************
.section 'Mapping Keys'
.* ******************************************************************
.np
A powerful feature in &edname is the ability to change the meaning of
any key in &cmdmode or &tinsmode
:period.
Using this feature, you can configure the editor any way which suits your
needs.
.np
The &cmdline commands
.keyref map
and
.keyref mapbase
are used to remap the definition of a key.  The syntax of these commands
is as follows:
.millust begin
map "!" &lt.key&gt. &lt.string&gt.
mapbase &lt.key&gt. &lt.string&gt.
.millust end
.np
Both commands remap the specified key
.param &lt.key&gt.
to execute the characters in
.param &lt.string&gt.
:period.
Whenever the specified key is pressed while in &cmdmode, it is equivalent
to typing the characters in
.param &lt.string&gt.
:period.
.np
In the
.keyref map
command, specifying the exclamation point ('!') causes the map to be
for &tinsmode, rather than &cmdmode
:period.
.np
It is possible for
.param &lt.string&gt.
to contain keys that are mapped themselves.  If you want to do a key
mapping that is in terms of the base definitions of the keys, then you
should use the
.keyref mapbase
command.
.np
To remove the mapping of a key, the &cmdline command
.keyref unmap
is used.  The syntax of this command is:
.millust begin
unmap &lt.key&gt.
.millust end
.np
If you need to specify a special key (e.g. ENTER, F1, etc) for
.param &lt.key&gt.
you specify a symbolic name for it.
There are a number of pre-defined key
symbols that are recognized when specifying which key is being
mapped or unmapped. These are described in the Appendix
:HDREF refid='symkey'.
:period.
.np
If you need to use one or more special keys (e.g. ENTER, F1, etc) in
.param &lt.string&gt.
then you may enter:
.np
:INCLUDE file='spmapkey'
.*
.exercises
:OL.

:LI.Edit the file "atest" created in the Exercises section of
:HDREF refid='maiaf'
in the previous chapter
:HDREF page=no refid='inter'.
:period.
Enter the following &cmdline command:
.millust begin
:map F5 dd
.millust end
Now, whenever you press
.param F5
:cont.,
a line will be deleted.
Try pressing
.param F5
a few times to verify this.

:LI.Enter the &cmdline command
.millust begin
:unmap F5
.millust end
and try pressing F5 again.  It will no longer delete lines.

:LI.Enter the &cmdline command
.millust begin
:map F6 :date\n
.millust end
Press
.param F6
:period.
The time and date will be displayed in the message window.
You will notice that the command window flashes as you push
.param F6
:period.

:LI.
.millust begin
:map F6 \x:date\n
.millust end
Press
.param F6
:period.
The time and date will be displayed in the message window.
You will notice that the command window no longer flashes.

:LI. Bring up the command window ( press ':') and press cursor up.  You will
notice that the date commands are in your history.  Now, enter the &cmdline
command:
.millust begin
:map F6 \x:\hdate\n
.millust end
Press
.param F6
a few times.  Bring up the command window again and cursor up.  You
will see that the date commands did not get added to the history.

:LI.Enter the &cmdline command:
.millust begin
:map! F6 \edwi
.millust end
Whenever you press
.param F6
in &tinsmode, you will exit insert mode (the \e is like pressing the
.param ESC
key), delete the current word, and then re-enter insert mode.
Try entering &tinsmode and pressing
.param F6
:period.

:LI.Exit &tinsmode and press
.param F6
:period.
You will notice that the date is still displayed, since the &tinsmode
and &cmdmode mappings for
.param F6
are different.

:INCLUDE file='exerquit'.

:eOL.

