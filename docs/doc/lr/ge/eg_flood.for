        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _setcolor( 1 )
        call _ellipse( _GBORDER, 120, 90, 520, 390 )
        call _setcolor( 2 )
        call _floodfill( 320, 240, 1 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
