        include 'graphapi.fi'
        include 'graph.fi'

        integer colors(16)/
     +       _BRIGHTWHITE, _YELLOW, _LIGHTMAGENTA,
     +       _LIGHTRED, _LIGHTCYAN, _LIGHTGREEN,
     +       _LIGHTBLUE, _GRAY, _WHITE, _BROWN,
     +       _MAGENTA, _RED, _CYAN,
     +       _GREEN, _BLUE, _BLACK/
        integer x, y

        call _setvideomode( _VRES16COLOR )
        do y = 0, 3
            do x = 0, 3
                call _setcolor( x + 4 * y )
                call _rectangle( _GFILLINTERIOR,
     +                x * 160, y * 120,
     +                ( x + 1 ) * 160, ( y + 1 ) * 120 )
            enddo
        enddo
        pause
        call _remapallpalette( colors )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
