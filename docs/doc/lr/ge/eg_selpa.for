        include 'graphapi.fi'
        include 'graph.fi'

        integer x, y, pal

        call _setvideomode( _MRES4COLOR )
        do y = 0, 1
            do x = 0, 1
                call _setcolor( x + 2 * y )
                call _rectangle( _GFILLINTERIOR,
     +                x * 160, y * 100,
     +                ( x + 1 ) * 160, ( y + 1 ) * 100 )
            enddo
        enddo
        do pal = 0, 3
            call _selectpalette( pal )
            pause
        enddo
        call _setvideomode( _DEFAULTMODE )
        end
