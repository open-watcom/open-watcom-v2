	include 'graphapi.fi'
	include 'graph.fi'
	include 'pgapi.fi'
	include 'pg.fi'

	integer NUM_VALUES
	parameter (NUM_VALUES=4)

	integer*4 categories( NUM_VALUES )
	real values( NUM_VALUES )
     +		    / 20, 45, 30, 25 /
	integer*1 bricks( 8 )
     +		    / 'ff'x, '80'x, '80'x, '80'x,
     +		      'ff'x, '08'x, '08'x, '08'x /

	record /chartenv/ env
	record /paletteentry/ pal( _PG_PALETTELEN )
	integer i

	categories( 1 ) = loc( 'Jan'c )
	categories( 2 ) = loc( 'Feb'c )
	categories( 3 ) = loc( 'Mar'c )
	categories( 4 ) = loc( 'Apr'c )

	call _setvideomode( _VRES16COLOR )
	call _pg_initchart()
	call _pg_defaultchart( env,
     +			_PG_COLUMNCHART, _PG_PLAINBARS )
	env.maintitle.title = 'Column Chart'c
	! get default palette and change 1st entry
	call _pg_getpalette( pal )
	pal( 2 ).color = 12
	do i = 1, 8
	    pal( 2 ).fill( i ) = bricks( i )
	enddo
	! use new palette
	call _pg_setpalette( pal )
	call _pg_chart( env, categories,
     +			values, NUM_VALUES )
	! reset palette to default
	call _pg_resetpalette()
	pause
	call _setvideomode( _DEFAULTMODE )
	end
