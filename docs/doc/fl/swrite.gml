.section WRITE Statement
.*
.mbox begin
      WRITE (cilist) [olist]
.mbox end
.synote 8
.mnote cilist
is a control information list of specifiers separated by commas:
.millust begin
[UNIT =] u
[FMT =] f
REC = rn
IOSTAT = ios
ERR = s
.millust end
.mnote olist
is an output list.
.endnote
.np
The
.kw WRITE
statement is used to transfer data from the executing FORTRAN program
to an external device or file.
.begnote Control Information List
.setptnt 0 8
.mnote [UNIT =] u
.br
.id u
is an
.us external unit identifier
or an
.us internal file identifier.
.autopoint
.point
An external unit identifier is a non-negative integer expression or an
asterisk
.mono (*)
in which case unit 6 is assumed.
.point
An internal file identifier is the name of a character variable,
character array, character array element, or character substring.
.endpoint
.np
If the optional
.kw UNIT=
specifier is omitted then the unit specifier must be the first item in
the list of specifiers.
.mnote [FMT =] f
.br
.id f
is a
.us format identifier.
A format identifier is one of the following:
.autopoint
.point
A statement label of a
.kw FORMAT
statement that appears in the same program unit as the format
identifier.
.point
An integer variable name that has been assigned the statement label of
a
.kw FORMAT
statement that appears in the same program unit as the format
identifier (see the
.kw ASSIGN
statement).
.point
.xt An integer array name.
.point
A character array name.
.point
Any character expression except one involving the concatenation of an
operand whose length specification is
.mono (*)
unless the operand is a symbolic constant (see the
.kw PARAMETER
statement).
.point
.ix 'list-directed format'
.ix 'format' 'list-directed'
An asterisk
.mono (*),
indicating
.us list-directed
formatting.
.point
.xt on
.ix 'namelist-directed format'
.ix 'format' 'namelist-directed'
A
.kw NAMELIST
name, indicating
.us namelist-directed
formatting.
.xt off
.endpoint
.np
If the optional
.kw FMT=
specifier is omitted then the format specifier must be the second item
in the list of specifiers and
.kw UNIT=
must not be specified for the first item in the list.
.mnote REC = rn
.br
.id rn
is an integer expression whose value must be positive.
It is the number of the record to be written when a file is
connected for direct access.
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
.endnote
.np
.us Output list -
An output list may contain one or more of the following:
.autonote
.note
A variable name.
.note
An array element name.
.note
A character substring name.
.note
An array name except an assumed-size dummy array.
.note
Any other expression except a character expression involving
concatenation of an operand whose length specification is an
asterisk in parentheses unless the operand is the symbolic name
of a constant (since the length can be determined at compile
time).
.note
An implied-DO list of the form:
.millust begin
( dlist, i = e1, e2 [,e3] )
.millust end
.pc
where
.id dlist
is composed of one or more of items (1) through (6).
.endnote
.exam begin
      WRITE( 6, 100 )X, Y, Z
      WRITE( UNIT=6, FMT=100 )X, Y, Z
100   FORMAT( 3F10.5 )
      WRITE( UNIT=6, FMT='( 3F10.5 )' )X, Y, Z
      WRITE( 6, '( 3F10.5 )' )X, Y, Z
.exam end
.pc
The above gives four examples of formatted
.kw WRITE
statements.
In all cases, the format conversion is identical but it was specified
in different ways.
When executed, the effect of all
.kw WRITE
statements is the same.
The unit number, used here, is 6.
There are, in fact, many other ways in which the
.kw WRITE
statement could have been written, all of which would have the same
effect when executed.
We have not shown use of all the specifiers.
.exam begin
      WRITE( 6, * )X, Y, Z
      WRITE( *, * )X, Y, Z
      WRITE( UNIT=6, FMT=* )X, Y, Z
      WRITE( UNIT=*, FMT=* )X, Y, Z
.exam end
.pc
The above four examples of list-directed formatted output are all
equivalent.
&product assumes unit 6 when the unit number identifier is an
asterisk (as in the second and fourth examples).
In the examples, the format identifier is an asterisk indicating
list-directed formatting.
.ix 'list-directed output'
.ix 'output' 'list-directed'
.exam begin
      WRITE( 8 )X, Y, Z
      WRITE( UNIT=8 )X, Y, Z
.exam end
.pc
The above gives two examples of unformatted
.kw WRITE
statements.
The unit number used in the example is 8.
When executed, the effect of both of these statements is the same.
The values of the variables
.id X,
.id Y
and
.id Z
are written to the file connected to unit 8 in their binary form
(a form quite incomprehensible to most human beings).
An advantage to using this particular form of the
.kw WRITE
statement is that no conversion is required between the internal
binary representation of the values and their textual
(human-readable) form (which means it takes less computer time to
process the data).
.autonote Notes:
.note
If no output list is specified then the effect of the
.kw WRITE
statement is to produce a record whose characters are all blanks.
.note
The
.kw REC=
specifier may not be used when list-directed output is specified.
.note
An implication of point (6) above is that nesting of implied-DO
lists is permitted.
For example, the output list
.millust begin
( (A(I,J), B(I,J), J = 1, 5), I = 1, 10 )
.millust end
.pc
may be broken down into the following components:
.millust begin
   A(I,J), B(I,J)
  (....dlist1...., J = 1, 5)
( .....dlist2..............., I = 1, 10 )
.millust end
.endnote
.np
For more information on input/output, see the chapter
entitled :HDREF refid='fio'..
For more information on formatted input/output, see the chapter
entitled :HDREF refid='fformat'..
