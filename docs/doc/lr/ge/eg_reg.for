	include 'graphapi.fi'
	include 'graph.fi'

	integer i, n
	character*10 buff

	call _setvideomode( _VRES16COLOR )
	n = _registerfonts( '*.fon'c )
	do i = 0, n - 1
	    write( buff, '(''n'', i2.2, a1 )' ) i, char(0)
	    call _setfont( buff )
	    call _moveto( 100, 100 )
	    call _outgtext( 'WATCOM Graphics'c )
	    pause
	    call _clearscreen( _GCLEARSCREEN )
	enddo
	call _unregisterfonts()
	call _setvideomode( _DEFAULTMODE )
	end
