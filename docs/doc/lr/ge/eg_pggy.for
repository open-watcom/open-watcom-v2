	include 'graphapi.fi'
	include 'graph.fi'
	include 'pgapi.fi'
	include 'pg.fi'

	integer NUM_VALUES
	parameter (NUM_VALUES=4)

	integer*4 categories( NUM_VALUES )
	real values( NUM_VALUES )
     +		    / 20, 45, 30, 25 /

	record /chartenv/ env
	integer*2 style( _PG_PALETTELEN )

	categories( 1 ) = loc( 'Jan'c )
	categories( 2 ) = loc( 'Feb'c )
	categories( 3 ) = loc( 'Mar'c )
	categories( 4 ) = loc( 'Apr'c )

	call _setvideomode( _VRES16COLOR )
	call _pg_initchart()
	call _pg_defaultchart( env,
     +			_PG_COLUMNCHART, _PG_PLAINBARS )
	env.maintitle.title = 'Column Chart'c
	! turn on yaxis grid, and use style 2
	env.yaxis.grid = 1
	env.yaxis.gridstyle = 2
	! get default style-set and change entry 2
	call _pg_getstyleset( style )
	style( 3 ) = '8888'x
	! use new style-set
	call _pg_setstyleset( style )
	call _pg_chart( env, categories,
     +			values, NUM_VALUES )
	! reset style-set to default
	call _pg_resetstyleset()
	pause
	call _setvideomode( _DEFAULTMODE )
	end
