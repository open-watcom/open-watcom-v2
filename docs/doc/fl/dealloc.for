      INTEGER*1 A(:)
*     N = 592953
      DO I = 1 000 000, 3 000 000
	  ALLOCATE( A(I), STAT=IALLOC )
	  IF( IALLOC .NE. 0 ) EXIT
	  A(I) = I
	  DEALLOCATE( A, STAT=IDEAL )
	  IF( IDEAL .NE. 0 ) PRINT *, 'DEALLOCATE error', I
      ENDDO
      PRINT *, 'ALLOCATE failed', I
      END
