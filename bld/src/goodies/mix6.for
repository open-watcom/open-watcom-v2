* MIX6.FOR - This FORTRAN program calls the C
*	     printf function.

* Compile/Link: wfl[386] mix6

*$pragma aux printf "!_" parm (value) caller []

	program mix6

	character cr/z0d/, nullchar/z00/

	call printf( 'Value is %ld.'//cr//nullchar, 47 )
	end
