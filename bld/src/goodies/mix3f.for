* MIX3F.FOR - This FORTRAN program calls a function written
*	      in C that passes back a string.
*
* Compile/Link: wfl[386] mix3f mix3c.obj /fe=mix3

	program mix3f

	character*80 sendstr
	character*80 cstring

	cstring = sendstr()
	print *, cstring(1:lentrim(cstring))
	end
