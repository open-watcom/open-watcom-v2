        include 'graphapi.fi'
        include 'graph.fi'

        integer col
        integer colors(16)/
     +       _BLACK, _BLUE, _GREEN,
     +       _CYAN, _RED, _MAGENTA,
     +       _BROWN, _WHITE, _GRAY, _LIGHTBLUE,
     +       _LIGHTGREEN, _LIGHTCYAN, _LIGHTRED,
     +       _LIGHTMAGENTA, _YELLOW, _BRIGHTWHITE/

        call _setvideomode( _VRES16COLOR )
        do col = 1, 16
            call _remappalette( 0, colors(col) )
            pause
        enddo
        call _setvideomode( _DEFAULTMODE )
        end
