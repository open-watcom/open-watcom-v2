* BINDATA.FOR - This program demonstrates how to read a
* binary data file that does not have any defined records.

      program bindata

      integer BinArray(20)
      integer i

      open( unit=1, file='bindata.fil',
     +	    access='sequential',
     +	    form='unformatted',
     +	    recordtype='fixed' )

*  Read 20 integers from the binary data file
      do i = 1, 20
	 read( 1 ) BinArray( i )
      end do

*  Write the extracted values to standard output
      do i = 1, 20
	 write( *, * ) BinArray( i )
      end do
      end
