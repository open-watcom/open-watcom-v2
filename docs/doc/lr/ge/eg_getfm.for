        include 'graphapi.fi'
        include 'graph.fi'

        integer*1 old_mask(8)
        integer*1 new_mask(8)/
     +                '81'x, '42'x, '24'x, '18'x,
     +                '18'x, '24'x, '42'x, '81'x/

        call _setvideomode( _VRES16COLOR )
        call _getfillmask( old_mask )
        call _setfillmask( new_mask )
        call _rectangle( _GFILLINTERIOR,
     +                   100, 100, 540, 380 )
        call _setfillmask( old_mask )
        pause
        call _setvideomode( _DEFAULTMODE )
        end
