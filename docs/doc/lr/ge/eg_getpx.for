        include 'graphapi.fi'
        include 'graph.fi'

        integer x, y, i
        real urand
        integer seed

        seed = 75347
        call _setvideomode( _VRES16COLOR )
        call _rectangle( _GBORDER, 100, 100, 540, 380 )
        do i = 0, 60000
            x = 101 + mod( int( urand( seed )*32767 ),
     +                     439 )
            y = 101 + mod( int( urand( seed )*32767 ),
     +                     279 )
            call _setcolor( _getpixel( x, y ) + 1 )
            call _setpixel( x, y )
        enddo
        pause
        call _setvideomode( _DEFAULTMODE )
        end
