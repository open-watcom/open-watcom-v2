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
:CHAPTER.&edname Error Messages
This appendix lists all of the various errors that &edname reports
during its operation.
:P.
In the following error message text, there are certain special characters
that you will not see when the message is displayed; instead, something
is filled in when you see it.  These special characters are:
:DEFLIST.
:DEFITEM.%s
An appropriate string is filled in by &edname
:PERIOD.
:DEFITEM.%c
An appropriate character is filled in by &edname
:PERIOD.
:DEFITEM.%d
An appropriate number is filled in by &edname
:PERIOD.
:eDEFLIST.
:P.
:DEFLIST.
:DEFITEM.*+ operand could be empty
A regular expression error, issued if you use * or + after an
empty sub-expression.

:DEFITEM.?+* follows nothing
A regular expression error, issued if you use a ?,+ or * without
using some sub-expression for them to operate on.

:DEFITEM.Already in dot mode
You pressed dot ('.'), and the string that
was executed tried to do another dot from command mode.

:DEFITEM.Already two marks on this line
Only two marks are allowed on any line.  This error is issued if
you try to place a third mark on the line.

:DEFITEM.Cannot delete character
You tried to delete a character that is not there.

:DEFITEM.Cannot insert the character value 0.
You typed the number 0 at the prompt from the
:HILITE.ALT_X
command mode keystroke.

:DEFITEM.Cannot open line number window
&edname could not open the line number window, probably due to an invalid
position of the command window.

:DEFITEM.Case toggle has nothing following
A regular expression error, issued if you use a case toggle
operator (@ or ~~) and do not place an expression after it.

:DEFITEM.Character '%c' not found
You used a 't', 'T', 'f', 'F', ',' or ';' command
mode keystroke, and the letter that you specified is not on the line.

:DEFITEM.Directory operation failed
You tried to 'cd' to a directory that does not exist.

:DEFITEM.Empty copy buffer
You tried to paste a copy buffer that has no contents.

:DEFITEM.End of file reached
Nosearchwrap is set and a search command got to the
end of the edit buffer without finding a string.

:DEFITEM.Expecting :
You coded an expression with the '?' operator and did not specify a ':'.

:DEFITEM.File close error
The "fclose" script command failed - this probably indicates a problem
with your hard disk.

:DEFITEM.File exists - use w! to force
You attempted to write a file out with a new name that already exists.

:DEFITEM.File has no name
You attempted to write a file that has not been given a name.

:DEFITEM.File is a tty
The file names "con", "lptN", "nul", and "prn" are special TTY files.
You attempted to read or edit a file like this.

:DEFITEM.File is read only
You attempted to modify a read-only file.

:DEFITEM.File is view only
You attempted to modify a view-only file.

:DEFITEM.File modified - use :q! to force
You attempted to quit a modified file.

:DEFITEM.File not FOPEN
In a script, you attempted to "fread" or "fwrite" a file that you did
not open with "fopen".

:DEFITEM.File not found
Issued whenever &edname is looking for a file and it cannot be found.

:DEFITEM.File open error
&edname will issue this error whenever a file cannot be opened.  Typically,
this happens when you try to edit a directory as a file.

:DEFITEM.File read error
This error occurs when &edname is trying to read a file.  This could
indicate that there is a problem with your hard disk.

:DEFITEM.File seek error
This error occurs when &edname is trying to seek to a position in a file.
This could indicate a problem with your hard disk.

:DEFITEM.File write error
This error occurs when &edname is trying to write a file.  This could
indicate that your hard disk is full, or that there is a problem with
your hard disk.

:DEFITEM.File "error.dat" not found
When a script is being processed, and an error token is needed, the
file "error.dat" is loaded.  If it cannot be found, then this
error is issued.

:DEFITEM.Input key map already running
If another input key mapping is run while an input key map is running,
this error is issued.

:DEFITEM.Insufficient stack for allocation
&edname tried to do an operation and did not have enough stack space.
Try increasing your stackk setting and trying the operation again.

:DEFITEM.Internal err: Invalid undo record found - undo stacks purged
:INCLUDE file='int_err'.

:DEFITEM.Internal err: Null pointer found
:INCLUDE file='int_err'.

:DEFITEM.Internal err: Open undo
:INCLUDE file='int_err'.

:DEFITEM.Internal err: Regexp corrupted pointer
:INCLUDE file='int_err'.

:DEFITEM.Internal err: Regexp foulup
:INCLUDE file='int_err'.

:DEFITEM.Internal err: Regexp memory corruption
:INCLUDE file='int_err'.

:DEFITEM.Internal error: Regular expression NULL argument
:INCLUDE file='int_err'.

:DEFITEM.Invalid abbreviation
You did not enter an "abbrev" command correctly.

:DEFITEM.Invalid alias command
You did not enter an "alias" command correctly.

:DEFITEM.Invalid ASSIGN
You did not enter script "assign" command correctly.

:DEFITEM.Invalid case command
You did not enter a valid operation after starting the case toggle ('~~')
command mode command.

:DEFITEM.Invalid change command
You did not enter a valid operation after starting the change ('c')
command mode command.

:DEFITEM.Invalid command
You entered an invalid command line command.

:DEFITEM.Invalid conditional expression
You did not code a script "if", "elseif", "quif", "until" or "while"
statement correctly.

:DEFITEM.Invalid data in file '%s' at line %d
This error is issued if one of the .dat files (error.dat, errmsg.dat,
keys.dat) contains invalid data.

:DEFITEM.Invalid delete command
You did not enter a valid operation after starting the delete ('d')
command mode command.

:DEFITEM.Invalid EXPR
You coded an invalid "expr" script command.

:DEFITEM.Invalid FCLOSE
You coded an invalid "fclose" script command.

:DEFITEM.Invalid find command
You issued an invalid search command.

:DEFITEM.Invalid FOPEN
You coded an invalid "fopen" script command.

:DEFITEM.Invalid FREAD
You coded an invalid "fread" script command.

:DEFITEM.Invalid FWRITE
You coded an invalid "fwrite" script command.

:DEFITEM.Invalid global command
You issued an invalid "global" command line command.

:DEFITEM.Invalid GOTO
You coded an invalid "goto" script command.

:DEFITEM.Invalid INPUT
You coded an invalid "input" script command.

:DEFITEM.Invalid key '%c'
You press a key that has no mapping in command mode.

:DEFITEM.Invalid LABEL
You coded an invalid "label" script command.

:DEFITEM.Invalid line address
You used a "copy" or "move" EX mode command, and specified an invalid
destination line.

:DEFITEM.Invalid line range
You used an invalid line range for a "global", "substitute" or shift
(">" or "<") command line command.  This occurs if you use line number
past the end of the file.

:DEFITEM.Invalid map command
You issued an invalid "map" or "mapbase" command.

:DEFITEM.Invalid mark - use 'a'-'z'
You attempted to set a mark that was not in the range 'a' to 'z'.

:DEFITEM.Invalid match command
You issued an invalid "match" command line command.

:DEFITEM.Invalid menu
You issued an invalid "menu" command line command.

:DEFITEM.Invalid redraw
You did not specify a valid key after starting
the 'z' command mode command - valid keys are dash ('-'),
ENTER, and dot ('.').

:DEFITEM.Invalid copy buffer '%c' - use '1'-'9' or 'a'-'z'
You tried to access an invalid copy buffer.

:DEFITEM.Invalid set command
You tried to set a variable that does not exist.

:DEFITEM.Invalid setcolor command
You issued an invalid "setcolor" command line command.

:DEFITEM.Invalid shift command
You did not enter a valid operation after starting a shift ('<' or '>')
command mode command.

:DEFITEM.Invalid substitute command
You issued an improper "substitute" command line command.

:DEFITEM.Invalid tag found
Your tags file contained an invalid tag entry.

:DEFITEM.Invalid window
You attempted an operation on an undefined window, or the window
that you attempted to define was invalid.

:DEFITEM.Invalid window data
The parameters to "border", "text", "hilight", or "dimension" were
invalid.

:DEFITEM.Invalid yank command
You did not enter a valid operation after starting the yank ('y')
command mode command.

:DEFITEM.invalid [] range
A regular expression error, issued when you specified an invalid
range inside the square brackets.

:DEFITEM.Label not found
In a script, you attempted to "goto" a label that was not defined.

:DEFITEM.Mark '%c' no longer valid
You tried to access a mark that has become invalid (the line or
column that had the mark no longer exists).

:DEFITEM.Mark '%c' not set
You tried to access an undefined mark.

:DEFITEM.Matching '%s' not found
You used the command mode key '%' to look for a matching string to
the string under the cursor, only the match could not be found.

:DEFITEM.nested *?+
A regular expression error, issued if you have two or more of the '*', '?'
or '+' characters in a row.

:DEFITEM.No character to replace
You used the command mode key 'r' to replace a character on an empty line.

:DEFITEM.No file specified
You issued a "compile", "load" or "source" command, and did not specify
a file.  Another source of this error is using the "write" command with
a line range and not specifying a file name.

:DEFITEM.No more undos
You issued an undo command, and there are no more undos to perform.

:DEFITEM.No more windows
You exceeded the limit of 250 windows that can be open at the same time.

:DEFITEM.No previous search string
You pressed 'n' or 'N' in command mode, but have not yet issued a search command.

:DEFITEM.No selection
You used the 'r' operator with 'd', 'c', '~', '>', '<', '!' in command
mode, but there is no selected region.  Another source of this error is
specifying the pound sign ('#') as a line range on the command line,
and having no selected region.

:DEFITEM.No string specified
You indicated that you are starting a string on the command line by
using a double quote ('"') or forward slash ('/'), but you did
not specify any characters at all for the string.

:DEFITEM.No such abbreviation
You attempted to use the command line command "unabbrev" for
an abbreviation that does not exist.

:DEFITEM.No such alias
You attempted to use the command line command "unalias" for
an alias that does not exist.

:DEFITEM.No such drive
You used the "cd" command line command to attempt to switch to a
disk drive that does not exist.

:DEFITEM.No such line
You specified a line number that does not exist in the current edit
buffer.

:DEFITEM.Not enough room in line
You attempted to add characters to a line, and the resulting line
would be longer than the setting of "maxlinelen".

:DEFITEM.Not that many words
You attempted a command mode command that tried to access more words than
were on the current line.

:DEFITEM.Not valid while in ex mode
You attempted to use a command from EX mode that was not allowed:
an "edit" command line command with no parameter, or a "read" command line
command with no parameter.

:DEFITEM.Nothing to match
You used the command mode percent ('%') keystroke, but there was nothing
on the current line that could be matched.

:DEFITEM.Only valid in ex mode
You attempted to use an EX mode only command from the command line: "append",
"change", "insert", or "list".

:DEFITEM.Out of memory
The editor ran out of memory.  Try removing some TSR's (terminate and
stay resident programs) or increasing swap space (maxswapk).

:DEFITEM.Repeat string too long
You entered a repeat count with too many digits, only 9 digits are allowed.

:DEFITEM.Copy buffer too big
You attempted to execute a copy buffer that is larger than 1024 bytes.

:DEFITEM.Script already loaded
You attempted to load a script that was previously loaded.

:DEFITEM.String '%s' not found
A search string cannot be found in the file.

:DEFITEM.Swap file full
&edname ran out of space in the swap file, but needed more space.
Increase the maxswapk setting.

:DEFITEM.Swap file open error
&edname could not open a swap file.  This could indicate that there
are too many open files, or that the directory is full (if the swap
file is on the root directory), or a problem with your hard disk.

:DEFITEM.Swap file read error
An error was encountered while attempting to read the swap file.  This
could indicate problems with your hard disk.

:DEFITEM.Swap file write error
An error was encountered while attempting to read the swap file.  This
could indicate problems with your hard disk, or that the disk is full.

:DEFITEM.Tag '%s' not found
If the tag you have requested cannot be found in the tags file, this
error is issued.

:DEFITEM.Too many ()
A regular expression error.  There is a limit of 21 nested parenthesis
in a regular expression.

:DEFITEM.Too many match items
When using the match command to add another match string.  There is
a limit of 9 different match pairs.

:DEFITEM.Top of file reached
Issued if you have nosearchwrap set and a search command gets to the
top of the edit buffer without finding a string.

:DEFITEM.Trailing \
A regular expression error, issued if a backslash ('\') is the last
character on a line.  Since a backslash is an escape character, it must
always have a character following it.

:DEFITEM.Unmatched ()
A regular expression error.  You have an unequal number of open and
closed round brackets.

:DEFITEM.Unmatched []
A regular expression error.  You have specified one square bracket, without
using the matching one.

:DEFITEM.Warning: file is read only
This warning is issued if "readonlycheck" is set.  The message is issued
every time you modify a read only file.

:eDEFLIST.
