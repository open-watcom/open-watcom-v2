        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _rectangle( _GFILLINTERIOR,
     +                   100, 100, 540, 380 )
        pause
        call _setviewport( 200, 200, 440, 280 )
        call _clearscreen( _GVIEWPORT )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
