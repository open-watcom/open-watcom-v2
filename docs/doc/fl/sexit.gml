.section EXIT Statement
.*
.mext begin
      EXIT    [: block-label]
.mext end
.np
The
.kw EXIT
statement is used to transfer control:
.autopoint
.point
from within a loop (DO, DO WHILE, WHILE or LOOP) to the
statement following the loop,
.point
from within a GUESS or ADMIT block to the statement following
the
.kw ENDGUESS
statement, or
.point
from within a remote block to the statement following the
.kw EXECUTE
statement that invoked the remote block.
.endpoint
.np
The
.kw EXIT
statement may be used to cause a transfer of control to the first
executable statement that follows the terminal statement of the
block which contains it.
Examples of such terminal statements are
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
.kw EXIT
statement is an extension to the FORTRAN 77 language.
.exam begin
      LOOP
          WRITE( UNIT=*, FMT='(A)' ) 'Enter a number'
          READ( UNIT=*, FMT='(F10.4)', IOSTAT=IOS ) X
          IF( IOS .NE. 0 ) EXIT
          IF( X .LT. 0 ) EXIT
          PRINT *, X, SQRT( X )
      END LOOP
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
