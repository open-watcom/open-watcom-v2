        include 'graphapi.fi'
        include 'graph.fi'

        integer i
        character*80 buff

        call _setvideomode( _TEXTC80 )
        call _settextwindow( 5, 20, 20, 40 )
        do i = 1, 10
            write( buff, '(''Line '', i2, a1, a1 )' )
     +             i, char(10), char(0)
            call _outtext( buff )
        enddo
        pause
        call _scrolltextwindow( _GSCROLLDOWN )
        pause
        call _scrolltextwindow( _GSCROLLUP )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
