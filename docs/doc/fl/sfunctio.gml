.section FUNCTION Statement
.*
.np
A
.kw FUNCTION
statement is used to define the start of a function subprogram.
There are two forms of the
.kw FUNCTION
function statement.
The second form is a &product extension.
.*
.beglevel
.*
.section Standard FUNCTION Statement
.*
.mbox begin
      [type] FUNCTION fun ( [d [, d] ...] )
.mbox end
.synote 6
.mnote type
is one of
.mono LOGICAL,
.mono INTEGER,
.mono REAL,
.mono DOUBLE PRECISION,
.mono COMPLEX
or
.mono CHARACTER
.id [*len].
.mnote fun
is a symbolic name of a function subprogram.
.mnote d
is a variable name, array name, or a dummy procedure name.
.id d
is called a
.us dummy argument.
.ix 'dummy argument'
.ix 'argument' 'dummy'
.mnote len
is called the length specification and is the length
(number of characters) of the result of the character function.
It has one of the following forms:
.begnote
.note (1)
An unsigned positive integer constant.
.note (2)
A positive integer constant expression enclosed in parentheses.
.note (3)
An asterisk in parentheses,
.mono (*).
.endnote
.endnote
.cp 9
.exam begin
      PRINT *, TMAX3( -1.0, 12.0, 5.0 )
      END

      FUNCTION TMAX3( ARGA, ARGB, ARGC )
          TMAX3 = ARGA
          IF( ARGB .GT. TMAX3 ) TMAX3 = ARGB
          IF( ARGC .GT. TMAX3 ) TMAX3 = ARGC
      END
.exam end
.pc
In the above example, the function
.id TMAX3
is defined to find the maximum of three real variables.
.autonote Notes:
.setptnt 0 5
.note
No dummy arguments need be specified in the
.kw FUNCTION
statement.
However, the parentheses () are mandatory.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
.*
.section Extended FUNCTION Statement
.*
.mext begin
      [type[*len]] FUNCTION fun[*len] ( [d [, d] ...] )
.mext end
.synote 6
.mnote type
is one of
.mono LOGICAL,
.mono INTEGER,
.mono REAL,
.mono DOUBLE PRECISION,
.mono COMPLEX,
.mono CHARACTER
or
.mono RECORD /typename/
.mnote fun
is a symbolic name of a function subprogram.
.mnote d
is a variable name, array name, or a dummy procedure name.
.id d
is called a
.us dummy argument.
.ix 'dummy argument'
.ix 'argument' 'dummy'
.mnote len
is called the length specification and
has one of the following forms:
.begnote
.note (1)
An unsigned positive integer constant.
.note (2)
A positive integer constant expression enclosed in parentheses.
.note (3)
An asterisk in parentheses,
.mono (*).
.endnote
.np
For valid values of
.id len,
refer to the appropriate type declaration statement.
.endnote
.np
This form of the
.kw FUNCTION
statement is an extension to the FORTRAN 77 language.
.cp 13
.exam begin
      INTEGER*2 MOD2, I, J
      I = 12
      J = 5
      PRINT *, MOD2( I, J )
      END

      INTEGER*2 FUNCTION MOD2( I, J )
      INTEGER*2 I, J
      INTEGER II, JJ
      II = I
      JJ = J
      MOD2 = MOD(II, JJ)
      END
.exam end
.autonote Notes:
.setptnt 0 5
.note
No dummy arguments need be specified in the
.kw FUNCTION
statement.
However, the parentheses () are mandatory.
.note
The length specification can appear only once in the
.kw FUNCTION
statement.
.endnote
.np
For more information, see the chapter entitled :HDREF refid='fsubp'..
.endlevel
