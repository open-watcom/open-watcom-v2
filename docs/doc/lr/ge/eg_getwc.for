        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ centre
        record /_wxycoord/ pos1, pos2

        call _setvideomode( _MAXRESMODE )
        ! draw a box 50 pixels square
        ! in the middle of the screen
        centre = _getviewcoord_w( 0.5, 0.5 )
        pos1 = _getwindowcoord( centre.xcoord - 25,
     +                          centre.ycoord - 25 )
        pos2 = _getwindowcoord( centre.xcoord + 25,
     +                          centre.ycoord + 25 )
        call _rectangle_wxy( _GBORDER, pos1, pos2 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
