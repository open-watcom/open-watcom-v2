	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _VRES16COLOR )
	call _grtext( 200, 100, 'WATCOM'c )
	call _setpixel( 200, 100 )
	call _settextalign( _CENTER, _HALF )
	call _grtext( 200, 200, 'Graphics'c )
	call _setpixel( 200, 200 )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
