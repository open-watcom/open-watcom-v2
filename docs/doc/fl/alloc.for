      DIMENSION A(:)
*     N = 592953
*     N = 600000
      N = 6000
      PRINT '(A,X,Z8)', 'N location is', LOC( N )
      IF( ALLOCATED( A ) )THEN
	  PRINT *, 'A is allocated'
      ELSE
	  PRINT *, 'A is not allocated'
      ENDIF
      ALLOCATE( A(N), STAT=IALLOC )
      IF( ALLOCATED( A ) )THEN
	  PRINT *, 'A is allocated'
	  PRINT '(A,X,Z8)', 'A location is', LOC( A )
      ELSE
	  PRINT *, 'A is not allocated'
      ENDIF
      PRINT *, 'ALLOCATE status =', IALLOC
      IF( IALLOC .NE. 0 ) STOP
      DO I = 1, N, 1
	  A(I) = I
      ENDDO
      DO I = 1, N, 1
	  IF( A(I) .NE. I )PRINT *, 'Error index=', I
      ENDDO
      END
