	include 'graphapi.fi'
	include 'graph.fi'

	record /xycoord/ old_vec

	call _setvideomode( _VRES16COLOR )
	old_vec = _getgtextvector()
	call _setgtextvector( 0, -1 )
	call _moveto( 100, 100 )
	call _outgtext( 'WATCOM Graphics'c )
	call _setgtextvector( old_vec.xcoord, old_vec.ycoord )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
