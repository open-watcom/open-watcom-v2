.section PRINT Statement
.*
.np
Two forms of the
.kw PRINT
statement are supported by &product..
.beglevel
.*
.section Standard PRINT Statement
.*
.mbox begin
      PRINT f [,olist]
.mbox end
.synote 7
.mnote f
is a format identifier.
.mnote olist
is an optional output list.
.endnote
.*
.section Extended PRINT Statement
.*
.mext begin
      PRINT, olist
.mext end
.synote 7
.mnote olist
is an output list.
.endnote
.*
.section Description of PRINT Statement
.*
.np
The
.kw PRINT
statement is used to transfer data from the executing FORTRAN
program to an external device or file.
.np
.us Format Identifier -
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
&product supports a variation of
.us list-directed
formatting in which the asterisk
.mono (*)
may be omitted.
It is equivalent to
.millust begin
PRINT * [,olist]
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
asterisk in parentheses unless the operand is the symbol name
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
      CHARACTER*5 S
      COMPLEX C
      S = 'Hello'
      I = 123
      X = 12.5
      C = (12.5,4.58)
      PRINT *, S, I, X, C
      END
.exam end
.pc
The above example illustrates
.us list-directed
.ix 'list-directed format'
.ix 'format' 'list-directed'
formatting using the
.kw PRINT
statement.
The asterisk specifies that the variables in the output list
are to be displayed in some format that is appropriate to the
type of the variable (hence the term "list-directed").
The CHARACTER variable
.id S
is printed using a suitable
.mono A
format descriptor.
The INTEGER variable
.id I
is printed using a suitable
.mono I
format descriptor.
The REAL variable
.id X
is printed using a suitable
.mono G
format descriptor.
The COMPLEX variable
.id C
is printed using a suitable
.mono G
format descriptor and is displayed with
enclosing parentheses and a comma.
Output from the above program would resemble the following.
.millust begin
Hello    123    12.5000000      (12.5000000,4.5799999)
.millust end
.exam begin
      CHARACTER*5 S
      COMPLEX C
      S = 'Hello'
      I = 123
      X = 12.5
      C = (12.5,4.58)
      PRINT, S, I, X, C
      END
.exam end
.pc
The above example illustrates a &product extension of
.us list-directed
.ix 'list-directed format'
.ix 'format' 'list-directed'
formatting using the
.kw PRINT
statement.
The asterisk is omitted but the results are exactly the same as in
the previous example.
.exam begin
      PRINT 100, X, Y, Z
100   FORMAT( 3F10.5 )
      PRINT '( 3F10.5 )', X, Y, Z
.exam end
.pc
The above gives two examples of the
.kw PRINT
statement.
In both cases, the format conversion is identical but it was
specified in different ways.
When executed, the effect of both
.kw PRINT
statements is the same.
.exam begin
      PRINT '(1X,100A1)', ('*',I=1,J)
.exam end
.pc
The above example illustrates a technique for producing histograms
using the implied DO-loop.
Each time this statement is executed, a number of asterisks are
printed, depending on the value of
.id J.
.autonote Notes:
.note
The
.kw PRINT
statement is implicitly a formatted output statement.
.note
The unit number that is
.us implicitly
used in the
.kw PRINT
statement is unit number 6.
.note
If no output list is specified then the effect of the
.kw PRINT
statement is to produce one or more records whose characters are
all blanks.
.note
FORTRAN 77 leaves the format of output in list-directed
formatting to the discretion of &product..
Hence other FORTRAN compilers may produce different results.
If the format of output must be consistent from one compiler to the
next then list-directed formatting should
.us not
be used.
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
.endlevel
