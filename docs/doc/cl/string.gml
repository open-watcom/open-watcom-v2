.ix 'string'
.ix 'type' 'string'
.pp
A
.us string
is a special form of the type "array of characters",
specifically an array of characters terminated by a
.ix 'null character'
.us null character.
The null character is
a character with the value zero, represented as
.mono \0
within a string, or as the character constant
.mono '\0'
..ct ..li .
Because string processing is such a common task in programming,
C provides a set of
library functions
for handling strings.
.pp
A string is represented by the address of
the first character in the string.
The
.us length
.ix 'string' 'length'
.ix 'length of a string'
of a string is the number of characters up to, but not including, the
null character.
.pp
An array can be initialized to be a string using the following form:
.cillust begin
type identifier
..ct .mono [] = { "
..ct string value
.monoon
..'ct " };
.monooff
.cillust end
.pc
(The braces are optional.)
For example,
.millust begin
char ident[] = "This is my program";
.millust end
.pc
declares
.mono ident
to be an array of 19 characters, the last of which has the value zero.
The string has 18 characters plus the null character.
.pp
In the above example,
.mono ident
is an array whose value is a string.
However, the quote-enclosed value used to initialize the array is
called a
.ix 'string literal'
.us string literal.
String literals are described in the "Constants" chapter.
.keep begin
.pp
A string may be used anywhere in a program where a "pointer to
.kw char
..ct "
may be used. For example, if the declaration,
.millust begin
char * ident;
.millust end
.keep break
.pc
was encountered, the statement,
.millust begin
ident = "This is my program";
.millust end
.keep end
.pc
would set the value of
.mono ident
to be the address of the string
.mono "This is my program".
