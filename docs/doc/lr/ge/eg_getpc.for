        include 'graphapi.fi'
        include 'graph.fi'

        record /xycoord/ pos

        real urand
        integer seed

        seed = 75347
        call _setvideomode( _VRES16COLOR )
        call _setvieworg(
     +       mod( int( urand( seed )*32767 ), 640 ),
     +       mod( int( urand( seed )*32767 ), 480 ) )
        pos = _getphyscoord( 0, 0 )
        call _rectangle( _GBORDER,
     +           - pos.xcoord, - pos.ycoord,
     +           639 - pos.xcoord, 479 - pos.ycoord )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
