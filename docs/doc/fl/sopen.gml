.section OPEN Statement
.*
.mbox begin
      OPEN (oplist)
.mbox end
.synote 10
.mnote oplist
must include the
.kw UNIT=
specifier and may include at most one
of each of the open specifiers listed below.
Specifiers are separated by commas.
.millust begin
    [UNIT =] u
    IOSTAT = ios
    ERR = s
    FILE = fin
    STATUS = sta
    ACCESS = acc
    FORM = fm
    RECL = rcl
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
    SHARE = shr
.millust end
.xt end
.endnote
.np
The
.kw OPEN
statement may be used to connect an existing file to a unit,
create a file that is preconnected, create a file and connect
it to a unit, or change certain specifications of a connection
between a file and a unit.
.begnote Open Specifiers
.cp 6
.mnote [UNIT =] u
.br
.id u
is an
.us external unit identifier.
An external unit identifier is a non-negative integer expression.
If the optional
.kw UNIT=
specifier is omitted then the specifier must be the first item in
the list of specifiers.
.cp 6
.mnote IOSTAT = ios
.br
is an
.us input/output status specifier.
The integer variable or integer array element
.id ios
is defined with zero if no error condition exists or a positive
integer value if an error condition exists.
.cp 6
.mnote ERR = s
.br
is an
.us error specifier
and
.id s
is a statement label.
When an error occurs, execution is transferred to the statement
labelled by
.id s.
.cp 6
.mnote FILE = fin
.br
.id fin
is a character expression whose value when trailing blanks are
removed is the name of a file to be connected to the specified
unit.
If this specifier is omitted and the unit is not connected to a
file, it becomes connected to a file determined by &product..
The name established by &product is described in the
section entitled :HDREF refid='units'.
of the chapter entitled :HDREF refid='fio' page=no..
.cp 6
.mnote STATUS = sta
.br
.id sta
is a character expression whose value when trailing blanks are
removed is
.mono 'OLD',
.mono 'NEW',
.mono 'SCRATCH',
or
.mono 'UNKNOWN'.
.begnote $break $setptnt 11
.cp 6
.mnote OLD
When
.mono OLD
is specified, a
.kw FILE=
specifier must be given.
The file must exist.
.cp 6
.mnote NEW
When
.mono NEW
is specified, a
.kw FILE=
specifier must be given.
The file must not exist.
Successful execution of the
.kw OPEN
statement creates the file and changes the status to
.mono OLD.
.cp 6
.mnote SCRATCH
.mono SCRATCH
may only be specified for an unnamed file (i.e.
.kw FILE=
is not allowed).
When the file is closed, it is deleted.
.cp 6
.mnote UNKNOWN
If
.mono UNKNOWN
is specified, the status is ignored.
If the
.kw STATUS=
specifier is omitted then
.mono UNKNOWN
is assumed.
.endnote
.cp 6
.mnote ACCESS = acc
.br
.id acc
is a character expression whose value when trailing blanks are
removed is
.mono 'SEQUENTIAL'
or
.mono 'DIRECT'.
It specifies the access method for the file.
If the
.kw ACCESS=
specifier is omitted then
.mono 'SEQUENTIAL'
is assumed.
If the file exists then the access method must be in the set of
allowed access methods for the file.
If the file does not exist then the file is created with a set of
allowed access methods that includes the specified access method.
.cp 10
.np
.xt on
&product also supports access
.mono 'APPEND'
which is a form of sequential access in which the file is positioned
at the endfile record.
The file must exist or the append access method must be in the set
of allowed access methods for the file.
In all other respects, the file is treated as if
.id 'SEQUENTIAL'
had been specified.
.xt off
.cp 6
.mnote FORM = fm
.br
.id fm
is a character expression whose value when trailing blanks are
removed is
.mono 'FORMATTED'
or
.mono 'UNFORMATTED'.
It specifies that the file is being connected for formatted or
unformatted input/output.
If the
.kw FORM=
specifier is omitted and the file is
being connected for direct access then
.mono 'UNFORMATTED'
is assumed.
If the
.kw FORM=
specifier is omitted and the file is being connected for sequential
access then
.mono 'FORMATTED'
is assumed.
If the file exists then the specified form must be included in the
set of allowed forms for the file.
If the file does not exist then the file is created with a set of
allowed forms that includes the specified form.
.cp 6
.mnote RECL = rcl
.br
.id rcl
is an integer expression whose value must be positive.
It specifies the length of each record in a file being connected for
direct access.
If the file is being connected for direct access, this specifier
must be given; otherwise it must be omitted.
.xt on
&product allows the
.kw RECL=
specifier for files opened for sequential access.
.xt off
.cp 6
.mnote BLANK = blnk
.br
.id blnk
is a character expression whose value when trailing blanks are
removed is
.mono 'NULL'
or
.mono 'ZERO'.
If
.mono 'NULL'
is specified then all blank
characters in numeric formatted input fields are ignored except
that an entirely blank field has a value of zero.
If
.mono 'ZERO'
is specified then all blank characters other than leading blanks are
treated as zeroes.
If this specifier is omitted then
.mono 'NULL'
is assumed.
This specifier may only be present for a file being connected for
formatted input/output.
.cp 6
.mnote ACTION = act
.br
.xt on
.id act
is a character expression whose value when trailing blanks are removed
is
.mono 'READ',
.mono 'WRITE'
or
.mono 'READWRITE'.
If
.mono 'READ'
is specified, data can only be read from the file.
If
.mono 'WRITE'
is specified, data can only be written to the file.
If
.mono 'READWRITE'
is specified, data can both be read and written.
The default is
.mono 'READWRITE'.
.xt off
.cp 6
.mnote CARRIAGECONTROL = cc
.br
.xt on
.id cc
is a character expression whose value when trailing blanks are removed
is
.mono 'YES',
or
.mono 'NO'.
If
.mono 'YES'
is specified,
&product will automatically add an extra character
at the beginning of each record.
This character will be interpreted as a carriage control character.
If
.mono 'NO'
is specified, records will be written to the file without adding a
carriage control character at the beginning of the record.
The default is
.mono 'NO'.
.xt off
.cp 6
.mnote RECORDTYPE = rct
.br
.xt on
.id rct
is a character expression whose value when trailing blanks are removed
specifies the type of record (or record structure) to be used for the
file.
The allowed values for
.id rct
depend on the system on which you are running the program.
See the compiler User's Guide for a discussion of the
.kw RECORDTYPE=
specifier.
.xt off
.cp 9
.mnote BLOCKSIZE = bl
.br
.xt on
.id bl
is an integer expression whose value specifies the internal buffer
size to be used for file input/output.
The allowed values for
.id bl
depend on the system on which you are running the program.
Generally, the larger the buffer, the faster the input/output.
See the compiler User's Guide for a discussion of the
.kw BLOCKSIZE=
specifier.
.xt off
.cp 6
.mnote SHARE = shr
.br
.xt on
.id shr
is a character expression whose value when trailing blanks are removed
specifies the way in which other processes can simultaneously access the
file.
The allowed values for
.id shr
depend on the system on which you are running the program.
See the compiler User's Guide for a discussion of the
.kw SHARE=
specifier.
.xt off
.endnote
.exam begin
      OPEN( UNIT=1, FILE='TEST', STATUS='UNKNOWN',
     +      ACCESS='SEQUENTIAL',
     +      FORM='FORMATTED', BLANK='ZERO' )
.exam end
.pc
In the above example, the file
.mono 'TEST',
containing FORMATTED records, is connected to unit 1.
The status of the file is
.mono 'UNKNOWN'
since we do not know if it already exists.
We will access the file sequentially, using formatted input/output
statements.
Blanks in numeric input data are to be treated as zeroes.
.autonote Notes:
.note
If the unit is already connected to a file that exists, the execution
of an
.kw OPEN
statement for that unit is permitted.
.begpoint
.point (a)
If the same file is opened then only the
.kw BLANK=
specifier may
be different.
The same file is opened if no
.kw FILE=
specifier was
given or if the
.kw FILE=
specifier refers to the same file.
.point (b)
If a different file is opened then the currently connected file
is automatically closed.
.endpoint
.note
If the file to be connected to the unit does not exist, but
is already preconnected to the unit, any properties specified
in the
.kw OPEN
statement are merged with and supersede those of the preconnection.
For example,
the
.kw RECL=
specification will override the record length attribute
defined by a preconnection of the file.
.note
The same file may not be connected to two or more different units.
.endnote
.np
For more information on input/output,
see the chapter entitled :HDREF refid='fio'..
