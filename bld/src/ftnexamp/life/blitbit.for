
        subroutine BlitBitMap( ps, x, y, on )
        integer ps, x, y, on
c
c       Draw a cell at location (x,y)
c
        include                 'gui.fi'
        integer                 start_x, start_y
        record /POINTL/         aptl(3)
        record /POINTL/         ptl

        start_x = BitInfo.cx*x
        start_y = BitInfo.cy*y
        aptl(1).x = start_x
        aptl(1).y = start_y
        aptl(2).x = start_x + BitInfo.cx
        aptl(2).y = start_y + BitInfo.cy
        aptl(3).x = 0
        aptl(3).y = 0
        if( on ) then
            call GpiBitBlt( ps, MemoryPS, 3, aptl, ROP_SRCCOPY, 0 )
        else
            call GpiBitBlt( ps, MemoryPS, 2, aptl, ROP_ONE, 0 )
        end if
        if( DrawGrid ) then
            ptl.x = start_x
            ptl.y = start_y
            call GpiMove( ps, ptl )
            ptl.x = start_x + BitInfo.cx
            ptl.y = start_y + BitInfo.cy
            call GpiBox( ps, DRO_OUTLINE, ptl, 0, 0 )
        end if

        end
