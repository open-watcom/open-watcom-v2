        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ start_pt, end_pt, inside_pt

        call _setvideomode( _VRES16COLOR )
        call _arc( 120, 90, 520, 390, 520, 90, 120, 390 )
        call _getarcinfo( start_pt, end_pt, inside_pt )
        call _moveto( start_pt.xcoord, start_pt.ycoord )
        call _lineto( end_pt.xcoord, end_pt.ycoord )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
