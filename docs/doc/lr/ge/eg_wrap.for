        include 'graphapi.fi'
        include 'graph.fi'

        integer i
        character buff*80

        call _setvideomode( _TEXTC80 )
        call _settextwindow( 5, 20, 20, 30 )
        call _wrapon( _GWRAPOFF )
        do i = 1, 3
            call _settextposition( 2 * i, 1 )
            write( buff,
     +             '(''Very very long line '', i2, a1)' )
     +             i, char(0)
            call _outtext( buff )
        enddo
        call _wrapon( _GWRAPON )
        do i = 4, 6
            call _settextposition( 2 * i, 1 )
            write( buff,
     +             '(''Very very long line '', i2, a1)' )
     +             i, char(0)
            call _outtext( buff )
        enddo
        pause
        call _setvideomode( _DEFAULTMODE )
        end
