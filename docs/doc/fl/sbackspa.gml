.section BACKSPACE Statement
.*
.mbox begin
      BACKSPACE u
      BACKSPACE (alist)
.mbox end
.synote 7
.mnote u
is an external unit identifier.
.mnote alist
is a list of backspace specifiers separated by commas:
.millust begin
[UNIT =] u
IOSTAT = ios
ERR = s
.millust end
.endnote
.np
Execution of a
.kw BACKSPACE
statement causes the file connected to the specified unit to be
positioned at the beginning of the preceding record.
If the preceding record is an endfile record then the file is
positioned at the beginning of the endfile record.
.begnote Backspace Specifiers
.setptnt 0 8
.mnote [UNIT =] u
.br
.id u
is an
.us external unit identifier.
An external unit identifier is a non-negative integer expression.
If the optional
.kw UNIT=
specifier is omitted then the specifier must be the first item in the
list of specifiers.
.mnote IOSTAT = ios
.br
is an
.us input/output status specifier.
The integer variable or integer array element
.id ios
is defined with zero if no error condition occurs or a positive
integer value if an error condition occurs.
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
          READ( UNIT=8, END=100, FMT=200 ) RECORD
      ENDLOOP
100   BACKSPACE( UNIT=8 )
      WRITE( UNIT=8, FMT=200 ) NEWREC
.exam end
.pc
In the previous example, we illustrate how one might append
a record to the end of an existing file.
.autonote Notes:
.note
The unit must be connected for sequential access.
.note
If the file is positioned before the first record then the
.kw BACKSPACE
statement has no effect.
.note
It is illegal to backspace a file that does not exist.
.endnote
.np
.xt begin
The FORTRAN 77 standard specifies that it is illegal to backspace over
records that were written using list-directed formatting; &product
allows it.
.xt end
.np
.xt begin
If the file has been opened with access
.mono 'APPEND',
which is a form of sequential access in which the file is positioned
at the endfile record, then the
.kw BACKSPACE
statement cannot be used.
.xt end
.np
For more information on input/output, see the chapter
entitled :HDREF refid='fio'..
