* MIX1F.FOR - This FORTRAN function accepts three integer
*	      arguments and returns their maximum.

* Compile: wfc[386] mix1f.for

	integer function tmax3( arga, argb, argc )
	integer arga, argb, argc

	tmax3 = arga
	if ( argb .gt. tmax3 ) tmax3 = argb
	if ( argc .gt. tmax3 ) tmax3 = argc
	end
