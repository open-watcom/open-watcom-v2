        include 'graphapi.fi'
        include 'graph.fi'

        integer col, old_col

        call _setvideomode( _VRES16COLOR )
        old_col = _getcolor()
        do col = 0, 15
            call _setcolor( col )
            call _rectangle( _GFILLINTERIOR,
     +                       100, 100, 540, 380 )
            pause
        enddo
        call _setcolor( old_col )
        call _setvideomode( _DEFAULTMODE )
        end
