        include 'graphapi.fi'
        include 'graph.fi'

        integer old_act

        call _setvideomode( _VRES16COLOR )
        old_act = _getplotaction()
        call _setplotaction( _GPSET )
        call _rectangle( _GFILLINTERIOR, 100, 100,
     +                                   540, 380 )
        pause
        call _setplotaction( _GXOR )
        call _rectangle( _GFILLINTERIOR, 100, 100,
     +                                   540, 380 )
        pause
        call _setplotaction( old_act )
        call _setvideomode( _DEFAULTMODE )
        end
