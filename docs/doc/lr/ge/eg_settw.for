        include 'graphapi.fi'
        include 'graph.fi'

        integer i
        integer*2 r1, c1, r2, c2
        character*80 buff

        call _setvideomode( _TEXTC80 )
        call _gettextwindow( r1, c1, r2, c2 )
        call _settextwindow( 5, 20, 20, 40 )
        do i = 1, 20
            write( buff, '(''Line '', i2, a1, a1 )' )
     +             i, char(10), char(0)
            call _outtext( buff )
        enddo
        pause
        call _settextwindow( r1, c1, r2, c2 )
        call _setvideomode( _DEFAULTMODE )
        end
