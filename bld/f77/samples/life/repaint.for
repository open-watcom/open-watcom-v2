
        subroutine RePaint( ps )
        integer ps
c
c       Re-draw the entire screen. It's been trashed.
c

        include                 'gui.fi'

        integer                 x,y
        record /RECTL/          rcl
        record /POINTL/         ptl

        include 'initarr.fi'
        call WinQueryWindowRect( WinHandle, rcl )
        call WinFillRect( ps, rcl, CLR_WHITE )
        do x = 1, ArraySizeX
            do y = 1, ArraySizeY
                if( CellArray( x, y ).alive ) then
                    call BlitBitMap( ps, x, y, .TRUE. )
                    CellArray( x, y ).drawn = .TRUE.
                end if
            end do
        end do
        if( DrawGrid ) then
            do x = 0, ArraySizeX
                ptl.x = x*BitInfo.cx
                ptl.y = 0
                call GpiMove( ps, ptl )
                ptl.y = WindowHeight
                call GpiLine( ps, ptl )
            end do
            do y = 0, ArraySizeY
                ptl.x = 0
                ptl.y = y*BitInfo.cy
                call GpiMove( ps, ptl )
                ptl.x = WindowWidth
                call GpiLine( ps, ptl )
            end do
            ptl.x = 0
            ptl.y = 0
            call GpiMove( ps, ptl )
            ptl.y = WindowHeight
            call GpiLine( ps, ptl )
            ptl.x = WindowWidth
            call GpiLine( ps, ptl )
            ptl.y = 0
            call GpiLine( ps, ptl )
        end if
        call FlipSelect()
        include 'finiarr.fi'

        end
