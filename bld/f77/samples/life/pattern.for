
        subroutine DrawPattern( pixel_x, pixel_y, turn_on )
        integer         pixel_x, pixel_y
        logical         turn_on
c
c       Draw the currently selected pattern at screen location (pixel_x,pixel_y).
c       If "turn_on" is false, we wipe out a rectangle the size of the pattern.
c
        include         'life.fi'

        integer         screen_x, screen_y
        integer         x, y
        integer         wrap_x, wrap_y
        integer         ps
        integer         dim_x, dim_y
        integer*1       pattern(:,:)

        include 'initarr.fi'
        screen_x = pixel_x / BitInfo.cx
        screen_y = pixel_y / BitInfo.cy
        dim_y = PatternDimY( Cursor )
        dim_x = PatternDimX( Cursor )
        allocate( pattern( 1:dim_x, 1:dim_y ), location=Patterns( Cursor ) )
        call GetPS( ps, .TRUE. )
        do y = 1, dim_y
            do x = 1, dim_x
                wrap_x = screen_x + x - 1
                wrap_y = screen_y + y - dim_y
                call WrapAround( wrap_x, wrap_y )
                if( .not. turn_on ) then
                    call TurnOffCell( ps, CellArray( wrap_x, wrap_y ),
     +                                wrap_x, wrap_y )
                else if( pattern( x, y ) ) then
                    call TurnOnCell( ps, CellArray( wrap_x, wrap_y ),
     +                               wrap_x, wrap_y )
                end if
            end do
        end do
        call GetPS( ps, .FALSE. )
        call SetCaption()
        deallocate( pattern )
        include 'finiarr.fi'

        end
