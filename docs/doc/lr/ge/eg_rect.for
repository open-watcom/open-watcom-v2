        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _rectangle( _GBORDER, 100, 100, 540, 380 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
