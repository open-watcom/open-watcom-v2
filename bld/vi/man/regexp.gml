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
.* 04-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id=rxchap.Regular Expressions
:CMT :SECTION.Introduction
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
Different characters in a regular expression match different things.
A list of all special (or "magical") characters is:
:UL
:LI. A backslash ('\') followed by a single character other than new line
matches that character.
:LI.The caret ('^') matches the beginning of a line.
:LI.The dollar sign ('$') matches the end of a line.
:LI.The dot ('.') matches any character.
:LI.A single character that does not have any other special meaning matches
that character.
:LI.A string enclosed in brackets [] matches any single character from
the string.  Ranges of ASCII character codes may be abbreviated as
in "a-z0-9".  A ']' may occur only as the first character of the
string.  A literal '-' must be placed where it cannot be mistaken as a
range indicator. If a caret ('^') occurs as the first character inside
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
:HILITE.[]
:CONT.,
then
:HILITE.*+?
:CONT., then concatenation, then
:HILITE.|
:PERIOD.
:LI.
All regular expressions following an at sign ('@') are to be treated
as case sensitive, regardless of the setting of
:KEYWORD.caseignore
:PERIOD.
:LI.
All regular expressions following a tilde ('~~') are to be treated
as case insensitive, regardless of the setting of
:KEYWORD.caseignore
:PERIOD.
:LI.
If an exclamation point ('!') occurs as the first character in a
regular expression, it causes the ignoring of the
:KEYWORD.magic
setting; that is,
all magic characters are treated as magical.
An exclamation point ('!') is treated as a regular character if it occurs
anywhere but at the very start of the regular expression.
:eUL
:P.
If a regular expression could match two different parts of the line,
it will match the one which begins earliest.
If both begin in the same place but match different lengths, or match
the same length in different ways, then the rules are more complicated.
:P.
In general, the possibilities in a list of branches are considered in
left-to-right order, the possibilities for `*', `+', and `?' are
considered longest-first, nested constructs are considered from the
outermost in, and concatenated constructs are considered leftmost-first.
The match that will be chosen is the one that uses the earliest
possibility in the first choice that has to be made.
If there is more than one choice, the next will be made in the same manner
(earliest possibility) subject to the decision on the first choice.
And so forth.
:P.
For example, `(ab|a)b*c' could match `abc' in one of two ways.
The first choice is between `ab' and `a'; since `ab' is earlier, and does
lead to a successful overall match, it is chosen.
Since the `b' is already spoken for,
the `b*' must match its last possibility _the empty string_ since
it must respect the earlier choice.
:P.
In the particular case where no `|'s are present and there is only one
`*', `+', or `?', the net effect is that the longest possible
match will be chosen.
So `ab*', presented with `xabbbby', will match `abbbb'.
Note that if `ab*' is tried against `xabyabbbz', it
will match `ab' just after `x', due to the begins-earliest rule.

:SUBSECT.Regular Expression BNF
A pseudo-BNF for regular expressions is:
:DEFLIST.
:DEFITEM.reg-exp
{branch}|{branch}|...
:DEFITEM.branch
{piece}{piece}...
:DEFITEM.piece
{atom{* or + or ?}}{atom{* or + or ?}}...
	      * - match 0 or more of the atom
	      + - match 1 or more of the atom
	      ? - match a match of the atom, or the null string
:DEFITEM.atom
(reg-exp) or range or ~ or @ or ^ or $ or \char or char
:DEFITEM.range
[ {^} char and/or charlo-charhi ]
:ADDLINE.
'^' causes negation of range.
:DEFITEM..
Match any character.
:DEFITEM.^
Match start of line.
:DEFITEM.$
Match end of line.
:DEFITEM.@
What follows is to be searched with case sensitivity.
:DEFITEM.~
What follows is to be searched without case sensitivity.
:DEFITEM.!
If it occurs as the first character in a regular expression,
it causes the ignoring of the
:KEYWORD.magic
setting; that is,
all magic characters are treated as magical.
:HILITE.!
is treated
as a regular character if it occurs anywhere but at
the very start of the regular expression.
:DEFITEM.char
Any character.
:DEFITEM.\char
Forces \char to be accepted as char (no special meaning)
except \t matches a tab character if
:KEYWORD.realtabs
is set.
:eDEFLIST.
:eSUBSECT.

:SECTION id='fmrx'.File Matching Regular Expressions
When specifying a file name in &edname, it is possible to use a
file matching regular expression.  This expression is similar to
a regular expression, but has a couple of differences:
:OL.
:LI.A dot ('.') specifies an actual dot in the file name.
:LI.An asterisk ('*') is equivalent to '.*' (matches 0 or more characters).
:LI.A question mark ('?') is equivalent to a regular expression dot ('.');
i.e., a question mark matches exactly one character.
:LI.Caret ('^') has no meaning.
:LI.Dollar sign ('$') has no meaning.
:LI.Backslash ('\') has no meaning (it is used as a directory separator).
:eOL.
Imagine the list of files:
:ILLUST.
a.c
abc.c
abc
bcd.c
bad
xyz.c
:eILLUST.
The following examples show how what files from the above list would
be matched by various file name regular expressions:
:ILLUST.
a*.c     - all files that start with 'a' and end in '.c'.
           matches: "a.c" and "abc.c"

:bILLUST.
(a|b)*.c - all files that start with an 'a' or a 'b' and end in '.c'
           matches: "a.c" "abc.c" and "bcd.c"
:bILLUST.

*d.c     - all files that end in 'd.c'.
	   matches: "bcd.c"
:bILLUST.

*        - all files.
:bILLUST.

*.*      - all files that have a dot in them.
           matches: a.c abc.c bcd.c xyz.c
:eILLUST.

:SECTION.Replacement Strings
If you are dealing with regular expression search and replace, then there
are some special character sequences in the replacement string.
:DEFLIST.
:DEFITEM.&
Each instance of `&' in the replacement string is replaced by the 
entire string of matched characters.

:DEFITEM.\\
Used to enter a '\' in the replacement string.

:DEFITEM.\n
Replaced with a new line.

:DEFITEM.\t
Replaced with a tab (if
:KEYWORD.realtabs
is set).

:DEFITEM.\<n>
Each instance of
:ITALICS.<n>
:CONT.,
where
:ITALICS.<n
> is a digit from 0 to 9, is replaced by the
n'th sub-expression in the regular expression.

:DEFITEM.\u
The next item in replacement string is changed to upper case.

:DEFITEM.\l
The next item in replacement string is changed to lower case.

:DEFITEM.\U
All items following
:HILITE.\U
in the replacement string are changed to
upper case, until a
:HILITE.\E
or
:HILITE.\e
is encountered.

:DEFITEM.\L
All items following
:HILITE.\L
in the replacement string are changed to lower case, until a
:HILITE.\E
or
:HILITE.\e
is encountered

:DEFITEM.\e
Terminate a
:HILITE.\U or
:HILITE.\L

:DEFITEM.\E
Terminate a
:HILITE.\U or
:HILITE.\L
:PERIOD.

:DEFITEM.\|<n>
Substitutes spaces up to column
:ITALICS.<n>
:CONT.,
so that the item that follows occurs at column
:ITALICS.<n>
:PERIOD.

:DEFITEM.\#
Substitutes current line number that the match occurred on.
:eDEFLIST.

:SECTION.Controlling Magic Characters
By default, all special characters in a regular expression are "magical";
that is, if a special character is used it has a special meaning.  To
use a special character, like
:HILITE.(
:CONT.,
it must be escaped:
:HILITE.\(
:PERIOD.
:P.
However, it is possible to change this using the
:KEYWORD.magic
setting and the
:KEYWORD.magicstr
setting.
If
:KEYWORD.magic
is set, then all special characters are magical.
If
:KEYWORD.magic
is NOT set, then any special characters listed in
:KEYWORD.magicstr
lose their special meaning, and are treated as regular characters.
For example, the following &cmdline commands
:ILLUST.
set nomagic
set magicstr=()
:eILLUST.
set up &edname so that the brackets
:HILITE.()
lose their special meaning.
To use the characters in their "magical" way, they must be escaped
with a
:HILITE.\
:PERIOD.
:P.
Replacement strings special character sequences can be disabled
by turning off the
:KEYWORD.regsubmagic
setting.

:SECTION.Regular Expression Examples
:SUBSECT.Matching Examples
This section gives examples of different types of regular expressions.
Each example
shows the regular expression, the initial string, and the result.
In the result, the part of the string that is matched is underlined.
:P.
:RXXMP.a+
:RXORIG.defabc
:RXRES.def a bc
:RXORIG.aaabca
:RXRES.aaa bca
:RXORIG.zzzaaayyy
:RXRES.zzz aaa yyy
:eRXXMP.

:RXXMP.^a+
:RXORIG.defabc
:RXRES.defabc
:RXORIG.aaabca
:RXRES.aaa bca
:RXORIG.zzzaaayyy
:RXRES.zzzaaayyy
:eRXXMP.

:RXXMP.ab*
:RXORIG.xabc
:RXRES.x ab c
:RXORIG.abbbbbcabc
:RXRES.abbbbb cabc
:RXORIG.dddacab
:RXRES.ddd a cab
:eRXXMP.

:RXXMP.ab*$
:RXORIG.xabc
:RXRES.abc
:RXORIG.abbbbbcabc
:RXRES.abbbbbcabc
:RXORIG.dddacab
:RXRES.* dddac ab
:RXORIG.defabbbbb
:RXRES.* def abbbbb
:eRXXMP.

:RXXMP.ab?
:RXORIG.abc
:RXRES.ab c
:RXORIG.abbbbbcabc
:RXRES.ab bbbbcabc
:RXORIG.acab
:RXRES.a cab
:eRXXMP.

:RXXMP.[abc]
:RXORIG.abc
:RXRES.a bc
:RXORIG.defb
:RXRES.* def b
:RXORIG.defcghi
:RXRES.def c ghi
:eRXXMP.

:RXXMP.a|b
:RXORIG.abc
:RXRES.a bc
:RXORIG.bac
:RXRES.b ac
:RXORIG.defabc
:RXRES.def a bc
:eRXXMP.

:RXXMP.[a-z]+
:RXORIG.abcdef
:RXRES.* abcdef
:RXORIG.abc0def
:RXRES.abc 0def
:RXORIG.0abcdef
:RXRES.* 0 abcdef
:eRXXMP.

:RXXMP.([^ ])*
:RXORIG.abc def
:RXRES.abc ' def'
:eRXXMP.

:RXXMP.(abc)|(def)
:RXORIG.abcdef
:RXRES.abc def
:RXORIG.zzzdefabc
:RXRES.zzz def abc
:eRXXMP.

:RXXMP.^(abc)|(def)
:RXORIG.abcdef
:RXRES.abc def
:RXORIG.zzzdefabc
:RXRES.zzz def abc
:RXORIG.zzzabcdef
:RXRES.* zzzabc def
:eRXXMP.

:RXXMP.((abc)+|(def))ghi
:RXORIG.defabcghi
:RXRES.* def abcghi
:RXORIG.abcabcghi
:RXRES.* abcabcghi
:RXORIG.abcdefghi
:RXRES.* abc defghi
:RXORIG.abcdef
:RXRES.abcdef
:eRXXMP.
:eSUBSECT.

:SUBSECT.Replacement Examples
Regular expressions and replacement expressions.  Each example
shows the regular expression and the replacement expression,
the initial string, the match, and the resulting string after the replacement.
:P.
The regular expression and the replacement are separated by forward
slashes ('/'). For example, in the string
:ILLUST.
:ITALICS./([a-z]+)((a|b))/Test:\1\2/
:eILLUST.
:ITALICS.([a-z]+)((a|b))
is the regular expression and
:ITALICS.Test:\1\2
is the replacement expression.
:P.
:eSUBSECT.
