.chap *refid=regexp Regular Expressions
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'regular expressions'
.ix 'Editor' 'regular expressions'
.ix 'expressions' 'regular'
Regular expressions are a powerful method of matching strings in your text.
Commands that use regular expressions are:
.begbull
.bull
forward slash (/) command mode key (search forwards)
.bull
question mark (?) command mode key (search backwards)
.bull
forward slash (/) command line address (search forwards)
.bull
question mark (?) command line address (search backwards)
.bull
substitute command line command
.bull
global command line command
.bull
egrep command line command
.bull
match command line command
.endbull
.np
Different characters in a regular expression match different things. A list
of all special (or "magical") characters is:
.ix 'magic characters' 'in regular expressions'
.ix 'regular expressions' 'magic characters'
.ix 'Editor' 'magic characters in regular expressions'
.begbull
.bull
A backslash (\) followed by a single character other than new line matches
that character
.bull
The caret (^) matches the beginning of a line
.bull
The dollar sign ($) matches the end of a line
.bull
The dot (.) matches any character
.bull
A single character that does not have any other special meaning matches
that character
.bull
A string enclosed in brackets [] matches any single character from the
string. Ranges of ASCII character codes may be abbreviated as in a
"a-z0-9". A ] may occur only as the first character of the string.
You must place a literal - where it cannot be mistaken as a range
indicator. If a caret (^) occurs as the first character inside the
brackets, then any characters NOT in the string are matched
.bull
A regular expression followed by an asterisk (*) matches a sequence
of 0 or more matches of the regular expression
.bull
A regular expression followed by a plus sign (+) matches one or more
matches of the regular expression
.bull
A regular expression followed by a question mark (?) matches zero or
one matches of the regular expression
.bull
Two regular expressions concatenated match a match of the first
followed by a match of the second
.bull
Two regular expressions separated by an or bar (|) match either a match
for the first or a match for the second
.bull
A regular expression enclosed in parentheses matches a match for the
regular expression
.bull
The order of precedence of operators at the same parenthesis
level is the following: {}, then *+?, then concatenation, then /.
.bull
All regular expressions following an at sign (@) are treated as
case sensitive
.bull
All regular expressions following a tilde (~~) are to be treated as
case insensitive
.bull
If an exclamation point (!) occurs as the first character in a
regular expression, it causes the ignoring of the
.kw magic
setting; that is, all magic characters are treated as magical. An
exclamation point (!) is treated as a regular character if it occurs
anywhere but at the very start of the regular expression.
.endbull
.np
If a regular expression could match two different parts of the line,
it will match the earliest one. If both begin in the same place, but
match different lengths, or match the same length in different
ways, then the rules are more complicated.
.np
In general, the possibilities in a list of branches are considered
from left to right, the possibilities for *, +, and ? are considered
longest first, nested constructs are considered from the outermost in,
and concatenated constructs are considered leftmost first. The match
that is chosen is the one that uses the earliest possibility in the
first choice that has to be made. If there is more than one choice, the
next will be made in the same manner (earliest possibility) subject to the
decision on the first choice. An so forth.
.np
For example, (ab|a)b*c could batch the string
.us abc
in one of two ways. The first choice is between
.us ab
and
.us a.
Since ab is earlier in the expression and does lead to a successful overall
match, it is chosen. Since the
.us b
is already spoken for, the
.us b*
must match its last possibility since it must respect the earlier choice.
.np
If there are no |'s present and only one *m, +, or ?, the net effect is
that the longest possible match will be chosen. So
.us ab
presented with xabbbby, will match
.us abbbbb.
Note that is
.us ab*
is tried against
.us xabyabbbz,
it will match
.us ab
just after
.us x,
due to the begins earliest rule.
.*
.section Regular Expression BNF
.*
.ix 'BNF' 'regular expressions'
.ix 'regular expressions' 'BNF'
.np
A pseudo-BNF for regular expressions is:
.begnote
.note reg-exp
{branch}|{branch}|...
.note branch
{piece}{piece}...
.note piece
{atom{* or + or ?}}{atom {* or + or ?}}... *&mdash.match 0 or
more of the atom; +&mdash.match 1 or more of the atom;
?&mdash.match a match of the atom, or the
null string.
.note atom
(reg-exp) or range or @ or ^ or $ or \char or char.
.note range
[{^} char and/or charlo-charhi]. ^ causes negation of range.
.note .
Match any character.
.note ^
Match start of line.
.note $
Match end of line.
.note @
Search with case sensitivity.
.note ~~
Search without case sensitivity.
.note !
If it occurs as the first character in a regular expression, the magic
setting is ignored; that is, all magic characters are treated as magical.
! is treated as a regular character if it occurs anywhere but at the very
start of the regular expression.
.note char
Any character.
.note \char
Forces \char to be accepted as char (no special meaning) except \t
matches a tab character if
.kw realtabs
is set.
.endnote
.*
.section File Matching Regular Expressions
.*
.np
.ix 'regular expressions' 'file matching'
.ix 'file matching' 'regular expressions'
When specifying a file name in the Editor, it is possible to use
a file matching regular expression. This expression is similar to a
regular expression, but has a couple of differences:
.autopoint
.point
A dot (.) specifies an actual dot in the file name.
.point
An asterisk (*) is the same as .* (matches 0 or more characters).
.point
A question mark (?) is the same as a regular expression dot (.);
that is, a question mark matches exactly one character.
.point
A caret (^) has no meaning.
.point
A dollar sign ($) has no meaning.
.point
The backslash (\) has no meaning. It is used to separate directories.
.endpoint
.np
Suppose we have the following list of files:
.millust begin
a.c
abc.c
abc
bcd.c
bad
xyz.c
.millust end
.np
The following examples show how the files from the above list are
matched by various file name regular expressions.
.begnote $break
.note a*.c
All files that start with
.monoon
a
.monooff
and end in
.monoon
 .c.
.monooff
Therefore, it matches
.monoon
a.c
.monooff
and
.monoon
abc.c
.monooff
.note (a|b)*.c
All files that start with an a or a b and end in .c. Therefore, it matches
.monoon
a.c, abc.c,
.monooff
and
.monoon
bcd.c
.monooff
.note *d.c
All files that end in
.monoon
d.c.
.monooff
Therefore, it matches
.monoon
bcd.c
.monooff
.note *
All files.
.note *.*
All files that have a dot in them. Therefore, it matches
.monoon
a.c, abc.c, bcd.c, xyz.c
.monooff
.endnote
.*
.section Replacement Strings
.*
.ix 'regular expressions' 'replacement strings'
.ix 'replacement strings' 'in regular expressions'
.np
There are special characters to use in the replacement string if you use
regular expressions in the search and replace function.
.begnote
.note &
Replace each instance of & in the replacement string with the entire
string of matched characters
.note \\
Enter a \ in the replacement string
.note \n
Replace with a new line
.note \t
Replace with a tab (if
.kw realtabs
is set)
.note \<n>
Replace each instance of <
.us n
>, where <
.us n
> is a digit from 0 to 9, with the n'th sub-expression in the regular
expression
.note \u
Change the next item in the replacement string to upper case
.note \l
Change the next item in the replacement string to lower case
.note \U
Change all items following \U in the replacement string with upper case,
until \e, or \E is encountered
.note \L
Change all items following \L in the replacement string with lower case,
until \e, or \E is encountered
.note \e, \E
End the change to upper (\U) or lower case (\L)
.note \|<n>
Substitute spaces up to column <n>, so that the item that follows occurs
at column <n>
.note \#
Substitute the current line number on which the match occurred.
.endnote
.*
.section Controlling Magic Characters
.*
.ix 'regular expressions' 'magic characters'
.ix 'magic characters' 'in regular expressions'
.np
By default, all special characters in a regular expression are
magical; that is, if a special character is used, it has a special
meaning.
To use a special character, like (, you must escape it (\().
.np
However, it is possible to change this using the
.kw magic
setting in the General Options dialog. If
.kw magic
is turned on, then all special characters are magical. If
.kw magic
is turned off, then any special characters listed in
.us Magic String Edit Control
in the General Options dialog lose their special meaning and are
treated as regular characters.
If
.kw magic
is turned off and you want to use the characters in their magical way,
you must escape them with a \.
