.cp 12
.if '&typstmt' eq 'DOUBLE COMPLEX' .do begin
.*
.section &typstmt Statement: Data Initialization
.*
. .mext begin
      &typstmt name [/cl/] [,name[/cl/]] ...
. .mext end
.do end
.el .if '&typstmt' eq 'DOUBLE PRECISION' .do begin
.*
.section Extended &typstmt Statement: Data Initialization
.*
. .mext begin
      &typstmt name [/cl/] [,name[/cl/]] ...
. .mext end
.do end
.el .do begin
.*
.section Extended &typstmt Statement: Data Initialization
.*
. .mext begin
      &typstmt[*len[,]] name [/cl/] [,name[/cl/]] ...
. .mext end
.do end
.synote 6
.mnote name
is as described in the previous section.
.if '&typstmt' ne 'DOUBLE COMPLEX' .do begin
.if '&typstmt' ne 'DOUBLE PRECISION' .do begin
.mnote len
is as described in the previous section.
.do end
.do end
.mnote cl
is a list of the form:
.np
.mono k [,k] ...
.mnote k
is one of the forms:
.np
.mono c
.np
.mono r*c
(equivalent to
.id r
successive appearances of
.id c)
.mnote c
is a constant or the symbolic name of a constant
.mnote r
is an unsigned positive integer constant or the symbolic name of a
constant.
.endnote
.np
This form of the
.kw &typstmt
statement is
.if '&typstmt' eq 'DOUBLE COMPLEX' .do begin
also
.do end
an extension to the FORTRAN 77 language.
The rules for data initialization are the same as for the
.kw DATA
statement.
.if '&typstmt' eq 'REAL' .do begin
.exam begin
      REAL A/1.2/, B(10)/10*5.0/
.exam end
.pc
In the previous example,
.id A
is initialized with the real constant
.mono 1.2
and each element of the array
.id B
is initialized with the real constant
.mono 5.0.
.do end
.if '&typstmt' eq 'DOUBLE COMPLEX' .do begin
.exam begin
      DOUBLE COMPLEX A/(4D4,5.1D4)/, B(10)/10*(5D1,3.1D1)/
.exam end
.pc
In the previous example,
.id A
is initialized with the double precision complex constant
.mono (4D4,5.1D4)
and each element of the array
.id B
is initialized with the double precision complex constant
.mono (5D1,3.1D1).
.do end
.if '&typstmt' eq 'DOUBLE PRECISION' .do begin
.exam begin
      DOUBLE PRECISION A/4D4/, B(10)/10*5D1/
.exam end
.pc
In the previous example,
.id A
is initialized with the double precision constant
.mono 4D4
and each element of the array
.id B
is initialized with the double precision constant
.mono 5D1.
.do end
.if '&typstmt' eq 'INTEGER' .do begin
.exam begin
      INTEGER A/100/, B(10)/10*0/
.exam end
.pc
In the previous example,
.id A
is initialized with the integer constant
.mono 100
and each element of the array
.id B
is initialized with the integer constant
.mono 0.
.do end
.if '&typstmt' eq 'LOGICAL' .do begin
.exam begin
      LOGICAL A/.TRUE./, B(10)/10*.FALSE./
.exam end
.pc
In the previous example,
.id A
is initialized with the logical constant
.monoon
.li .TRUE.
.monooff
and each element of the array
.id B
is initialized with the logical constant
.monoon
.li .FALSE..
.monooff
.do end
.if '&typstmt' eq 'COMPLEX' .do begin
.exam begin
      COMPLEX A/(.4,-.3)/, B(10)/10*(0,1)/
.exam end
.pc
In the previous example,
.id A
is initialized with the complex constant
.mono (.4,-.3)
and each element of the array
.id B
is initialized with the complex constant
.mono (0,1).
.do end
.if '&typstmt' eq 'CHARACTER' .do begin
.exam begin
      CHARACTER*5 A/'AAAAA'/, B*3(10)/10*'111'/
.exam end
.pc
In the previous example,
.id A
is initialized with the character constant
.mono 'AAAAA'
and each element of the array
.id B
is initialized with the character constant
.mono '111'.
.do end
