      OPEN( UNIT=1, FILE='PEOPLE.DAT', BLOCKSIZE=512 )
*     OPEN( UNIT=1, FILE='PEOPLE.DAT' )
      NBL = -1
      INQUIRE( FILE='PEOPLE.DAT', BLOCKSIZE=NBL )
      PRINT *, 'File PEOPLE.DAT blocksize is', NBL
      INQUIRE( UNIT=1, BLOCKSIZE=NBL )
      PRINT *, 'Unit 1 blocksize is', NBL
      CLOSE( UNIT=1 )
      INQUIRE( UNIT=5, BLOCKSIZE=NBL )
      PRINT *, 'Unit 5 blocksize is', NBL
      INQUIRE( UNIT=6, BLOCKSIZE=NBL )
      PRINT *, 'Unit 6 blocksize is', NBL
      OPEN( UNIT=7, FILE='PEOPLE.DAT', BLOCKSIZE=63*1024 )
      NBL = -1
      INQUIRE( FILE='PEOPLE.DAT', BLOCKSIZE=NBL )
      PRINT *, 'File PEOPLE.DAT blocksize is', NBL
      INQUIRE( UNIT=7, BLOCKSIZE=NBL )
      PRINT *, 'Unit 7 blocksize is', NBL
      CLOSE( UNIT=7 )
      RETURN
      END
