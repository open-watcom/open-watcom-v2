	include 'graphapi.fi'
	include 'graph.fi'

	integer old_col
	integer old_bk

	call _setvideomode( _TEXTC80 )
	old_col = _gettextcolor()
	old_bk = _getbkcolor()
	call _settextcolor( 7 )
	call _setbkcolor( _BLUE )
	call _outtext( ' WATCOM '//char(10)//
     +		       'Graphics'c )
	call _settextcolor( old_col )
	call _setbkcolor( old_bk )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
