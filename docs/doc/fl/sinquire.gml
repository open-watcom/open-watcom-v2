.section INQUIRE Statement
.*
.np
The
.kw INQUIRE
statement is used to ask about certain properties of a named file
or its connection to a particular unit.
There are two forms of the
.kw INQUIRE
statement; inquire by file name and inquire by unit.
.beglevel
.*
.section INQUIRE by FILE
.*
.mbox begin
      INQUIRE (iflist)
.mbox end
.synote 8
.mnote iflist
includes the
.kw FILE=
specifier and may include at most one
of each of the inquiry specifiers listed below.
Specifiers are separated by commas.
The
.kw FILE=
specifier has the form
.millust FILE = fin
where
.id fin
is a character expression whose value when trailing blanks are
removed is the name of a file being inquired about.
The file need not exist or be connected to a unit.
.endnote
.exam begin
      LOGICAL EX, OD
      INTEGER NUM
      INQUIRE( FILE='ROLL',EXIST=EX,OPENED=OD,NUMBER=NUM )
.exam end
.pc
In the above example, information is requested on the file
.mono PAYROLL.
In particular, we want to know if it exists, whether it is connected
to a unit, and what the unit number is (if it is indeed connected).
.*
.se tmplvl=&WDWlvl+3
.keep &tmplvl
.*
.section INQUIRE by UNIT
.*
.mbox begin
      INQUIRE (iulist)
.mbox end
.synote 8
.mnote iulist
includes the
.kw UNIT=
specifier and may include at most one
of each of the inquiry specifiers listed below.
Specifiers are separated by commas.
The
.kw UNIT=
specified has the form
.millust [UNIT =] u
where
.id u
is an
.us external unit identifier.
An external unit identifier is a non-negative integer
expression.
If the optional
.kw UNIT=
specifier is omitted then the
specifier must be the first item in the list of specifiers.
.endnote
.exam begin
      LOGICAL EX, OD
      CHARACTER*30 FN
      INQUIRE( UNIT=7, EXIST=EX, OPENED=OD, NAME=FN )
.exam end
.pc
In the above example, information is requested on unit 7.
In particular, we want to know if the unit exists, whether it is
connected to a file, and, if so, what the file name is.
.*
.section Inquiry Specifiers
.*
.np
The following inquiry specifiers are supported.
.millust begin
    IOSTAT = ios
    ERR = s
    EXIST = ex
    OPENED = od
    NUMBER = num
    NAMED = nmd
    NAME = fn
    ACCESS = acc
    SEQUENTIAL = seq
    DIRECT = dir
    FORM = fm
    FORMATTED = fmt
    UNFORMATTED = unf
    RECL = rcl
    NEXTREC = nr
    BLANK = blnk
.millust end
.np
.xt begin
As an extension to the FORTRAN 77 language, the following inquiry
specifiers are also supported.
.millust begin
    ACTION = act
    CARRIAGECONTROL = cc
    RECORDTYPE = rct
    BLOCKSIZE = bl
.millust end
.xt end
.begnote $break
.mnote IOSTAT = ios
is an
.us input/output status specifier.
The integer
variable or integer array element
.id ios
is defined with zero if no error condition exists or a
positive integer value if an error condition exists.
.mnote ERR = s
is an
.us error specifier
and
.id s
is a statement label.
When an error occurs, execution is transferred to the statement
labelled by
.id s.
.mnote EXIST = ex
.id ex
is a logical variable or logical array element.
.np
Inquire by file: The value .TRUE. is assigned if a file exists
with the specified name; otherwise the value .FALSE. is assigned.
.np
Inquire by unit: The value .TRUE. is assigned if the specified
unit exists; otherwise the value .FALSE. is assigned.
.mnote OPENED = od
.id od
is a logical variable or logical array element.
.np
Inquire by file: The value .TRUE. is assigned if the specified
file is connected to a unit; otherwise the value .FALSE. is assigned.
.np
Inquire by unit: The value .TRUE. is assigned if the specified
unit is connected to a file; otherwise the value .FALSE. is assigned.
.mnote NUMBER = num
.id num
is an integer variable or integer array element that is assigned the
value of the unit number to which the file is connected.
If no unit is connected to the file then
.id num
becomes undefined.
.mnote NAMED = nmd
.id nmd
is a logical variable or logical array element name that is assigned
the value .TRUE. if the file has a name; otherwise the value .FALSE.
is assigned.
.mnote NAME = fn
.id fn
is a character variable or character array element.
.xt on
&product also permits
.id fn
to be a character substring.
.xt off
.np
It is assigned the name of the file, if the file has a name;
otherwise it becomes undefined.
The file name that is returned need not be the same as that given
in a
.kw FILE=
specifier but it must be suitable for use in the
.kw FILE=
specification of an
.kw OPEN
statement (e.g., the file name returned may have additional system
qualifications attached to it).
.mnote ACCESS = acc
.id acc
is a character variable or character array element.
.xt on
&product also permits
.id acc
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'SEQUENTIAL'
if the file is connected for sequential access.
It is assigned the value
.mono 'DIRECT'
if the file is connected for direct access.
It is assigned an undefined value if there is no connection.
.mnote SEQUENTIAL = seq
.id seq
is a character variable or character array element.
.xt on
&product also permits
.id seq
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'YES'
if
.kw SEQUENTIAL
is included in the set of
allowed access methods for the file, the value
.mono 'NO'
if
.kw SEQUENTIAL
is not included in the set of allowed access methods for the
file, or
.mono 'UNKNOWN'
if &product is unable to determine
whether or not
.kw SEQUENTIAL
is included in the set of allowed
access methods for the file.
.mnote DIRECT = dir
.id dir
is a character variable or character array element.
.xt on
&product also permits
.id dir
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'YES'
if
.kw DIRECT
is included in the set of allowed access methods for the file,
the value
.mono 'NO'
if
.kw DIRECT
is not included in the set of allowed access methods for the file,
or
.mono 'UNKNOWN'
if &product is unable to determine whether or
not
.kw DIRECT
is included in the set of allowed access methods for the file.
.mnote FORM = fm
.id fm
is a character variable or character array element.
.xt on
&product also permits
.id fm
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'FORMATTED'
if the file is connected for
formatted input/output, the value
.mono 'UNFORMATTED'
if the file is
connected for unformatted input/output, or an undefined value if
there is no connection.
.mnote FORMATTED = fmt
.id fmt
is a character variable or character array element.
.xt on
&product also permits
.id fmt
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'YES'
if
.kw FORMATTED
is included in the set of
allowed forms for the file, the value
.mono 'NO'
if
.kw FORMATTED
is not included in the set of allowed forms for the file, or
.mono 'UNKNOWN'
if &product is unable to determine whether or not
.kw FORMATTED
is included in the set of allowed forms for the file.
.mnote UNFORMATTED = unf
.id unf
is a character variable or character array element.
.xt on
&product also permits
.id unf
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'YES'
if
.kw UNFORMATTED
is included in the set of
allowed forms for the file, the value
.mono 'NO'
if
.kw UNFORMATTED
is not included in the set of allowed forms for the file, or
.mono 'UNKNOWN'
if &product is unable to determine whether or not
.kw UNFORMATTED
is included in the set of allowed forms for the file.
.mnote RECL = rcl
.id rcl
is an integer variable or integer array element that is assigned
the value of the record length of the file connected for direct
access.
If the file is connected for formatted input/output, the length is
the number of characters.
If the file is connected for unformatted input/output, the length is
measured in processor-dependent units (bytes).
See the compiler User's Guide for a discussion of record length or
size.
If there is no connection or if the file is not connected for direct
access then the value is undefined.
.xt on
The
.kw RECL=
specifier is also allowed if the file is connected for sequential access.
.xt off
.mnote NEXTREC = nr
.id nr
is an integer variable or integer array element that is
assigned the value
.id n+1,
where
.id n
is the record number of the last record read or written on the
file connected for direct access.
If the file is connected but no records have been read or written
then the value is 1.
If the file is not connected for direct access or if the position of
the file can not be determined because of an input/output error then
.id nr
becomes undefined.
.mnote BLANK = blnk
.id blnk
is a character variable or character array element.
.xt on
&product also permits
.id blnk
to be a character substring.
.xt off
.np
It is assigned the value
.mono 'NULL'
if null blank control is in effect for the file connected for
formatted input/output, and is assigned the value
.mono 'ZERO'
if zero blank control is in effect for the file connected for
formatted input/output.
If there is no connection, or if the file is not connected for
formatted input/output,
.id blnk
becomes undefined.
.cp 7
.mnote ACTION = act
.xt on
.id act
is a character variable or character array element.
&product also permits
.id act
to be a character substring.
.np
It is assigned the value
.mono 'READ'
if data can only be read from the file,
.mono 'WRITE'
if data can only be written from the file, and
.mono 'READWRITE'
if data can be both read and written.
.xt off
.mnote CARRIAGECONTROL = cc
.xt on
.id cc
is a character variable or character array element.
&product also permits
.id cc
to be a character substring.
.np
It is assigned the value
.mono 'YES'
if the first character of each record is interpreted as a carriage
control character and
.mono 'NO'
if no interpretation is placed on the first character of each record.
.xt off
.keep 11
.mnote RECORDTYPE = rct
.xt on
.id rct
is a character variable or character array element.
&product also permits
.id rct
to be a character substring.
.np
It is assigned a value that represents the record type
(or record structure) that is used for the file.
The value assigned depends on the system on which you are running the
program.
See the compiler User's Guide for a discussion of record types.
.xt off
.mnote BLOCKSIZE = bl
.xt on
.id bl
is an integer variable or integer array element.
.np
It is assigned a value that represents the internal buffer size that
is used for input/output operations on the file.
The value assigned depends on the system on which you are running the
program.
See the compiler User's Guide for a discussion of default internal
buffer size.
.xt off
.endnote
.*
.section Definition Status of Specifiers - Inquire by File
.*
.np
The following table summarizes which specifier variables or array
elements become defined with values under what conditions when
using the
.kw FILE=
specifier.
.millust begin
    IOSTAT = ios          (1)
    EXIST = ex            (2)
    OPENED = od           (2)
    NUMBER = num          (4)
    NAMED = nmd           (3)
    NAME = fn             (3)
    ACCESS = acc          (5)
    SEQUENTIAL = seq      (3)
    DIRECT = dir          (3)
    FORM = fm             (5)
    FORMATTED = fmt       (3)
    UNFORMATTED = unf     (3)
    RECL = rcl            (5)
    NEXTREC = nr          (5)
    BLANK = blnk          (5)
.xtnada
.xt begin
    ACTION = act          (5)
    CARRIAGECONTROL = cc  (5)
    RECORDTYPE = rct      (5)
    BLOCKSIZE = bl        (5)
.xt end
.millust end
.autonote
.note
The
.kw IOSTAT=
specifier variable is always defined with the most
recent error status.
If an error occurs during execution of the
.kw INQUIRE
statement then the error status is defined with a positive integer;
otherwise the status is that of the most recent input/output statement
which referenced that file.
.note
The specifier always becomes defined unless an error condition occurs.
.note
This specifier becomes defined with a value only if the file name
specified in the
.kw FILE=
specifier is an acceptable file name and the named file exists.
Also, no error condition can occur during the execution of the
.kw INQUIRE
statement.
.note
This specifier becomes defined with a value if and only if
.id od
becomes defined with the value .TRUE..
Also, no error condition can occur during the execution of the
.kw INQUIRE
statement.
.note
This specifier
.us may
become defined with a value only if
.id od
becomes defined with the value .TRUE..
However, there may be other conditions under which this specifier
does not become defined with a value.
In other words, (5) is a necessary, but not sufficient condition.
For example,
.id blnk
is undefined if the file is not connected for formatted input/output.
.endnote
.*
.section Definition Status of Specifiers - Inquire by Unit
.*
.np
The following table summarizes which specifier variables or array
elements become defined with values under what conditions when
using the
.kw UNIT=
specifier.
.millust begin
    IOSTAT = ios          (1)
    EXIST = ex            (2)
    OPENED = od           (2)
    NUMBER = num          (3)
    NAMED = nmd           (3)
    NAME = fn             (3)
    ACCESS = acc          (3)
    SEQUENTIAL = seq      (3)
    DIRECT = dir          (3)
    FORM = fm             (3)
    FORMATTED = fmt       (3)
    UNFORMATTED = unf     (3)
    RECL = rcl            (3)
    NEXTREC = nr          (3)
    BLANK = blnk          (3)
.xtnada
.xt begin
    ACTION = act          (3)
    CARRIAGECONTROL = cc  (3)
    RECORDTYPE = rct      (3)
    BLOCKSIZE = bl        (3)
.xt end
.millust end
.autonote
.note
The
.kw IOSTAT=
specifier variable is always defined with the most recent error
status.
If an error occurs during execution of the
.kw INQUIRE
statement then the error status is defined with a positive integer;
otherwise the status is that of the most recent input/output statement
which referenced that unit.
.note
This specifier always becomes defined unless an error condition occurs.
.note
This specifier becomes defined with a value only if the specified
unit exists and if a file is connected to the unit.
Also, no error condition can occur during the execution of the
.kw INQUIRE
statement.
.endnote
.endlevel
.np
For more information on input/output,
see the chapter entitled :HDREF refid='fio'..
