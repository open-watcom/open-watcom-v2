.chap *refid=fformat Format
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
A
.us format
specification used in conjunction with formatted I/O provides a
means of specifying the way internal data is converted to a
character string and vice versa.
A format specification can be given in two ways.
.autopoint
.point
In a
.kw FORMAT
statement.
.point
As values of character expressions or character arrays.
.endpoint
.*
.section The FORMAT Statement
.*
.np
The form of a
.kw FORMAT
statement is
.mbox begin
      label    FORMAT fs
.mbox end
.synote 7
.mnote label
is the statement label used by an I/O statement to identify the
.kw FORMAT
statement.
.mnote fs
is a format specification which will be described later.
.endnote
.exam begin
      REAL X
      X = 234.43
      PRINT 100, X
100   FORMAT(F10.2)
      END
.exam end
.pc
In the previous example, the
.kw PRINT
statement uses the format specification in the
.kw FORMAT
statement whose statement label is 100 to display the value of
.id X.
.*
.section FORMAT as a Character Expression
.*
.np
Instead of specifying the statement label of a
.kw FORMAT
statement, a character expression can be used.
The previous example could be modified as follows and achieve the
identical result.
.exam begin
      REAL X
      X = 234.43
      PRINT '(F10.2)', X
      END
.exam end
.np
When using a character expression to represent a format specification,
the format specification can be preceded by blank characters and followed
by any character data without affecting the format specification.
The following example produces the identical result to the previous
example.
.exam begin
      REAL X
      X = 234.43
      PRINT '    (F10.2) THIS IS FOR X', X
      END
.exam end
.np
If a character array is used to describe the format specification,
the format specification is considered to be the concatenation of
all the character array elements in the order given by array element
ordering described in the chapter entitled :HDREF refid='farrays'..
Note that if a character array element is used, the format specification
is considered to be only that array element.
.exam begin
      REAL X
      CHARACTER*5 FMTSPEC(3)
      X = 234.43
      FMTSPEC(1)='('
      FMTSPEC(2)='F10.2'
      FMTSPEC(3)=')'
      PRINT FMTSPEC, X
      END
.exam end
.*
.section Format Specification
.*
.np
.ix 'format' 'see also' 'edit descriptor'
A
.us format specification
.ix 'format specification'
has the following form.
.mbox begin
      ( [flist] )
.mbox end
.synote 8
.mnote flist
is a list whose items are separated by commas.
The forms of the items in
.id flist
are:
.np
.id [r] ed
.np
.id ned
.np
.id [r] fs
.mnote ed
is a repeatable edit descriptor.
.mnote ned
is a nonrepeatable edit descriptor.
.mnote fs
is a format specification with a nonempty list
.id flist.
.mnote r
is a positive unsigned integer constant called a
.us repeat specification.
.ix 'format' 'repeat specification'
.endnote
.np
The comma separating the items of
.id flist
can be omitted in the following cases.
.autopoint
.point
Between a
.id P
edit descriptor and an
.id F, E, D
or
.id G
edit descriptor which immediately follows.
.point
Before or after a slash edit descriptor.
.point
Before or after a colon edit descriptor.
.endpoint
.np
.xt begin
&product allows the omission of a comma between the items of
.id flist.
Care should be taken when omitting commas between edit descriptors.
For example, the format specification
.mono (I5 2I3)
may appear to be an
.mono I5
edit descriptor followed by two
.mono I3
edit descriptors when in actuality it is interpreted as an
.mono I52
edit descriptor followed by an
.mono I3
edit descriptor.
.xt end
.*
.section Repeatable Edit Descriptors
.*
.np
The forms of
.us repeatable edit descriptors
.ix 'repeatable edit descriptor'
.ix 'edit descriptor' 'repeatable'
.ix 'extension' 'E edit descriptor'
.ix 'extension' 'Z edit descriptor'
are:
.millust begin
    Iw
    Iw.m
    Fw.d
    Ew.d
    Ew.dEe
    Dw.d
    Gw.d
    Gw.dEe
    Lw
    A
    Aw
.millust end
.np
.xt begin
As an extension to the FORTRAN 77 language, the following repeatable
edit descriptors are also supported.
.millust begin
    Ew.dDe
    Zw
.millust end
.xt end
.synote
.mnote ~b
.id I, F, E, D, G, L, A
and
.id Z
indicate the method of editing.
.np
.id w
and
.id e
are positive unsigned integer constants.
.np
.id d
and
.id m
are unsigned integer constants.
.endnote
.*
.section Nonrepeatable Edit Descriptors
.*
.np
The forms of
.us nonrepeatable edit descriptors
.ix 'nonrepeatable edit descriptors'
.ix 'edit descriptor' 'repeatable'
.ix '$ edit descriptor'
.ix '\ edit descriptor'
.ix 'extension' 'X edit descriptor'
.ix 'extension' '$ edit descriptor'
.ix 'extension' '\ edit descriptor'
are:
.millust begin
    'hh...h' (apostrophe)
    nHhh...h
    Tc
    TLc
    TRc
    nX
    /
    :
    S
    SP
    SS
    kP
    BN
    BZ
    X
.millust end
.np
.xt begin
As an extension to the FORTRAN 77 language, the following
nonrepeatable edit descriptors are also supported.
.millust begin
    $
    \
.millust end
.xt end
.synote
.mnote ~b
Apostrophe,
.id H, T, TL, TR, X, /, :, S, SP, SS, P, BN, BZ,
.xt on
.id \
and
.id $
.xt off
indicate the method of editing.
.np
.id h
is a character.
.np
.id n
and
.id c
are positive unsigned integer constants.
.np
.id k
is an optionally signed integer constant.
.endnote
.np
.xt begin
&product allows edit descriptors to be specified using lower case
letters.
.xt end
.*
.section Editing
.*
.np
Edit descriptors are used to describe the way the editing between
internal representation of data and the characters of a record in a file
is to take place.
When the edit descriptors
.id I, F, E, D, G, L, A, H, Z
or apostrophe are
processed, they process a sequence of characters called a
.us field.
.ix 'format' 'field'
On input, the field is the character data read from a record; on output
it is the character data written to a record.
The number of characters in a field is called the
.us field width.
.ix 'format' 'field width'
.*
.beglevel
.*
.section Apostrophe Editing
.*
.np
The
.us apostrophe edit descriptor
.ix 'apostrophe edit descriptor'
.ix 'edit descriptor' 'apostrophe'
has the same form as a character constant
and can only be used on output.
It causes the characters in the format specification enclosed in quotes
to be written.
The field width is the number of characters enclosed in quotes.
.exam begin
      PRINT '(''HI THERE'')'
      END
.exam end
.pc
In the previous example, the string
.millust begin
HI THERE
.millust end
.pc
would be the output produced by the
.kw PRINT
statement.
.*
.section H Editing
.*
.np
The
.id nH
.ix 'H edit descriptor'
.ix 'edit descriptor' 'H'
edit descriptor causes the
.id n
characters following the
.id H,
including blanks, to be written.
Like the apostrophe edit descriptor, it can only appear in a format
specification used for output.
.exam begin
      PRINT '(8HHI THERE)'
      END
.exam end
.pc
In the previous example, the string
.millust begin
HI THERE
.millust end
.pc
would be the output produced by the
.kw PRINT
statement.
.*
.section Positional Editing: T, TL, TR and X Editing
.*
.np
.ix 'positional edit descriptor'
.ix 'edit descriptor' 'positional'
.ix 'edit descriptor' 'positional T'
.ix 'edit descriptor' 'positional TL'
.ix 'edit descriptor' 'positional TR'
.ix 'edit descriptor' 'positional X'
.ix 'T edit descriptor'
.ix 'TL edit descriptor'
.ix 'TR edit descriptor'
.ix 'X  edit descriptor'
The
.id T,
.id TL,
.id TR
and
.id X
edit descriptors specify at which position the next character will be
read from or written to the record.
In the case of input, this allows data to be read more than once with
different edit descriptors.
On output, it is possible to overwrite data previously written.
.np
On output it is possible to use positional editing to create a record
in which gaps appear.
That is, there may be parts of the record where no data has been
written.
The parts of a record in which no data has been written are filled with
blanks.
The effect is as if the record was previously initialized to blanks.
Note that positioning does not cause any data to be transmitted.
.np
The
.id Tc
edit descriptor specifies that the next character to be
transmitted is to be from the
.id c
.ct th
character position in the record.
The
.id TLc
edit descriptor specifies that the next character to be
transmitted is to be from the
.id c
.ct th
position backward from the current position.
The
.id TRc
edit descriptor is identical to the
.id TLc
edit descriptor except
that positioning is forward from the current position.
The
.id nX
edit descriptor behaves identically to the
.id TRc
edit descriptor;
the transmission of the next character is
.id n
character positions forward from the current position.
.xt on
If
.id n
is omitted then the transmission of the next character is 1 character
position forward from the current position.
.xt off
.exam begin
      PRINT '(''THE NUMBER       IS AN INTEGER'',TL19,
     $      ''12345'')'
      END
.exam end
.pc
The output produced is
.millust begin
THE NUMBER 12345 IS AN INTEGER
.millust end
.*
.section Slash Editing
.*
.np
The
.us slash edit descriptor
.ix 'slash edit descriptor'
.ix 'edit descriptor' 'slash'
indicates the end of data transfer on the current record.
On input from a record connected for sequential access, the remaining
characters in the record are skipped and the file is positioned to the
start of the next record.
Note that entire records may be skipped.
On output, a new record is created and becomes the last and current
record of the file.
Note that a record with no characters can be written.
If the file is an internal file or a direct access file, the record
is filled with blanks.
.np
For a file connected for direct access, the current record number is
increased by one and the file is positioned at the beginning of that
record.
.*
.section Colon Editing
.*
.np
The
.us colon edit descriptor
.ix 'colon edit descriptor'
.ix 'edit descriptor' 'colon'
terminates processing of the format
specification if there are no more items in the I/O list.
If there are items remaining in the I/O list, the colon edit
descriptor has no effect.
.*
.section S, SP and SS Editing
.*
.np
The
.id S,
.id SP
and
.id SS
.ix 'S edit descriptor'
.ix 'SS edit descriptor'
.ix 'SP edit descriptor'
.ix 'edit descriptor' 'S'
.ix 'edit descriptor' 'SS'
.ix 'edit descriptor' 'SP'
edit descriptors control optional plus characters in
numeric output fields.
They only effect the
.id I,
.id F,
.id E,
.id D
and
.id G
edit descriptors during output
and have no effect on input.
The FORTRAN 77 standard specifies that before processing a format
specification, the appearance of a plus sign in numeric output fields
is optional and is determined by the processor.
&product does not produce plus signs in numeric output fields.
When an
.id SP
edit descriptor is encountered, a plus sign is produced
in any subsequent position that optionally contains a plus sign.
When as
.id SS
edit descriptor is encountered, a plus sign is not produced
in any subsequent position that optionally contains a plus sign.
If an
.id S
edit descriptor is encountered, the option is returned to the
processor.
.exam begin
      PRINT '(1H<,I5,SP,I5,SS,I5,1H>)',1,2,3
      END
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is:
.millust begin
<    1   +2    3>
.millust end
.*
.section P Editing
.*
.np
The form of a
.id P
edit descriptor is
.ix 'P edit descriptor'
.ix 'edit descriptor' 'P'
.id kP
where
.id k
is an optionally signed integer constant called the
.us scale factor.
.ix 'scale factor'
The value of the scale factor is zero at the beginning of each I/O
statement.
The scale factor applies to all subsequent
.id F,
.id E,
.id D
and
.id G
edit descriptors until another scale factor is encountered.
The scale factor affects editing in the following way.
.autopoint
.point
On input with
.id F,
.id E,
.id D
and
.id G
editing, provided that no exponent exists
in the field, the effect is that the
represented number equals the internally represented number multiplied
by
.mono 10**k.
.point
On input with
.id F,
.id E,
.id D
and
.id G
editing, the scale factor has no effect
if there is an exponent in the field.
.point
On
.id F
output editing, the effect is that the represented number equals
the internally represented number multiplied
by
.mono 10**k.
.point
On output with
.id E
and
.id D
editing, the simple real constant
(see the chapter entitled :HDREF refid='ftypes'.)
part of the data is multiplied by
.id 10**k
and the exponent is reduced by
.id k.
.point
On output with
.id G
editing, the scale factor has no effect unless the
magnitude of the datum is outside the range that allows
.id F
editing (see the section entitled :HDREF refid='gedit'.).
If
.id E
editing is required, the scale factor has the same effect as
with
.id E
output editing.
.endpoint
.*
.section BN and BZ Editing
.*
.np
The
.id BN
and
.id BZ
.ix 'BN edit descriptor'
.ix 'BZ edit descriptor'
.ix 'edit descriptor' 'BN'
.ix 'edit descriptor' 'BZ'
edit descriptors are used to describe the interpretation
of embedded blanks in numeric input fields.
They only effect
.id I,
.id F,
.id E,
.id D
and
.id G
editing and have no effect during output.
When a
.id BN
edit descriptor is encountered in a format specification,
embedded blanks in subsequent numeric input fields are ignored.
However, a field of all blanks has the value of zero.
If a
.id BZ
edit descriptor is encountered, then all embedded blanks
in subsequent numeric input fields are treated as zeroes.
At the beginning of each I/O statement, all blanks are treated as
zeroes or ignored depending on the value of the
.kw BLANK=
specifier
(see the
.kw OPEN
statement) currently in effect for the unit.
.*
.section $ or \ Editing (Extension)
.*
.np
.ix '$ edit descriptor'
.ix '\ edit descriptor'
.ix 'edit descriptor' '$'
.ix 'edit descriptor' '\'
.xt begin
The $ and \ edit descriptors behave identically.
The $ and \ edit descriptors are intended for output to an interactive
device such as a terminal.
They are a &product extensions.
The output record is displayed at the terminal leaving the cursor at
the end of the record; the carriage return at the end of the line is
suppressed.
Its use is intended for prompting for input so that the response can
be entered immediately following the prompt.
.xt end
.np
Depending on the type of terminal, the prompt may be returned as part
of the input.
An application must be aware of the way a particular terminal behaves.
The following example demonstrates this.
Note that the format specification in the
.kw FORMAT
statement labelled
.mono 20
ignores the first eleven characters of the
response since the prompt also appears in the response.
.exam begin
      INTEGER AGE
      WRITE(6,FMT=10)
10    FORMAT( 'Enter age: ',$ )
      READ(5,20) AGE
20    FORMAT( 11X, I2 )
      PRINT *,'Your age is ',AGE
      END
.exam end
.pc
If the terminal you are using does not return the prompt as part of the
response (that is, a read from the terminal only includes characters
typed at the terminal), the format specification in the
.kw FORMAT
statement labelled
.mono 20
must be changed, as in the following example, to achieve the same result.
.exam begin
      INTEGER AGE
      WRITE(6,FMT=10)
10    FORMAT( 'Enter age: ',\/ )
      READ(5,20) AGE
20    FORMAT( I2 )
      PRINT *,'Your age is ',AGE
      END
.exam end
.*
.section Numeric Editing: I, F, E, D and G Edit Descriptors
.*
.np
Numeric edit descriptors are used to specify I/O of integer, real,
double precision, complex and double precision complex data.
The following rules apply to all numeric edit descriptors.
.autopoint
.point
On input, leading blanks are not significant.
The interpretation of blanks other than leading blanks is determined
by any
.id BN
or
.id BZ
edit descriptors in effect and the
.kw BLANK=
specifier (see the
.kw OPEN
statement).
A field of all blanks is always zero.
Plus signs are optional.
.point
On input, with
.id F,
.id E,
.id D
and
.id G
editing, the decimal location specified
in the edit descriptor is overridden by a decimal point appearing
in the input field.
.point
On output, the plus sign is optional and is determined by the
.id S,
.id SP
and
.id SS
edit descriptors.
A negative quantity is represented by a negative sign.
A minus sign is never produced when outputting a value of zero.
.point
On output, the representation is always right justified in the field
with leading blanks inserted at the beginning of the field if the
number of characters in the representation is less than the field width.
.point
On output, if the number of characters in the external representation
is greater than the field width or an exponent exceeds its specified
length using
.id Ew.dEe,
.id Gw.dEe,
.id Ew.dDe
or
.id Gw.dDe
edit descriptors, the entire field
is filled with asterisks.
.endpoint
.*
.beglevel
.*
.section Integer Editing: Iw and Iw.m Edit Descriptors
.*
.np
The
.id Iw
and
.id Iw.m
.ix 'I edit descriptor'
.ix 'edit descriptor' 'numeric' 'I'
edit descriptors indicate that the field width
of the field to be edited is
.id w.
The item in the I/O list must be of type integer; on input the
I/O list item will be defined by integer data, on output the I/O
list item must be defined with an integer datum.
.np
On input, the
.id Iw.m
edit descriptor is treated identically to the
.id Iw
edit descriptor.
The output field for the
.id Iw
edit descriptor consists of zero or
more leading blanks followed by a minus sign if the value of the
I/O list item is negative or an optional plus sign otherwise,
followed by the magnitude of the integer datum with no leading zeroes.
Note that the integer constant contains at least one digit.
On output, the
.id Iw.m
edit descriptor specifies that at least
.id m
digits are to be displayed with leading zeroes if necessary.
The value of
.id m
must be less than or equal to the value of
.id w.
If
.id m
is zero and the value of the datum is zero, then the output
field is filled with blanks.
.exam begin
      PRINT '(1H<,I4.4,I5,1H>)',23,2345
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string:
.millust begin
<0023 2345>
.millust end
.*
.section Floating-point Editing: F, E, D and G Edit Descriptors
.*
.np
The
.id F,
.id E,
.id D
and
.id G
edit descriptors describe the editing of real,
double precision, complex and double precision complex data.
The I/O list item corresponding to one of these edit descriptors
must be of type real, double precision, complex or double precision
complex.
On input, the I/O list item will become defined with a datum whose
type is the same as that of the I/O list item.
On output, the I/O list item must be defined with a datum whose
type is that of the I/O list item.
.*
.section F Editing
.*
.np
An
.id F
edit descriptor has the form
.ix 'F edit descriptor'
.ix 'edit descriptor' 'numeric' 'F'
.id Fw.d
where
.id w
is the field width and
.id d
is the number of digits in the fractional part.
The input field consists of an optional sign, followed by a string
of digits optionally containing a decimal point.
If the decimal point is omitted, the rightmost
.id d
digits with leading zeroes assumed if necessary, are interpreted as
the fractional part of the value represented.
An exponent of one of the following forms may follow.
.autopoint
.point
A signed integer constant.
.point
An
.id E
or
.id D
followed by an optionally signed integer constant.
.endpoint
.np
Consider the following example, where the decimal point is omitted.
The formula used in the evaluation is:
.millust begin
                       -d      (exponent subfield)
(integer subfield) x 10    x 10
.millust end
.pc
If the specification is F10.8 and the input quantity is 31415E+5
then the following conversion takes place.
.millust begin
                   -8      5
      00031415 x 10    x 10

                   5
=   .00031415 x 10

=   31.415
.millust end
.pc
In other words, the decimal point is assumed to lie to the left of the
8 digits (padded with zeroes on the left) forming the fractional part
of the input value.
.np
The output field produced by an
.id F
edit descriptor consists of blanks
if necessary followed by a minus sign if the item in the I/O list
is negative or an optional plus sign otherwise, followed by
a string of digits containing a decimal point which represents the
magnitude of the I/O list item.
The string representing the magnitude of the I/O list item is
modified according to the scale factor and is rounded to
.id d
fractional digits.
An optional leading zero is produced only if the magnitude of the I/O
list item is less than one.
Note that a leading zero is required if there would otherwise be no
digits in the output field.
.exam begin
      PRINT '(1H<,F8.4,1H>)', 234.43
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string:
.millust begin
<234.4300>
.millust end
.*
.section E and D Editing
.*
.np
The
.id Ew.d,
.id Dw.d
and
.id Ew.dEe
edit descriptors indicate that the field
.ix 'E edit descriptor'
.ix 'edit descriptor' 'numeric' 'E'
.ix 'D edit descriptor'
.ix 'edit descriptor' 'numeric' 'D'
width is
.id w,
the fractional part contains
.id d
digits unless a scale
factor greater than one is in effect, and the exponent consists of
.id e
digits.
The
.id e
has no effect on input.
.np
The form of the input field and processing of it is the same as that
for
.id F
editing.
The form of the output field is:
.mbox begin
      [+] [0] . x  x  ... x  exp
      [-]        1  2      d
.mbox end
.synote
.mnote ~p
indicates a plus or minus sign.
.mnote x's
are the
.id d
most significant digits of the value after rounding.
.mnote exp
is a decimal exponent.
.endnote
.np
The form of the exponent is as follows.
.autopoint
.point
When using the
.id Ew.d
edit descriptor, the form of the exponent is
.millust begin
E+nn
 or       if |exp| <= 99
E-nn
.millust end
.pc
and
.millust begin
+nnn
 or       if 99 < |exp| <= 999
-nnn
.millust end
.pc
.point
When using the
.id Ew.dEe
edit descriptor, the form of the exponent is
.millust begin
E+n ... n   where |exp| <= (10**e)-1
 - 1     e
.millust end
.point
When using the
.id Dw.d
edit descriptor, the form of the exponent is
.millust begin
D+nn
 or       if |exp| <= 99
D-nn
.millust end
.pc
and
.millust begin
+nnn
 or       if 99 < |exp| <= 999
-nnn
.millust end
.endpoint
.np
Note that a sign in the exponent is always present.
If the exponent is 0, a plus sign is used.
The forms
.id Ew.d
and
.id Dw.d
are not to be used if
.mono |exp| > 999.
.np
.exam begin
      PRINT '(1H<,E10.4,1H>,1H<,E9.4,1H>,1H<,E12.4E3,1H>)',
     $      .5, .5, .5
      END
.exam end
.pc
The output from the
.kw PRINT
statement in the previous example is the string:
.millust begin
<0.5000E+00><.5000E+00>< 0.5000E+000>
.millust end
.np
The scale factor
.id k
in a
.id P
edit descriptor controls decimal normalization
as follows:
.autopoint
.point
If
.mono -d < k <= 0,
then the output field contains
.id |k|
leading zeroes and
.id d-|k|
significant digits after the decimal point.
.point
If
.mono 0 < k < d+2,
the output field contains exactly
.id k
significant digits to the left of the decimal point and
.id d-k+1
significant digits to the right of the decimal point.
.point
Other values of
.id k
are not permitted.
.endpoint
.np
.xt begin
The
.id Ew.dDe
edit descriptor behaves in the same way as the
.id Ew.dEe
edit descriptor on input; on output the only difference is that the
letter
.id D
is used to mark the exponent instead of the letter
.id E.
.xt end
.*
.section *refid=gedit G Editing
.*
.np
The
.id Gw.d
and
.id Gw.dEe
edit descriptors indicate that the field
.ix 'D edit descriptor'
.ix 'edit descriptor' 'numeric' 'D'
width is
.id w,
the fractional part contains
.id d
digits unless a scale
factor greater than one is in effect, and the exponent consists of
.id e
digits.
.np
.id G
input editing is the same as
.id F
input editing.
.np
The representation on
.id G
output editing depends on the magnitude of the
datum being edited.
Let
.id M
be the magnitude of the datum being edited.
Then
.id G
output editing behaves as follows.
.autopoint
.point
If
.mono M < 0.1
or
.mono M >= 10**d,
.id Gw.d
output editing is equivalent to
.id kPEw.d
output editing and
.id Gw.dEe
output editing is equivalent to
.id kPEw.dEe
output editing where
.id k
is the scaling factor currently in effect.
.point
If
.mono 0.1 <= M < 10**d,
the scale factor has no effect and the value of
.id M
determines the editing as shown in the following table.
.endpoint
.if &e'&dohelp eq 0 .do begin
.cp 15
.* .box on 1 19 45
.sr c0=&INDlvl+1
.sr c1=&INDlvl+19
.sr c2=&INDlvl+45
.box on &c0 &c1 &c2
\Magnitude of Datum    \Equivalent Edit Descriptor\
.box
\0.1<=M<1              \F<w-n>.d followed by n blanks\
\1<=M<10               \F<w-n>.<d-1> followed by n blanks\
\    .                 \           .\
\    .                 \           .\
\    .                 \           .\
\10**(d-2)<=M<10**(d-1)\F<w-n>.1 followed by n blanks\
\10**(d-1)<=M<10**d    \F<w-n>.0 followed by n blanks\
.box off
.do end
.el .do begin
.millust begin
+-----------------------+-----------------------------------+
| Magnitude of Datum    | Equivalent Edit Descriptor        |
+-----------------------+-----------------------------------+
| 0.1<=M<1              | F<w-n>.d followed by n blanks     |
| 1<=M<10               | F<w-n>.<d-1> followed by n blanks |
|     .                 |            .                      |
|     .                 |            .                      |
|     .                 |            .                      |
| 10**(d-2)<=M<10**(d-1)| F<w-n>.1 followed by n blanks     |
| 10**(d-1)<=M<10**d    | F<w-n>.0 followed by n blanks     |
+-----------------------+-----------------------------------+
.millust end
.do end
.synote 8
.mnote <w-n>
stands for the integer represented by evaluating
.id w-n.
.mnote <d-1>
stands for the integer represented by evaluating
.id d-1.
.mnote n
is 4 for
.id Gw.d
editing and
.id e+2
for
.id Gw.dEe
editing.
.endnote
.exam begin
      PRINT '(1H<,G12.6,1H>,1H<,G12.4E4,1H>)', .5, .5
      END
.exam end
.pc
The output from the
.kw PRINT
statement in the previous example is the string:
.millust begin
<0.500000    ><0.5000      >
.millust end
.*
.section Complex Editing
.*
.np
Since a complex datum consists of a pair of real or double precision
data, the editing
.ix 'complex edit descriptor'
.ix 'edit descriptor' 'numeric' 'complex'
of a complex datum is specified by two successive pairs of
.id F,
.id E,
.id D
or
.id G
edit descriptors.
The two descriptors may be different and may be separated by any number
of non-repeatable edit descriptors.
.xt on
Double precision complex editing is identical to complex editing.
.xt off
.endlevel
.*
.section L Edit Descriptor
.*
.np
The
.id Lw
.ix 'L edit descriptor'
.ix 'edit descriptor' 'L'
edit descriptor is used for I/O list items of type logical.
The field width is
.id w.
.np
On input the I/O list item will become defined with a datum of
type logical.
The input field consists of optional blanks, followed by an optional
decimal point followed by a
.mono T
or
.mono F
for true or false respectively.
The
.mono T
and
.mono F
may be followed by additional characters in the field.
.xt on
&product allows
.mono t
and
.mono f
in addition to
.mono T
and
.mono F
on input.
.xt off
.np
On output, the I/O list item must be defined with a datum of type
logical.
The output field consists of
.id w-1
blanks followed by a
.mono T
for true or
.mono F
for false.
.exam begin
      PRINT '(1H<,L3,L5,1H>)',.TRUE.,.FALSE.
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string:
.millust begin
<  T    F>
.millust end
.*
.section A Edit Descriptor
.*
.np
The
.id A[w]
edit descriptor is used for I/O list items of type character.
.ix 'A edit descriptor'
.ix 'edit descriptor' 'A'
On input, the I/O list item becomes defined with character data.
On output, the I/O list item must be defined with character data.
If
.id w
is specified in the edit descriptor, the field width is
.id w;
otherwise the field width is the number of characters in the I/O
list item.
.keep
.np
.xt begin
&product also permits I/O list items of non-character data types.
On input, the I/O list item becomes defined with the binary
representation of the character data.
On output, the I/O list item is assumed to be defined with character
data.
.xt end
.np
If
.id len
is the length of the I/O list item and
.id w
is specified in
.id A
input editing so that
.id w
is greater than
.id len,
the rightmost
.id len
characters of the input field will be taken.
If
.id w
is less than
.id len,
then the
.id w
characters in the input field will be taken and padded with
.id len-w
blanks.
.np
If
.id w
is specified in
.id A
output editing so that
.id w
is greater than
.id len,
then the output field will consist of
.id w-len
blanks followed by the
.id len
characters of the I/O list item.
If
.id w
is less than or equal to
.id len,
the output field will consist of the first
.id w
characters of the I/O list item.
.exam begin
      PRINT '(1H<,A5,A8,1H>)','ABCDEFG','123'
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string:
.millust begin
<ABCDE     123>
.millust end
.*
.section Z Editing (Extension)
.*
.np
.xt begin
The
.id Zw
edit descriptor is used to display the hexadecimal representation of
.ix 'Z edit descriptor'
.ix 'edit descriptor' 'Z'
data or read hexadecimal data.
It is a &product extension.
The
.id Zw
edit descriptor can be used for I/O list items of any type.
The field width is
.id w.
.xt end
.np
On output,
.id w
must be greater than or equal to twice the size (in bytes) of the
I/O list item since
each byte is represented by two hexadecimal digits.
For example, real data requires four bytes.
Hence,
.id w
must be at least eight.
.exam begin
      PRINT '(1H<,Z8,1H>)', 256
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string:
.millust begin
<00000100>
.millust end
.np
If
.id w
is greater then the number of hexadecimal digits required to represent
the data, the leftmost print positions of the output field are filled
with blanks.
.exam begin
      PRINT '(1H<,Z10,1H>)','ABCD'
.exam end
.pc
The output produced by the
.kw PRINT
statement in the previous example is the string
.millust begin
<  C1C2C3C4>
.millust end
.pc
if the EBCDIC character set is being used or
.millust begin
<  41424344>
.millust end
.pc
if the ASCII character set is being used.
.np
On input, if
.id w
is greater than twice the size (in bytes) of the I/O list item,
the leftmost characters are truncated from the input field.
For example, if the input field contains the string
.millust begin
91A2C3D4
.millust end
.pc
and is read into a character variable whose length is two,
the character would contain the hexadecimal data
.mono C3D4.
If
.id w
is less than twice the size (in bytes) of the I/O item,
the I/O item is padded to the left with hexadecimal zeroes.
For example, if the input field contains the string
.millust begin
81C1
.millust end
.pc
and is read into a character variable whose length is four,
the character would contain the hexadecimal data
.mono 000081C1.
.endlevel
.*
.section Format-Directed Input/Output
.*
.np
.ix 'format-directed I/O'
.us Format-directed input/output
(I/O) is formatted input or output controlled by a format
specification.
The action taken during formatted input or output depends on the
next edit descriptor in the format specification and the next
item in the input/output list if one exists.
.np
A format specification is processed from left to right.
An edit descriptor or a format specification with a repeat specification
of
.id r
is processed as a list of
.id r
edit descriptors or format specifications.
A repeat specification of one is equivalent to no repeat specification.
.np
For each repeatable edit descriptor in the format specification, there
corresponds one item in the I/O list except an I/O list item of type
complex where two repeatable floating-point edit descriptors are
required.
Non-repeatable edit descriptors do not correspond to any I/O list
item; they communicate information directly with the record.
Whenever a repeatable edit descriptor is encountered in a format
specification, there must be a corresponding item in the I/O list.
The edited information is transmitted appropriately between
the item and the record.
.np
Format processing is terminated when any of the following conditions
occur.
.autopoint
.point
When an edit descriptor has no
corresponding item in the I/O list.
.point
When a colon edit descriptor is encountered and there are no more
items in the I/O list.
.point
When the right parenthesis is encountered and there are no more
items in the I/O list.
.endpoint
.np
If the right parenthesis of the complete format specification is
encountered and the I/O list has not been
exhausted, the file is positioned at the next record and
format processing resumes at the start of the format
specification terminated by the last preceding right parenthesis.
If there is no such right parenthesis, format processing resumes
at the start of the complete format specification.
The part of the format specification that is reused must contain
at least one repeatable edit descriptor.
If format processing resumes at a left parenthesis preceded by a repeat
specification, the repeat specification is also reused.
The scale factor, sign control edit descriptors and blank control
edit descriptors are not affected when part of a format specification
is reused.
.*
.section List-Directed Formatting
.*
.np
.ix 'list-directed formatting'
.us List-directed formatting
is input/output without a format specification.
.exam begin
      READ( un, * ) X, Y, Z
      READ( UNIT=un, FMT=* ) X, Y, Z
      READ *, X, Y, Z
      WRITE( un, * ) X, Y, Z
      WRITE( UNIT=un, FMT=* ) X, Y, Z
      PRINT *, X, Y, Z
.exam end
.pc
In the previous example, an asterisk instead of a format specification
indicates list-directed formatting.
.keep
.np
.xt begin
Omitting the asterisk and format specification also indicates
list-directed formatting.
.exam begin
      READ, X, Y, Z
      PRINT, X, Y, Z
.exam end
.xt end
.np
Records used during list-directed formatting are called list-directed
records.
A list-directed record is a sequence of values and value separators.
Any sequence of blanks is treated as a single blank except when
it appears in a character constant.
The end of a record has the same effect as a blank character.
.np
A
.us value
is one of the following:
.autopoint
.point
A constant.
.point
A null value.
.point
.id r*c
.point
.id r*
.endpoint
.synote
.mnote r
is an unsigned, nonzero integer constant.
.mnote c
is a constant.
.endnote
.pc
The
.id r*c
form is equivalent to
.id r
successive occurrences of
.id c.
The
.id r*
form is equivalent to
.id r
successive occurrences of the null value.
In these two forms, blanks are permitted only where they are allowed
in the constant
.id c.
.np
A
.us value separator
is one of the following:
.autopoint
.point
A comma preceded and followed by any number of blanks.
.point
A slash preceded and followed by any number of blanks.
A slash as a value separator terminates the execution of the input
statement.
The definition status of the remaining input items in the input list
remains the same as it was prior to the input statement.
.point
Any number of blanks between two values.
.endpoint
.np
A null value is specified by having no character between successive value
separators, no characters preceding the first value separator in a
record or the
.id r*
form.
It has no effect on the current value of the input item.
Note that the end of record following a value separator does not
specify a null value.
.*
.beglevel
.*
.section List-Directed Input
.*
.np
The input forms acceptable to format specifications for a given type
are also acceptable for list-directed formatting with certain exceptions.
.autonote
.note
Blanks are never used as zeroes and blanks embedded in constants are not
allowed except in character constants.
.note
An input item of type real or double precision must have an input
field suitable for F editing except that no fractional digits are assumed
unless a decimal point is present in the field.
Such a field will be called a
.us numeric input field.
.note
An input item of type complex or double precision complex
must consist of a left parenthesis followed
by two numeric input fields separated by a comma and followed by a
right parenthesis.
The numeric input fields may be preceded or followed by blanks.
The end of record can only appear between the first numeric field and the
comma or between the comma and the second numeric field.
Note that a null value must not be used as the real or imaginary part
but may represent the entire complex constant.
.note
An input item of type logical must not include either a slash or a comma
among the optional characters allowed in L editing.
.note
An input item of type character consists of a non-empty string of
characters enclosed in apostrophes.
Apostrophes in character constants are represented by two consecutive
apostrophes without a blank or end of record separating them.
Character constants may span records.
If this is the case, the end of record does cause a blanks to be inserted
into the character constant.
Note that a comma or slash in a character constant is not a value
separator.
A character input field is assigned to a character input item
as though it were a character assignment.
.endnote
.*
.section List-Directed Output
.*
.np
The form of the output field produced by list-directed output is
similar to the form required by list-directed input.
The output of a character constant does not include the enclosing
quotes and an apostrophe in a character constant is output as a
single apostrophe.
The values are separated by one or more blanks.
When printed, each record will start with a blank if the file is
a carriage-control oriented file.
For example, the source listing file produced by &product is
such a file.
.endlevel
.*
.section Namelist-Directed Formatting (Extension)
.*
.np
.xt begin
The
.kw READ
.ct ,
.kw WRITE
.ct , and
.kw PRINT
statements may be used to transmit data between a file and the
variables specified in a
.kw NAMELIST
statement.
.xt end
.keep 20
.exam begin
      CHARACTER*20 NAME
      CHARACTER*20 STREET
      CHARACTER*15 CITY
      CHARACTER*20 STATE
      CHARACTER*20 COUNTRY
      CHARACTER*10 ZIP_CODE
      INTEGER AGE
      INTEGER MARKS(10)
      NAMELIST /nl/ NAME, STREET, CITY, STATE,
     +              COUNTRY, ZIP_CODE, AGE, MARKS
          .
          .
          .
      READ( un, nl )
      READ( UNIT=un, FMT=nl )
      READ nl
      WRITE( un, nl )
      WRITE( UNIT=un, FMT=nl )
      PRINT nl
.exam end
.*
.beglevel
.*
.section Namelist-Directed Input (Extension)
.*
.np
The input data must be in a special format.
The first character in each record must be blank.
The second character in the first record of a group of data records
must be an ampersand (&amp) or dollar sign ($) immediately followed
by the
.kw NAMELIST
name.
The
.kw NAMELIST
name must be followed by a blank and must not contain any imbedded
blanks.
This name is followed by data items, optionally separated by commas.
The end of a data group is signaled by the character "&amp" or "$",
optionally followed by the string "END".
If the "&amp" character was used to start the group, then it must be
used to end the group.
If the "$" character was used to start the group, then it must be
used to end the group.
.millust begin
12345678901234567890...
 &NL
 item1, item2, item3,
 item4, item5, ...
 &END
.millust end
.np
The form of the data items in an input record is:
.begnote $break
.note Name = Constant
The name may be a variable name, an array element name, or a character
substring name.
The constant may be integer, real, complex, logical or character.
Logical constants may be in the form "T" or ".TRUE" and "F" or
".FALSE".
Character constants must be contained within apostrophes.
Subscripts and substring indices must be of integer type.
.note ArrayName = Set of Constants
The set of constants consists of constants of the type integer, real,
complex, logical or character.
The constants are separated by commas.
The number of constants must be less than or equal to the number of
elements in the array.
Successive occurrences of the same constant may be represented in the
form
.id r*constant,
where
.id r
is a non-zero integer constant specifying the number of times the
constant is to occur.
Consecutive commas within a list indicate that the values of the
array elements remain unchanged.
.endnote
.np
The variable and array names specified in the input file must appear
in the
.kw NAMELIST
list, but the order is not important.
A name that has been made equivalent to a name in the input data cannot
be substituted for that name in the
.kw NAMELIST
list.
The list can contain names of items in
.kw COMMON
but must not contain dummy argument names.
.np
Each data record must begin with a blank followed by a complete
variable or array name or constant.
Embedded blanks are not permitted in names or constants.
Trailing blanks after integers and exponents are treated as zeros.
.exam begin
 &amp.PERSON
    NAME = 'John Doe'
    STREET = '22 Main St.' CITY = 'Smallville'
    STATE = 'Texas'        COUNTRY = 'U.S.A.'
    ZIP_CODE = '78910-1203'
    MARKS = 73, 82, 3*78, 89, 2*93, 91, 88
    AGE = 23
 &amp.END
.exam end
.np
The input forms acceptable to format specifications for a given type
are also acceptable for namelist-directed formatting with certain
exceptions.
.autonote
.note
Blanks are never used as zeroes and blanks embedded in constants are
not allowed except in character constants.
.note
An input item of type real or double precision must have an input
field suitable for F editing except that no fractional digits are
assumed unless a decimal point is present in the field.
Such a field will be called a
.us numeric input field.
.note
An input item of type complex or double precision complex must consist
of a left parenthesis followed by two numeric input fields separated
by a comma and followed by a right parenthesis.
The numeric input fields may be preceded or followed by blanks.
The end of record can only appear between the first numeric field and
the comma or between the comma and the second numeric field.
Note that a null value must not be used as the real or imaginary part
but may represent the entire complex constant.
.note
An input item of type logical must not include either a slash or a comma
among the optional characters allowed in L editing.
.note
An input item of type character consists of a non-empty string of
characters enclosed in apostrophes.
Apostrophes in character constants are represented by two consecutive
apostrophes without a blank or end of record separating them.
Character constants may span records.
If this is the case, the end of record does cause a blank to be inserted
into the character constant.
Note that a comma or slash in a character constant is not a value
separator.
A character input field is assigned to a character input item
as though it were a character assignment.
.endnote
.*
.section Namelist-Directed Output
.*
.np
The form of the data items in an output record is suitable for input
using a namelist-directed
.kw READ
statement.
.autonote
.note
Output records are written using the ampersand character (&amp), not
the dollar sign ($), although the dollar sign is accepted as an
alternative during input.
That is, the output data is preceded by "&amp.name" and is followed by
"&amp.END".
.note
All variable and array names specified in the
.kw NAMELIST
list and their values are written out, each according to
its type.
.note
Character data is enclosed in apostrophes.
.note
The fields for the data are made large enough to contain all
the significant digits.
.note
The values of a complete array are written out in columns.
.endnote
.endlevel
