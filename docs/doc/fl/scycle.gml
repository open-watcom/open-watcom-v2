.section CYCLE Statement
.*
.mext begin
      CYCLE   [: block-label]
.mext end
.np
The
.kw CYCLE
statement may be used to cause a transfer of control from within a
loop to the terminal statement of a corresponding
.kw DO
.ct ,
.kw DO WHILE
.ct ,
.kw WHILE
or
.kw LOOP
statement.
If
.id block-label
is present then control is transferred to the terminal statement of
the block identified by that block label.
The
.kw CYCLE
statement is an extension to the FORTRAN 77 language.
.exam begin
      LOOP
          WRITE( UNIT=*, FMT='(A)' ) 'Enter a number'
          READ( UNIT=*, FMT='(F10.4)', IOSTAT=IOS ) X
          IF( IOS .NE. 0 ) CYCLE
          IF( X .LT. 0 ) EXIT
          PRINT *, X, SQRT( X )
      END LOOP
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
