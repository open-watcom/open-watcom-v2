cx      character*20    filename
cx      len = strlen( loc( name ) )
cx      call memcpy( loc( filename ), loc( name ), len )
cx      open( unit=1, file=filename, form='formatted' )
cx      read( 1, '(A)' ) record
cx      close( unit=1 )
