        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _setvieworg( 320, 240 )
        call _ellipse( _GBORDER, -200, -150, 200, 150 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
