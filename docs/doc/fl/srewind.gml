.section REWIND Statement
.*
.mbox begin
      REWIND u
      REWIND (alist)
.mbox end
.synote 7
.mnote u
is an external unit identifier.
.mnote alist
is a list of rewind specifiers separated by commas:
.millust begin
[UNIT =] u
IOSTAT = ios
ERR = s
.millust end
.endnote
.np
Execution of a
.kw REWIND
statement causes the file connected to the specified unit to be
positioned at the beginning (or before the first record) of the file.
.begnote Rewind Specifiers
.setptnt 0 8
.mnote [UNIT =] u
.br
.id u
is an
.us external unit identifier.
An external unit identifier is a non-negative integer expression.
If the optional
.mono UNIT=
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
.cp 11
.exam begin
      LOOP
          READ( UNIT=7, END=100, FMT=200 )RECORD
          PRINT *, RECORD
      ENDLOOP
100   REWIND( UNIT=7 )
      LOOP
          READ( UNIT=7, END=101, FMT=200 )RECORD
          WRITE( UNIT=8, FMT=200 )RECORD
      ENDLOOP
101   CONTINUE
.exam end
.pc
In the previous example, we illustrate how one might process
the records in a file twice.
After reaching the endfile record, a
.kw REWIND
statement is executed and the file is read a second time.
.autonote Notes:
.note
The unit must be connected for sequential access.
.note
If the file is positioned at the beginning of the file then the
.kw REWIND
statement has no effect.
.note
It is permissible to rewind a file that does not exist but it has no
effect.
.endnote
.np
For more information on input/output, see the chapter
entitled :HDREF refid='fio'..
