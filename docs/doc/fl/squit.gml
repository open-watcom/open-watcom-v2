.section QUIT Statement
.*
.mext begin
      QUIT    [: block-label]
.mext end
.np
The
.kw QUIT
statement may be used to cause a transfer of control to the first
executable statement that follows the terminal statement of the
block which contains it.
Examples of such terminal statements are
.kw ADMIT
.ct ,
.kw CASE
.ct ,
.kw END DO
.ct ,
.kw END LOOP
.ct ,
.kw END WHILE
.ct ,
.kw UNTIL
.ct , etc.
If
.id block-label
is present then control is transferred out of the block identified
by that block label.
The
.kw QUIT
statement is an extension to the FORTRAN 77 language.
.exam begin
      LOOP
          WRITE( UNIT=*, FMT='(A)' ) 'Enter a number'
          READ( UNIT=*, FMT='(F10.4)', IOSTAT=IOS ) X
          IF( IOS .NE. 0 ) QUIT
          IF( X .LT. 0 ) QUIT
          PRINT *, X, SQRT( X )
      END LOOP
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
