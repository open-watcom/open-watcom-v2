.section ENDFILE Statement
.*
.mbox begin
      ENDFILE u
      ENDFILE (alist)
.mbox end
.synote 7
.mnote u
is an external unit identifier.
.mnote alist
is a list of endfile specifiers separated by commas:
.millust begin
[UNIT =] u
IOSTAT = ios
ERR = s
.millust end
.endnote
.np
Execution of an
.kw ENDFILE
statement causes an endfile record to be written to the file
connected to the specified unit.
The file is then positioned after the endfile record.
If the file may be connected for
.us direct access,
only those records before the endfile record are considered to
have been written.
Thus, only those records before the endfile record may be read during
subsequent direct access connections to the file.
.begnote Endfile Specifiers
.setptnt 0 8
.mnote [UNIT =] u
.br
.id u
is an
.us external unit identifier.
An external unit identifier is a non-negative integer
expression.
If the optional
.kw UNIT=
specifier is omitted then the
specifier must be the first item in the list of specifiers.
.mnote IOSTAT = ios
.br
is an
.us input/output status specifier.
The integer variable or integer array element
.id ios
is defined with zero if no error condition exists or a positive
integer value if an error condition exists.
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
.endnote
.exam begin
      LOOP
          READ( UNIT=7, END=100, FMT=200 )RECORD
          WRITE( UNIT=8, FMT=200 )RECORD
      ENDLOOP
100   ENDFILE( UNIT=8 )
.exam end
.pc
In the previous example, we illustrate how one might read all the
records from one file (unit 7), write them to another file
(unit 8) and then write an endfile record to the end of the file
on unit 8.
.autonote Notes:
.note
The unit must be connected for sequential access.
.note
After execution of an
.kw ENDFILE
statement, a
.kw BACKSPACE
or
.kw REWIND
statement must be used to reposition the file before
any other input/output statement which refers to this file
can be executed.
.note
If the file did not exist before execution of the
.kw ENDFILE
statement then it will be created after execution of this
statement.
.endnote
.np
For more information on input/output, see the chapter
entitled :HDREF refid='fio'..
