        include 'graphapi.fi'
        include 'graph.fi'

        integer valid_rows(8)/
     +          14, 25, 28, 30,
     +          34, 43, 50, 60/

        integer i, j, rows
        character*80 buff

        do i = 0, 7
            rows = valid_rows( i )
            if( _settextrows( rows ) .eq. rows )then
                do j = 1, rows
                    write( buff, '(''Line '', i2, a1 )' )
     +                     j, char(0)
                    call _settextposition( j, 1 )
                    call _outtext( buff )
                enddo
                pause
            endif
        enddo
        call _setvideomode( _DEFAULTMODE )
        end
