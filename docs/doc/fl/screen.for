      CHARACTER*1 SCREEN(:,:), CH
      N = 80*25
      ALLOCATE( SCREEN(0:1,0:N-1), LOCATION='B8000000'x )
      CH = ')'
      DO I = 0, N-1
	  IF( I/80*80 .EQ. I) CH = CHAR( ICHAR( CH ) + 1 )
	  SCREEN(0,I) = CH
      ENDDO
      DEALLOCATE( SCREEN, STAT=I )
      PRINT *, 'Status', I
      END
