*  MIX2F.FOR - This FORTRAN program calls a C function to
*	       compute the max of three numbers.
*
* Compile/Link: wfl[386] mix2f mix2c.obj /fe=mix2

*$pragma aux tmax3 "*_" parm (value)

	program mix2f

	integer*4  tmax3
	integer*4  result
	integer*4  i, j, k

	i = -1
	j = 12
	k = 5
	result = tmax3( i, j, k )
	print *, 'Maximum is ', result
	end
