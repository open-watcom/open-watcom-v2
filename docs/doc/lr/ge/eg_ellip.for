        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _ellipse( _GBORDER, 120, 90, 520, 390 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
