        include 'graphapi.fi'
        include 'graph.fi'

        integer x, y
        real urand
        integer seed

        seed = 75347
        call _setvideomode( _VRES16COLOR )
        while( _grstatus() .eq. _GROK )do
            x = mod( int( urand( seed )*32767 ), 700 )
            y = mod( int( urand( seed )*32767 ), 500 )
            call _setpixel( x, y )
        endwhile
        pause
        call _setvideomode( _DEFAULTMODE )
        end
