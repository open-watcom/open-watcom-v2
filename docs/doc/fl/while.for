      X = 0.0
      WHILE( X .LT. 10.0 )DO
	  PRINT *, X, SQRT( X )
	  X = X + 1.0
      ENDWHILE
      DO 10 WHILE( X .LT. 20.0 )
	  PRINT *, X, SQRT( X )
	  X = X + 1.0
10    CONTINUE
      END
