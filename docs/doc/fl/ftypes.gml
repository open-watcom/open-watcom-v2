.chap *refid=ftypes Names, Data Types and Constants
.*
.if &e'&dohelp eq 1 .do begin
.np
The following sections describe naming rules, data types supported
by &product, and formats for constants.
.do end
.*
.section Symbolic Names
.*
.np
.ix 'symbolic names'
.us Symbolic names
are names that represent variables, arrays, functions, etc.
Names are formed using any of the upper-case letters A-Z and the
digits 0-9, the first of which must be a letter.
Symbolic names are limited to 6 characters in length.
The following are examples of symbolic names.
.millust begin
    AMOUNT
    AGE
    CUST73
.millust end
.np
.xt begin
.ix 'symbolic names' 'dollar sign ($) in'
.ix 'symbolic names' 'underscore (_) in'
.ix 'symbolic names' 'lower case letters in'
.ix 'dollar sign ($)' 'in symbolic names'
.ix 'underscore (_)' 'in symbolic names'
.ix 'lower case letters' 'in symbolic names'
&product extends the allowable characters that can make up a symbolic
name to include the lower-case letters a-z, the dollar sign ($) and the
underscore (_).
Note that the dollar sign and the underscore are treated as letters
and are therefore allowed as the first letter of a symbolic name.
Furthermore, &product allows symbolic names of up to 32 characters.
The following are examples of permissible symbolic names.
.millust begin
    Evaluate
    $Cheque
    ComputeAverage
    _device
    IO$ERROR
    student_total
.millust end
.np
&product makes no distinction between upper and lower case letters.
The following symbolic names are identical.
.millust begin
    Account
    ACCount
    ACCOUNT
.millust end
.xt end
.np
Spaces are allowed in symbolic names and are ignored.
The following symbolic names are identical.
.millust begin
    C R E DIT
    CRE D I T
.millust end
.np
FORTRAN 77 allows certain keywords such as WRITE to be used as symbolic
names.
.ix 'keywords'
In &product, all
.us keywords
satisfy the requirements of a symbolic name.
A keyword is a sequence of letters that is interpreted in a special
way by &product..
Whether a string of characters is interpreted as a keyword or as a
symbolic name depends on the context in which it is used.
In the following example, the first statement is an assignment
statement assigning the value 2 to the symbolic name
.id DO10I.
The second statement is the beginning of a DO-loop.
.exam begin
      DO10I=1
      DO10I=1,10
.exam end
.sr nunit   = '4 bytes'
.sr cunit   = '1 byte'
.*
.section Data Types
.*
.np
There are 6 basic data types in FORTRAN 77; logical, integer, real,
double precision, complex and character.
.xt on
&product provides an additional data type, namely double precision
complex (
.ct
.kw DOUBLE COMPLEX
or
.kw COMPLEX*16
.ct ).
&product also supports the creation of more complex user-defined data
types
using the
.kw STRUCTURE
statement.
.xt off
.np
Each data type can be classified as numeric, logical or character.
Each datum occupies a sequence of storage units.
Numeric data and logical data occupy numeric storage units whereas
character data occupy character storage units.
In &product, a numeric storage unit occupies &nunit and a
character storage unit occupies &cunit..
.np
The following table summarizes all data types supported by &product..
.ix 'data type' 'summary'
.if &e'&dohelp eq 0 .do begin
.cp 22
.* .box on 1 18 26 36
.sr c0=&INDlvl+1
.sr c1=&INDlvl+18
.sr c2=&INDlvl+26
.sr c3=&INDlvl+36
.box on &c0 &c1 &c2 &c3
\ Data Type   \ Size      \ Standard\
\             \(in bytes) \ FORTRAN\
.box
\ LOGICAL            \ 4  \ yes
\ LOGICAL*1          \ 1  \ extension
\ LOGICAL*4          \ 4  \ extension
\ INTEGER            \ 4  \ yes
\ INTEGER*1          \ 1  \ extension
\ INTEGER*2          \ 2  \ extension
\ INTEGER*4          \ 4  \ extension
\ REAL               \ 4  \ yes
\ REAL*4             \ 4  \ extension
\ REAL*8             \ 8  \ extension
\ DOUBLE PRECISION   \ 8  \ yes
\ COMPLEX            \ 8  \ yes
\ COMPLEX*8          \ 8  \ extension
\ DOUBLE COMPLEX     \ 16 \ extension
\ COMPLEX*16         \ 16 \ extension
\ CHARACTER          \ 1  \ yes
\ CHARACTER*n        \ n  \ yes
.box off
.do end
.el .do begin
.millust begin
+--------------------+-----------+-------------+
| Data Type          | Size      | Standard    |
|                    |(in bytes) | FORTRAN     |
+--------------------+-----------+-------------+
| LOGICAL            | 4         | yes         |
| LOGICAL*1          | 1         | extension   |
| LOGICAL*4          | 4         | extension   |
| INTEGER            | 4         | yes         |
| INTEGER*1          | 1         | extension   |
| INTEGER*2          | 2         | extension   |
| INTEGER*4          | 4         | extension   |
| REAL               | 4         | yes         |
| REAL*4             | 4         | extension   |
| REAL*8             | 8         | extension   |
| DOUBLE PRECISION   | 8         | yes         |
| COMPLEX            | 8         | yes         |
| COMPLEX*8          | 8         | extension   |
| DOUBLE COMPLEX     | 16        | extension   |
| COMPLEX*16         | 16        | extension   |
| CHARACTER          | 1         | yes         |
| CHARACTER*n        | n         | yes         |
+--------------------+-----------+-------------+
.millust end
.do end
.np
Detailed information on the size and range of values supported by
each of these data types is provided in the User's Guide.
.*
.section Data Type of a Name
.*
.np
A name must only have one data type.
Its type is specified by the appearance of that name in a type
statement.
If a name does not appear in any type statement then an implied type
is assigned to it by the "first letter rule".
A name not appearing in any type statement and beginning with any of
the letters I, J, K, L, M or N is assigned the type integer.
A name not appearing in any type statement and beginning with any other
letter is assigned the type real.
The implied type of a letter can be changed by an
.kw IMPLICIT
statement.
.np
The type associated with a name defines the
type of the data it is to contain.
For example, if A is of type integer, then the storage unit which
A occupies is assumed to contain integer data.
Note that the data type of an array element is the same as the data type
associated with the array name.
.np
The data type of a function name specifies the type of the
result returned by the function when it is referenced.
A name that identifies a specific intrinsic function has type as
specified in the chapter entitled :HDREF refid='fsubp'..
.ix 'function' 'generic'
.ix 'generic function'
A
.us generic function
name has no type associated with it; its type is
determined by the type of its argument(s).
The appearance of a generic function in a type statement is not
sufficient to remove the generic properties of that name.
For example, if SIN
was declared to be of type real, it could still
be called with an argument of type complex.
The type of an external function reference is determined in the same
way as for variables and arrays.
The actual type of the external function is determined implicitly by
its name or explicitly by its appearance in a
.kw FUNCTION
or type statement.
Note that an
.kw IMPLICIT
statement can affect the type of the external function being defined.
.*
.section Constants
.*
.np
.ix 'constant'
A
.us constant
can be one of arithmetic, logical or character.
Each constant has a data type and value associated with it and,
once established in a program, cannot be changed.
Arithmetic constants consist of those constants whose data type is one
of integer, real, double precision, complex or double precision complex.
Logical constants consist of those constants whose data type is logical
and character constants consist of those constants whose data type
is character.
The string of characters representing a constant determines its value
and data type.
The blank character is insignificant for all but character constants.
.beglevel
.*
.section Integer Constants
.*
.np
.ix 'constant' 'integer'
.ix 'integer constant'
An
.us integer constant
is formed by a non-empty string of digits
preceded by an optional sign.
.np
The following are examples of integer constants.
.millust begin
    1423
    +345
    -34565788
.millust end
.*
.section Real Constants
.*
.np
.ix 'simple real constant'
We first define a
.us simple real constant
as follows: an optional sign followed by an integer part
followed by a decimal point followed by a fractional part.
The integer and fractional parts are non-empty strings of digits.
Either can be omitted but not both.
.np
.ix 'constant' 'real'
.ix 'real constant'
A
.us real constant
has one of the following forms.
.autopoint
.point
A simple real constant.
.point
A simple real constant followed by an
.mono E
followed by an optionally signed
integer constant.
.point
An integer constant followed by an
.mono E
followed by an optionally signed integer constant.
.endpoint
.np
.ix 'exponent'
The optionally signed integer constant that follows the
.mono E
is called the
.us exponent.
The value of a real constant that contains an exponent is
the value of the constant preceding the
.mono E
multiplied by the power of ten
determined by the exponent.
.np
The following are examples of real constants.
.millust begin
    123.764
    .4352344
    1423.34E12
    +345.E-4
    -.4565788E3
    2E6
    1234.
.millust end
.*
.section Double Precision Constant
.*
.np
.ix 'constant' 'double precision'
.ix 'double precision constant'
A
.us double precision constant
has one of the following forms.
.autopoint
.point
A simple real constant followed by a
.mono D
followed by an optionally signed
integer constant.
.point
An integer constant followed by a
.mono D
followed by an optionally signed
integer constant.
.endpoint
.np
.ix 'exponent'
The optionally signed integer constant that follows the
.mono D
is called the
.us exponent.
The value of a double precision constant that contains an exponent
is the value of the constant preceding the
.mono D
multiplied by the power of
ten determined by the double precision exponent.
Note that the resulting approximation is of greater precision than
the equivalent real constant.
The approximations may be of equal precision if the approximations
are exact representations.
For example,
.mono 0D0
and
.mono 0E0
are double and single precision constants respectively, both
representing zero with the same precision.
.np
The following are examples of double precision constants.
.millust begin
    1423.34D12
    +345.D-4
    -.4565788D5
    2D6
.millust end
.*
.section Complex Constant
.*
.np
.ix 'constant' 'complex'
.ix 'complex constant'
A
.us complex constant
consists of a left parenthesis, followed by a real or integer constant
representing the real part of the complex constant, followed by a
comma, followed by a real or integer constant representing
the imaginary part of the complex constant, followed by a right
parenthesis.
.np
The following are examples of complex constants.
.millust begin
    ( 1423.34E12, 3 )
    ( +345, 4 )
.millust end
.*
.section Double Precision Complex Constant (Extension)
.*
.np
.ix 'constant' 'double precision complex'
.ix 'double precision complex constant'
.xt begin
A
.us double precision complex constant
has the same form as a complex constant except that at least one of the
real and imaginary parts
.us must
be a double precision constant.
.xt end
.np
The following are examples of double precision complex constants.
.millust begin
    ( 1423.34D12, 3 )
    ( +345, 4D2 )
.millust end
.*
.section Logical Constant
.*
.np
.ix 'constant' 'logical'
.ix 'logical constant'
A
.us logical constant
can have one of the following forms.
.autopoint
.point
.mono .TRUE.
representing the value true.
.point
.mono .FALSE.
representing the value false.
.endpoint
.*
.section Character Constant
.*
.np
.ix 'constant' 'character'
.ix 'character constant'
A
.us character constant
consists of an apostrophe followed by any string of characters
followed by an apostrophe.
The apostrophes are not part of the datum.
If an apostrophe is to appear as part of the datum it must be followed
immediately by another apostrophe.
Note that blanks are significant.
The length of the character constant is the number of characters
appearing between the delimiting apostrophes.
Consecutive apostrophes in a character datum represent one character,
namely the apostrophe.
A character constant must not have length 0.
.np
The following are examples of character constants.
.millust begin
    'ABCDEFG1234567'
    'There''s always tomorrow'
.millust end
.*
.section String Constant (Extension)
.*
.np
.xt begin
.ix 'constant' 'string'
.ix 'string constant'
A
.us string constant
consists of an apostrophe followed by any string of characters
followed by an apostrophe and then the letter
.mono C
or
.mono c.
The apostrophes are not part of the datum.
The datum is stored in memory with a terminating NUL character
(CHAR(0)).
If an apostrophe is to appear as part of the datum it must be followed
immediately by another apostrophe.
Note that blanks are significant.
The length of the string constant is the number of characters
appearing between the delimiting apostrophes plus one for the
terminating NUL character (CHAR(0)).
Consecutive apostrophes in a string datum represent one character,
namely the apostrophe.
A string constant must not have length 0.
A string constant may be used anywhere a character constant may be used.
.np
The following are examples of string constants.
.millust begin
    'Hello there'C
    'There''s always tomorrow'c
    'The result for %s=%d'c
.millust end
.xt end
.*
.section Hollerith Constants (Extension)
.*
.np
.ix 'constant' 'hollerith'
.ix 'hollerith constant'
.xt begin
A
.us hollerith constant
consists of a positive unsigned integer constant
.id n
followed by the letter
.mono H or
.mono h
followed by a string of exactly
.id n
characters.
The actual data is the
.id n
characters following the letter
.mono H
or
.mono h.
A hollerith constant is another way of representing character data.
.xt end
.keep
.np
.xt begin
Actually, hollerith constants are treated as character constants
and can be used wherever a character constant can be used.
Hollerith constants are different from character constants in that
a quote is represented by two quotes in character constants and by
a single quote in hollerith constants.
.np
The following are examples of hollerith constants.
.millust begin
    5HABCDEFG
    10h xxxxx '44
.millust end
.pc
.xt end
.*
.section Hexadecimal Constants (Extension)
.*
.np
.ix 'constant' 'hexadecimal'
.ix 'hexadecimal constant'
.xt begin
Two forms of
.us hexadecimal constant
are supported.
The first form can only be used in type declaration or
.kw DATA
statements.
The second form may be used anywhere an integer constant may be used.
.np
The first form of hexadecimal constant consists of the letter
.mono Z
or
.mono z
followed by a string of hexadecimal digits.
A hexadecimal digit can be any digit or one of the letters
.mono A, B, C, D, E
or
.mono F
(the lower case of these letters is also acceptable).
The actual data is the hexadecimal digits following the letter
.mono Z
or
.mono z.
Hexadecimal constants of this form can only be used in type
declaration statements and
.kw DATA
statements for initializing memory with binary patterns.
.np
The following are examples of the first form of hexadecimal constant.
.millust begin
    z1234
    Zac
.millust end
.pc
The first example is equivalent to the binary pattern
.mono 0001 0010 0011 0100.
The second example is equivalent to the binary pattern
.mono 1010 1100.
.np
The second form of hexadecimal constant consists of an apostrophe
followed by any string of hexadecimal digits followed by an apostrophe
and then the letter
.mono X
or
.mono x.
A hexadecimal digit can be any digit or one of the letters
.mono A, B, C, D, E
or
.mono F
(the lower case of these letters is also acceptable).
The actual data is the hexadecimal digits placed inside apostrophes.
.np
The following are examples of the second form of hexadecimal constant.
.millust begin
    '1234'x
    'ac'X
.millust end
.pc
The first example is equivalent to the binary pattern
.mono 0001 0010 0011 0100.
The second example is equivalent to the binary pattern
.mono 1010 1100.
.xt end
.*
.section Octal Constants (Extension)
.*
.np
.ix 'constant' 'octal'
.ix 'octal constant'
.xt begin
An
.us octal constant
consists of an apostrophe followed by any string of octal digits
followed by an apostrophe and then the letter
.mono O
or
.mono o.
An octal digit can be any of the digits 0 through 7.
The actual data is the octal digits placed inside apostrophes.
An octal constant may be used anywhere an integer constant may be used.
.xt end
.keep 15
.np
.xt begin
The following are examples of octal constants.
.millust begin
    '1234'o
    '37'O
.millust end
.pc
The first example is equivalent to the binary pattern
.mono 001 010 011 100.
The second example is equivalent to the binary pattern
.mono 011 111.
.xt end
.endlevel
.*
.section Symbolic Constants
.*
.np
It is possible to give a constant a symbolic name.
This is done through
.kw PARAMETER
statements.
For more details, see the section on the
.kw PARAMETER
statement in the chapter entitled :HDREF refid='fstats'..
