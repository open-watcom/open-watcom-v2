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
.* Description:  Root file of VI documentation.
.*
.* Date         By              Reason
.* ----         --              ------
.* 06-aug-92    Craig Eisler    initial draft
.* 02-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=scripts 'Editor Script Language'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.*
The &edname supports a powerful command language.  In a script,
you may use any &cmdline command, along with a number of special commands
explicitly for the script environment.
.np
White space is ignored in a script file, unless a line starts with a
right angle bracket ('&gt.'). Comments may be imbedded in a script file
by starting the line with pound sign ('#').
.np
A script is invoked using the &cmdline command
.keyref source
:period.
Examples are:

.millust begin
source test2.vi
.millust end

.millust begin
source test.vi parm1 parm2 parm3 parm4
.millust end

.np
A script may be invoked with a set of optional parameters.
These optional parameters are accessed in the script by using
.var %n
:period.
Every occurrence of
.var %n
in the script is replaced by the corresponding parameter.
To access parameter above 9, brackets must surround the number.  This
is because:

.millust begin
%10
.millust end

.np
cannot be distinguished from the variable
.var %1
followed by a 0, and
the variable
.var %10
:period.
To remove the ambiguity, brackets are used:

.millust begin
%(10)
.millust end

.np
All parameters can be accessed by using
.var %*
:period.
.np
To allow multiple words
in a single parameter, delimit the sequence by forward slashes ('/') or
double quotes ('"').  For example, the line

.millust begin
source test.vi "a b c" d e
.millust end

.np
would cause the script test.vi to have the following variables defined:

.millust begin
%* = a b c d e
%1 = a b c
%2 = d
%3 = e
.millust end

.np
General variables, both local and global, are also supported in the
editor script language. Any line in a script
that is not one of the script commands
has all the variables on it expanded before the line is processed.
Script commands can manipulate the variables. For more information, see
the section
:HDREF refid='scrvars'.
.if &e'&dohelp eq 0 .do begin
of this chapter.
.do end
.np
There are several useful &cmdline
commands dealing with &edvi scripts, they are:

.*
:DL break.
.*
:DT.compile
:DD.Used to compile a script.
This allows much faster execution of the script
by &edvi.
:period.

:DT.load
:DD.Used to make a script resident in &edvi.
:period.
This allows much faster invocation of the script,
since &edvi does not have to search for it or parse it.
.*
:eDL.
.*
.np
If a system command is spawned from a script (using the exclamation point ('!')
command), then &edvi does not
pause after the system command is finished running.  However, if
you set
.keyref pauseonspawnerr 1
:cont.,
then &edvi will pause after a system command is executed from a script
if the system command returned an error.
.*
.* ******************************************************************
.section *refid=scrvars 'Script Variables'
.* ******************************************************************
.*
.np
General variables are supported in a &edvi script.
Variables are preceded by a percent symbol ('%').
Variables with more than one letter must be enclosed by brackets, for
example:

.millust begin
%a     - references variable named a.
%(abc) - references variable named abc.
.millust end

.np
The brackets are required to disambiguate single letter variables followed
by text from multiple letter variables.
.np
Both local and global variables are supported.  They are distinguished
by the case of the first letter:  local variables must begin with
a lower case letter, and global variables begin with an upper case
variable.  Example variables:

.millust begin
%A     - global variable named A.
%a     - local variable named a.
%(AbC) - global variable named AbC.
%(abC) - local variable named abC.
.millust end

.np
Global variables are valid for the life of the editing
session.
.np
Local variables are only valid for the life of the script that they
are used in.
.*
.beglevel
.*
.* ******************************************************************
.section 'Pre-defined Global Variables'
.* ******************************************************************
.*
.np
There are a number of global variables that take on values as the
editor runs, they are:
.*
:DL break.
.*
:DT.%C
:DD.Contains the current column number in the current edit buffer.

:DT.%D
:DD.Drive of current file, based on the actual path.

:DT.%(D1)
:DD.Drive of current file, as typed by the user.  This could have no value.

:DT.%E
:DD.File name extension of current file.

:DT.%F
:DD.Current file name (including name and extension).

:DT.%H
:DD.Home directory of a file. This is the directory where the edit command
was issued.

:DT.%N
:DD.Name of the current file, extension removed.

:DT.%M
:DD.Modified status of the current file - set to 1 if the file
has been modified, and a 0 otherwise.

:DT.%(OS)
:DD.What operating system the editor is hosted on. Possible values are:
:UL compact.
:LI.dos (protect and real mode).
:LI.unix (QNX, Linux or other Unix-based systems)
:LI.os2
:LI.os2v2
:LI.nt
:eUL.

:DT.%(OS386)
:DD.This variable is set to 1 is the host operating system is 386
(or higher) based.  The possible 386 environments are:
:UL compact.
:LI.dos (protect mode).
:LI.os2v2
:LI.nt
:LI.unix (when running on a 386)
:eUL.

:DT.%P
:DD.Path of current file (no extension, name, or drive) based on the actual
full path to the file.

:DT.%(P1)
:DD.Path of current file (no extension, name, or drive) based on the name
typed by the user.  This could have no value.

:DT.%R
:DD.Contains the current row (line number) in the current edit buffer.

:DT.%(SH)
:DD.Height of entire screen in characters.

:DT.%(SW)
:DD.Width of entire screen in characters.

:DT.%(Sysrc)
:DD.Return code from last system command.
.*
:eDL.
.*
.endlevel
.*
.* ******************************************************************
.section 'Hook Scripts'
.* ******************************************************************
.*
.np
&edvi has several hook points where a script,
if defined by the user, is invoked.  This allows you to
intercept the editor at key points
to change its behaviour.  A script
that is invoked at a hook point
is referred to as a
.keyword hook script
:period.
.np
Each hook script
is identified by a particular global variable.  Whenever &edvi
reaches a hook point, it checks if the global variable is defined,
and if it is, the global variables contents are treated like a script
name, and that script is invoked.
.np
The hook points are:
:UL.
:LI.after a new file has been read.
:LI.before a modified file is saved and exited.
:LI.after return is pressed on the &cmdline
:period.
:LI.whenever an unmodified file is modified.
:LI.whenever a selected (highlighted) column range is
chosen (via mouse click or keyboard).
:LI.whenever a selected (highlighted) line range
is chosen (via mouse click or keyboard).
:eUL.
.*
:DL break.
.*
:DT.Read Hook
:DD.The hook script
is called just after a new file has been read into the editor.
.np
The script invoked is the file specified by the global variable
.var %(Rdhook)
:period.

:DT.Write Hook
:DD.The hook script
is called just before a modified file
is to be saved and exited.
.np
The script invoked is the file specified by the global variable
.var %(Wrhook).

:DT.Command Hook
:DD.The hook script
is called after the return is pressed from the &cmdline
:period.
The global variable
.var %(Com)
contains the current command string, and may be modified.
Whatever it is modified to is what will be processed by the &cmdline
processor.
.np
The script invoked is the file specified by the global variable
.var %(Cmdhook)
:period.

:DT.Modified Hook
:DD.The hook script
is called whenever
a command is about to modify an unmodified file.  If the file is
modified, the hook is not called.
.np
The script invoked is the file specified by the global variable
.var %(Modhook)
:period.

:DT.Mouse Columns Sel Hook
:DD.The hook script
is called whenever a selected column range
has been picked.  Picking a selected region is done by
right-clicking the region with the mouse, or by double clicking
the region with the mouse, or by using the underscore ('_') &cmdmode
keystroke.
.np
The script is invoked with the following parameters:
:DL break.
:DT.%1
:DD.The selected string.
:DT.%2
:DD.Line on screen of selected string.
:DT.%3
:DD.Column on screen of start of selected string.
:DT.%4
:DD.Column on screen of end of selected string.
:eDL.
The script invoked is the file is specified by the global variable
.var %(MCselhook)
:period.

:DT.Mouse Lines Sel Hook
:DD.The hook script
is called whenever a selected line range
has been picked.  Picking a selected region is done by
right-clicking the region with the mouse, or by double clicking
the region with the mouse, or by using the underscore ('_')
&cmdmode keystroke.
:DL break.
:DT.%1
:DD.Line on screen where selection request occurred.
:DT.%2
:DD.Column on screen where selection request occurred.
:DT.%3
:DD.First line of selected region.
:DT.%4
:DD.Last line of selected region.
:eDL.
The script invoked is the file specified by the global
variable
.var %(MLselhook)
:period.
.*
:eDL.
.*
.* ******************************************************************
.section *refid='screxpr' 'Script Expressions'
.* ******************************************************************
.*
.np
&edvi allows the use of constant expressions in its script
language. Long integers and strings may be used in an expression.
Some sample expressions are:
.millust begin
5*3+12
(7*7)+10*((3+5)*8+9)
5 &gt.= %(var)
("%(str)" == "foo") || ("%(str)" == "bar")
(5+%i*3 == 15)
rdonly == 1
.millust end
.np
An expression is composed of operators and tokens.  Operators act
on the tokens to give a final result.
.np
A token in an expression may be a special keyword, a boolean setting
value, an integer, or
a string.
.np
A string is indicated by surrounding the string with double quotes (").
A token is an integer if it starts with a numeric
digit (0 to 9).
.np
If a token starts with a dot ('.'), then the remainder of the token
is assumed to be a setting token.  This token evaluates to be
1 or 0 for a boolean setting, or to the actual value of the setting
for all others.
.millust begin
 .autoindent - 1 if autoindent is true, 0 otherwise
 .ai         - 1 if autoindent is true, 0 otherwise
 .autosave   - current value of autosave
 .tmpdir     - current tmpdir string
.millust end
.np
If a token is not surrounded by double quotes, and is not a keyword and
is not an integer, then that token is assumed to be a string.
.np
If an expression contains conditional operators, then the result of the
expression is a boolean value (1 or 0). The following script language
control flow commands expect boolean results:
:UL compact.
:LI.:cont.
.keyref if
:LI.:cont.
.keyref elseif
:LI.:cont.
.keyref quif
:LI.:cont.
.keyref while
:LI.:cont.
.keyref until
:eUL.
.np
The following are conditional operators in an expression:
:UL compact.
:LI.:cont.
.keyword ==
(equal to)
:LI.:cont.
.keyword !=
(not equal to)
:LI.:cont.
.keyword &gt.
(greater than)
:LI.:cont.
.keyword &gt.=
(greater than or equal to)
:LI.:cont.
.keyword &lt.
(less than)
:LI.:cont.
.keyword &lt.=
(less than or equal to)
:LI.:cont.
.keyword &&
(boolean AND)
:LI.:cont.
.keyword ||
(boolean OR)
:eUL.
.np
An expression may also operate on its token using various mathematical
operators, these operators are
:UL compact.
:LI.:cont.
.keyword +
(plus)
:LI.:cont.
.keyword -
(minus)
:LI.:cont.
.keyword *
(multiply)
:LI.:cont.
.keyword /
(divide)
:LI.:cont.
.keyword **
(exponentiation)
:LI.:cont.
.keyword &caret.
(bitwise NOT)
:LI.:cont.
.keyword |
(bitwise OR)
:LI.:cont.
.keyword &
(bitwise AND)
:LI.:cont.
.keyword &gt.&gt.
(bit shift down)
:LI.:cont.
.keyword &lt.&lt.
(bit shift up)
:eUL.
.np
Special keyword tokens are:
.*
:DL break.
.*
:DT.ERR_???
:DD.These are symbolic representations of all possible errors while
executing in &edvi..  These values are found in
:fname.error.dat:efname.
:period.
These values are described in the appendix
:HDREF refid=errcode.
:period.

:DT.lastrc
:DD.This keyword evaluates to the return code issued by the last command run
in the script. Possible values to compare against are found in
:fname.error.dat:efname.
:period.
These values may are described in the appendix
:HDREF refid=errcode.
:period.

:DT.rdonly
:DD.This keyword evaluates to 1 if the current file is read only, and
0 if it is not read only.

:DT.config
:DD.This keyword evalutes to a number representing the current mode
the screen is configured to. Possible values are:
:DL break.
:DT.100
:DD.Screen is in color mode.
:DT.10
:DD.Screen is in black and white mode.
:DT.1
:DD.Screen is in monochrome mode.
:eDL.
.np
This may be used to have different configurations built into
your
.keyword configuration script
:fname.ed.cfg:efname.
:period.

:DT.black
:DD.This keyword evalutes to the integer representing the color black (0).

:DT.blue
:DD.This keyword evalutes to the integer representing the color blue (1).

:DT.green
:DD.This keyword evalutes to the integer representing the color green (2).

:DT.cyan
:DD.This keyword evalutes to the integer representing the color cyan (3).

:DT.red
:DD.This keyword evalutes to the integer representing the color red (4).

:DT.magenta
:DD.This keyword evalutes to the integer representing the color magenta (5).

:DT.brown
:DD.This keyword evalutes to the integer representing the color brown (6).

:DT.white
:DD.This keyword evalutes to the integer representing the color white (7).

:DT.dark_gray
:DD.This keyword evalutes to the integer representing the color dark_gray (8).

:DT.light_blue
:DD.This keyword evalutes to the integer representing the color light_blue (9).

:DT.light_green
:DD.This keyword evalutes to the integer representing the color light_green (10).

:DT.light_cyan
:DD.This keyword evalutes to the integer representing the color light_cyan (11).

:DT.light_red
:DD.This keyword evalutes to the integer representing the color light_red (12).

:DT.light_magenta
:DD.This keyword evalutes to the integer representing the color light_magenta (13).

:DT.yellow
:DD.This keyword evalutes to the integer representing the color yellow (14).

:DT.bright_white
:DD.This keyword evalutes to the integer representing the color bright_white (15).

:eDL.
.*
.beglevel
.*
.* ******************************************************************
.section 'Script Expression BNF'
.* ******************************************************************
.*
.np
This section describes a BNF for the construction of constant expressions.
.*
:DL break.
.*
:DT.expression
:DD.: conditional-exp

:DT. conditional-exp
:DD.: log-or-exp
.br
| log-or-exp ? expression : conditional-exp

:DT.log-or-exp
:DD.: log-and-exp
.br
| log-or-exp || log-and-exp

:DT.log-and-exp
:DD.: bit-or-exp
.br
| log-and-exp && bit-or-exp

:DT.bit-or-exp
:DD.: bit-xor-exp
.br
| bit-or-exp | bit-xor-exp

:DT.bit-xor-exp
:DD.: bit-and-exp
.br
| bit-xor-exp &caret. bit-and-exp

:DT.bit-and-exp
:DD.: equality-exp
.br
| bit-and-exp & equality-exp

:DT.equality-exp
:DD.: relational-exp
.br
| equality-exp == relational-exp
| equality-exp != relational-exp

:DT.relational-exp
:DD.: shift-exp
.br
| relational-exp &gt. shift-exp
.br
| relational-exp &lt. shift-exp
.br
| relational-exp &gt.= shift-exp
.br
| relational-exp &lt.= shift-exp

:DT.shift-exp
:DD.: additive-exp
.br
| shift-exp &lt.&lt. additive-exp
.br
| shift-exp &gt.&gt. additive-exp

:DT.additive-exp
:DD.: multiplicative-exp
.br
| additive-exp + multiplicative-exp
.br
| additive-exp - multiplicative-exp

:DT.multiplicative-exp
:DD.: exponent-exp
.br
| multiplicative-exp * exponent-exp
.br
| multiplicative-exp / exponent-exp
.br
| multiplicative-exp % exponent-exp

:DT.exponent-exp
:DD.: unary-exp
.br
| exponent-exp ** unary-exp

:DT.unary-exp
:DD.: primary-exp
.br
| - unary-exp
.br
| &tilde. unary-exp
.br
| ! unary-exp

:DT.primary-exp
:DD.: token
.br
| ( expression )

:DT.token
:DD.: INTEGER
.br
| STRING
.br
| KEYWORD
.*
:eDL.
.*
.endlevel
.*
.* ******************************************************************
.section 'Control Flow Commands'
.* ******************************************************************
.*
.np
This section gives a brief overview of the control flow commands of the
&edvi script language.  For a full description of all
script commands, see the next section
:HDREF refid='scrcmds'.
:period.
.*
.beglevel
.*
.* ******************************************************************
.section *refid=loop 'The LOOP Block'
.* ******************************************************************
.*
.np
The
.keyword loop
block is similar to the
.keyword do-while
construct in C.  The flow of the loop may be modified using the
.keyref break
:cont.,
.keyref continue
:cont.
or
.keyref quif
script commands.
.np
The loop may be set to run until a termination condition is met by using the
.keyword loop
-
.keyref until
commands.
.np
The loop may be set to run without any termination condition by using the
.keyword loop
-
.keyref endloop
commands.
.np
An overview of a 
.keyword loop
block is:
.millust begin
loop
    break
    continue
    quif &lt.expr&gt.
until &lt.expr&gt.

loop
    break
    continue
    quif &lt.expr&gt.
endloop
.millust end
.*
.* ******************************************************************
.section *refid=while 'The WHILE Block'
.* ******************************************************************
.*
.np
The
.keyword while
block is similar to the
.keyword while
loop construct in C.  The flow of the while loop may be modified using the
.keyref break
:cont.,
.keyref continue
:cont.
or
.keyref quif
script commands.
.np
The while loop is set up using the
.keyword while
-
.keyref endwhile
commands.
.np
An overview of the
.keyword while
block is:
.millust begin
while &lt.expr&gt.
    break
    continue
    quif &lt.expr&gt.
endwhile
.millust end
.*
.* ******************************************************************
.section *refid=if 'The IF Block'
.* ******************************************************************
.*
.np
The
.keyword if
block is similar to the
.keyword if-else
construct in C.
.np
An overview of the
.keyword if
block is:
.millust begin
if &lt.expr&gt.
elseif &lt.expr&gt.
elseif &lt.expr&gt.
else
endif
.millust end
.*
.endlevel
.*
.* ******************************************************************
.section *refid=scrcmds 'Script Commands'
.* ******************************************************************
.*
.np
:INCLUDE file='cmdintro'.
For example, in its syntax model,
the ASSIGN command is specified as "ASSIGN", indicating that all the
letters are required (there are no abbreviations, only "assign" is
accepted as the command).
.np
The term
.param &lt.expr&gt.
is used to indicate an expression in the following commands.
Expressions are discussed in full detail in the section
:HDREF refid='screxpr'.
of this chapter.
.np
Script variables are used by some of the following commands.  Variables
are discussed in full detail in the section
:HDREF refid='scrvars'.
.if &e'&dohelp eq 0 .do begin
of this chapter.
.do end
.np
When a script command terminates, 
.var lastrc
is sometimes set to a value.  This value may be tested in an expression.
Script commands that set this have a
.keyword Returns
section.
.* ******************************************************************
.fnlist begin '&edvi. Script Commands'
.* ******************************************************************
.begfunc ATOMIC
.syntx * ATOMIC
.begdescr
This command causes all editing actions done by the script
to all be part of one undo record.  This way, the action of the entire
script can be eliminated with a single
.keyref 'undo (command)'
:cont.; i.e., it is an atomic action.
.enddescr
.endfunc

.begfunc ASSIGN
.syntx * ASSIGN &lt.v1&gt. "=" /&lt.val&gt./"r$@xl"
.begdescr
This command is used to assign the value
.param &lt.val&gt.
to the variable &parm1.
:period.
.np
The forward slashes ('/') around
.param &lt.val&gt.
are only need if there are spaces in
.param &lt.val&gt.
:cont.,
or if one of the special flags
.param r
:cont.,
.param x
:cont.,
.param l
:cont.,
.param $
or
.param @
is required at the end.
.np
The special flags have the following meaning:
.*
:DL break.
.*
:DT.r
:DD.When this flag is used,
.param &lt.val&gt.
may contain regular expression replacement
strings (using the last regular expression searched for). 
For more information on regular expressions, see the chapter
:HDREF refid='rxchap'.
:period.

:DT.l
:DD.When this flag is used,
.param &lt.val&gt.
is assumed to be an expression that indicates a line number. The expression
is evaluated, and the data on the corresponding line number is assigned
to &parm1
:period.

:DT.x
:DD.When this flag is used,
.param &lt.val&gt.
is assumed to be an expression, and is evaluated. The result is
assigned to &parm1.
:period.
For another way of assigning expression results to a variable, see the
.keyref expr
script command.

:DT.$ (dollar sign)
:DD.When this flag is used,
.param &lt.val&gt.
is assumed to be the name of an operating system environment variable,
and the contents of that environment variable is what is assigned to &parm1.
:period.

:DT.@
:DD.When this flag is used,
.param &lt.val&gt.
may be the name of one of the
.keyref set
command parameters. &parm1 will be given the current value of that
parameter.
.*
:eDL.
.*
.np
.param &lt.val&gt.
may be coded as a special operator.  If
.param &lt.val&gt.
is coded this way, the forward slashes ('/') must NOT be used.
The special operators are:
.*
:DL break.
.*
:DT.strlen &lt.v&gt.
:DD.Computes the length of the variable
.param &lt.v&gt.
:period.
This value is assigned to &parm1.
:period.

:DT.strchr &lt.v&gt. &lt.c&gt.
:DD.Computes the offset of the character
.param &lt.c&gt.
in the variable
.param &lt.v&gt.
:period.
The offset is assigned to &parm1.
:period.
Note that the character
.param &lt.c&gt.
may be a variable, the value of which will be expanded before offset
is computed.
:period.

:DT.substr &lt.v&gt. &lt.n1&gt. &lt.n2&gt.
:DD.Computes a substring of the string contained in the variable
.param &lt.v&gt.
:period.
The substring is composed of characters from offset
.param &lt.n1&gt.
to offset
.param &lt.n2&gt.
:period.
The substring is assigned to &parm1.
:period.
Note that the parameters 
.param &lt.n1&gt.
and
.param &lt.n2&gt.
may be variables, the values of which will be expanded before the substring
is computed.
.*
:eDL.
.enddescr
.xmplsect begin
.begxmpl assign %a = foobar
The variable
.var %a
gets the string
.param foobar
assigned to it.
.endxmpl

.begxmpl assign %(Path) = /path/$
The global variable
.var %(Path)
gets the data stored in the
.param path
environment variable assigned to it.
.endxmpl

.begxmpl assign %b = strlen %a
Assigns the length of the contents of the local variable
.var %a
to the local variable
.var %b
:period.
Assuming the local variable
.var %a
has the string
.param abcdefg
assigned to it, then
.var %b
gets
.param 7
assigned to it.
.endxmpl

.begxmpl assign %b = strchr %a b
Assigns the offset of the letter
.var b
in the string contained in the local variable
.var %a
to the local variable
.var %b
:period.
Assuming the local variable
.var %a
has the string
.param abcdefg
assigned to it, then
.var %b
gets
.param 2
assigned to it.
.endxmpl

.begxmpl assign %(Substr) = substr %a 2 4
Assigns the characters from offset 2 to offset 4
in the string contained in the local variable
.var %a
to the global variable
.var %(Substr)
:period.
Assuming the local variable
.var %a
has the string
.param abcdefg
assigned to it, then
.var %b
gets
.param bcd
assigned to it.
.endxmpl

.begxmpl assign %(res) = /abc %(str) def/
Assuming
.var %(str)
has been assigned the value
.param xyz
:cont.,
then the string
.param abc xyz def
is assigned to the local variable
.var %(res)
.endxmpl

.begxmpl assign %(Result) = /100*30+(50-17)*10/x
The value
.param 3330
is assigned to the global variable
.var %(Result)
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis expr
.alsosee end
.endfunc

.begfunc BREAK
.syntx * BREAK
.begdescr
Unconditionally exits the current looping block. This breaks out
of
.keyref loop
-
.keyref endloop
:cont.,
.keyref loop
-
.keyref until
and
.keyref while
-
.keyref endwhile
blocks.
.enddescr
.alsosee begin
.seethis continue
.seethis endloop
.seethis endwhile
.seethis loop
.seethis quif
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc CONTINUE
.syntx * CONTINUE
.begdescr
Restarts the current looping block. This causes a jump to the top of
.keyref loop
-
.keyref endloop
:cont.,
.keyref loop
-
.keyref until
and
.keyref while
-
.keyref endwhile
blocks.
.enddescr
.alsosee begin
.seethis break
.seethis endloop
.seethis endwhile
.seethis loop
.seethis quif
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc ENDIF
.syntx * ENDIF
.begdescr
Terminates an
.keyref if
-
.keyref elseif
-
.keyref else
block.
.enddescr
.alsosee begin
.seethis if
.seethis elseif
.seethis else
.alsosee end
.endfunc

.begfunc ENDLOOP
.syntx * ENDLOOP
.begdescr
Terminates a loop block.  Control goes to the top of the current loop.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endwhile
.seethis loop
.seethis quif
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc ENDWHILE
.syntx * ENDWHILE
.begdescr
Terminates a while block.  Control goes to the top of the current
while loop.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endloop
.seethis loop
.seethis quif
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc ELSEIF
.syntx * ELSEIF &lt.expr&gt.
.begdescr
An alternate case in an
.keyref if
block.  If the opening
.keyref if
script command
and none of the
.keyword elseif
script commands prior to this one were executed, then this
.keyword elseif
is executed.
.np
Any variables contained in &parm1 are expanded before the expression
is evaluated.
.np
If &parm1 is true, then the code following the
.keyword elseif
is executed.  If &parm1 is false, control goes to the next
.keyword elseif
:cont.,
.keyref else
or
.keyref endif
command.
.enddescr
.alsosee begin
.seethis if
.seethis else
.seethis endif
.alsosee end
.endfunc

.begfunc ELSE
.syntx * ELSE
.begdescr
This is the alternate case in an
.keyref if
block.  If none of the preceding
.keyref if
or
.keyref elseif
statements are true, the code following the
.keyword else
command is executed.
.enddescr
.alsosee begin
.seethis if
.seethis elseif
.seethis endif
.alsosee end
.endfunc

.begfunc EXPR
.syntx * EXPR &lt.v1&gt. "=" &lt.expr&gt.
.begdescr
Assigns the expression &parm3 to the variable &parm1
:period.
.np
Any variables contained in &parm3 are expanded before the expression
is evaluated.
.enddescr
.xmplsect begin
.begxmpl expr %(Num) = 100*30+50
Assigns the value
.param 3050
to the global variable
.var %(Num)
:period.
.endxmpl
.begxmpl expr %a = %(SW)-10
Assuming a screen width of 80, then this assigns the value
.param 70
to the local variable
.var %a
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis assign
.seethis eval
.alsosee end
.endfunc

.begfunc FCLOSE
.syntx FCLOSE &lt.n&gt.
.begdescr
Closes file &parm1 previously opened with a
.keyref fopen
script command.
.enddescr
.xmplsect begin
.begxmpl fclose 1
Closes file 1.
.endxmpl
.xmplsect end
.alsosee begin
.seethis fopen
.seethis fread
.seethis fwrite
.alsosee end
.endfunc

.begfunc FOPEN
.syntx * FOPEN &lt.name&gt. &lt.n&gt. &lt.how&gt.
.begdescr
This command opens file &parm1, assigning it file handle &parm2.
:period.
.np
&parm2 may be a value from 1 to 9.  This number is used to identify
the file for future
.keyref fread
:cont.,
.keyref fwrite
or
.keyref fclose
script commands.
.np
&parm3 specifies the method that the file is opened. Methods are:
.*
:DL break.
.*
:DT.a
:DD.Opens file for append.

:DT.r
:DD.Opens file for read.

:DT.w
:DD.Opens file for write.

:DT.x
:DD.Checks if the file exists.  This does not actually open the file,
so no
.keyref fclose
is required.
:eDL.
.enddescr
.returns begin
.retval ERR_NO_ERR.
The setting of
.keyword lastrc
if the open/existence check is a success.
.retval ERR_FILE_NOT_FOUND
The setting of
.keyword lastrc
if the open/existence check is a fails.
.returns end
.xmplsect begin
.begxmpl fopen test.dat 1 r
Opens file test.dat for read, and uses file handle 1.
.endxmpl
.begxmpl fopen test.dat 2 w
Opens file test.dat for write, and uses file handle 2.
.endxmpl
.begxmpl fopen test.dat 1 x
Tests if the file test.dat exists.
.endxmpl
.begxmpl fopen test.dat 9 a
Opens file test.dat for append, and uses file handle 9.
.endxmpl
.xmplsect end
.alsosee begin
.seethis fclose
.seethis fread
.seethis fwrite
.alsosee end
.endfunc

.begfunc FREAD
.syntx * FREAD &lt.n&gt. &lt.v1&gt.
.begdescr
Reads a line from the file identified by handle &parm1.
:period.
The line is stored in the variable &parm2
:period.
.enddescr
.returns begin
.retval ERR_NO_ERR
The setting of
.keyword lastrc
if the read was successful.
.retval END_OF_FILE
The setting of
.keyword lastrc
if end of file was encountered.
.retval ERR_FILE_NOT_OPEN
The setting of
.keyword lastrc
if the file being read was not opened with
.keyref fopen
:period.
.returns end
.xmplsect begin
.begxmpl fread 1 %(line)
Reads the next line from file handle 1 into the variable
.var %(line)
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis fclose
.seethis fopen
.seethis fwrite
.alsosee end
.endfunc

.begfunc FWRITE
.syntx * FWRITE &lt.n&gt. &lt.v1&gt.
.begdescr
Writes the contents of the variable &parm2 to the file identified
by handle &parm1.
:period.
.enddescr
.returns begin
.retval ERR_NO_ERR
The setting of
.keyword lastrc
if the write was successful.
.retval ERR_FILE_NOT_OPEN
The setting of
.keyword lastrc
if the file being written was not opened with
.keyref fopen
:period.
.returns end
.xmplsect begin
.begxmpl fwrite 3 %(line)
Writes the contents of the variable 
.var %(line)
to file handle 3
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis fclose
.seethis fopen
.seethis fread
.alsosee end
.endfunc

.begfunc GET
.syntx * GET &lt.v1&gt.
.begdescr
Waits for the user to type a single keystroke, and then assigns
the keystroke into variable &parm1
:period.
.enddescr
.xmplsect begin
.begxmpl get %(ch)
Waits for a key to be pressed, and then assigns the key to the local
variable
.var %(ch)
:period.
.endxmpl
.xmplsect end
.alsosee begin
.seethis floatmenu
.seethis input
.alsosee end
.endfunc

.begfunc GOTO
.syntx * GOTO &lt.label&gt.
.begdescr
Transfers control to point in script with label &parm1 defined.
.enddescr
.alsosee begin
.seethis label
.alsosee end
.endfunc

.begfunc IF
.syntx * IF &lt.expr&gt.
.begdescr
Starts an
.keyword if
block.
.np
Any variables contained in &parm1 are expanded before the expression
is evaluated.
.np
If &parm1 is true, then the code following the
.keyword if
is executed.  If &parm1 is false, control goes to the next
.keyref elseif
:cont.,
.keyref else
or
.keyref endif
command.
.enddescr
.alsosee begin
.seethis elseif
.seethis else
.seethis endif
.alsosee end
.endfunc

.begfunc INPUT
.syntx * INPUT &lt.v1&gt.
.begdescr
Open a window (the
.keyref commandwindow
:cont.) and get a string from the user.  The string is assigned to the
variable &parm1.
:period.
.np
If &parm1 was assigned a value before the
.keyword input
script command was executed, then that value is used as a prompt
string in the input window.
.enddescr
.returns begin
.retval ERR_NO_ERR.
The setting of
.keyword lastrc
if a string was entered.
.retval NO_VALUE_ENTERED
The setting of
.keyword lastrc
if the user pressed
.param ESC
to cancel the input string.
.returns end
.xmplsect begin
.begxmpl input %(str)
Get a string from the user, placing the result in the local variable
.var %(str)
:period.
If
.var %(str)
had no previous value, then the user would be prompted with:
.millust begin
Enter Value:
.millust end
However, if
.var %(str)
had the value
.param Type in a filename:
:cont.,
then the user would be prompted with:
.millust begin
Type in a filename:
.millust end
.endxmpl
.xmplsect end
.alsosee begin
.seethis floatmenu
.seethis get
.alsosee end
.endfunc

.begfunc LABEL
.syntx * LABEL &lt.name&gt.
.begdescr
Defines the a label with the name &parm1 at the current line in the script.
.enddescr
.alsosee begin
.seethis goto
.alsosee end
.endfunc

.begfunc LOOP
.syntx * LOOP
.begdescr
Start a loop block.  This is the top of the block, after a
.keyref continue
:cont.,
.keyref endloop
or
.keyref until
control returns to the instruction after the
.keyword loop
command.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endloop
.seethis endwhile
.seethis loop
.seethis quif
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc NEXTWORD
.syntx * NEXTWORD &lt.srcvar&gt. &lt.resvar&gt.
.begdescr
Remove the next space-delimited word from the variable &parm1
:period.
The word is placed in the variable specified by &parm1.
:period.
Both &parm1 and &parm2 must be variables only.
.enddescr
.xmplsect begin
.begxmpl nextword %a %b
If %a has
.mono 'this is a test'
assigned to it, then after this command
is processed, %a will have
.mono 'is a test'
assigned to it, and %b will have
.mono 'this'
assigned to it.
.endxmpl
.xmplsect end
.endfunc

.begfunc QUIF
.syntx * QUIF &lt.expr&gt.
.begdescr
Conditionally quit current loop or while loop block.
.np
Any variables contained in &parm1 are expanded before the expression
is evaluated.
.np
If &parm1 is true, the current looping block is exited and execution
resumes at the line after the end of the current block.
.np
If &parm1 is false, execution continues at the next line.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endloop
.seethis endwhile
.seethis loop
.seethis until
.seethis while
.alsosee end
.endfunc

.begfunc RETURN
.syntx * RETURN &lt.rc&gt.
.begdescr
Exit the script, returning &parm1.
:period.
.np
If the script was invoked by another script, then this value becomes
.keyword lastrc
:period.
.np
If the script was invoked at the &cmdline, then this return code
is reported as the appropriate error, if &parm1 is not
.param ERR_NO_ERR
:period.
.np
There are symbolic values for various error codes.
These values are described in the appendix
:HDREF refid=errcode.
:period.
.enddescr
.endfunc

.begfunc UNTIL
.syntx * UNTIL &lt.expr&gt.
.begdescr
Closes a loop block.
.np
Any variables contained in &parm1 are expanded before the expression
is evaluated.
.np
If &parm1 is true, the first line after the loop block
is executed.
.np
If &parm1 is false, then
control is returned to the top of the loop block, and the loop
executes again.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endloop
.seethis endwhile
.seethis loop
.seethis quif
.seethis while
.alsosee end
.endfunc

.begfunc WHILE
.syntx * WHILE &lt.expr&gt.
.begdescr
Start a loop block.  If &parm1 is true, the body of the loop
is executed.  If &parm1 is false, execution transfers to the
instruction after the
.keyref endwhile
command.
.np
Any variables contained in &parm1 are expanded before the expression
is evaluated.
.np
This is the top of the block, after a
.keyref continue
or
.keyref endwhile
control returns to the
.keyword while
command.
.enddescr
.alsosee begin
.seethis break
.seethis continue
.seethis endloop
.seethis endwhile
.seethis loop
.seethis quif
.seethis until
.alsosee end
.endfunc
.*
.fnlist end
.*
.* ******************************************************************
.section 'Script Examples'
.* ******************************************************************
.*
.np
The following section describes a number of the scripts
that are provided with &edvi.. Each script
is discussed in detail.

.sesect begin 'err.vi'
This is a simple script that edits a file that has the exact same
name as the current file, only has the extension .err.
.sexmp begin
.seline edit %D%P%N.err
.sexmp end
.seref begin
.serefer 1
The global variable
.var %D
contains the drive of the current file.
The global variable
.var %P
contains the full path to the current file.
The global variable
.var %N
contains the name of the current file (extension removed).
These are combined with the .err extension to create a full path
to an error file.  This file is edited.
.seref end
.sesect end

.sesect begin 'lnum.vi'
This script prompts the user for a line number, and
if a line number is entered, goes to that line.
.sexmp begin
.seline assign %a = /Enter Line Number:/
.seline input %a
.seline if lastrc != NO_VALUE_ENTERED
.seline     %a
.seline endif
.sexmp end
.seref begin
.serefer 1 2
These lines assigns the string
.param Enter Line Number:
to the local variable
.var %a
:period.
This value will be used by the
.keyref input
command on line 2 to prompt the user.
.serefer 3 5
As long as the input was not cancelled by the user
(by pressing the ESC key),
the line the user typed is executed directly.
This assumes that the user will type a number.
.seref end
.sesect end

.sesect begin 'qall.vi'
This script
tries to quit each file being edited.  If the file has been modified,
the user is prompted if he wishes to save the file.  If he replies 'y',
the file is saved.  If he replies 'n', the file is discarded.  If
he presses the ESC key and cancels the input, the script is exited.
.sexmp begin
.seline loop
.seline 
.seline     quit
.seline     if lastrc == ERR_FILE_MODIFIED
.seline         assign %a = /Save "%F" (y\/n)?/
.seline         input %a
.seline         quif lastrc == NO_VALUE_ENTERED
.seline         if "%a" == y
.seline             write
.seline             quit
.seline         else
.seline             quit!
.seline         endif
.seline     endif
.seline     quif lastrc != ERR_NO_ERR
.seline 
.seline endloop
.sexmp end
.seref begin
.serefer 1
Starts the loop.
.serefer 3 4
Tries to to quit the file.  If the quit command fails, and the
return code is
.param ERR_FILE_MODIFIED
(the
.keyref quit
command will fail if the file being abandoned is modified),
then the code from lines 5-14 is executed.
.serefer 5 6
Assigns the string
.param Save "&lt.filename&gt." (y/n)?
to the local variable
.var %a
:period.
This value will be used by the
.keyref input
command on line 6 to prompt the user.
.serefer 7
This exits the main loop if the user cancels the input prompt by
pressing the ESC key.
.serefer 8 13
If the user typed the letter y, then the edit buffer is saved and
exited, otherwise the contents of the edit buffer are discarded.
.serefer 15
This exits the main loop if any of the previous commands did
not return the "everything is OK" return code,
.param ERR_NO_ERR
:period.
.serefer 17
Ends the loop.  Control is returned to line 3.
.seref end
.sesect end

.sesect begin 'wrme.vi'
This example is the default write hook script
:period.
This is called just before a edit buffer is saved and exited.
If the file has a null name, then the user is prompted for a name. If
he cancels the prompt, then the save is aborted. Otherwise, the
new name is set and the save continues.
.sexmp begin
.seline if "%F" != ""
.seline     return ERR_NO_ERR
.seline endif
.seline assign %a = /Enter file name:/
.seline input %a
.seline if lastrc == NO_VALUE_ENTERED
.seline     return DO_NOT_CLEAR_MESSAGE_WINDOW
.seline endif
.seline echo off
.seline set filename = %a
.seline echo on
.seline return ERR_NO_ERR
.sexmp end
.seref begin
.serefer 1 3
Checks if the current file name is the empty string.  If it is not,
then there is a filename and the script returns
.param ERR_NO_ERR
to indicate that processing is to continue.
.serefer 4 8
The user is prompted with
.param Enter a file name:
:period.
If he cancels the
.keyref input
command by pressing the ESC key, then the script returns
.param DO_NOT_CLEAR_MESSAGE_WINDOW
:cont.,
which is not an error condition but causes the save process to abort
(remember, a hook point stops what it is doing if an
non-zero return code is returned from the hook script).
.serefer 9
Echo is disabled so that the setting of the filename will not
cause the normal message to appear in the message window.
.serefer 10
The filename is set to whatever the user typed in.
.serefer 11
Echo is enabled.
.serefer 12
The script returns
.param ERR_NO_ERR
to indicate that processing is to continue.
.seref end
.sesect end

.sesect begin 'proc.vi'
This example prompts the user for a procedure name.  If the user types
one, then a procedure skeleton is added:
.millust begin
/*
 * ProcName
 */
void ProcName(  )
{

} /* ProcName */
.millust end
and the user is left in input mode on the space before the
closing bracket (')').
.sexmp begin
.seline assign %a = /Procedure Name:/
.seline input %a
.seline if lastrc == NO_VALUE_ENTERED
.seline     return
.seline endif
.seline atomic
.seline echo off
.seline assign %x = /autoindent/@
.seline set noautoindent
.seline execute \e0o/*\n * %a\n */\n\e0ivoid %a( @ )\n{\n\n} /* %a */\n\e
.seline if %x == 1
.seline     set autoindent
.seline endif
.seline  -4
.seline execute \e0f@x
.seline echo on
.seline echo 1 Procedure %a added
.seline echo 2 " "
.seline keyadd i
.sexmp end

.seref begin
.serefer 1 5
The user is prompted with
.param Procedure Name:
:period.
If he cancels the
.keyref input
command by pressing the ESC key, then the script exits.
.serefer 6
The script is an
.keyref atomic
one; so all modifications to the edit buffer can be undone with a
single
.keyref 'undo (command)'
:period.
.serefer 7
Disables any output to the message window.
.serefer 8
This line gets the current state of the
.keyref autoindent 1
setting, and saves it the the local variable
.var %x
:period.
.serefer 9
Turns off autoindent, so that the text to be inserted will line up
properly.
.serefer 10
This line simulates the typing of a number of keystrokes at the keyboard.
The effect of these keys is to generate the following:
.millust begin
/*
 * ProcName
 */
void ProcName( @ )
{

} /* ProcName */
.millust end
.serefer 11 13
The local variable
.var %x
is set to the previous value of
.keyref autoindent 1
:period.
If
.keyref autoindent 1
was on before, then this turns it back on.
.serefer 14
This backs the cursor up to the line
.millust begin
void ProcName( @ )
.millust end
.serefer 15
This line simulates the typing of a number of keystrokes at the keyboard.
The effect of these keystrokes is to move forward to the '@' character
and delete it.  This leaves the cursor in the position necessary to
enter procedure parameters.
.serefer 16
Enables output to the message window.
.serefer 17 18
Displays a message.
.serefer 19
Adds the key 'i' to the keyboard buffer.  Once the script exits, &edvi
will process this key as if the user had typed it.  Thus, once the
script is done, the user is left inserting text.
.seref end
.sesect end

