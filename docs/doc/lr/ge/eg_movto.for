        include 'graphapi.fi'
        include 'graph.fi'

        call _setvideomode( _VRES16COLOR )
        call _moveto( 100, 100 )
        call _lineto( 540, 100 )
        call _lineto( 320, 380 )
        call _lineto( 100, 100 )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
