	include 'graphapi.fi'
	include 'graph.fi'
	include 'pgapi.fi'
	include 'pg.fi'

	integer NUM_VALUES
	parameter (NUM_VALUES=4)
	integer NUM_SERIES
	parameter (NUM_SERIES=2)

	integer*4 labels( NUM_SERIES )
	real x( NUM_SERIES, NUM_VALUES )
     +		    / 5, 15, 30, 40, 10, 20, 30, 45 /
	real y( NUM_SERIES, NUM_VALUES )
     +		    / 10, 15, 30, 45, 40, 30, 15, 5 /

	record /chartenv/ env

	labels( 1 ) = loc( 'Jan'c )
	labels( 2 ) = loc( 'Feb'c )

	call _setvideomode( _VRES16COLOR )
	call _pg_initchart()
	call _pg_defaultchart( env,
     +		      _PG_SCATTERCHART, _PG_POINTANDLINE )
	env.maintitle.title = 'Scatter Chart'c
	call _pg_analyzescatterms( env, x, y, NUM_SERIES,
     +			NUM_VALUES, NUM_VALUES, labels )
	! display x-axis labels with 2 decimal places
	env.xaxis.autoscale = 0
	env.xaxis.ticdecimals = 2
	call _pg_chartscatterms( env, x, y, NUM_SERIES,
     +			NUM_VALUES, NUM_VALUES, labels )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
