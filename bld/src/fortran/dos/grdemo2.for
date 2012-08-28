c$noextensions

        include 'graphapi.fi'


        block data

        integer NUMSECT
        parameter (NUMSECT=10)

        integer*1 Masks(0:7,0:7)
        common /masks/ Masks

        integer Values(0:NUMSECT-1)
        common /values/ Values

        character Main_Title*21
        character Y_Axis_Title*18
        character X_Axis_Title*11
        common /titles/ Main_Title, Y_Axis_Title, X_Axis_Title

        data X_Axis_Title/'Fiscal Year'/
        data Y_Axis_Title/'Net Gain (x $1000)'/
        data Main_Title/'Business Applications'/
        data Values/ 20, 30, 40, 35, 50, 60, 75, 70, 80, 90/
        data ((Masks(i,j), i = 0, 7), j = 0, 7 )/
     +           'ff'x, '81'x, 'ff'x, '42'x, 'ff'x, '24'x, 'ff'x, '18'x,
     +           '81'x, '42'x, '24'x, '18'x, '18'x, '24'x, '42'x, '81'x,
     +           '99'x, '18'x, '24'x, 'c3'x, 'c3'x, '24'x, '18'x, '99'x,
     +           'aa'x, '55'x, 'aa'x, '55'x, 'aa'x, '55'x, 'aa'x, '55'x,
     +           '88'x, '44'x, '22'x, '11'x, '88'x, '44'x, '22'x, '11'x,
     +           '18'x, 'db'x, '3c'x, '18'x, '18'x, '3c'x, 'db'x, '18'x,
     +           '11'x, '22'x, '44'x, '88'x, '11'x, '22'x, '44'x, '88'x,
     +           '18'x, '18'x, '18'x, 'ff'x, 'ff'x, '18'x, '18'x, '18'x/

        end


        program grdemo2

        ! This program draws bar and pie graphs for the
        ! data specified above.

        include 'graph.fi'

        record /videoconfig/ VC
        common VC

        if( _setvideomode( _MAXCOLORMODE ) .eq. 0 )then
            print *, 'No graphics adapter present'
            stop
        endif
        call _getvideoconfig( VC ) ! fill videoconfig structure
        call NewColours()

        call Title()
        call BarGraph()
        call PieGraph()

        call _settextposition( VC.numtextrows, VC.numtextcols - 18 )
        call _outtext( 'Press Enter key...'//char(0) )
        pause
        call _setvideomode( _DEFAULTMODE )
        end


        subroutine NewColours()

        ! Select a new colour set

        include 'graph.fi'

        integer AxisColour, TitleColour, BorderColour
        common /colours/ AxisColour, TitleColour, BorderColour

        record /videoconfig/ VC
        common VC

        integer i
        integer newcolrs(0:15)
        data newcolrs/_BLACK, _LIGHTCYAN, _LIGHTMAGENTA, _BRIGHTWHITE,
     +                _GREEN, _LIGHTBLUE, _GRAY, _LIGHTRED,
     +                _CYAN, _YELLOW, _RED, _LIGHTGREEN,
     +                _BROWN, _MAGENTA, _BLUE, _WHITE/

        if( VC.adapter .ge. _MCGA )then
            do i = 0, 15
                call _remappalette( i, newcolrs( i ) )
            enddo
        endif
        if( VC.numcolors .eq. 2 )then
            AxisColour = 1
            TitleColour = 1
            BorderColour = 1
        else
            AxisColour = 1
            TitleColour = 2
            BorderColour = 3
        endif
        end


        subroutine Title()

        ! Draw main title and graph boxes.

        include 'graph.fi'

        integer AxisColour, TitleColour, BorderColour
        common /colours/ AxisColour, TitleColour, BorderColour

        character Main_Title*21
        character Y_Axis_Title*18
        character X_Axis_Title*11
        common /titles/ Main_Title, Y_Axis_Title, X_Axis_Title

        call _setcolor( BorderColour )
        call _settextalign( _CENTER, _TOP )
        call _setcharsize_w( 0.08, 1.0 / len( Main_Title ) )
        call _grtext_w( 0.5, 1.0, Main_Title//char(0) )
        call _rectangle_w( _GBORDER, 0.00, 0.00, 0.49, 0.90 )  ! left half
        call _rectangle_w( _GBORDER, 0.51, 0.00, 1.00, 0.90 )  ! right half
        end


        subroutine DoAxes( xleft, ybottom, xlen, ylen )

        ! Draw the axes of bar graph

        include 'graph.fi'

        record /videoconfig/ VC
        common VC

        character Main_Title*21
        character Y_Axis_Title*18
        character X_Axis_Title*11
        common /titles/ Main_Title, Y_Axis_Title, X_Axis_Title

        integer AxisColour, TitleColour, BorderColour
        common /colours/ AxisColour, TitleColour, BorderColour

        real xleft, ybottom, xlen, ylen
        real xright, ytop, y, yinc

        xright = xleft + xlen
        ytop   = ybottom + ylen

        call _setcolor( AxisColour )
        call _moveto_w( xleft,  ytop )
        call _lineto_w( xleft,  ybottom )
        call _lineto_w( xright, ybottom )

        ! Draw the tick marks on the y-axis

        yinc = ylen / 10
        y = ybottom
        dowhile( y .lt. ytop )
            call _moveto_w( xleft, y )
            call _lineto_w( xleft - 0.01, y )
            y = y + yinc
        enddo

        ! Draw the x-axis and y-axis titles

        call _setcolor( TitleColour )
        call _settextalign( _CENTER, _HALF )
        call _settextorient( 0, 1 )
        call _setcharsize_w( 0.06, ( ylen * VC.numypixels ) /
     +                       ( len( Y_Axis_Title ) *  VC.numxpixels ) )
        call _grtext_w( xleft - 0.05, ybottom + ylen / 2,
     +                       Y_Axis_Title//char(0) )
        call _setcharsize_w( 0.06, xlen / len( X_Axis_Title ) )
        call _settextorient( 1, 0 )
        call _grtext_w( xleft + xlen / 2, ybottom - 0.05,
     +                          X_Axis_Title//char(0) )
        end


        subroutine DoBars( xleft, ybottom, xlen, ylen )

        ! Draw bars of graph.

        include 'graph.fi'

        record /videoconfig/ VC
        common VC

        integer NUMSECT
        parameter (NUMSECT=10)

        integer*1 Masks(0:7,0:7)
        common /masks/ Masks

        integer Values(0:NUMSECT-1)
        common /values/ Values

        real xleft, ybottom, xlen, ylen
        real x1, y1, x2, y2, bar_width
        integer i

        bar_width = ( 2 * xlen ) / ( 3 * NUMSECT + 1 )
        y1 = ybottom
        do i = 0, NUMSECT - 1
            x1 = xleft + ( 3 * i + 1 ) * bar_width / 2
            x2 = x1 + bar_width
            y2 = y1 + ylen * Values( i ) / 100
            call _setcolor( mod( i, VC.numcolors - 1 ) + 1 )
            call _setfillmask( Masks( 0, mod( i, 8 ) ) )
            call _rectangle_w( _GFILLINTERIOR, x1, y1, x2, y2 )
            call _rectangle_w( _GBORDER, x1, y1, x2, y2 )
        enddo
        end


        subroutine BarGraph()

        ! Draw bar graph on left side of the screen.

        call DoAxes( 0.10, 0.15, 0.35, 0.7 )
        call DoBars( 0.10, 0.15, 0.35, 0.7 )
        end


        subroutine PieGraph()

        ! Draw pie graph.

        include 'graph.fi'

        record /videoconfig/ VC
        common VC

        integer NUMSECT
        parameter (NUMSECT=10)

        double precision PI
        parameter (PI=3.141592654)

        integer*1 Masks(0:7,0:7)
        common /masks/ Masks

        integer Values(0:NUMSECT-1)
        common /values/ Values

        integer i, total
        real x1, y1, x2, y2, x3, y3, x4, y4
        real xc, yc, xradius, yradius, theta

        ! Calculate data for pie graph.

        total = 0
        do i = 0, NUMSECT - 1
            total = total + Values( i )
        enddo

        ! Calculate centre and radius of pie

        xc = 0.75
        yc = 0.45
        xradius = 0.20
        yradius = 0.20 * 4 / 3

        ! Calculate bounding rectangle

        x1 = xc - xradius
        y1 = yc - yradius
        x2 = xc + xradius
        y2 = yc + yradius

        ! Draw the slices

        x3 = xc + xradius
        y3 = yc
        theta = 0.0
        do i = 0, NUMSECT - 1
            theta = theta + Values( i ) * 2 * PI / total
            x4 = xc + xradius * cos( theta )
            y4 = yc + yradius * sin( theta )
            call _setcolor( mod( i, ( VC.numcolors - 1 ) ) + 1 )
            call _setfillmask( Masks( 0, mod( i, 8 ) ) )
            call _pie_w( _GFILLINTERIOR, x1, y1, x2, y2,
     +                                   x3, y3, x4, y4 )
            call _pie_w( _GBORDER, x1, y1, x2, y2, x3, y3, x4, y4 )
            x3 = x4
            y3 = y4
        enddo
        end
