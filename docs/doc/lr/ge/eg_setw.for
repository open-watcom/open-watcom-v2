	include 'graphapi.fi'
	include 'graph.fi'

	call _setvideomode( _MAXRESMODE )
	call draw_house( 'Default window'c )
	call _setwindow( .TRUE., -0.5, -0.5, 1.5, 1.5 )
	call draw_house( 'Larger window'c )
	call _setwindow( .TRUE., 0.0, 0.0, 0.5, 1.0 )
	call draw_house( 'Left side'c )
	call _setvideomode( _DEFAULTMODE )
	end

	subroutine draw_house( msg )

	include 'graph.fi'
	character*80 msg

	call _clearscreen( _GCLEARSCREEN )
	call _outtext( msg )
	call _rectangle_w( _GBORDER, 0.2, 0.1, 0.8, 0.6 )
	call _moveto_w( 0.1, 0.5 )
	call _lineto_w( 0.5, 0.9 )
	call _lineto_w( 0.9, 0.5 )
	call _arc_w( 0.4, 0.5, 0.6, 0.3,
     +		     0.6, 0.4, 0.4, 0.4 )
	call _rectangle_w( _GBORDER, 0.4, 0.1, 0.6, 0.4 )
	pause
	end
