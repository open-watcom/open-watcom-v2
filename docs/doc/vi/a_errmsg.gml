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
.* Description:  Error Messages.
.*
.* Date         By              Reason
.* ----         --              ------
.* 18-aug-92    Craig Eisler    initial draft
.* 03-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap The &edname Error Messages
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.*
.np
This appendix lists all of the various errors that &edvi reports
during its operation.
.np
In the following error message text, there are certain special characters
that you will not see when the message is displayed; instead, something
is filled in when you see it.  These special characters are:
.*
:DL break.
.*
:DT.%s
:DD.An appropriate string is filled in by &edvi
:period.

:DT.%c
:DD.An appropriate character is filled in by &edvi
:period.

:DT.%d
:DD.An appropriate number is filled in by &edvi
:period.
.*
:eDL.
.*
:DL break.
.*
:DT.*+ operand could be empty
:DD.A regular expression error, issued if you use * or + after an
empty sub-expression.

:DT.?+* follows nothing
:DD.A regular expression error, issued if you use a ?, + or * without
using some sub-expression for them to operate on.

:DT.Already in dot mode
:DD.You pressed dot ('.'), and the string that
was executed tried to do another dot from command mode.

:DT.Already two marks on this line
:DD.Only two marks are allowed on any line.  This error is issued if
you try to place a third mark on the line.

:DT.Cannot delete character
:DD.You tried to delete a character that is not there.

:DT.Cannot insert the character value 0.
:DD.You typed the number 0 at the prompt from the
.param ALT_X
command mode keystroke.

:DT.Cannot open line number window
:DD.&edvi could not open the line number window, probably due to an invalid
position of the command window.

:DT.Case toggle has nothing following
:DD.A regular expression error, issued if you use a case toggle
operator (@ or &tilde.) and do not place an expression after it.

:DT.Character '%c' not found
:DD.You used a 't', 'T', 'f', 'F', ',' or ';' command
mode keystroke, and the letter that you specified is not on the line.

:DT.Directory operation failed
:DD.You tried to 'cd' to a directory that does not exist.

:DT.Empty copy buffer
:DD.You tried to paste a copy buffer that has no contents.

:DT.End of file reached
:DD."nosearchwrap" is set and a search command got to the
end of the edit buffer without finding a string.

:DT.Expecting :
:DD.You coded an expression with the '?' operator and did not specify a ':'.

:DT.File close error
:DD.The "fclose" script command failed - this probably indicates a problem
with your hard disk.

:DT.File exists - use w! to force
:DD.You attempted to write a file out with a new name that already exists.

:DT.File has no name
:DD.You attempted to write a file that has not been given a name.

:DT.File is a tty
:DD.The file names "con", "lptN", "nul", and "prn" are special TTY files.
You attempted to read or edit a file like this.

:DT.File is read only
:DD.You attempted to modify a read-only file.

:DT.File is view only
:DD.You attempted to modify a view-only file.

:DT.File modified - use :q! to force
:DD.You attempted to quit a modified file.

:DT.File not FOPEN
:DD.In a script, you attempted to "fread" or "fwrite" a file that you did
not open with "fopen".

:DT.File not found
:DD.Issued whenever &edvi is looking for a file and it cannot be found.

:DT.File open error
:DD.&edvi will issue this error whenever a file cannot be opened.  Typically,
this happens when you try to edit a directory as a file.

:DT.File read error
:DD.This error occurs when &edvi is trying to read a file.  This could
indicate that there is a problem with your hard disk.

:DT.File seek error
:DD.This error occurs when &edvi is trying to seek to a position in a file.
This could indicate a problem with your hard disk.

:DT.File write error
:DD.This error occurs when &edvi is trying to write a file.  This could
indicate that your hard disk is full, or that there is a problem with
your hard disk.

:DT.File "error.dat" not found
:DD.When a script is being processed, and an error token is needed, the
file "error.dat" is loaded.  If it cannot be found, then this
error is issued.

:DT.Input key map already running
:DD.If another input key mapping is run while an input key map is running,
this error is issued.

:DT.Insufficient stack for allocation
:DD.&edvi tried to do an operation and did not have enough stack space.
Try increasing your stackk setting and trying the operation again.

:DT.Internal err: Invalid undo record found - undo stacks purged
:DD.:INCLUDE file='int_err'.

:DT.Internal err: Null pointer found
:DD.:INCLUDE file='int_err'.

:DT.Internal err: Open undo
:DD.:INCLUDE file='int_err'.

:DT.Internal err: Regexp corrupted pointer
:DD.:INCLUDE file='int_err'.

:DT.Internal err: Regexp foulup
:DD.:INCLUDE file='int_err'.

:DT.Internal err: Regexp memory corruption
:DD.:INCLUDE file='int_err'.

:DT.Internal error: Regular expression NULL argument
:DD.:INCLUDE file='int_err'.

:DT.Invalid abbreviation
:DD.You did not enter an "abbrev" command correctly.

:DT.Invalid alias command
:DD.You did not enter an "alias" command correctly.

:DT.Invalid ASSIGN
:DD.You did not enter script "assign" command correctly.

:DT.Invalid case command
:DD.You did not enter a valid operation after starting the case toggle ('&tilde.')
command mode command.

:DT.Invalid change command
:DD.You did not enter a valid operation after starting the change ('c')
command mode command.

:DT.Invalid command
:DD.You entered an invalid command line command.

:DT.Invalid conditional expression
:DD.You did not code a script "if", "elseif", "quif", "until" or "while"
statement correctly.

:DT.Invalid data in file '%s' at line %d
:DD.This error is issued if one of the .dat files (error.dat, errmsg.dat,
keys.dat) contains invalid data.

:DT.Invalid delete command
:DD.You did not enter a valid operation after starting the delete ('d')
command mode command.

:DT.Invalid EXPR
:DD.You coded an invalid "expr" script command.

:DT.Invalid FCLOSE
:DD.You coded an invalid "fclose" script command.

:DT.Invalid find command
:DD.You issued an invalid search command.

:DT.Invalid FOPEN
:DD.You coded an invalid "fopen" script command.

:DT.Invalid FREAD
:DD.You coded an invalid "fread" script command.

:DT.Invalid FWRITE
:DD.You coded an invalid "fwrite" script command.

:DT.Invalid global command
:DD.You issued an invalid "global" command line command.

:DT.Invalid GOTO
:DD.You coded an invalid "goto" script command.

:DT.Invalid INPUT
:DD.You coded an invalid "input" script command.

:DT.Invalid key '%c'
:DD.You press a key that has no mapping in command mode.

:DT.Invalid LABEL
:DD.You coded an invalid "label" script command.

:DT.Invalid line address
:DD.You used a "copy" or "move" EX mode command, and specified an invalid
destination line.

:DT.Invalid line range
:DD.You used an invalid line range for a "global", "substitute" or shift
("&gt." or "&lt.") command line command.  This occurs if you use line number
past the end of the file.

:DT.Invalid map command
:DD.You issued an invalid "map" or "mapbase" command.

:DT.Invalid mark - use 'a'-'z'
:DD.You attempted to set a mark that was not in the range 'a' to 'z'.

:DT.Invalid match command
:DD.You issued an invalid "match" command line command.

:DT.Invalid menu
:DD.You issued an invalid "menu" command line command.

:DT.Invalid redraw
:DD.You did not specify a valid key after starting
the 'z' command mode command - valid keys are dash ('-'),
ENTER, and dot ('.').

:DT.Invalid copy buffer '%c' - use '1'-'9' or 'a'-'z'
:DD.You tried to access an invalid copy buffer.

:DT.Invalid set command
:DD.You tried to set a variable that does not exist.

:DT.Invalid setcolor command
:DD.You issued an invalid "setcolor" command line command.

:DT.Invalid shift command
:DD.You did not enter a valid operation after starting a shift ('<' or '>')
command mode command.

:DT.Invalid substitute command
:DD.You issued an improper "substitute" command line command.

:DT.Invalid tag found
:DD.Your tags file contained an invalid tag entry.

:DT.Invalid window
:DD.You attempted an operation on an undefined window, or the window
that you attempted to define was invalid.

:DT.Invalid window data
:DD.The parameters to "border", "text", "hilight", or "dimension" were
invalid.

:DT.Invalid yank command
:DD.You did not enter a valid operation after starting the yank ('y')
command mode command.

:DT.invalid [] range
:DD.A regular expression error, issued when you specified an invalid
range inside the square brackets.

:DT.Label not found
:DD.In a script, you attempted to "goto" a label that was not defined.

:DT.Mark '%c' no longer valid
:DD.You tried to access a mark that has become invalid (the line or
column that had the mark no longer exists).

:DT.Mark '%c' not set
:DD.You tried to access an undefined mark.

:DT.Matching '%s' not found
:DD.You used the command mode key '%' to look for a matching string to
the string under the cursor, only the match could not be found.

:DT.nested *?+
:DD.A regular expression error, issued if you have two or more of the '*', '?'
or '+' characters in a row.

:DT.No character to replace
:DD.You used the command mode key 'r' to replace a character on an empty line.

:DT.No file specified
:DD.You issued a "compile", "load" or "source" command, and did not specify
a file.  Another source of this error is using the "write" command with
a line range and not specifying a file name.

:DT.No more undos
:DD.You issued an undo command, and there are no more undos to perform.

:DT.No more windows
:DD.You exceeded the limit of 250 windows that can be open at the same time.

:DT.No previous search string
:DD.You pressed 'n' or 'N' in command mode, but have not yet issued a search command.

:DT.No selection
:DD.You used the 'r' operator with 'd', 'c', '&tilde.', '&gt.', '&lt.', '!' in command
mode, but there is no selected region.  Another source of this error is
specifying the pound sign ('#') as a line range on the command line,
and having no selected region.

:DT.No string specified
:DD.You indicated that you are starting a string on the command line by
using a double quote ('"') or forward slash ('/'), but you did
not specify any characters at all for the string.

:DT.No such abbreviation
:DD.You attempted to use the command line command "unabbrev" for
an abbreviation that does not exist.

:DT.No such alias
:DD.You attempted to use the command line command "unalias" for
an alias that does not exist.

:DT.No such drive
:DD.You used the "cd" command line command to attempt to switch to a
disk drive that does not exist.

:DT.No such line
:DD.You specified a line number that does not exist in the current edit
buffer.

:DT.Not enough room in line
:DD.You attempted to add characters to a line, and the resulting line
would be longer than the setting of "maxlinelen".

:DT.Not that many words
:DD.You attempted a command mode command that tried to access more words than
were on the current line.

:DT.Not valid while in ex mode
:DD.You attempted to use a command from EX mode that was not allowed:
an "edit" command line command with no parameter, or a "read" command line
command with no parameter.

:DT.Nothing to match
:DD.You used the command mode percent ('%') keystroke, but there was nothing
on the current line that could be matched.

:DT.Only valid in ex mode
:DD.You attempted to use an EX mode only command from the command line: "append",
"change", "insert", or "list".

:DT.Out of memory
:DD.The editor ran out of memory.  Try removing some TSR's (terminate and
stay resident programs) or increasing swap space (maxswapk).

:DT.Repeat string too long
:DD.You entered a repeat count with too many digits, only 9 digits are allowed.

:DT.Copy buffer too big
:DD.You attempted to execute a copy buffer that is larger than 1024 bytes.

:DT.Script already loaded
:DD.You attempted to load a script that was previously loaded.

:DT.String '%s' not found
:DD.A search string cannot be found in the file.

:DT.Swap file full
:DD.&edvi ran out of space in the swap file, but needed more space.
Increase the maxswapk setting.

:DT.Swap file open error
:DD.&edvi could not open a swap file.  This could indicate that there
are too many open files, or that the directory is full (if the swap
file is on the root directory), or a problem with your hard disk.

:DT.Swap file read error
:DD.An error was encountered while attempting to read the swap file.  This
could indicate problems with your hard disk.

:DT.Swap file write error
:DD.An error was encountered while attempting to read the swap file.  This
could indicate problems with your hard disk, or that the disk is full.

:DT.Tag '%s' not found
:DD.If the tag you have requested cannot be found in the tags file, this
error is issued.

:DT.Too many ()
:DD.A regular expression error.  There is a limit of 21 nested parenthesis
in a regular expression.

:DT.Too many match items
:DD.When using the match command to add another match string.  There is
a limit of 9 different match pairs.

:DT.Top of file reached
:DD.Issued if you have nosearchwrap set and a search command gets to the
top of the edit buffer without finding a string.

:DT.Trailing \
:DD.A regular expression error, issued if a backslash ('\') is the last
character on a line.  Since a backslash is an escape character, it must
always have a character following it.

:DT.Unmatched ()
:DD.A regular expression error.  You have an unequal number of open and
closed round brackets.

:DT.Unmatched []
:DD.A regular expression error.  You have specified one square bracket, without
using the matching one.

:DT.Warning: file is read only
:DD.This warning is issued if "readonlycheck" is set.  The message is issued
every time you modify a read only file.

:eDL.
