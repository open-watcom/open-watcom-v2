        include 'graphapi.fi'
        include 'graph.fi'

        integer DASHED
        parameter (DASHED='f0f0'x)

        integer old_style

        call _setvideomode( _VRES16COLOR )
        old_style = _getlinestyle()
        call _setlinestyle( DASHED )
        call _rectangle( _GBORDER, 100, 100, 540, 380 )
        call _setlinestyle( old_style )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
