	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _VRES16COLOR )
	call _grtext( 200, 100, 'WATCOM'c )
	call _settextpath( _PATH_DOWN )
	call _grtext( 200, 200, 'Graphics'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
