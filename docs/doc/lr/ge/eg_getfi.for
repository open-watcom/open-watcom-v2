	include 'graphapi.fi'
	include 'graph.fi'

	record /_fontinfo/ info

	integer width

	call _setvideomode( _VRES16COLOR )
	call _getfontinfo( info )
	call _moveto( 100, 100 )
	call _outgtext( 'WATCOM Graphics'c )
	width = _getgtextextent( 'WATCOM Graphics'c )
	call _rectangle( _GBORDER, 100, 100,
     +			 100 + width, 100 + info.pixheight )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
