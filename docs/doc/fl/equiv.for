      REAL A(2,10),B(20),C(2,2,5)
      EQUIVALENCE (A(5),B(1)),(B(1),C(1))
      DO I = 1, 2
	  DO J = 1, 10
	      A(I,J) = -(I+J)
	  ENDDO
      ENDDO
      DO I = 1, 20
	  B(I) = I
      ENDDO
      DO I = 1, 2
	  PRINT '(10F6.2)', (A( I, J ), J=1,10)
      ENDDO
      DO I = 1, 2
	DO J = 1, 2
	  PRINT '(10F6.2)', (C( I, J, K ), K=1,5)
	ENDDO
      ENDDO
      END
