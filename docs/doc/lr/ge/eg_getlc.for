        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ pos1, pos2

        integer seed
        real urand

        seed = 75347
        call _setvideomode( _VRES16COLOR )
        call _setvieworg(
     +        mod( int( urand( seed )*32767 ), 640 ),
     +        mod( int( urand( seed )*32767 ), 480 ) )
        pos1 = _getviewcoord( 0, 0 )
        pos2 = _getviewcoord( 639, 479 )
        call _rectangle( _GBORDER,
     +                   pos1.xcoord, pos1.ycoord,
     +                   pos2.xcoord, pos2.ycoord )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
