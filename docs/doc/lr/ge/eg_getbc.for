        include 'graphapi.fi'
        include 'graph.fi'

        integer bk, old_bk
        integer colors(16)/
     +       _BLACK, _BLUE, _GREEN,
     +       _CYAN, _RED, _MAGENTA,
     +       _BROWN, _WHITE, _GRAY, _LIGHTBLUE,
     +       _LIGHTGREEN, _LIGHTCYAN, _LIGHTRED,
     +       _LIGHTMAGENTA, _YELLOW, _BRIGHTWHITE/

        call _setvideomode( _VRES16COLOR )
        old_bk = _getbkcolor()
        do bk = 1, 16
            call _setbkcolor( colors( bk ) )
            pause
        enddo
        call _setbkcolor( old_bk )
        call _setvideomode( _DEFAULTMODE )
        end
