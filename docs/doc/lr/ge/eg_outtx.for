	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _TEXTC80 )
	call _settextposition( 10, 30 )
	call _outtext( 'WATCOM Graphics'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
