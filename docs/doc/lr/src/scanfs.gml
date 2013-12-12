.newtext Format Control String:
The format control string consists of zero or more
.us format directives
that specify acceptable input file data.
Subsequent arguments are pointers to various types of objects that are
assigned values as the format string is processed.
.np
A format directive can be a sequence of one or more white-space
characters, an
.us ordinary character,
or a
.us conversion specifier.
An ordinary character in the format string is any character, other
than a white-space character or the percent character (%), that is not
part of a conversion specifier.
A conversion specifier is a sequence of characters in the format
string that begins with a percent character (%) and is followed, in
sequence, by the following:
.begbull
.bull
an optional assignment suppression indicator: the asterisk character
(*);
.bull
an optional decimal integer that specifies the
.us maximum field width
to be scanned for the conversion;
.if &farfnc ne 0 .do begin
.bull
an optional
.us pointer-type
specification: one of "N" or "W";
.do end
.bull
an optional
.us type length
specification: one of "hh", "h", "l", "ll", "j", "z", "t", "L" or "I64";
.bull
a character that specifies the type of conversion to be performed: one
of the characters "cCdeEfFgGinopsSuxX[".
.endbull
.np
As each format directive in the format string is processed, the
directive may successfully complete, fail because of a lack of input
data, or fail because of a matching error as defined by the particular
directive.
If end-of-file is encountered on the input data before any characters
that match the current directive have been processed (other than
leading white-space where permitted), the directive fails for lack of
data.
If end-of-file occurs after a matching character has been processed,
the directive is completed (unless a matching error occurs), and the
function returns without processing the next directive.
If a directive fails because of an input character mismatch, the
character is left unread in the input stream.
Trailing white-space characters, including new-line characters, are
not read unless matched by a directive.
When a format directive fails, or the end of the format string is
encountered, the scanning is completed and the function returns.
.np
When one or more white-space characters (space "&sysrb", horizontal
tab "\t", vertical tab "\v", form feed "\f", carriage return "\r", new
line or linefeed "\n") occur in the format string, input data up to
the first non-white-space character is read, or until no more data
remains.
If no white-space characters are found in the input data, the scanning
is complete and the function returns.
.np
An ordinary character in the format string is expected to match the
same character in the input stream.
.np
A conversion specifier in the format string is processed as follows:
.begbull
.bull
for conversion types other than "[", "c", "C" and "n", leading
white-space characters are skipped
.bull
for conversion types other than "n", all input characters, up to any
specified maximum field length, that can be matched by the conversion
type are read and converted to the appropriate type of value; the
character immediately following the last character to be matched is
left unread; if no characters are matched, the format directive fails
.bull
unless the assignment suppression indicator ("*") was specified, the
result of the conversion is assigned to the object pointed to by the
next unused argument (if assignment suppression was specified, no
argument is skipped); the arguments must correspond in number, type
and order to the conversion specifiers in the format string
.endbull
.if &farfnc ne 0 .do begin
.np
A pointer-type specification is used to indicate the type of pointer
used to locate the next argument to be scanned:
.begnote $setptnt 5
.note W
pointer is a far pointer
.note N
pointer is a near pointer
.endnote
.np
The pointer-type specification is only effective on platforms that use
a segmented memory model, although it is always recognized.
.do end
.np
The pointer type defaults to that used for data in the memory model
for which the program has been compiled.
.np
A type length specifier affects the conversion as follows:
.begbull
.bull
"hh" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id signed char
or
.id unsigned char.
.bull
"hh" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id signed char.
.bull
"h" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id short int
or
.id unsigned short int.
.bull
.ix 'fixed-point'
"h" causes an "f" conversion to assign a fixed-point number to an object
of type
.id long
consisting of a 16-bit signed integer part and a 16-bit unsigned
fractional part.
The integer part is in the high 16 bits and the fractional part is in
the low 16 bits.
.millust begin
struct fixpt {
    unsigned short fraction; /* Intel architecture! */
      signed short integral;
};

struct fixpt foo1 =
  { 0x8000, 1234 }; /* represents 1234.5 */
struct fixpt foo2 =
  { 0x8000, -1 };   /* represents -0.5 (-1+.5) */
.millust end
.bull
"h" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id short int.
*.
.if &'length(&wfunc.) ne 0 .do begin
.bull
"h" causes an "s" operation to convert the input string to an ASCII
character string.
For
.id &func.
this specifier is redundant.
For &wfunc, this specifier is required if the wide character input
string is to be converted to an ASCII character string; otherwise it
will not be converted.
.do end
*.
.bull
"l" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id long int
or
.id unsigned long int.
.bull
"l" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id long int.
.bull
"l" causes an "e", "f" or "g" (floating-point) conversion to assign
the converted value to an object of type
.id double.

.if &'length(&wfunc.) ne 0 .do begin
.bull
"l" or "w" cause an "s" operation to convert the input string to a
wide character string.
For
.id &func.
this specifier is required if the input ASCII string is to
be converted to a wide character string; otherwise it will not be
converted.
.do end
*.
.bull
"ll" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id long long
or
.id unsigned long long
(e.g., %lld).
.bull
"ll" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id long long int.
.bull
.ix 'intmax_t'
.ix 'uintmax_t'
"j" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id intmax_t
or
.id uintmax_t.
.bull
"j" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id intmax_t.
.bull
.ix 'size_t'
"z" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id size_t
or the corresponding signed integer type.
.bull
"z" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of signed integer
type corresponding to
.id size_t.
.bull
.ix 'ptrdiff_t'
"t" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id ptrdiff_t
or the corresponding unsigned integer type.
.bull
"t" causes an "n" (read length assignment) operation to assign the
number of characters that have been read to an object of type
.id ptrdiff_t.
.bull
.ix '__int64'
"I64" causes a "d", "i", "o", "u" or "x" (integer) conversion to
assign the converted value to an object of type
.id __int64
or
.id unsigned __int64
(e.g., %I64d).
.bull
.ix 'long double'
"L" causes an "e", "f" or "g" (floating-point) conversion to assign
the converted value to an object of type
.id long double.
.endbull
.np
The valid conversion type specifiers are:
.begnote $setptnt 5
.note c
Any sequence of characters in the input stream of the length specified
by the field width, or a single character if no field width is
specified, is matched.
The argument is assumed to point to the first element of a character
array of sufficient size to contain the sequence, without a
terminating null character ('\0').
For a single character assignment, a pointer to a single object of
type
.id char
is sufficient.
.note C
A sequence of multibyte characters in the input stream is matched.
Each multibyte character is converted to a wide character of type
.id wchar_t.
The number of wide characters matched is specified by the field width
(1 if no field width is specified).
The argument is assumed to point to the first element of an array of
.id wchar_t
of sufficient size to contain the sequence.
No terminating null wide character (L'\0') is added.
For a single wide character assignment, a pointer to a single object
of type
.id wchar_t
is sufficient.
.note d
A decimal integer, consisting of an optional sign, followed by one or
more decimal digits, is matched.
The argument is assumed to point to an object of type
.id int.
.note e, f, g
A floating-point number, consisting of an optional sign ("+" or
"&minus."), followed by one or more decimal digits, optionally
containing a decimal-point character, followed by an optional exponent
of the form "e" or "E", an optional sign and one or more decimal
digits, is matched.
The exponent, if present, specifies the power of ten by which the
decimal fraction is multiplied.
The argument is assumed to point to an object of type
.id float.
.note i
An optional sign, followed by an octal, decimal or hexadecimal
constant is matched.
An octal constant consists of "0" and zero or more octal digits.
A decimal constant consists of a non-zero decimal digit and zero or
more decimal digits.
A hexadecimal constant consists of the characters "0x" or "0X"
followed by one or more (upper- or lowercase) hexadecimal digits.
The argument is assumed to point to an object of type
.id int.
.note n
No input data is processed.
Instead, the number of characters that have already been read is
assigned to the object of type
.id unsigned int
that is pointed to by the argument.
The number of items that have been scanned and assigned (the return
value) is not affected by the "n" conversion type specifier.
.note o
An octal integer, consisting of an optional sign, followed by one or
more (zero or non-zero) octal digits, is matched.
The argument is assumed to point to an object of type
.id int.
.note p
A hexadecimal integer, as described for "x" conversions below, is
matched.
The converted value is further converted to a value of type
.id void*
and then assigned to the object pointed to by the argument.
.note s
A sequence of non-white-space characters is matched.
The argument is assumed to point to the first element of a character
array of sufficient size to contain the sequence and a terminating
null character, which is added by the conversion operation.
.note S
A sequence of multibyte characters is matched.
None of the multibyte characters in the sequence may be single byte
white-space characters.
Each multibyte character is converted to a wide character.
The argument is assumed to point to the first element of an array of
.id wchar_t
of sufficient size to contain the sequence and a terminating null wide
character, which is added by the conversion operation.
.note u
An unsigned decimal integer, consisting of one or more decimal digits,
is matched.
The argument is assumed to point to an object of type
.id unsigned int.
.note x
A hexadecimal integer, consisting of an optional sign, followed by an
optional prefix "0x" or "0X", followed by one or more (upper- or
lowercase) hexadecimal digits, is matched.
The argument is assumed to point to an object of type
.id int.
.note [c1c2...]
The longest, non-empty sequence of characters, consisting of any of
the characters
.mono c1, c2, ...
called the
.us scanset,
in any order, is matched.
.mono c1
cannot be the caret character ('^').
If
.mono c1
is "]", that character
is considered to be part of the scanset and a second "]"
is required to end the format directive.
The argument is assumed to point to the first element of a character
array of sufficient size to contain the sequence and a terminating
null character, which is added by the conversion operation.
.note [^c1c2...]
The longest, non-empty sequence of characters, consisting of any
characters
.us other than
the characters between the "^" and "]", is matched.
As with the preceding conversion, if
.mono c1
is "]", it is considered to be part of the scanset and a second "]"
ends the format directive.
The argument is assumed to point to the first element of a character
array of sufficient size to contain the sequence and a terminating
null character, which is added by the conversion operation.
.np
For example, the specification
.mono %[^\n]
will match an entire input line up to but not including the newline
character.
.endnote
.np
A conversion type specifier of "%" is treated as a single ordinary
character that matches a single "%" character in the input data.
A conversion type specifier other than those listed above causes
scanning to terminate and the function to return.
.np
Conversion type specifiers "E", "F", "G", "X" have meaning identical to
their lowercase equivalents.
.np
The line
.blkcode begin
scanf( "%s%*f%3hx%d", name, &hexnum, &decnum )
.blkcode end
.blktext begin
with input
.blktext end
.blkcode begin
some_string 34.555e-3 abc1234
.blkcode end
.blktext begin
will copy
.mono "some_string"
into the array
.id name,
skip
.mono 34.555e-3
..ct ,
assign
.mono 0xabc
to
.id hexnum
and
.id 1234
to
.id decnum.
The return value will be 3.
.np
The program
.blktext end
.blkcode begin
#include <stdio.h>

void main( void )
{
    char string1[80], string2[80];

    scanf( "%[abcdefghijklmnopqrstuvwxyz"
           "ABCDEFGHIJKLMNOPQRSTUVWZ ]%*2s%[^\n]",
           string1, string2 );
    printf( "%s\n%s\n", string1, string2 );
}
.blkcode end
.blktext begin
with input
.blktext end
.blkcode begin
They may look alike, but they don't perform alike.
.blkcode end
.blktext begin
will assign
.blktext end
.blkcode begin
"They may look alike"
.blkcode end
.blktext begin
to
.id string1,
skip the comma (the
.mono "%*2s"
will match only the comma; the following blank terminates that field),
and assign
.blktext end
.blkcode begin
" but they don't perform alike."
.blkcode end
.blktext begin
to
.id string2.
.blktext end
.oldtext end
.if &farfnc eq 0 .do begin
.class begin ISO C90
.br The I64 modifier is an extension to ISO C.
.class end
.do end
.el .do begin
.class begin ISO C90
.br The N, W pointer size modifiers and the I64 modifier are extensions to ISO C.
.class end
.do end
.system
