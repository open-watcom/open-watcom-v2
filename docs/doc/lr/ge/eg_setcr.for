        include 'graphapi.fi'
        include 'graph.fi'

        integer*2 x1, y1, x2, y2

        call _setvideomode( _VRES16COLOR )
        call _getcliprgn( x1, y1, x2, y2 )
        call _setcliprgn( 130, 100, 510, 380 )
        call _ellipse( _GBORDER, 120, 90, 520, 390 )
        pause
        call _setcliprgn( x1, y1, x2, y2 )
        call _setvideomode( _DEFAULTMODE )
        end
