	include 'graphapi.fi'
	include 'graph.fi'

	integer*2 old_shape

	old_shape = _gettextcursor()
	call _settextcursor( '0007'x )
	call _outtext(
     +	     char(10)//'Block cursor'c )
	pause
	call _settextcursor( '0407'x )
	call _outtext(
     +	     char(10)//'Half height cursor'c )
	pause
	call _settextcursor( '2000'x )
	call _outtext(
     +	     char(10)//'No cursor'c )
	pause
	end
