c$noextensions

        include 'graphapi.fi'
        include 'pgapi.fi'

        program pgdemo

        ! Initialize graphics library and presentation graphics
        ! system and then display several sample charts.

        include 'graph.fi'
        include 'pg.fi'
        include 'pgdemo.fi'

        integer mode, fgetcmd, cmdlen, atoi, i
        character*128 cmdline

        data values / 20, 45, 30, 25, 30, 25, 40, 15 /
        data x / 5, 15, 30, 40, 10, 20, 30, 45 /
        data y / 10, 15, 30, 45, 40, 30, 15, 5 /
        data explode / 1, 0, 0, 0 /

        categories( 1 ) = loc( 'Jan'//char(0) )
        categories( 2 ) = loc( 'Feb'//char(0) )
        categories( 3 ) = loc( 'Mar'//char(0) )
        categories( 4 ) = loc( 'Apr'//char(0) )
        labels( 1 ) = loc( 'Apples'//char(0) )
        labels( 2 ) = loc( 'Oranges'//char(0) )

        cmdlen = fgetcmd( cmdline )
        if( cmdlen .ne. 0 )then
            do i = 1, cmdlen
                if( cmdline(i:i) .ne. ' ' ) exit
            enddo
            if( i .gt. cmdlen )then
                mode = _MAXRESMODE
            else
                mode = atoi( cmdline(i:cmdlen) )
            endif
        else
            mode = _MAXRESMODE
        endif
        if( _setvideomode( mode ) .eq. 0 )then
            print *, 'Cannot initialize video mode'
            stop
        endif
        call _getvideoconfig( vconfig )
        xmax = vconfig.numxpixels
        ymax = vconfig.numypixels
        xmid = xmax / 2
        ymid = ymax / 2
        call _pg_initchart()
        call simple_chart()
        call press_key()
        call _clearscreen( _GCLEARSCREEN )
        call more_charts()
        call press_key()
        call _clearscreen( _GCLEARSCREEN )
        call scatter_charts()
        call press_key()
        call _setvideomode( _DEFAULTMODE )
        end


        subroutine press_key()

        ! Display a message and wait for the user to press a key

        include 'graph.fi'
        include 'pg.fi'
        include 'pgdemo.fi'

        integer len
        character*(40) msg
        record /_fontinfo/ info

        call _getfontinfo( info )
        msg = ' Press Enter key '//char(0)
        len = _getgtextextent( msg )
        call _setcolor( 4 )
        call _rectangle( _GFILLINTERIOR, xmax - len , 0,
     +                                   xmax - 1, info.pixheight + 2 )
        call _setcolor( 15 )
        call _moveto( xmax - len, 1 )
        call _outgtext( msg )
        pause
        end


        subroutine simple_chart()

        ! Display a simple column chart

        include 'graph.fi'
        include 'pg.fi'
        include 'pgdemo.fi'

        record /chartenv/ env

        call _pg_defaultchart( env, _PG_COLUMNCHART, _PG_PLAINBARS )
        env.maintitle.title = 'Column Chart'//char(0)
        call _pg_chart( env, categories, values, NUM_VALUES )
        end


        subroutine more_charts()

        ! Display several sample charts on different areas of the screen

        include 'graph.fi'
        include 'pg.fi'
        include 'pgdemo.fi'

        record /chartenv/ env

        ! use only left half of screen
        call _pg_defaultchart( env, _PG_COLUMNCHART, _PG_PLAINBARS )
        env.maintitle.title = 'Column Chart'//char(0)
        env.chartwindow.x1 = 0
        env.chartwindow.y1 = 0
        env.chartwindow.x2 = xmid - 1
        env.chartwindow.y2 = ymax - 1
        call _pg_chart( env, categories, values, NUM_VALUES )

        ! use top right corner
        call _pg_defaultchart( env, _PG_BARCHART, _PG_PLAINBARS )
        env.maintitle.title = 'Bar Chart'//char(0)
        env.chartwindow.x1 = xmid
        env.chartwindow.y1 = 0
        env.chartwindow.x2 = xmax - 1
        env.chartwindow.y2 = ymid - 1
        env.legend.place = _PG_BOTTOM
        call _pg_chartms( env, categories, values, NUM_SERIES,
     +                    NUM_VALUES, NUM_VALUES, labels )

        ! use lower right corner
        call _pg_defaultchart( env, _PG_PIECHART, _PG_NOPERCENT )
        env.maintitle.title = 'Pie Chart'//char(0)
        env.chartwindow.x1 = xmid
        env.chartwindow.y1 = ymid
        env.chartwindow.x2 = xmax - 1
        env.chartwindow.y2 = ymax - 1
        call _pg_chartpie( env, categories,
     +                     values, explode, NUM_VALUES )
        end


        subroutine scatter_charts()

        ! Display two scatter charts beside each other;
        ! one using default options, the other customized

        include 'graph.fi'
        include 'pg.fi'
        include 'pgdemo.fi'

        record /chartenv/ env

        ! use left half of screen
        call _pg_defaultchart( env, _PG_SCATTERCHART, _PG_POINTANDLINE )
        env.maintitle.title = 'Scatter Chart'//char(0)
        env.chartwindow.x1 = 0
        env.chartwindow.y1 = 0
        env.chartwindow.x2 = xmid - 1
        env.chartwindow.y2 = ymax - 1
        call _pg_chartscatterms( env, x, y, NUM_SERIES,
     +                      NUM_VALUES, NUM_VALUES, labels )

        ! use right half of screen
        call _pg_defaultchart( env, _PG_SCATTERCHART, _PG_POINTANDLINE )
        env.maintitle.title = 'Scatter Chart'//char(0)
        env.subtitle.title = '(1991 Production)'//char(0)
        env.chartwindow.x1 = xmid
        env.chartwindow.y1 = 0
        env.chartwindow.x2 = xmax - 1
        env.chartwindow.y2 = ymax - 1
        env.chartwindow.background = 2
        env.datawindow.background = 8
        env.legend.legendwindow.background = 8
        env.legend.place = _PG_BOTTOM
        env.maintitle.justify = _PG_LEFT
        call _pg_analyzescatterms( env, x, y, NUM_SERIES,
     +                      NUM_VALUES, NUM_VALUES, labels )
        env.yaxis.autoscale = 0
        env.yaxis.scalemin = 0.0
        env.yaxis.scalemax = 60.0
        env.yaxis.ticinterval = 20.0
        env.yaxis.grid = 1
        env.yaxis.gridstyle = 2
        call _pg_chartscatterms( env, x, y, NUM_SERIES,
     +                      NUM_VALUES, NUM_VALUES, labels )
        end


        integer function atoi( str )

        character*(*) str
        integer i, j

        atoi = 0
        do i = 1, len( str )
            if( str(i:i) .ne. ' ' ) quit
        end do
        do j = i, len( str )
            if( ( str(j:j) .lt. '0' ) .or. ( str(j:j) .gt. '9' ) ) quit
            atoi = atoi * 10 + ( ichar( str(j:j) ) - ichar( '0' ) )
        end do

        end
