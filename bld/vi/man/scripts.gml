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
.* 06-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id='scripts'.Editor Script Language
:CMT :SECTION.Introduction
&edname supports a powerful command language.  In a script,
you may use any &cmdline command, along with a number of special commands
explicitly for the script environment.
:P.
White space is ignored in a script file, unless a line starts with a
right angle bracket ('>'). Comments may be imbedded in a script file
by starting the line with pound sign ('#').
:P.
A script is invoked using the &cmdline command
:KEYWORD.source
:PERIOD.
Examples are:
:P.
:ILLUST.
source test2.vi
:eILLUST.
:ILLUST.
source test.vi parm1 parm2 parm3 parm4
:eILLUST.
:P.
A script may be invoked with a set of optional parameters.
These optional parameters are accessed in the script by using
:HILITE.%n
:PERIOD.
Every occurrence of
:HILITE.%n
in the script is replaced by the corresponding parameter.
To access parameter above 9, brackets must surround the number.  This
is because:
:ILLUST.
%10
:eILLUST.
cannot be distinguished from the variable
:HILITE.%1
followed by a 0, and
the variable
:HILITE.%10
:PERIOD.
To remove the ambiguity, brackets are used:
:ILLUST.
%(10)
:eILLUST.
All parameters can be accessed by using
:HILITE.%*
:PERIOD.
:P.
To allow multiple words
in a single parameter, delimit the sequence by forward slashes ('/') or
double quotes ('"').  For example, the line
:ILLUST.
source test.vi "a b c" d e
:eILLUST.
would cause the script test.vi to have the following variables defined:
:ILLUST.
%* = a b c d e
%1 = a b c
%2 = d
%3 = e
:eILLUST.
:P.
General variables, both local and global, are also supported in the
editor script language. Any line in a script
that is not one of the script commands
has all the variables on it expanded before the line is processed.
Script commands can manipulate the variables. For more information, see
the section
:HDREF refid='scrvars'.
of this chapter.
:P.
There are several useful &cmdline
commands dealing with &edname scripts, they are:
:DEFLIST.
:DEFITEM.compile
Used to compile a script.
This allows much faster execution of the script
by &edname.
:PERIOD.
:DEFITEM.load
Used to make a script resident in &edname.
:PERIOD.
This allows much faster invocation of the script,
since &edname does not have to search for it or parse it.
:eDEFLIST.
:P.
If a system command is spawned from a script (using the exclamation point ('!')
command), then &edname does not
pause after the system command is finished running.  However, if
you set
:KEYWORD.pauseonspawnerr
:CONT.,
then &edname will pause after a system command is executed from a script
if the system command returned an error.
:SECTION id='scrvars'.Script Variables
General variables are supported in a &edname script.
Variables are preceded by a percent symbol ('%').
Variables with more than one letter must be enclosed by brackets, for
example:
:ILLUST.
%a     - references variable named a.
%(abc) - references variable named abc.
:eILLUST.
The brackets are required to disambiguate single letter variables followed
by text from multiple letter variables.
:P.
Both local and global variables are supported.  They are distinguished
by the case of the first letter:  local variables must begin with
a lower case letter, and global variables begin with an upper case
variable.  Example variables:
:ILLUST.
%A     - global variable named A.
%a     - local variable named a.
%(AbC) - global variable named AbC.
%(abC) - local variable named abC.
:eILLUST.
Global variables are valid for the life of the editing
session.
:P.
Local variables are only valid for the life of the script that they
are used in.
:P.
:SUBSECT.Pre-defined Global Variables
There are a number of global variables that take on values as the
editor runs, they are:
:DEFLIST.
:DEFITEM.%C   	 
Contains the current column number in the current edit buffer.
:DEFITEM.%D   	 
Drive of current file, based on the actual path.
:DEFITEM.%(D1)  	 
Drive of current file, as typed by the user.  This could have no value.
:DEFITEM.%E   	 
File name extension of current file.
:DEFITEM.%F   	 
Current file name (including name and extension).
:DEFITEM.%H   	 
Home directory of a file. This is the directory where the edit command
was issued.
:DEFITEM.%N   	 
Name of the current file, extension removed.
:DEFITEM.%M   	 
Modified status of the current file - set to 1 if the file
has been modified, and a 0 otherwise.
:DEFITEM.%(OS)    
What operating system the editor is hosted on. Possible values are:
:UL compact.
:LI.dos (protect and real mode).
:LI.unix (QNX, Linux or other Unix-based systems)
:LI.os2
:LI.os2v2
:LI.nt
:eUL.
:DEFITEM.%(OS386) 
This variable is set to 1 is the host operating system is 386
(or higher) based.  The possible 386 environments are:
:UL compact.
:LI.dos (protect mode).
:LI.os2v2
:LI.nt
:LI.unix (when running on a 386)
:eUL.
:DEFITEM.%P   	 
Path of current file (no extension, name, or drive) based on the actual
full path to the file.
:DEFITEM.%(P1)	 
Path of current file (no extension, name, or drive) based on the name
typed by the user.  This could have no value.
:DEFITEM.%R   	 
Contains the current row (line number) in the current edit buffer.
:DEFITEM.%(SH)	 
Height of entire screen in characters.
:DEFITEM.%(SW)	 
Width of entire screen in characters.
:DEFITEM.%(Sysrc)
Return code from last system command.
:eDEFLIST.
:eSUBSECT.
:SECTION.Hook Scripts
&edname has several hook points where a script,
if defined by the user, is invoked.  This allows you to
intercept the editor at key points
to change its behaviour.  A script
that is invoked at a hook point
is referred to as a
:KEYWORD.hook script
:PERIOD.
:P.
Each
:KEYWORD.hook script
is identified by a particular global variable.  Whenever &edname
reaches a hook point, it checks if the global variable is defined,
and if it is, the global variables contents are treated like a script
name, and that script is invoked.
:P.
The hook points are:
:UL.
:LI.after a new file has been read.
:LI.before a modified file is saved and exited.
:LI.after return is pressed on the &cmdline
:PERIOD.
:LI.whenever an unmodified file is modified.
:LI.whenever a selected (highlighted) column range is
chosen (via mouse click or keyboard).
:LI.whenever a selected (highlighted) line range
is chosen (via mouse click or keyboard).
:eUL.
:DEFLIST.
:DEFITEM.Read Hook
The
:KEYWORD.hook script
is called just after a new file has been read into the editor.
:BLANKLINE.
The script invoked is the file specified by the global variable
:HILITE.%(Rdhook)
:PERIOD.

:DEFITEM.Write Hook
The
:KEYWORD.hook script
is called just before a modified file
is to be saved and exited.
:BLANKLINE.
The script invoked is the file specified by the global variable
:HILITE.%(Wrhook).

:DEFITEM.Command Hook
The
:KEYWORD.hook script
is called after the return is pressed from the &cmdline
:PERIOD.
The global variable
:HILITE.%(Com)
contains the current command string, and may be modified.
Whatever it is modified to is what will be processed by the &cmdline
processor.
:BLANKLINE.
The script invoked is the file specified by the global variable
:HILITE.%(Cmdhook)
:PERIOD.

:DEFITEM.Modified Hook
The
:KEYWORD.hook script
is called whenever
a command is about to modify an unmodified file.  If the file is
modified, the hook is not called.
:BLANKLINE.
The script invoked is the file specified by the global variable
:HILITE.%(Modhook)
:PERIOD.

:DEFITEM.Mouse Columns Sel Hook
The
:KEYWORD.hook script
is called whenever a selected column range
has been picked.  Picking a selected region is done by
right-clicking the region with the mouse, or by double clicking
the region with the mouse, or by using the underscore ('_') &cmdmode
keystroke.
:BLANKLINE.
The script is invoked with the following parameters:
:DEFLIST.
:DEFITEM.%1
The selected string.
:DEFITEM.%2
Line on screen of selected string.
:DEFITEM.%3
Column on screen of start of selected string.
:DEFITEM.%4
Column on screen of end of selected string.
:eDEFLIST.
The script invoked is the file is specified by the global variable
:HILITE.%(MCselhook)
:PERIOD.

:DEFITEM.Mouse Lines Sel Hook
The
:KEYWORD.hook script
is called whenever a selected line range
has been picked.  Picking a selected region is done by
right-clicking the region with the mouse, or by double clicking
the region with the mouse, or by using the underscore ('_')
&cmdmode keystroke.
:BLANKLINE.
:DEFLIST.
:DEFITEM.%1
Line on screen where selection request occurred.
:DEFITEM.%2
Column on screen where selection request occurred.
:DEFITEM.%3
First line of selected region.
:DEFITEM.%4
Last line of selected region.
:eDEFLIST.
The script invoked is the file specified by the global
variable
:HILITE.%(MLselhook)
:PERIOD.
:eDEFLIST.

:SECTION id='screxpr'.Expressions
&edname allows the use of constant expressions in its script
language. Long integers and strings may be used in an expression.
Some sample expressions are:
:ILLUST.
5*3+12
(7*7)+10*((3+5)*8+9)
5 >= %(var)
("%(str)" == "foo") || ("%(str)" == "bar")
(5+%i*3 == 15)
rdonly == 1
:eILLUST.
:P.
An expression is composed of operators and tokens.  Operators act
on the tokens to give a final result.
:P.
A token in an expression may be a special keyword, a boolean setting
value, an integer, or
a string.
:P.
A string is indicated by surrounding the string with double quotes (").
A token is an integer if it starts with a numeric
digit (0 to 9).
:P.
If a token starts with a dot ('.'), then the remainder of the token
is assumed to be a setting token.  This token evaluates to be
1 or 0 for a boolean setting, or to the actual value of the setting
for all others.
:ILLUST.
 .autoindent - 1 if autoindent is true, 0 otherwise
 .ai         - 1 if autoindent is true, 0 otherwise
 .autosave   - current value of autosave
 .tmpdir     - current tmpdir string
:eILLUST.
:P.
If a token is not surrounded by double quotes, and is not a keyword and
is not an integer, then that token is assumed to be a string.
:P.
If an expression contains conditional operators, then the result of the
expression is a boolean value (1 or 0). The following script language
control flow commands expect boolean results:
:UL compact.
:LI.:KEYWORD.if
:LI.:KEYWORD.elseif
:LI.:KEYWORD.quif
:LI.:KEYWORD.while
:LI.:KEYWORD.until
:eUL.
:P.
The following are conditional operators in an expression:
:UL compact.
:LI.== (equal to)
:LI.!= (not equal to)
:LI.> (greater than)
:LI.>= (greater than or equal to)
:LI.< (less than)
:LI.<= (less than or equal to)
:LI.&& (boolean AND)
:LI.|| (boolean OR)
:eUL.
An expression may also operate on its token using various mathematical
operators, these operators are
:UL compact.
:LI.+ (plus)
:LI.- (minus)
:LI.* (multiply)
:LI./ (divide)
:LI.** (exponentiation)
:LI.^ (bitwise NOT)
:LI.| (bitwise OR)
:LI.& (bitwise AND)
:LI.>> (bit shift down)
:LI.<< (bit shift up)
:eUL.
:P.
Special keyword tokens are:
:DEFLIST.
:DEFITEM.ERR_???
These are symbolic representations of all possible errors while
executing in &edname..  These values are found in
:KEYWORD.error.dat
:PERIOD.
These values are described in the appendix
:HDREF refid=errcode.
:PERIOD.

:DEFITEM.lastrc
This keyword evaluates to the return code issued by the last command run
in the script. Possible values to compare against are found in
:KEYWORD.error.dat
These values may are described in the appendix
:HDREF refid=errcode.
:PERIOD.

:DEFITEM.rdonly
This keyword evaluates to 1 if the current file is read only, and
0 if it is not read only.

:DEFITEM.config
This keyword evalutes to a number representing the current mode
the screen is configured to. Possible values are:
:DEFLIST.
:DEFITEM.100
Screen is in color mode.
:DEFITEM.10
Screen is in black and white mode.
:DEFITEM.1
Screen is in monochrome mode.
:eDEFLIST.
This may be used to have different configurations built into
your
:KEYWORD.configuration script
:KEYWORD.ed.cfg
:PERIOD.
:DEFITEM.black
This keyword evalutes to the integer representing the color black (0).

:DEFITEM.blue
This keyword evalutes to the integer representing the color blue (1).

:DEFITEM.green
This keyword evalutes to the integer representing the color green (2).

:DEFITEM.cyan
This keyword evalutes to the integer representing the color cyan (3).

:DEFITEM.red
This keyword evalutes to the integer representing the color red (4).

:DEFITEM.magenta
This keyword evalutes to the integer representing the color magenta (5).

:DEFITEM.brown
This keyword evalutes to the integer representing the color brown (6).

:DEFITEM.white
This keyword evalutes to the integer representing the color white (7).

:DEFITEM.dark_gray
This keyword evalutes to the integer representing the color dark_gray (8).

:DEFITEM.light_blue
This keyword evalutes to the integer representing the color light_blue (9).

:DEFITEM.light_green
This keyword evalutes to the integer representing the color light_green (10).

:DEFITEM.light_cyan
This keyword evalutes to the integer representing the color light_cyan (11).

:DEFITEM.light_red
This keyword evalutes to the integer representing the color light_red (12).

:DEFITEM.light_magenta
This keyword evalutes to the integer representing the color light_magenta (13).

:DEFITEM.yellow
This keyword evalutes to the integer representing the color yellow (14).

:DEFITEM.bright_white
This keyword evalutes to the integer representing the color bright_white (15).

:eDEFLIST.
:SUBSECT.Expression BNF
This section describes a BNF for the construction of constant expressions.
:DEFLIST.break
:DEFITEM.expression
: conditional-exp
:DEFITEM. conditional-exp
: log-or-exp
:BREAK.
| log-or-exp ? expression : conditional-exp

:DEFITEM.log-or-exp
: log-and-exp
:BREAK.
| log-or-exp || log-and-exp

:DEFITEM.log-and-exp
: bit-or-exp
:BREAK.
| log-and-exp && bit-or-exp

:DEFITEM.bit-or-exp
: bit-xor-exp
:BREAK.
| bit-or-exp | bit-xor-exp

:DEFITEM.bit-xor-exp
: bit-and-exp
:BREAK.
| bit-xor-exp ^ bit-and-exp

:DEFITEM.bit-and-exp
: equality-exp
:BREAK.
| bit-and-exp & equality-exp

:DEFITEM.equality-exp
: relational-exp
:BREAK.
| equality-exp == relational-exp
:BREAK.
| equality-exp != relational-exp

:DEFITEM.relational-exp
: shift-exp
:BREAK.
| relational-exp > shift-exp
:BREAK.
| relational-exp < shift-exp
:BREAK.
| relational-exp >= shift-exp
:BREAK.
| relational-exp <= shift-exp

:DEFITEM.shift-exp
: additive-exp
:BREAK.
| shift-exp << additive-exp
:BREAK.
| shift-exp >> additive-exp

:DEFITEM.additive-exp
: multiplicative-exp
:BREAK.
| additive-exp + multiplicative-exp
:BREAK.
| additive-exp - multiplicative-exp

:DEFITEM.multiplicative-exp
: exponent-exp
:BREAK.
| multiplicative-exp * exponent-exp
:BREAK.
| multiplicative-exp / exponent-exp
:BREAK.
| multiplicative-exp % exponent-exp

:DEFITEM.exponent-exp
: unary-exp
:BREAK.
| exponent-exp ** unary-exp

:DEFITEM.unary-exp
: primary-exp
:BREAK.
| - unary-exp
:BREAK.
| ~~ unary-exp
:BREAK.
| ! unary-exp

:DEFITEM.primary-exp
: token
:BREAK.
| ( expression )

:DEFITEM.token
: INTEGER
:BREAK.
| STRING
:BREAK.
| KEYWORD
:eDEFLIST.
:eSUBSECT.

:SECTION.Control Flow Commands
This section gives a brief overview of the control flow commands of the
&edname script language.  For a full description of all
script commands, see the next section
:HDREF refid='scrcmds'.
:PERIOD.
:P.
:SUBSECT.The LOOP Block
The
:KEYWORD.loop
block is similar to the
:HILITE.do-while
construct in C.  The flow of the loop may be modified using the
:HILITE.break
:CONT.,
:HILITE.continue
:CONT.
or
:HILITE.quif
script commands.
:P.
The loop may be set to run until a termination condition is met by using the
:KEYWORD.loop
-
:KEYWORD.until
commands.
:P.
The loop may be set to run without any termination condition by using the
:KEYWORD.loop
-
:KEYWORD.endloop
commands.
:P.
An overview of a loop block is:
:ILLUST.
loop
    break
    continue
    quif <expr>
until <expr>

loop
    break
    continue
    quif <expr>
endloop
:eILLUST.
:eSUBSECT.

:SUBSECT.The WHILE Block
The
:KEYWORD.while
block is similar to the
:HILITE.while
loop construct in C.  The flow of the while loop may be modified using the
:HILITE.break
:CONT.,
:HILITE.continue
:CONT.
or
:HILITE.quif
script commands.
:P.
The while loop is set up using the
:KEYWORD.while
-
:KEYWORD.endwhile
commands.
:P.
An overview of the
:KEYWORD.while
block is:
:ILLUST.
while <expr>
    break
    continue
    quif <expr>
endwhile
:eILLUST.
:eSUBSECT.

:SUBSECT.The IF Block
The
:KEYWORD.if
block is similar to the
:HILITE.if-else
construct in C.
:P.
An overview of the
:KEYWORD.if
block is:
:ILLUST.
if <expr>
elseif <expr>
elseif <expr>
else
endif
:eILLUST.
:eSUBSECT.

:SECTION id='scrcmds'.Script Commands
:INCLUDE file='cmdintro'.
For example, in its syntax model,
the ASSIGN command is specified as "ASSIGN", indicating that all the
letters are required (there are no abbreviations, only "assign" is
accepted as the command).
:P.
The term
:HILITE.<expr>
is used to indicate an expression in the following commands.
Expressions are discussed in full detail in the section
:HDREF refid='screxpr'.
of this chapter.
:P.
Script variables are used by some of the following commands.  Variables
are discussed in full detail in the section
:HDREF refid='scrvars'.
of this chapter.
:P.
When a script command terminates, 
:HILITE.lastrc
is sometimes set to a value.  This value may be tested in an expression.
Script commands that set this have a
:HILITE.Returns
section.
:P.
:FNLIST.Editor Script Commands

:FUNC.ATOMIC
:SYNTAX.* ATOMIC
This command causes all editing actions done by the script
to all be part of one undo record.  This way, the action of the entire
script can be eliminated with a single
:KEYWORD.undo
command; i.e., it is an atomic action.
:eFUNC.

:FUNC.ASSIGN
:SYNTAX.* ASSIGN <v1> "=" /<val>/"r$@xl"
This command is used to assign the value
:CMDPARM.<val>
to the variable &parm1.
:PERIOD.
:BLANKLINE.
The forward slashes ('/') around
:CMDPARM.<val>
are only need if there are spaces in
:CMDPARM.<val>
:CONT.,
or if one of the special flags
:HILITE.r
:CONT.,
:HILITE.x
:CONT.,
:HILITE.l
:CONT.,
:HILITE.$
or
:HILITE.@
is required at the end.
:BLANKLINE.
The special flags have the following meaning:
:DEFLIST.
:DEFITEM.r
When this flag is used,
:CMDPARM.<val>
may contain regular expression replacement
strings (using the last regular expression searched for). 
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:PERIOD.
:DEFITEM.l
When this flag is used,
:CMDPARM.<val>
is assumed to be an expression that indicates a line number. The expression
is evaluated, and the data on the corresponding line number is assigned
to &parm1
:PERIOD.
:DEFITEM.x
When this flag is used,
:CMDPARM.<val>
is assumed to be an expression, and is evaluated. The result is
assigned to &parm1.
:PERIOD.
For another way of assigning expression results to a variable, see the
:KEYWORD.expr
script command.
:DEFITEM.$ (dollar sign)
When this flag is used,
:CMDPARM.<val>
is assumed to be the name of an operating system environment variable,
and the contents of that environment variable is what is assigned to &parm1.
:PERIOD.
:DEFITEM.@
When this flag is used,
:CMDPARM.<val>
may be the name of one of the
:KEYWORD.set
command parameters. &parm1 will be given the current value of that
parameter.
:eDEFLIST.
:BLANKLINE.
:CMDPARM.<val>
may be coded as a special operator.  If
:CMDPARM.<val>
is coded this way, the forward slashes ('/') must NOT be used.
The special operators are:
:DEFLIST.
:DEFITEM.strlen <v>
Computes the length of the variable
:CMDPARM.<v>
:PERIOD.
This value is assigned to &parm1.
:PERIOD.
:DEFITEM.strchr <v> <c>
Computes the offset of the character
:CMDPARM.<c>
in the variable <v>.
The offset is assigned to &parm1.
:PERIOD.
Note that the character
:CMDPARM.<c>
may be a variable, the value of which will be expanded before offset
is computed.
:PERIOD.
:DEFITEM.substr <v> <n1> <n2>
Computes a substring of the string contained in the variable
:CMDPARM.<v>
:PERIOD.
The substring is composed of characters from offset
:CMDPARM.<n1>
to offset
:CMDPARM.<n2>
:PERIOD.
The substring is assigned to &parm1.
:PERIOD.
Note that the parameters 
:CMDPARM.<n1>
and
:CMDPARM.<n2>
may be variables, the values of which will be expanded before the substring
is computed.
:eDEFLIST.
:EXAMPLE.assign %a = foobar
The variable
:ITALICS.%a
gets the string
:ITALICS.foobar
assigned to it.

:EXAMPLE.assign %(Path) = /path/$
The global variable
:ITALICS.%(Path)
gets the data stored in the
:ITALICS.path
environment variable assigned to it.

:EXAMPLE.assign %b = strlen %a
Assigns the length of the contents of the local variable
:ITALICS.%a
to the local variable
:ITALICS.%b
:PERIOD.
:BLANKLINE.
Assuming the local variable
:ITALICS.%a
has the string
:ITALICS.abcdefg
assigned to it, then
:ITALICS.%b
gets
:ITALICS.7
assigned to it.

:EXAMPLE.assign %b = strchr %a b
Assigns the offset of the letter
:ITALICS.b
in the string contained in the local variable
:ITALICS.%a
to the local variable
:ITALICS.%b
:PERIOD.
:BLANKLINE.
Assuming the local variable
:ITALICS.%a
has the string
:ITALICS.abcdefg
assigned to it, then
:ITALICS.%b
gets
:ITALICS.2
assigned to it.

:EXAMPLE.assign %(Substr) = substr %a 2 4
Assigns the characters from offset 2 to offset 4
in the string contained in the local variable
:ITALICS.%a
to the global variable
:ITALICS.%(Substr)
:PERIOD.
:BLANKLINE.
Assuming the local variable
:ITALICS.%a
has the string
:ITALICS.abcdefg
assigned to it, then
:ITALICS.%b
gets
:ITALICS.bcd
assigned to it.

:EXAMPLE.assign %(res) = /abc %(str) def/
Assuming
:ITALICS.%(str)
has been assigned the value
:ITALICS.xyz
:CONT.,
then the string
:ITALICS.abc xyz def
is assigned to the local variable
:ITALICS.%(res)
:EXAMPLE.assign %(Result) = /100*30+(50-17)*10/x
The value
:ITALICS.3330
is assigned to the global variable
:ITALICS.%(Result)
:PERIOD.

:SEEALSO.
:SEE.expr
:eSEEALSO.
:eFUNC.
      
:FUNC.BREAK
:SYNTAX.* BREAK
Unconditionally exits the current looping block. This breaks out
of
:KEYWORD.loop
-
:KEYWORD.endloop
:CONT.,
:KEYWORD.loop
-
:KEYWORD.until
and
:KEYWORD.while
-
:KEYWORD.endwhile
blocks.
:SEEALSO.
:SEE.continue
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.CONTINUE
:SYNTAX.* CONTINUE
Restarts the current looping block. This causes a jump to the top of
:KEYWORD.loop
-
:KEYWORD.endloop
:CONT.,
:KEYWORD.loop
-
:KEYWORD.until
and
:KEYWORD.while
-
:KEYWORD.endwhile
blocks.
:SEEALSO.
:SEE.break
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.ENDIF
:SYNTAX.* ENDIF
Terminates an
:KEYWORD.if
-
:KEYWORD.elseif
-
:KEYWORD.else
block.
:SEEALSO.
:SEE.if
:SEE.elseif
:SEE.else
:eSEEALSO.
:eFUNC.

:FUNC.ENDLOOP
:SYNTAX.* ENDLOOP
Terminates a loop block.  Control goes to the top of the current loop.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.ENDWHILE
:SYNTAX.* ENDWHILE
Terminates a while block.  Control goes to the top of the current
while loop.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endloop
:SEE.loop
:SEE.quif
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.ELSEIF
:SYNTAX.* ELSEIF <expr>
An alternate case in an
:KEYWORD.if
block.  If the opening
:KEYWORD.if
script command
and none of the
:KEYWORD.elseif
script commands prior to this one were executed, then this
:KEYWORD.elseif
is executed.
:BLANKLINE.
Any variables contained in &parm1 are expanded before the expression
is evaluated.
:BLANKLINE.
If &parm1 is true, then the code following the
:KEYWORD.elseif
is executed.  If &parm1 is false, control goes to the next
:KEYWORD.elseif
:CONT.,
:KEYWORD.else
or
:KEYWORD.endif
command.
:SEEALSO.
:SEE.if
:SEE.else
:SEE.endif
:eSEEALSO.
:eFUNC.

:FUNC.ELSE
:SYNTAX.* ELSE
This is the alternate case in an
:KEYWORD.if
block.  If none of the preceding
:KEYWORD.if
or
:KEYWORD.elseif
statements are true, the code following the
:KEYWORD.else
command is executed.
:SEEALSO.
:SEE.if
:SEE.elseif
:SEE.endif
:eSEEALSO.
:eFUNC.

:FUNC.EXPR
:SYNTAX.* EXPR <v1> "=" <expr>
Assigns the expression &parm3 to the variable &parm1
:PERIOD.
:BLANKLINE.
Any variables contained in &parm3 are expanded before the expression
is evaluated.
:EXAMPLE.expr %(Num) = 100*30+50
Assigns the value
:ITALICS.3050
to the global variable
:ITALICS.%(Num)
:PERIOD.
:EXAMPLE.expr %a = %(SW)-10
Assuming a screen width of 80, then this assigns the value
:ITALICS.70
to the local variable
:ITALICS.%a
:PERIOD.

:SEEALSO.
:SEE.assign
:SEE.eval
:eSEEALSO.
:eFUNC.

:FUNC.FCLOSE
:SYNTAX.FCLOSE <n>
Closes file &parm1 previously opened with a
:KEYWORD.fopen
script command.
:EXAMPLE.fclose 1
Closes file 1.
:SEEALSO.
:SEE.fopen
:SEE.fread
:SEE.fwrite
:eSEEALSO.
:eFUNC.

:FUNC.FOPEN
:SYNTAX.* FOPEN <name> <n> <how>
This command opens file &parm1, assigning it file handle &parm2.
:PERIOD.
:BLANKLINE.
&parm2 may be a value from 1 to 9.  This number is used to identify
the file for future
:KEYWORD.fread
:CONT.,
:KEYWORD.fwrite
or
:KEYWORD.fclose
script commands.
:BLANKLINE.
&parm3 specifies the method that the file is opened. Methods are:
:DEFLIST.
:DEFITEM.a
Opens file for append.
:DEFITEM.r
Opens file for read.
:DEFITEM.w
Opens file for write.
:DEFITEM.x
Checks if the file exists.  This does not actually open the file,
so no
:KEYWORD.fclose
is required.
:eDEFLIST.
:RETURNS.
:RETVAL.ERR_NO_ERR.
The setting of
:KEYWORD.lastrc
if the open/existence check is a success.
:RETVAL.ERR_FILE_NOT_FOUND
The setting of
:KEYWORD.lastrc
if the open/existence check is a fails.
:eRETURNS.
:EXAMPLE.fopen test.dat 1 r
Opens file test.dat for read, and uses file handle 1.
:EXAMPLE.fopen test.dat 2 w
Opens file test.dat for write, and uses file handle 2.
:EXAMPLE.fopen test.dat 1 x
Tests if the file test.dat exists.
:EXAMPLE.fopen test.dat 9 a
Opens file test.dat for append, and uses file handle 9.
:SEEALSO.
:SEE.fclose
:SEE.fread
:SEE.fwrite
:eSEEALSO.
:eFUNC.

:FUNC.FREAD
:SYNTAX.* FREAD <n> <v1>
Reads a line from the file identified by handle &parm1.
:PERIOD.
The line is stored in the variable &parm2
:PERIOD.
:RETURNS.
:RETVAL.ERR_NO_ERR
The setting of
:KEYWORD.lastrc
if the read was successful.
:RETVAL.END_OF_FILE
The setting of
:KEYWORD.lastrc
if end of file was encountered.
:RETVAL.ERR_FILE_NOT_OPEN
The setting of
:KEYWORD.lastrc
if the file being read was not opened with
:KEYWORD.fopen
:PERIOD.
:eRETURNS.
:EXAMPLE.fread 1 %(line)
Reads the next line from file handle 1 into the variable
:ITALICS.%(line)
:PERIOD.
:SEEALSO.
:SEE.fclose
:SEE.fopen
:SEE.fwrite
:eSEEALSO.
:eFUNC.

:FUNC.FWRITE
:SYNTAX.* FWRITE <n> <v1>
Writes the contents of the variable &parm2 to the file identified
by handle &parm1.
:PERIOD.
:RETURNS.
:RETVAL.ERR_NO_ERR
The setting of
:KEYWORD.lastrc
if the write was successful.
:RETVAL.ERR_FILE_NOT_OPEN
The setting of
:KEYWORD.lastrc
if the file being written was not opened with
:KEYWORD.fopen
:PERIOD.
:eRETURNS.
:EXAMPLE.fwrite 3 %(line)
Writes the contents of the variable 
:ITALICS.%(line)
to file handle 3
:PERIOD.
:SEEALSO.
:SEE.fclose
:SEE.fopen
:SEE.fread
:eSEEALSO.
:eFUNC.

:FUNC.GET
:SYNTAX.* GET <v1>
Waits for the user to type a single keystroke, and then assigns
the keystroke into variable &parm1
:PERIOD.
:EXAMPLE.get %(ch)
Waits for a key to be pressed, and then assigns the key to the local
variable
:ITALICS.%(ch)
:PERIOD.
:SEEALSO.
:SEE.floatmenu
:SEE.input
:eSEEALSO.
:eFUNC.

:FUNC.GOTO
:SYNTAX.* GOTO <label>
Transfers control to point in script with label &parm1 defined.
:SEEALSO.
:SEE.label
:eSEEALSO.
:eFUNC.

:FUNC.IF
:SYNTAX.* IF <expr>
Starts an
:KEYWORD.if
block.
:BLANKLINE.
Any variables contained in &parm1 are expanded before the expression
is evaluated.
:BLANKLINE.
If &parm1 is true, then the code following the
:KEYWORD.if
is executed.  If &parm1 is false, control goes to the next
:KEYWORD.elseif
:CONT.,
:KEYWORD.else
or
:KEYWORD.endif
command.
:SEEALSO.
:SEE.elseif
:SEE.else
:SEE.endif
:eSEEALSO.
:eFUNC.

:FUNC.INPUT
:SYNTAX.* INPUT <v1>
Open a window (the
:KEYWORD.commandwindow
) and get a string from the user.  The string is assigned to the
variable &parm1.
:PERIOD.
:BLANKLINE.
If &parm1 was assigned a value before the
:KEYWORD.input
script command was executed, then that value is used as a prompt
string in the input window.
:RETURNS.
:RETVAL.ERR_NO_ERR.
The setting of
:KEYWORD.lastrc
if a string was entered.
:RETVAL.NO_VALUE_ENTERED
The setting of
:KEYWORD.lastrc
if the user pressed
:HILITE.ESC
to cancel the input string.
:eRETURNS.
:EXAMPLE.input %(str)
Get a string from the user, placing the result in the local variable
:ITALICS.%(str)
:PERIOD.
If
:ITALICS.%(str)
had no previous value, then the user would be prompted with:
:ILLUST.
Enter Value:
:eILLUST.
However, if
:ITALICS.%(str)
had the value
:ITALICS.Type in a filename:
:CONT.,
then the user would be prompted with:
:ILLUST.
Type in a filename:
:eILLUST.
:SEEALSO.
:SEE.floatmenu
:SEE.get
:eSEEALSO.
:eFUNC.

:FUNC.LABEL
:SYNTAX.* LABEL <name>
Defines the a label with the name &parm1 at the current line in the script.
:SEEALSO.
:SEE.goto
:eSEEALSO.
:eFUNC.

:FUNC.LOOP
:SYNTAX.* LOOP
Start a loop block.  This is the top of the block, after a
:KEYWORD.continue
:CONT.,
:KEYWORD.endloop
or
:KEYWORD.until
control returns to the instruction after the
:KEYWORD.loop
command.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.NEXTWORD
:SYNTAX.* NEXTWORD <srcvar> <resvar>
Remove the next space-delimited word from the variable &parm1
:PERIOD.
The word is placed in the variable specified by &parm1.
:PERIOD.
Both &parm1 and &parm2 must be variables only.
:EXAMPLE.nextword %a %b
If %a has
:MONO.'this is a test'
assigned to it, then after this command
is processed, %a will have
:MONO.'is a test'
assigned to it, and %b will have
:MONO.'this'
assigned to it.
:eFUNC.

:FUNC.QUIF
:SYNTAX.* QUIF <expr>
Conditionally quit current loop or while loop block.
:BLANKLINE.
Any variables contained in &parm1 are expanded before the expression
is evaluated.
:BLANKLINE.
If &parm1 is true, the current looping block is exited and execution
resumes at the line after the end of the current block.
:BLANKLINE.
If &parm1 is false, execution continues at the next line.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.until
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.RETURN
:SYNTAX.* RETURN <rc>
Exit the script, returning &parm1.
:PERIOD.
:BLANKLINE.
If the script was invoked by another script, then this value becomes
:KEYWORD.lastrc
:PERIOD.
:BLANKLINE.
If the script was invoked at the &cmdline, then this return code
is reported as the appropriate error, if &parm1 is not
:HILITE.ERR_NO_ERR
:PERIOD.
:BLANKLINE.
There are symbolic values for various error codes.
These values are described in the appendix
:HDREF refid=errcode.
:PERIOD.
:eFUNC.

:FUNC.UNTIL
:SYNTAX.* UNTIL <expr>
Closes a loop block.
:BLANKLINE.
Any variables contained in &parm1 are expanded before the expression
is evaluated.
:BLANKLINE.
If &parm1 is true, the first line after the loop block
is executed.
:BLANKLINE.
If &parm1 is false, then
control is returned to the top of the loop block, and the loop
executes again.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.while
:eSEEALSO.
:eFUNC.

:FUNC.WHILE
:SYNTAX.* WHILE <expr>
Start a loop block.  If &parm1 is true, the body of the loop
is executed.  If &parm1 is false, execution transfers to the
instruction after the
:KEYWORD.endwhile
command.
:BLANKLINE.
Any variables contained in &parm1 are expanded before the expression
is evaluated.
:BLANKLINE.
This is the top of the block, after a
:KEYWORD.continue
or
:KEYWORD.endwhile
control returns to the
:KEYWORD.while
command.
:SEEALSO.
:SEE.break
:SEE.continue
:SEE.endloop
:SEE.endwhile
:SEE.loop
:SEE.quif
:SEE.until
:eSEEALSO.
:eFUNC.

:eFNLIST.

:SECTION.Script Examples
The following section describes a number of the scripts
that are provided with &edname.. Each script
is discussed in detail.

:SESECT.err.vi
This is a simple script that edits a file that has the exact same
name as the current file, only has the extension .err.
:SEXMP.
:SELINE.edit %D%P%N.err
:eSEXMP.
:SEREF.
:SEREFER.1
The global variable
:ITALICS.%D
contains the drive of the current file.
The global variable
:ITALICS.%P
contains the full path to the current file.
The global variable
:ITALICS.%N
contains the name of the current file (extension removed).
These are combined with the .err extension to create a full path
to an error file.  This file is edited.
:eSEREF.
:eSESECT.

:SESECT.lnum.vi
This script prompts the user for a line number, and
if a line number is entered, goes to that line.
:SEXMP.
:SELINE.assign %a = /Enter Line Number:/
:SELINE.input %a
:SELINE.if lastrc != NO_VALUE_ENTERED
:SELINE.    %a
:SELINE.endif
:eSEXMP.
:SEREF.
:SEREFER.1 2
These lines assigns the string
:ITALICS.Enter Line Number:
to the local variable
:ITALICS.%a
:PERIOD.
This value will be used by the
:KEYWORD.input
command on line 2 to prompt the user.
:SEREFER.3 5
As long as the input was not cancelled by the user
(by pressing the ESC key),
the line the user typed is executed directly.
This assumes that the user will type a number.
:eSEREF.
:eSESECT.

:SESECT.qall.vi
This script
tries to quit each file being edited.  If the file has been modified,
the user is prompted if he wishes to save the file.  If he replies 'y',
the file is saved.  If he replies 'n', the file is discarded.  If
he presses the ESC key and cancels the input, the script is exited.
:SEXMP.
:SELINE.loop
:SELINE.
:SELINE.    quit
:SELINE.    if lastrc == ERR_FILE_MODIFIED
:SELINE.        assign %a = /Save "%F" (y\/n)?/
:SELINE.        input %a
:SELINE.        quif lastrc == NO_VALUE_ENTERED
:SELINE.        if "%a" == y
:SELINE.            write
:SELINE.            quit
:SELINE.        else
:SELINE.            quit!
:SELINE.        endif
:SELINE.    endif
:SELINE.    quif lastrc != ERR_NO_ERR
:SELINE.
:SELINE.endloop
:eSEXMP.
:SEREF.
:SEREFER.1
Starts the loop.
:SEREFER.3 4
Tries to to quit the file.  If the quit command fails, and the
return code is
:HILITE.ERR_FILE_MODIFIED
(the
:KEYWORD.quit
command will fail if the file being abandoned is modified),
then the code from lines 5-14 is executed.
:SEREFER.5 6
Assigns the string
:ITALICS.Save "<filename>" (y/n)?
to the local variable
:ITALICS.%a
:PERIOD.
This value will be used by the
:KEYWORD.input
command on line 6 to prompt the user.
:SEREFER.7
This exits the main loop if the user cancels the input prompt by
pressing the ESC key.
:SEREFER.8 13
If the user typed the letter y, then the edit buffer is saved and
exited, otherwise the contents of the edit buffer are discarded.
:SEREFER.15
This exits the main loop if any of the previous commands did
not return the "everything is OK" return code,
:HILITE.ERR_NO_ERR
:PERIOD.
:SEREFER.17
Ends the loop.  Control is returned to line 3.
:eSEREF.
:eSESECT.

:SESECT.wrme.vi
This example is the default write
:KEYWORD.hook script
:PERIOD.
This is called just before a edit buffer is saved and exited.
If the file has a null name, then the user is prompted for a name. If
he cancels the prompt, then the save is aborted. Otherwise, the
new name is set and the save continues.
:SEXMP.
:SELINE.if "%F" != ""
:SELINE.    return ERR_NO_ERR
:SELINE.endif
:SELINE.assign %a = /Enter file name:/
:SELINE.input %a
:SELINE.if lastrc == NO_VALUE_ENTERED
:SELINE.    return DO_NOT_CLEAR_MESSAGE_WINDOW
:SELINE.endif
:SELINE.echo off
:SELINE.set filename = %a
:SELINE.echo on
:SELINE.return ERR_NO_ERR
:eSEXMP.
:SEREF.
:SEREFER.1 3
Checks if the current file name is the empty string.  If it is not,
then there is a filename and the script returns
:HILITE.ERR_NO_ERR
to indicate that processing is to continue.
:SEREFER.4 8
The user is prompted with
:ITALICS.Enter a file name:
:PERIOD.
If he cancels the
:KEYWORD.input
command by pressing the ESC key, then the script returns
:HILITE.DO_NOT_CLEAR_MESSAGE_WINDOW
:CONT.,
which is not an error condition but causes the save process to abort
(remember, a hook point stops what it is doing if an
non-zero return code is returned from the
:KEYWORD.hook script
).
:SEREFER.9
Echo is disabled so that the setting of the filename will not
cause the normal message to appear in the message window.
:SEREFER.10
The filename is set to whatever the user typed in.
:SEREFER.11
Echo is enabled.
:SEREFER.12
The script returns
:HILITE.ERR_NO_ERR
to indicate that processing is to continue.
:eSEREF.
:eSESECT.


:SESECT.proc.vi
This example prompts the user for a procedure name.  If the user types
one, then a procedure skeleton is added:
:ILLUST.
/*
 * ProcName
 */
void ProcName(  )
{

} /* ProcName */
:eILLUST.
and the user is left in input mode on the space before the
closing bracket (')').
:SEXMP.
:SELINE.assign %a = /Procedure Name:/
:SELINE.input %a
:SELINE.if lastrc == NO_VALUE_ENTERED
:SELINE.    return
:SELINE.endif
:SELINE.atomic
:SELINE.echo off
:SELINE.assign %x = /autoindent/@
:SELINE.set noautoindent
:SELINE.execute \e0o/*\n * %a\n */\n\e0ivoid %a( @ )\n{\n\n} /* %a */\n\e
:SELINE.if %x == 1
:SELINE.    set autoindent
:SELINE.endif
:SELINE. -4
:SELINE.execute \e0f@x
:SELINE.echo on
:SELINE.echo 1 Procedure %a added
:SELINE.echo 2 " "
:SELINE.keyadd i
:eSEXMP.

:SEREF.
:SEREFER.1 5
The user is prompted with
:ITALICS.Procedure Name:
:PERIOD.
If he cancels the
:KEYWORD.input
command by pressing the ESC key, then the script exits.
:SEREFER.6
The script is an
:KEYWORD.atomic
one; so all modifications to the edit buffer can be undone with a
single
:KEYWORD.undo
command.
:SEREFER.7
Disables any output to the message window.
:SEREFER.8
This line gets the current state of the
:KEYWORD.autoindent
setting, and saves it the the local variable
:ITALICS.%x
:PERIOD.
:SEREFER.9
Turns off autoindent, so that the text to be inserted will line up
properly.
:SEREFER.10
This line simulates the typing of a number of keystrokes at the keyboard.
The effect of these keys is to generate the following:
:ILLUST.
/*
 * ProcName
 */
void ProcName( @ )
{

} /* ProcName */
:eILLUST.
:SEREFER.11 13
The local variable
:ITALICS.%x
is set to the previous value of
:KEYWORD.autoindent.
If
:KEYWORD.autoindent
was on before, then this turns it back on.
:SEREFER.14
This backs the cursor up to the line
:ILLUST.
void ProcName( @ )
:eILLUST.
:SEREFER.15
This line simulates the typing of a number of keystrokes at the keyboard.
The effect of these keystrokes is to move forward to the '@' character
and delete it.  This leaves the cursor in the position necessary to
enter procedure parameters.
:SEREFER.16
Enables output to the message window.
:SEREFER.17 18
Displays a message.
:SEREFER.19
Adds the key 'i' to the keyboard buffer.  Once the script exits, &edname
will process this key as if the user had typed it.  Thus, once the
script is done, the user is left inserting text.
:eSEREF.
:eSESECT.
