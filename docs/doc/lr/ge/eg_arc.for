        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _arc( 120, 90, 520, 390, 500, 20, 450, 460 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
