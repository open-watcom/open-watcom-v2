        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ points(5)/
     +         319, 140, 224, 209, 261, 320,
     +         378, 320, 415, 209/

        call _setvideomode( _VRES16COLOR )
        call _polygon( _GBORDER, 5, points )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
