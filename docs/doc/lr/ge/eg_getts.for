	include 'graphapi.fi'
	include 'graph.fi'

	record /textsettings/ ts

	call _setvideomode( _VRES16COLOR )
	call _gettextsettings( ts )
	call _grtext( 100, 100, 'WATCOM'c )
	call _setcharsize( 2 * ts.height, 2 * ts.width )
	call _grtext( 100, 300, 'Graphics'c )
	call _setcharsize( ts.height, ts.width )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
