.cp 8
.section Extended &typstmt Statement: Length Specification
.*
.mext begin
      &typstmt[*len[,]] name [,name] ...
.mext end
.synote 6
.mnote name
is one of the following forms:
.np
.id v[*len]
.np
.id a[*len](d)
.np
.id a(d)[*len]
.mnote v
is a variable name, array name, symbolic name of a constant, function
name or dummy procedure name.
.mnote a
is an array name.
.mnote (d)
is that part of the array declarator defining the dimensions of the
array.
.mnote len
is called the
.us length specification
.ix 'length specification'
and is an unsigned positive integer constant or an integer constant
expression enclosed in parentheses whose value is
.if '&typstmt' eq 'REAL' .do begin
4 or 8.
.do end
.el .if '&typstmt' eq 'INTEGER' .do begin
1, 2 or 4.
.do end
.el .if '&typstmt' eq 'LOGICAL' .do begin
1 or 4.
.do end
.el .if '&typstmt' eq 'COMPLEX' .do begin
8 or 16.
.do end
.endnote
.np
This form of the
.kw &typstmt
statement is a &product extension to the FORTRAN 77 language.
The length specification specifies the number of bytes of storage
that will be allocated for the name appearing in the
.kw &typstmt
statement.
.if '&typstmt' eq 'REAL' .do begin
The default length specification is 4.
A length specification of 8 specifies that the data type of the name
appearing in the
.kw &typstmt
statement is to be double precision.
.do end
.el .if '&typstmt' eq 'INTEGER' .do begin
The default length specification is 4.
A length specification of 1 or 2 does not change the data type; it merely
restricts the magnitude of the integer that can be represented.
See the chapter entitled :HDREF refid='ftypes'. for more information.
.do end
.el .if '&typstmt' eq 'LOGICAL' .do begin
The default length specification is 4.
A length specification of 1 only changes the storage requirement from 4
bytes to 1 byte; the values of true and false can be represented
regardless of the length specification.
.do end
.el .if '&typstmt' eq 'COMPLEX' .do begin
The default length specification is 8.
A length specification of 16 specifies that the data type of the name
appearing in the
.kw &typstmt
statement is to be double precision complex.
.do end
.np
The length specification immediately following the word
.mono &typstmt
is the length specification for each entity in the statement not having
its own length specification.
If a length specification is not specified the default length
specification is used.
An entity with its own specification overrides
the default length specification or the length specification immediately
following the word
.mono &typstmt..
Note that for an array the length specification applies to
.us each
element of the array.
.if '&typstmt' eq 'REAL' .do begin
.exam begin
      DIMENSION C(-5:5)
      REAL A, B*8(10), C*8
      REAL*8 X
.exam end
.pc
In the previous example,
.id X
is declared to be a variable of type double precision,
.id A
is declared to be a variable of type real and
.id B
and
.id C
are declared to be arrays of type double precision.
.do end
.el .if '&typstmt' eq 'INTEGER' .do begin
.exam begin
      DIMENSION C(-5:5)
      INTEGER A, B*2(10), C*2
      INTEGER*1 X
.exam end
.pc
In the previous example,
.id X
is declared to be a variable of type integer and occupying 1 byte
of storage,
.id A
is declared to be a variable of type integer and occupying 4 bytes of
storage and
.id B
and
.id C
are declared to be arrays of type integer with
.us each
element of the array occupying 2 bytes.
.do end
.el .if '&typstmt' eq 'LOGICAL' .do begin
.exam begin
      DIMENSION C(-5:5)
      LOGICAL A, B*1(10), C*1
      LOGICAL*4 X
.exam end
.pc
In the previous example,
.id X
is declared to be a variable of type logical and occupying 4 bytes
of storage,
.id A
is declared to be a variable of type logical and occupying 4 bytes of
storage and
.id B
and
.id C
are declared to be arrays of type logical with
.us each
element of the array occupying 1 byte.
.do end
.el .if '&typstmt' eq 'COMPLEX' .do begin
.exam begin
      DIMENSION C(-5:5)
      COMPLEX A, B*16(10), C*16
      COMPLEX*16 X
.exam end
.pc
In the previous example,
.id X
is declared to be a variable of type double precision complex,
.id A
is declared to be a variable of type complex and
.id B
and
.id C
are declared to be arrays of type double precision complex.
.do end
