        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ old_pos

        call _setvideomode( _VRES16COLOR )
        old_pos = _getcurrentposition()
        call _moveto( 100, 100 )
        call _lineto( 540, 100 )
        call _lineto( 320, 380 )
        call _lineto( 100, 100 )
        call _moveto( old_pos.xcoord, old_pos.ycoord )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
