      LOOP
	  WRITE( UNIT=*, FMT='(A)' ) 'Enter a number'
	  READ( UNIT=*, FMT='(F10.4)', IOSTAT=IOS ) X
	  IF( IOS .NE. 0 ) CYCLE
	  IF( X .LT. 0 ) EXIT
	  PRINT *, X, SQRT( X )
      ENDLOOP
      PRINT *, 'Done'
      END
