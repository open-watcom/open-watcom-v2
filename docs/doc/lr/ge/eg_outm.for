        include 'graphapi.fi'
        include 'graph.fi'

        integer i
        character*20 buf

        call _clearscreen( _GCLEARSCREEN )
        do i = 0, 255
            call _settextposition( 1 + mod( i, 16 ),
     +                             1 + 5 * ( i / 16 ) )
            buf( 1:1 ) = char( i )
            call _outmem( buf, 1 )
        enddo
        pause
        end
