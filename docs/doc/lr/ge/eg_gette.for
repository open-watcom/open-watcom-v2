	include 'graphapi.fi'
	include 'graph.fi'

	record /xycoord/ concat, extent(4)

	call _setvideomode( _VRES16COLOR )
	call _grtext( 100, 100, 'hot'c )
	call _gettextextent( 100, 100, 'hot'c,
     +			     concat, extent )
	call _polygon( _GBORDER, 4, extent )
	call _grtext( concat.xcoord, concat.ycoord,
     +		      'dog'c )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
