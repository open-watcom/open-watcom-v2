.section &typstmt Statement
.*
.np
The
.kw &typstmt
statement is a type declaration statement and can be used to declare a
name to be of type &typname..
The implicit type of the name, whether defined by the "first letter
rule" (see the chapter entitled :HDREF refid='ftypes'.) or by an
.kw IMPLICIT
statement, is either confirmed or overridden.
However, once a name has been declared to be of type &typname, it
cannot appear in another type declaration statement.
.np
There are various forms of the
.kw &typstmt
statement.
The following sections describe them.
.beglevel
.if '&typstmt' eq 'CHARACTER' .do begin
.im schar
.do end
.el .do begin
.if '&typstmt' eq 'DOUBLE COMPLEX' .do begin
.*
.section Simple &typstmt Statement
.*
.mext begin
      &typstmt name [,name] ...
.mext end
.do end
.el .do begin
.*
.section Standard &typstmt Statement
.*
.mbox begin
      &typstmt name [,name] ...
.mbox end
.do end
.synote 6
.mnote name
is a variable name, array name, array declarator, symbolic name of a
constant, function name or dummy procedure name.
.endnote
.np
.if '&typstmt' eq 'DOUBLE COMPLEX' .do begin
The
.kw &typstmt
statement is an extension to the FORTRAN 77 language.
.do end
.el .do begin
This form is the standard form of the
.kw &typstmt
statement.
.do end
.exam begin
      DIMENSION C(-5:5)
      &typstmt A, B(10), C
.exam end
.pc
In the previous example,
.id A
is defined to be a variable of type &typname and
.id B
and
.id C
are defined to be arrays of type &typname..
.if '&typstmt' ne 'DOUBLE COMPLEX' .do begin
.if '&typstmt' ne 'DOUBLE PRECISION' .do begin
.im stypext
.do end
.do end
.do end
.im stypdat
.endlevel
