.section READ Statement
.*
.np
Three forms of the
.kw READ
statement are supported by &product..
.beglevel
.*
.section Standard READ Statement
.*
.mbox begin
      READ (cilist) [ilist]
      READ f [,ilist]
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
END = s
.millust end
.mnote f
is a format identifier.
.mnote ilist
is an optional input list.
.endnote
.*
.section Extended READ Statement
.*
.mext begin
      READ, ilist
.mext end
.synote 8
.mnote ilist
is an input list.
.endnote
.*
.section Description of READ Statement
.*
.np
The
.kw READ
statement is used to transfer data from a device or file into the
executing FORTRAN program.
As shown above, &product supports three forms of the
.kw READ
statement.
.begnote Control Information List and Format Identifier
.setptnt 0 8
.cp 5
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
in which case unit 5 is assumed.
.point
An internal file identifier is the name of a character variable,
character array, character array element, or character substring.
.endpoint
.np
If the optional
.kw UNIT=
specifier is omitted then the unit specifier must be the first item in
the list of specifiers.
.cp 5
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
.np
.xt begin
&product supports a third form of the
.kw READ
statement in which the asterisk
.mono (*)
may be omitted.
.ix 'list-directed format'
.ix 'format' 'list-directed'
This is a form of
.us list-directed
formatting in which unit 5 is assumed.
It is equivalent to
.millust begin
READ * [,ilist]
.millust end
.pc
.xt end
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
.cp 5
.mnote REC = rn
.br
.id rn
is an integer expression whose value must be positive.
It is the number of the record to be read when a file is
connected for direct access.
.cp 5
.mnote IOSTAT = ios
.br
is an input/output status specifier.
The integer variable or integer array element
.id ios
is defined with zero if no error occurs, a positive integer value if
an error occurs, or a negative integer value if an end-of-file occurs.
.cp 5
.mnote ERR = s
.br
is an error specifier and
.id s
is a statement label.
When an error occurs, execution is transferred to the statement
labelled by
.id s.
.cp 5
.mnote END = s
.br
is an end-of-file specifier and
.id s
is a statement label.
When an end-of-file occurs, execution is transferred to the statement
labelled by
.id s.
.endnote
.np
.us Input list -
An input list may contain one or more of the following:
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
An implied-DO list of the form:
.millust begin
( dlist, i = e1, e2 [,e3] )
.millust end
.pc
where
.id dlist
is composed of one or more of items (1) through (5).
.endnote
.exam begin
      READ( 5, 100 )X, Y, Z
      READ( UNIT=5, FMT=100 )X, Y, Z
100   FORMAT( 3F10.5 )
      READ( UNIT=5, FMT='( 3F10.5 )' )X, Y, Z
      READ( 5, '( 3F10.5 )' )X, Y, Z
.exam end
.pc
.ix 'formatted input'
.ix 'input' 'formatted'
The above gives four examples of formatted
.kw READ
statements, using the first of three supported forms of the
.kw READ
statement.
In all cases, the format conversion is identical but it was specified
in different ways.
When executed, the effect of all
.kw READ
statements is the same.
The unit number that is
.us explicitly
used in this form of the
.kw READ
statement is unit number 5.
There are, in fact, many other ways in which the
.kw READ
statement could have been written, all of which would have the
same effect when executed.
We have not shown the use of all the specifiers.
.exam begin
      READ 100, X, Y, Z
100   FORMAT( 3F10.5 )
      READ '( 3F10.5 )', X, Y, Z
.exam end
.pc
The above gives two examples of formatted
.kw READ
statements, using the second of three supported forms of the
.kw READ
statement.
In both cases, the format conversion is identical but it was specified
in different ways.
When executed, the effect of both
.kw READ
statements is the same.
The unit number that is
.us implicitly
used in this form of the
.kw READ
statement is unit number 5.
.exam begin
      READ( 5, * )X, Y, Z
      READ( *, * )X, Y, Z
      READ( UNIT=5, FMT=* )X, Y, Z
      READ( UNIT=*, FMT=* )X, Y, Z
      READ *, X, Y, Z
      READ  , X, Y, Z
.exam end
.pc
The above six examples of list-directed formatted input are all
equivalent.
&product assumes unit 5 when the unit number identifier is an
asterisk (as in the second and fourth examples).
In the fifth example, the asterisk is a format identifier indicating
list-directed formatting.
.ix 'list-directed input'
.ix 'input' 'list-directed'
The fifth and sixth examples are examples of the second and
third forms, respectively, of the
.kw READ
statement in which &product assumes unit 5.
When the format identifier is an asterisk or when the third form
of the
.kw READ
statement is used, we call this
list-directed
.us list-directed
.ix 'list-directed format'
.ix 'format' 'list-directed'
formatting.
.exam begin
      READ( 8 )X, Y, Z
      READ( UNIT=8 )X, Y, Z
.exam end
.pc
The above gives two examples of unformatted
.ix 'unformatted input'
.ix 'input' 'unformatted'
.kw READ
statements.
The unit number used in the example is 8.
When executed, the effect of both of these statements is the same.
The values of the variables
.id X,
.id Y
and
.id Z
are read from the file connected to unit 8.
The values are stored in the file in their binary form (a form quite
incomprehensible to most human beings).
An advantage to using this particular form of the
.kw READ
statement is that no conversion is required between the internal
binary representation of the values and their textual
(human-readable) form (which means it takes less computer time to
process the data).
.autonote Notes:
.note
The
.kw REC=
specifier may not be used when list-directed
output is specified.
.note
If no input list is specified then the effect of the
.kw READ
statement is to skip one or more records in the file.
.note
An implication of point (5) above is that nesting of implied-DO
lists is permitted.
For example, the input list
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
For more information on formatted input/output, see the
chapter :HDREF refid='fformat'..
.endlevel
