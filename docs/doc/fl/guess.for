      LOOP : PRLOOP
	  GUESS
	      LINE = LINE + 1
	      READ *, ICODE, X
	      AT END, EXIT :PRLOOP
	      IF( ICODE .NE. 1 )EXIT
	      LINE = LINE + 1
	      READ *, ICODE, Y
	      AT END, EXIT
	      IF( ICODE .NE. 2 )EXIT
	      PRINT *, X, Y
	  ADMIT
	      PRINT *, 'INVALID SEQUENCE: LINE =', LINE
	      EXIT :PRLOOP
	  END GUESS
      END LOOP
      END
