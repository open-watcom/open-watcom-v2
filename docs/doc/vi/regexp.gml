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
.* 04-aug-92    Craig Eisler    initial draft
.* 02-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=rxchap 'Regular Expressions'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction to Regular Expressions'
:cmt. .do end
.*
.np
Regular expressions are a powerful method of matching strings in your text.
Commands that use regular expressions are:
:UL.
:LI.forward slash ('/') command mode key (search forwards)
:LI.question mark ('?') command mode key (search backwards)
:LI.forward slash ('/') command line address (search forwards)
:LI.question mark ('?') command line address (search backwards)
:LI.substitute command line command
:LI.global command line command
:LI.egrep command line command
:LI.match command line command
:eUL.
.np
Different characters in a regular expression match different things.
A list of all special (or "magical") characters is:
:UL
:LI. A backslash ('\') followed by a single character other than new line
matches that character.
:LI.The caret ('&caret.') matches the beginning of a line.
:LI.The dollar sign ('$') matches the end of a line.
:LI.The dot ('.') matches any character.
:LI.A single character that does not have any other special meaning matches
that character.
:LI.A string enclosed in brackets [] matches any single character from
the string.  Ranges of ASCII character codes may be abbreviated as
in "a-z0-9".  A ']' may occur only as the first character of the
string.  A literal '-' must be placed where it cannot be mistaken as a
range indicator. If a caret ('&caret.') occurs as the first character inside
the brackets, then any characters NOT in the string are matched.
:LI.A regular expression followed by an asterisk ('*')
matches a sequence of 0 or more matches of the regular expression.
:LI.A regular expression followed by a plus sign ('+') matches one or
more matches of the regular expression.
:LI.A regular expression followed by a question mark ('?') matches zero
or one matches of the regular expression.
:LI.Two regular expressions concatenated match a match of the first
followed by a match of the second.
:LI. Two regular expressions separated by an or bar ('|')
match either a match for the first or a match for the second.
:LI.A regular expression enclosed in parentheses matches a match for
the regular expression.
:LI. The order of precedence of operators at the same parenthesis level is
the following:
.bd []
:cont.,
then
.bd *+?
:cont., then concatenation, then
.bd |
:period.
:LI.
All regular expressions following an at sign ('@') are to be treated
as case sensitive, regardless of the setting of
.keyref caseignore 1
:period.
:LI.
All regular expressions following a tilde ('&tilde.') are to be treated
as case insensitive, regardless of the setting of
.keyref caseignore 1
:period.
:LI.
If an exclamation point ('!') occurs as the first character in a
regular expression, it causes the ignoring of the
.keyref magic 1
setting; that is,
all magic characters are treated as magical.
An exclamation point ('!') is treated as a regular character if it occurs
anywhere but at the very start of the regular expression.
:eUL
.np
If a regular expression could match two different parts of the line,
it will match the one which begins earliest.
If both begin in the same place but match different lengths, or match
the same length in different ways, then the rules are more complicated.
.np
In general, the possibilities in a list of branches are considered in
left-to-right order, the possibilities for `*', `+', and `?' are
considered longest-first, nested constructs are considered from the
outermost in, and concatenated constructs are considered leftmost-first.
The match that will be chosen is the one that uses the earliest
possibility in the first choice that has to be made.
If there is more than one choice, the next will be made in the same manner
(earliest possibility) subject to the decision on the first choice.
And so forth.
.np
For example, `(ab|a)b*c' could match `abc' in one of two ways.
The first choice is between `ab' and `a'; since `ab' is earlier, and does
lead to a successful overall match, it is chosen.
Since the `b' is already spoken for,
the `b*' must match its last possibility :SF font=1.the empty string:eSF since
it must respect the earlier choice.
.np
In the particular case where no `|'s are present and there is only one
`*', `+', or `?', the net effect is that the longest possible
match will be chosen.
So `ab*', presented with `xabbbby', will match `abbbb'.
Note that if `ab*' is tried against `xabyabbbz', it
will match `ab' just after `x', due to the begins-earliest rule.
.*
.beglevel
.* ******************************************************************
.section 'Regular Expression BNF'
.* ******************************************************************
.np
A pseudo-BNF for regular expressions is:
.*
:DL.
.*
:DT.reg-exp
:DD.{branch}|{branch}|...

:DT.branch
:DD.{piece}{piece}...

:DT.piece
:DD.{atom{* or + or ?}}{atom{* or + or ?}}...
              * - match 0 or more of the atom
              + - match 1 or more of the atom
              ? - match a match of the atom, or the null string

:DT.atom
:DD.(reg-exp) or range or &tilde. or @ or &caret. or $ or \char or char

:DT.range
:DD.[ {&caret.} char and/or charlo-charhi ]
.np
'&caret.' causes negation of range.

:DT..
:DD.Match any character.

:DT.&caret.
:DD.Match start of line.

:DT.$
:DD.Match end of line.

:DT.@
:DD.What follows is to be searched with case sensitivity.

:DT.&tilde.
:DD.What follows is to be searched without case sensitivity.

:DT.!
:DD.If it occurs as the first character in a regular expression,
it causes the ignoring of the
.keyref magic 1
setting; that is,
all magic characters are treated as magical.
.param !
is treated
as a regular character if it occurs anywhere but at
the very start of the regular expression.

:DT.char
:DD.Any character.

:DT.\char
:DD.Forces \char to be accepted as char (no special meaning)
except \t matches a tab character if
.keyref realtabs 1
is set.
.*
:eDL.
.*
.endlevel
.* ******************************************************************
.section *refid=fmrx 'File Matching Regular Expressions'
.* ******************************************************************
.np
When specifying a file name in &edvi, it is possible to use a
file matching regular expression.  This expression is similar to
a regular expression, but has a couple of differences:
:OL.
:LI.A dot ('.') specifies an actual dot in the file name.
:LI.An asterisk ('*') is equivalent to '.*' (matches 0 or more characters).
:LI.A question mark ('?') is equivalent to a regular expression dot ('.');
i.e., a question mark matches exactly one character.
:LI.Caret ('&caret.') has no meaning.
:LI.Dollar sign ('$') has no meaning.
:LI.Backslash ('\') has no meaning (it is used as a directory separator).
:eOL.
.np
Imagine the list of files:
.millust begin
a.c
abc.c
abc
bcd.c
bad
xyz.c
.millust end
.np
The following examples show how what files from the above list would
be matched by various file name regular expressions:
.millust begin
a*.c     - all files that start with 'a' and end in '.c'.
           matches: "a.c" and "abc.c"
.np
(a|b)*.c - all files that start with an 'a' or a 'b' and end in '.c'
           matches: "a.c" "abc.c" and "bcd.c"
.np
*d.c     - all files that end in 'd.c'.
           matches: "bcd.c"
.np
*        - all files.
.np
*.*      - all files that have a dot in them.
           matches: a.c abc.c bcd.c xyz.c
.millust end
.* ******************************************************************
.section 'Replacement Strings'
.* ******************************************************************
.np
If you are dealing with regular expression search and replace, then there
are some special character sequences in the replacement string.
.*
:DL.
.*
:DT.&
:DD.Each instance of `&' in the replacement string is replaced by the 
entire string of matched characters.

:DT.\\
:DD.Used to enter a '\' in the replacement string.

:DT.\n
:DD.Replaced with a new line.

:DT.\t
:DD.Replaced with a tab (if
.keyref realtabs 1
is set).

:DT.\&lt.n&gt.
:DD.Each instance of
.param &lt.n&gt.
:cont.,
where
.param &lt.n&gt.
is a digit from 0 to 9, is replaced by the
n'th sub-expression in the regular expression.

:DT.\u
:DD.The next item in replacement string is changed to upper case.

:DT.\l
:DD.The next item in replacement string is changed to lower case.

:DT.\U
:DD.All items following
.param \U
in the replacement string are changed to
upper case, until a
.param \E
or
.param \e
is encountered.

:DT.\L
:DD.All items following
.param \L
in the replacement string are changed to lower case, until a
.param \E
or
.param \e
is encountered

:DT.\e
:DD.Terminate a
.param \U or
.param \L

:DT.\E
:DD.Terminate a
.param \U or
.param \L
:period.

:DT.\|&lt.n&gt.
:DD.Substitutes spaces up to column
.param &lt.n&gt.
:cont.,
so that the item that follows occurs at column
.param &lt.n&gt.
:period.

:DT.\#
:DD.Substitutes current line number that the match occurred on.
:eDL.
.* ******************************************************************
.section 'Controlling Magic Characters'
.* ******************************************************************
.np
By default, all special characters in a regular expression are "magical";
that is, if a special character is used it has a special meaning.  To
use a special character, like
.keyword (
:cont.,
it must be escaped:
.keyword \(
:period.
.np
However, it is possible to change this using the
.keyref magic 1
setting and the
.keyref magicstring
setting.
If
.keyref magic 1
is set, then all special characters are magical.
If
.keyref magic 1
is NOT set, then any special characters listed in
.keyref magicstring
lose their special meaning, and are treated as regular characters.
For example, the following &cmdline commands
.millust begin
set nomagic
set magicstring=()
.millust end
set up &edvi so that the brackets
.keyword ()
lose their special meaning.
To use the characters in their "magical" way, they must be escaped
with a
.keyword \
:period.
.np
Replacement strings special character sequences can be disabled
by turning off the
.keyref regsubmagic 1
setting.
.* ******************************************************************
.section 'Regular Expression Examples'
.* ******************************************************************
.np
The following sections contain examples of regular expression usage
for text matching and text replacement.
.beglevel
.* ******************************************************************
.section 'Matching Examples'
.* ******************************************************************
.np
This section gives examples of different types of regular expressions.
Each example
shows the regular expression, the initial string, and the result.
In the result, the part of the string that is matched is underlined.
.np
.rxxmp begin a+
.rxorig defabc
.rxres def a bc
.rxorig aaabca
.rxres aaa bca
.rxorig zzzaaayyy
.rxres zzz aaa yyy
.rxxmp end

.rxxmp begin ^a+
.rxorig defabc
.rxres defabc
.rxorig aaabca
.rxres aaa bca
.rxorig zzzaaayyy
.rxres zzzaaayyy
.rxxmp end

.rxxmp begin ab*
.rxorig xabc
.rxres x ab c
.rxorig abbbbbcabc
.rxres abbbbb cabc
.rxorig dddacab
.rxres ddd a cab
.rxxmp end

.rxxmp begin ab*$
.rxorig xabc
.rxres abc
.rxorig abbbbbcabc
.rxres abbbbbcabc
.rxorig dddacab
.rxres * dddac ab
.rxorig defabbbbb
.rxres * def abbbbb
.rxxmp end

.rxxmp begin ab?
.rxorig abc
.rxres ab c
.rxorig abbbbbcabc
.rxres ab bbbbcabc
.rxorig acab
.rxres a cab
.rxxmp end

.rxxmp begin [abc]
.rxorig abc
.rxres a bc
.rxorig defb
.rxres * def b
.rxorig defcghi
.rxres def c ghi
.rxxmp end

.rxxmp begin a|b
.rxorig abc
.rxres a bc
.rxorig bac
.rxres b ac
.rxorig defabc
.rxres def a bc
.rxxmp end

.rxxmp begin [a-z]+
.rxorig abcdef
.rxres * abcdef
.rxorig abc0def
.rxres abc 0def
.rxorig 0abcdef
.rxres * 0 abcdef
.rxxmp end

.rxxmp begin ([^ ])*
.rxorig abc def
.rxres abc ' def'
.rxxmp end

.rxxmp begin (abc)|(def)
.rxorig abcdef
.rxres abc def
.rxorig zzzdefabc
.rxres zzz def abc
.rxxmp end

.rxxmp begin ^(abc)|(def)
.rxorig abcdef
.rxres abc def
.rxorig zzzdefabc
.rxres zzz def abc
.rxorig zzzabcdef
.rxres * zzzabc def
.rxxmp end

.rxxmp begin ((abc)+|(def))ghi
.rxorig defabcghi
.rxres * def abcghi
.rxorig abcabcghi
.rxres * abcabcghi
.rxorig abcdefghi
.rxres * abc defghi
.rxorig abcdef
.rxres abcdef
.rxxmp end
.* ******************************************************************
.section 'Replacement Examples'
.* ******************************************************************
.np
Regular expressions and replacement expressions.  Each example
shows the regular expression and the replacement expression,
the initial string, the match, and the resulting string after the replacement.
.np
The regular expression and the replacement are separated by forward
slashes ('/'). For example, in the string
.millust begin
/([a-z]+)((a|b))/Test:\1\2/
.millust end
.bd ([a-z]+)((a|b))
is the regular expression and
.bd Test:\1\2
is the replacement expression.
.*
.endlevel
.*

