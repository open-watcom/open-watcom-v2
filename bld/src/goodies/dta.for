* DTA.FOR
* This program demonstrates the use of the FINTR
* function to list the files of the current directory.
* Interrupt 21 Functions for FIND FIRST, FIND NEXT,
* and GET DTA are used.

* Compile & Link: set finclude=\watcom\src\fortran\dos
*		  wfl386 /l=dos4g dta

*$pragma aux GetDS = "mov ax,ds" value [ax]

	program dta
	implicit integer*2 (i-n)
	integer*2 res
	integer*2 GetDS
	integer*4 dir, addr
	integer*1 dta(:)
	character fname*1(12), fname2*12
	equivalence (fname, fname2)

* DTA is declared as a FAR array.  When referencing an array
* element, the pointer to the array is a FAR pointer.  With a
* character variable, the result is a pointer to a string
* control block (SCB).	The run-time library expects the SCB
* to contain a near pointer.  To get around the problem, we
* define the DTA as a byte array, then use the CHAR function
* to get the character equivalent for printing a filename.

*$pragma array dta far

	include 'dos.fi'
*
* Listing of current directory
*
	call fsystem( 'dir/w *.*'//char(0) )
	dir = loc( '*.*'//char(0) )

	i = 0
10	i = i + 1
	if( i .eq. 1 )then
*
* Find first file
*
	    AH = '4E'x
	    ECX = 0
	    DS = GetDS()
	    EDX = dir
	else
*
* Find next file
*
	    AH = '4F'x
	endif
	call fintr( '21'x, regs )
	res = AX

	if( res .eq. 0 )then
*
* Extract filename from DTA
*
	    AH = '2F'x
	    call fintr( '21'x, regs )

	    addr = ISHL( IAND( INT( ES ), '0000FFFF'x ), 16 )
	    addr = IOR( addr, IAND( INT( BX ), '0000FFFF'x ) )
	    allocate( dta(0:42), location=addr )
	    fname2 = ' '
	    do j = 30, 41
		if( dta(j) .eq. 0 ) goto 20
		fname(j - 29) = char( dta(j) )
	    enddo
20	    print *, fname2
	    deallocate( dta )
	    goto 10
	endif

	end
