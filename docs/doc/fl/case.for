      CHARACTER CH
      READ *, CH
      SELECT CASE ( CH )
      CASE( 'a' : 'z' )
	  IF( CH .EQ. 'c' )EXIT
	  PRINT *, 'Lower case letter'
      CASE( 'A' : 'Z' )
	  PRINT *, 'Upper case letter'
      CASE( '0' : '9' )
	  PRINT *, 'Digit'
      CASE DEFAULT
	  PRINT *, 'Special letter'
      END SELECT
      END
