.newtext Format Control String:
The format control string consists of
.us ordinary characters,
that are written exactly as they occur in the format string, and
.us conversion specifiers,
that cause argument values to be written as they are encountered
during the processing of the format string.
An ordinary character in the format string is any character, other
than a percent character (%), that is not part of a conversion
specifier.
A conversion specifier is a sequence of characters in the format
string that begins with a percent character (%) and is followed, in
sequence, by the following:
.begbull
.bull
zero or more
.us format control flags
that can modify the final effect of the format directive;
.bull
an optional decimal integer, or an asterisk character ('*'), that
specifies a
.us minimum field width
to be reserved for the formatted item;
.bull
an optional
.us precision
specification in the form of a period character (.),
followed by an optional decimal integer or an asterisk character (*);
.bull
an optional
.us type length
specification: one of "hh", "h", "l", "ll", "j", "z", "t", "L", "I64", "w"
.if &farfnc eq 0 .do begin
.ct
; and
.do end
.el .do begin
.ct ,
"N" or "W"; and
.do end
.bull
a character that specifies the type of conversion to be performed: one of
the characters "bcCdeEfFgGinopsSuxX".
.endbull
.np
The valid format control flags are:
.begnote $setptnt 3
.note "&minus."
the formatted item is left-justified within the field; normally, items
are right-justified
.note "+"
a signed, positive object will always start with a plus character
(+); normally, only negative items begin with a sign
.note "&sysrb"
a signed, positive object will always start with a space character; if
both "+" and "&sysrb." are specified, "+" overrides "&sysrb."
.note "#"
an alternate conversion form is used:
.begbull
.bull
for "b" (unsigned binary) and "o" (unsigned octal) conversions, the
precision is incremented, if necessary, so that the first digit is "0".
.bull
for "x" or "X" (unsigned hexadecimal) conversions, a non-zero value
is prepended with "0x" or "0X" respectively.
.bull
for "e", "E", "f", "F", "g" or "G" (any floating-point) conversions, the
result always contains a decimal-point character, even if no digits
follow it; normally, a decimal-point character appears in the result only
if there is a digit to follow it.
.bull
in addition to the preceding, for "g" or "G" conversions, trailing
zeros are not removed from the result.
.endbull
.endnote
.np
If no field width is specified, or if the value that is given is less
than the number of characters in the converted value (subject to any
precision value), a field of sufficient width to contain the converted
value is used.
If the converted value has fewer characters than are specified by the
field width, the value is padded on the left (or right, subject to the
left-justification flag) with spaces or zero characters ("0").
If the field width begins with "0" and no precision is specified, the
value is padded with zeros; otherwise the value is padded with spaces.
If the field width is "*", a value of type
.id int
from the argument list is used (before a precision argument or a
conversion argument) as the minimum field width.
A negative field width value is interpreted as a left-justification
flag, followed by a positive field width.
.np
As with the field width specifier, a precision specifier of "*" causes a
value of type
.id int
from the argument list to be used as the precision specifier.
If no precision value is given, a precision of 0 is used.
The precision value affects the following conversions:
.begbull
.bull
For "b", "d", "i", "o", "u", "x" and "X" (integer) conversions, the
precision specifies the minimum number of digits to appear.
.bull
For "e", "E", "f" and "F" (fixed-precision, floating-point) conversions, the
precision specifies the number of digits to appear after the
decimal-point character.
.bull
For "g" and "G" (variable-precision, floating-point) conversions, the
precision specifies the maximum number of significant digits to appear.
.bull
For "s" or "S" (string) conversions, the precision specifies the
maximum number of characters to appear.
.endbull
.np
A type length specifier affects the conversion as follows:
.begbull
.bull
"hh" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) format
conversion to treat the argument as a
.id signed char
or
.id unsigned char
argument.
Note that, although the argument may have been promoted to an
.id int
as part of the function call, the value is converted to the smaller
type before it is formatted.
.bull
"hh" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id signed char.
.bull
"h" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) format
conversion to treat the argument as a
.id short int
or
.id unsigned short int
argument.
Note that, although the argument may have been promoted to an
.id int
as part of the function call, the value is converted to the smaller
type before it is formatted.
.bull
.ix 'fixed-point'
"h" causes an "f" format conversion to interpret a
.id long
argument as a fixed-point number consisting of a 16-bit signed integer
part and a 16-bit unsigned fractional part.
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
.np
The value is formatted with the same rules as for floating-point values.
This is a &company extension.
.bull
"h" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id short int.
.bull
"h" causes an "s" operation to treat the argument string as an ASCII
character string composed of 8-bit characters.
.np
For
.kw printf
and related byte input/output functions, this specifier is redundant.
For
.kw wprintf
and related wide character input/output functions, this specifier is
required if the argument string is to be treated as an 8-bit ASCII
character string; otherwise it will be treated as a wide character
string.
.millust begin
printf(    "%s%d", "Num=", 12345 );
wprintf( L"%hs%d", "Num=", 12345 );
.millust end
.bull
"l" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion to
process a
.id long int
or
.id unsigned long int
argument.
.bull
"l" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id long int.
.bull
"l" or "w" cause an "s" operation to treat the argument string as a
wide character string (a string composed of characters of type
.kw wchar_t
.ct ).
.np
For
.kw printf
and related byte input/output functions, this specifier is required if
the argument string is to be treated as a wide character string;
otherwise it will be treated as an 8-bit ASCII character string.
For
.kw wprintf
and related wide character input/output functions, this specifier is
redundant.
.millust begin
printf(  "%ls%d", L"Num=", 12345 );
wprintf( L"%s%d", L"Num=", 12345 );
.millust end
.bull
.ix 'long long'
"ll" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion to
process a
.id long long
or
.id unsigned long long
argument (e.g., %lld).
.bull
"ll" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id long long int.
.bull
.ix 'intmax_t'
.ix 'uintmax_t'
"j" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion to
process an
.id intmax_t
or
.id uintmax_t
argument.
.bull
"j" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id intmax_t.
.bull
.ix 'size_t'
"z" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion to
process a
.id size_t
or the corresponding signed integer type argument.
.bull
"z" causes an "n" (converted length assignment) operation to assign the
converted length to an object of signed integer type corresponding to
.id size_t.
.bull
.ix 'ptrdiff_t'
"t" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion to
process a
.id ptrdiff_t
or the corresponding unsigned integer type argument.
.bull
"t" causes an "n" (converted length assignment) operation to assign the
converted length to an object of type
.id ptrdiff_t.
.bull
.ix '__int64'
"I64" causes a "b", "d", "i", "o", "u", "x" or "X" (integer) conversion
to process an
.id __int64
or
.id unsigned __int64
argument (e.g., %I64d).
.bull
.ix 'long double'
"L" causes an "e", "E", "f", "F", "g", "G" (double) conversion to process a
.id long double
argument.
.if &farfnc ne 0 .do begin
.bull
"W" causes the pointer associated with "n", "p", "s" conversions to
be treated as a far pointer.
.bull
"N" causes the pointer associated with "n", "p", "s" conversions to
be treated as a near pointer.
.do end
.endbull
.np
The valid conversion type specifiers are:
.begnote $setptnt 3
.note b
An argument of type
.id int
is converted to an unsigned binary notation and written to the output
stream.
The default precision is 1, but if more digits are required, leading
zeros are added.
.note c
An argument of type
.id int
is converted to a value of type
.id char
and the corresponding ASCII character code is written to the output
stream.
.note C
An argument of type
.id wchar_t
is converted to a multibyte character and written to the output stream.
.note d, i
An argument of type
.id int
is converted to a signed decimal notation and written to the output
stream.
The default precision is 1, but if more digits are required, leading
zeros are added.
.note e, E
An argument of type
.id double
is converted to a decimal notation in the form
.mono &lbrkt.&minus.&rbrkt.d.ddde&lbrkt.+|&minus.&rbrkt.ddd
similar to FORTRAN exponential (E) notation.
The leading sign appears (subject to the format control flags) only if
the argument is negative.
If the argument is non-zero, the digit before the decimal-point character
is non-zero.
The precision is used as the number of digits following the
decimal-point character.
If the precision is not specified, a default precision of six is used.
If the precision is 0, the decimal-point character is suppressed.
The value is rounded to the appropriate number of digits.
For "E" conversions, the exponent begins with the character "E" rather
than "e".
The exponent sign and a three-digit number
(that indicates the power of ten
by which the decimal fraction is multiplied) are always produced.
.note f, F
An argument of type
.id double
is converted to a decimal notation in the form
.mono &lbrkt.&minus.&rbrkt.ddd.ddd
similar to FORTRAN fixed-point (F) notation.
The leading sign appears (subject to the format control flags) only if
the argument is negative.
The precision is used as the number of digits following the
decimal-point character.
If the precision is not specified, a default precision of six is used.
If the precision is 0, the decimal-point character is suppressed,
otherwise, at least one digit is produced before the decimal-point
character.
The value is rounded to the appropriate number of digits.
.note g, G
An argument of type
.id double
is converted using either the "f" or "e" (or "F" or "E", for a "G" conversion)
style of conversion depending on the value of the argument.
In either case, the precision specifies the number of significant digits
that are contained in the result.
"e" style conversion is used only if the exponent from such a conversion
would be less than &minus.4 or greater than the precision.
Trailing zeros are removed from the result and a decimal-point character
only appears if it is followed by a digit.
.note n
The number of characters that have been written to the output stream is
assigned to the integer pointed to by the argument.
No output is produced.
.note o
An argument of type
.id int
is converted to an unsigned octal notation and written to the output
stream.
The default precision is 1, but if more digits are required, leading
zeros are added.
.note p, P
An argument of type
.id void *
is converted to a value of type
.id int
and the value is formatted as for a hexadecimal ("x") conversion.
.note s
Characters from the string specified by an argument of type
.id char *
or
.id wchar_t *
.ct , up to,
but not including the terminating null character ('\0'), are written
to the output stream.
If a precision is specified, no more than that many characters (bytes)
are written (e.g., %.7s)

.if &'length(&wfunc.) ne 0 .do begin
.np
For &func, this specifier refers to an ASCII character string unless
the "l" or "w" modifiers are used to indicate a wide character string.
.np
For &wfunc, this specifier refers to a wide character string unless
the "h" modifier is used to indicate an ASCII character string.
.do end
*.
.note S
Characters from the string specified by an argument of type
.id wchar_t *
.ct , up to,
but not including the terminating null wide character (L'\0'), are
converted to multibyte characters and written to the output stream.
If a precision is specified, no more than that many characters (bytes)
are written (e.g., %.7S)
.note u
An argument of type
.id int
is converted to an unsigned decimal notation and written to the output
stream.
The default precision is 1, but if more digits are required, leading
zeros are added.
.note x, X
An argument of type
.id int
is converted to an unsigned hexadecimal notation and written to the
output stream.
The default precision is 1, but if more digits are required, leading
zeros are added.
Hexadecimal notation uses the digits "0" through "9"
and the characters "a" through "f" or "A" through "F"
for "x" or "X" conversions respectively, as the hexadecimal digits.
Subject to the alternate-form control flag, "0x" or "0X" is prepended
to the output.
.endnote
.np
Any other conversion type specifier character, including another percent
character (%), is written to the output stream with no special
interpretation.
.np
The arguments must correspond with the conversion type specifiers, left
to right in the string; otherwise, indeterminate results will occur.
.np
.ix '_finite'
.ix 'infinity'
.ix 'NaN'
If the value corresponding to a floating-point specifier is infinity, or
not a number (NaN), then the output will be "inf" or "-inf" for infinity,
and "nan" or "-nan" for NaN's. If the conversion specifier is an uppercase
character (ie. "E", "F", or "G"), the output will be uppercase as well
("INF", "NAN"), otherwise the output will be lowercase as noted above.
.np
The pointer size specification ("N" or "W") is only effective on platforms
that use a segmented memory model, although it is always recognized.
.np
For example, a specifier of the form
.id "%8.*f"
will define a field to be at least 8 characters wide, and will get the
next argument for the precision to be used in the conversion.
.oldtext end
.if &farfnc eq 0 .do begin
.class ANSI (except for b and I64 specifiers)
.do end
.el .do begin
.class ANSI (except for N, W pointer size modifiers and b, I64 specifiers)
.do end
.system
