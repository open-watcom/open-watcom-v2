	include 'graphapi.fi'
	include 'graph.fi'

	character*30 name

	call _setvideomode( _TEXTC80 )
	call _settextposition( 2, 1 )
	call _displaycursor( _GCURSORON )
	call _outtext( 'Cursor ON'//char(10)//char(10)
     +		       //'Enter your name >'c )
	read( *, '(a30)' ) name
	call _displaycursor( _GCURSOROFF )
	call _settextposition( 6, 1 )
	call _outtext( 'Cursor OFF'//char(10)//char(10)
     +		       //'Enter your name >'c )
	read( *, '(a30)' ) name
	call _setvideomode( _DEFAULTMODE )
	end
