	include 'graphapi.fi'
	include 'graph.fi'
	include 'pgapi.fi'
	include 'pg.fi'

	integer NUM_VALUES
	parameter (NUM_VALUES=4)

	real x( NUM_VALUES )
     +		    / 5, 25, 45, 65 /
	real y( NUM_VALUES )
     +		    / 5, 45, 25, 65 /
	integer*1 diamond( 8 )
     +		   / '10'x, '28'x, '44'x, '82'x,
     +		     '44'x, '28'x, '10'x, '00'x /

	record /chartenv/ env
	integer*1 old_def( 8 )

	call _setvideomode( _VRES16COLOR )
	call _pg_initchart()
	call _pg_defaultchart( env,
     +		      _PG_SCATTERCHART, _PG_POINTANDLINE )
	env.maintitle.title = 'Scatter Chart'c
	! change asterisk character to diamond
	call _pg_getchardef( ichar( '*' ), old_def )
	call _pg_setchardef( ichar( '*' ), diamond )
	call _pg_chartscatter( env, x, y, NUM_VALUES )
	call _pg_setchardef( ichar( '*' ), old_def )
	pause
	call _setvideomode( _DEFAULTMODE )
	end
