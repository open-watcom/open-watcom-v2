	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _VRES16COLOR )
	call _grtext( 100, 100, 'WATCOM'c )
	call _setcharspacing( 20 )
	call _grtext( 100, 300, 'Graphics'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
