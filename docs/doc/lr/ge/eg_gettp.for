	include 'graphapi.fi'
	include 'graph.fi'

	record /rccoord/ old_pos

	call _setvideomode( _TEXTC80 )
	old_pos = _gettextposition()
	call _settextposition( 10, 40 )
	call _outtext( 'WATCOM Graphics'c )
	call _settextposition( old_pos.row, old_pos.col )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
