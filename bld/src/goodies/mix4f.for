* MIX4F.FOR - This FORTRAN program calls a function written
*	      in C and passes it a string.
*
* Compile/Link: wfl[386] mix4f mix4c.obj /fe=mix4

*$pragma aux cstr "!_" parm (value)

	program mix4f

	character*80 forstring

	forstring = 'This is a FORTRAN string'//char(0)
	call cstr( forstring )
	end
