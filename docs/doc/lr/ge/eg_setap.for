        include 'graphapi.fi'
        include 'graph.fi'

        integer old_apage, old_vpage

        call _setvideomode( _HRES16COLOR )
        old_apage = _getactivepage()
        old_vpage = _getvisualpage()
        ! draw an ellipse on page 0
        call _setactivepage( 0 )
        call _setvisualpage( 0 )
        call _ellipse( _GFILLINTERIOR, 100, 50,
     +                                 540, 150 )
        ! draw a rectangle on page 1
        call _setactivepage( 1 )
        call _rectangle( _GFILLINTERIOR, 100, 50,
     +                                   540, 150 )
        pause
        ! display page 1
        call _setvisualpage( 1 )
        pause
        call _setactivepage( old_apage )
        call _setvisualpage( old_vpage )
        call _setvideomode( _DEFAULTMODE )
        end
