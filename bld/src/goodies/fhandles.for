*  FHANDLES.FOR
*
*  This FORTRAN program grows the number of file handles so
*  more than 16 files can be opened. This program
*  illustrates the interaction between GROWHANDLES and
*  the DOS 5.0 file system. If you are running a network
*  such as Novell's NetWare, this will also affect the
*  number of available file handles. In the actual trial,
*  FILES=40 was specified in CONFIG.SYS.

*  Compile/Link: set finclude=\watcom\src\fortran
*		 wfl[386] fhandles

*     Get proper typing information from include file
      include 'fsublib.fi'

      integer i, j, maxh, maxo
      integer tmpfile
      integer units(7:57)

      do i = 25, 40
*	Count 5 for stdin, stdout, stderr, stdaux,
*	and stdprn
	print 100, 5 + i
	maxh = growhandles( 5 + i )
	print *, 'Growhandles=',maxh
	maxo = 0
	do j = 7, 7 + maxh
	    print *, 'Attempting file', j
	    units(j) = tmpfile(j)
	    if( units(j) .eq. 0 )goto 10
	    maxo = maxo + 1
	enddo
10	print 101, maxo, maxh
	do j = 7, 7 + maxo
	    close( units(j) )
	enddo
      enddo
100   format('Trying for ',I2,' handles... ',$)
101   format(I2,'/',I2,' temp files opened')
      end

      integer function tmpfile( un )
      integer un, ios
      open( unit=un, status='SCRATCH', iostat=ios )
      if( ios .eq. 0 )then
	write( unit=un, fmt='(I2)', err=20 ) un
	tmpfile = un
	return
      endif
20    tmpfile = 0
      end
