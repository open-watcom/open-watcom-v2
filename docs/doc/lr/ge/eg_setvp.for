        include 'graphapi.fi'
        include 'graph.fi'

        integer XSIZE, YSIZE
        parameter (XSIZE=380)
        parameter (YSIZE=280)

        call _setvideomode( _VRES16COLOR )
        call _setviewport( 130, 100,
     +                     130 + XSIZE, 100 + YSIZE )
        call _ellipse( _GBORDER, 0, 0, XSIZE, YSIZE )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
