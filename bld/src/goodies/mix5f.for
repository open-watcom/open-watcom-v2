* MIX5F.FOR - This program shows how a FORTRAN common
*	      block can be accessed from C.
*
* Compile/Link: wfl[386] mix5f mix5c.obj /fe=mix5

	program mix5f
	external put
	common/cblk/i,j

	i=12
	j=10
	call put
	print *, 'i = ', i
	print *, 'j = ', j
	end
