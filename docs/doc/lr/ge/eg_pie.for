        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _pie( _GBORDER, 120, 90, 520, 390,
     +                       140, 20, 190, 460 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
