.section CLOSE Statement
.*
.mbox begin
      CLOSE (cllist)
.mbox end
.synote 8
.mnote cllist
is a list of close specifiers separated by commas:
.millust begin
[UNIT =] u
IOSTAT = ios
ERR = s
STATUS = sta
.millust end
.endnote
.np
A
.kw CLOSE
statement is used to terminate the connection of a file to
the specified unit.
.begnote Close Specifiers
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
specifier is omitted then the specifier must be the first item in the
list of specifiers.
.mnote IOSTAT = ios
.br
is an input/output status specifier.
The integer variable or integer array element
.id ios
is defined with zero if no error condition occurs or a positive
integer value if an error condition occurs.
.mnote ERR = s
.br
is an error specifier and
.id s
is a statement label.
When an error occurs, execution is transferred to the statement
labelled by
.id s.
.mnote STATUS = sta
.br
is a status specifier and
.id sta
is a character expression whose value when trailing blanks are
removed evaluates to one of
.mono 'KEEP'
or
.mono 'DELETE'.
.begnote
.setptnt 0 10
.mnote KEEP
.mono 'KEEP'
may not be specified for a file whose status is
.mono 'SCRATCH'
(see description of the
.kw OPEN
statement).
If the file exists, it will exist after execution of the
.kw CLOSE
statement.
If the file does not exist, it will not exist after execution of the
.kw CLOSE
statement.
If not specified,
.mono 'KEEP'
is assumed, unless the file status is
.mono 'SCRATCH'
in which case
.mono 'DELETE'
is assumed.
.mnote DELETE
If
.mono 'DELETE'
is specified, the file will not exist after execution of the
.kw CLOSE
statement.
.endnote
.endnote
.exam begin
      LOOP
          READ( UNIT=8, END=100, FMT=200 ) RECORD
      ENDLOOP
100   CLOSE( UNIT=8 )
.exam end
.pc
In the previous example, we illustrate how one might process
the records in a file and then terminate the connection of the file
to unit 8 using the
.kw CLOSE
statement.
.autonote Notes:
.note
Execution of a
.kw CLOSE
statement specifying a unit that is not connected to a file
or a unit that is connected to a file that does not exist
has no effect.
.note
It is possible to connect the unit to another file after a
.kw CLOSE
statement has been executed.
.note
It is possible to connect the unit to the same file after
a
.kw CLOSE
statement has been executed, provided that the file still exists.
.note
It is possible to connect the file to another unit after
a
.kw CLOSE
statement has been executed, provided that the file still exists.
.note
At the termination of execution of the program, for whatever the reason
of termination, any units that are connected are closed.
Each unit is closed with status
.mono 'KEEP'
unless the file status was
.mono 'SCRATCH',
in which case the unit is closed with status
.mono 'DELETE'.
The effect is the same as if a
.kw CLOSE
statement is executed without a
.kw STATUS=
specifier.
.endnote
.np
For more information on input/output, see the chapter
entitled :HDREF refid='fio'..
