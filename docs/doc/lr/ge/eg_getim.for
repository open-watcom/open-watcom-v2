        include 'graphapi.fi'
        include 'graph.fi'

        integer*1 image(:)
        integer y, image_size, istat

        call _setvideomode( _VRES16COLOR )
        call _ellipse( _GFILLINTERIOR,
     +                 100, 100, 200, 200 )
        image_size = _imagesize( 100, 100, 201, 201 )
        allocate( image(image_size), stat = istat )
        if( istat .eq. 0 )then
            call _getimage( 100, 100, 201, 201, image )
            call _putimage( 260, 200, image, _GPSET )
            call _putimage( 420, 100, image, _GPSET )
            do y = 100, 280, 20
                call _putimage( 420, y, image, _GXOR )
                call _putimage( 420, y+20, image, _GXOR )
            enddo
            deallocate( image )
        endif
        pause
        call _setvideomode( _DEFAULTMODE )
        end
