
        subroutine SelectOff()
c
c       Turn off the selected region, if there is one
c
        include 'life.fi'

        if( MouseMode .ne. MENU_SELECT ) return
        if( RegionIsSelected ) call XORSelectedRegion()
        RegionIsSelected = .FALSE.

        end


        subroutine GetCoords( x1, x2, thestart, theend, grid )
        integer x1, x2, thestart, theend, grid
c
c       Return the grid co-ordinates x1,x2 based on screen co-ordinates start,end
c        grid size (in pixels) grid.
c
        include 'life.fi'

        integer temp
        integer start
        integer end

        start=thestart
        end=theend

        if( start .gt. end ) then
            temp = start
            start = end
            end = temp
        end if
        x1 = start / grid
        x2 = end / grid - 1

        end



        subroutine GetSelectedCoords( x1, x2, y1, y2 )
        integer x1, x2, y1, y2
c
c       Get the grid co-ordinates of the currently selected region
c
        include 'life.fi'

        if( RegionIsSelected ) then
            call GetCoords( x1, x2, SelectStartX, SelectEndX, BitInfo.cx )
            call GetCoords( y1, y2, SelectStartY, SelectEndY, BitInfo.cy )
        else
            x1 = 0
            x2 = ArraySizeX
            y1 = 0
            y2 = ArraySizeY
        end if
        end



        subroutine FlipSelect()
c
c       Flip the selected region highlight. (Turn on if off and vice-versa)
c
        include 'life.fi'

        if( RegionIsSelected ) call XORSelectedRegion()

        end



        subroutine NotSame( x, max, step, start )
        integer x, max, step, start
c
c       utility -- see NotSameAsStart
c
        include 'life.fi'

        if( x .eq. start ) then
            if( x + step .lt. max ) then
                x = x + step
            else
                x = x - step
            end if
        end if

        end



        subroutine NotSameAsStart( x, y )
        integer x,y
c
c       Modify x, y so that they are not the same as SelectStartX, SelectStartY
c
        include 'life.fi'

        call NotSame( x, WindowWidth, BitInfo.cx, SelectStartX )
        call NotSame( y, WindowHeight, -BitInfo.cy, SelectStartY )

        end



        subroutine SnapToGrid( x, y )
        integer x,y
c
c       Modify x, y so that they are exactly on grid co-ordinates
c

        include 'life.fi'

        if( x .lt. 0 ) then
            x = 0
            return
        end if
        x = x - mod( x, BitInfo.cx )
        while( x .gt. WindowWidth ) do
            x = x - BitInfo.cx
        end while

        if( y .lt. 0 ) then
            y = 0
            return
        end if
        y = y + BitInfo.cy
        y = y - mod( y, BitInfo.cy )
        while( y .ge. WindowHeight ) do
            y = y - BitInfo.cy
        end while
        if( y .lt. 0 ) y = 0

        end



        function EndSelect( x,y )
        integer x,y
c
c       We are done a select (the user let the mouse button go).
c
        include 'life.fi'

        if( MouseMode .ne. MENU_SELECT ) then
            EndSelect = .FALSE.
            return
        end if
        call SnapToGrid( x, y )
        RegionIsSelected = .TRUE.
        EndSelect = .TRUE.

        end



        function MoveSelect( x, y )
        integer x,y
c
c       The mouse moved. Update the select rectangle.
c
        include 'life.fi'

        if( MouseMode .ne. MENU_SELECT ) then
            MoveSelect = .FALSE.
            return
        end if
        call SnapToGrid( x, y )
        call NotSameAsStart( x, y )
        if( SelectEndX .ne. x .or. SelectEndY .ne. y ) then
            call XORSelectedRegion()
            SelectEndX = x
            SelectEndY = y
            call XORSelectedRegion()
        end if
        MoveSelect = .TRUE.

        end



        function StartSelect( x, y )
        integer x, y
c
c       Record the spot at which selection started
c
        include 'life.fi'

        if( MouseMode .ne. MENU_SELECT ) then
            StartSelect = .FALSE.
            return
        end if
        if( RegionIsSelected ) call XORSelectedRegion()
        call SnapToGrid( x, y )
        SelectStartX = x
        SelectStartY = y
        call NotSameAsStart( x, y )
        SelectEndX = x
        SelectEndY = y
        call XORSelectedRegion()
        StartSelect = .TRUE.

        end
