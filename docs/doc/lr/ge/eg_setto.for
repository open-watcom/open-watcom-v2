	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _VRES16COLOR )
	call _grtext( 200, 100, 'WATCOM'c )
	call _settextorient( 1, 1 )
	call _grtext( 200, 200, 'Graphics'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
