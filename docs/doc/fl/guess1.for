      CHARACTER CH
      READ *, CH
      GUESS
	  IF( CH .LT. 'a' )QUIT
	  IF( CH .GT. 'z' )QUIT
	  PRINT *, 'Lower case letter'
      ADMIT
	  IF( CH .LT. 'A' )QUIT
	  IF( CH .GT. 'Z' )QUIT
	  PRINT *, 'Upper case letter'
      ADMIT
	  IF( CH .LT. '0' )QUIT
	  IF( CH .GT. '9' )QUIT
	  PRINT *, 'Digit'
      ADMIT
	  PRINT *, 'Special character'
      END GUESS
      END
