
        subroutine Clear
c
c       Kill all cells in the Selected Region, and update the screen
c
        include         'life.fi'

        integer         x,y
        integer         start_x, end_x
        integer         start_y, end_y
        integer         ps

        include 'initarr.fi'
        call GetSelectedCoords( start_x, end_x, start_y, end_y )

        if( .not. RegionIsSelected ) Generation = 0
        call GetPS( ps, .TRUE. )
        call FlipSelect()
        do x = start_x, end_x
            do y = start_y, end_y
                call TurnOffCell( ps, CellArray( x,y ), x, y )
            end do
        end do
        call FlipSelect()
        call GetPS( ps, .FALSE. )
        call SetCaption()
        include 'finiarr.fi'

        end



        subroutine Randomize()
c
c       Randomize cells in the Selected Region, and update the screen
c
        include         'life.fi'

        integer         x,y
        integer         start_x, end_x
        integer         start_y, end_y
        integer         ps

        include 'initarr.fi'
        call GetSelectedCoords( start_x, end_x, start_y, end_y )
        if( .not. RegionIsSelected ) Generation = 0
        call FlipSelect()
        call GetPS( ps, .TRUE. )
        do x = start_x, end_x
            do y = start_y, end_y
                if( Random( 4 ) .eq. 1 ) then
                    call TurnOnCell( ps, CellArray( x,y ), x, y )
                else
                    call TurnOffCell( ps, CellArray( x,y ), x, y )
                end if
            end do
        end do
        call FlipSelect()
        call GetPS( ps, .FALSE. )
        call SetCaption()
        include 'finiarr.fi'

        end



        function ReSizeArray( width, height )
        integer width,height
c
c       Record the new size of the window and redraw the screen accordingly.
c       Allocate a new cell array, and copy as much of the old array into
c       it as is sensible.
c
        include         'life.fi'

        integer                 old_grid_x,old_grid_y
        integer                 x, y, max_x, max_y
        integer                 oldmem
        record /cell_type/      oldarray(:,:)
        record /cell_type/      CellArray(:,:)
        record /cell_type/      kludge

        call SelectOff()
        WindowWidth = width
        WindowHeight = height
        old_grid_x = ArraySizeX
        old_grid_y = ArraySizeY
        oldmem = ArrayMem
        ArraySizeX = WindowWidth / BitInfo.cx
        ArraySizeY = WindowHeight / BitInfo.cy
        if( ArraySizeX .eq. 0 ) then
            ArraySizeX = 1
        end if
        if( ArraySizeY .eq. 0 ) then
            ArraySizeY = 1
        end if
        oldmem = ArrayMem
        ArrayMem = malloc( (ArraySizeX+2)*(ArraySizeY+2)*isizeof(kludge) )
        if( ArrayMem .eq. 0 ) then
            call NoMemory()
            ReSizeArray = .FALSE.
            return
        end if
        allocate( CellArray( 0:ArraySizeX+1,0:ArraySizeY+1 ),location=ArrayMem )
        do x = 0, ArraySizeX+1
            do y = 0, ArraySizeY+1
                CellArray( x, y ).alive = .FALSE.
                CellArray( x, y ).drawn = .FALSE.
                CellArray( x, y ).next_alive = .FALSE.
            end do
        end do
        if( oldmem .eq. 0 ) then
            call Randomize()
            Mode = MENU_RESUME
        else
            allocate( oldarray(  0:old_grid_x+1,0:old_grid_y+1 ),location=oldmem )
            max_x = ArraySizeX
            if( ArraySizeX .gt. old_grid_x )then
                max_x = old_grid_x
            end if
            max_y = ArraySizeY
            if( ArraySizeY .gt. old_grid_y )then
                max_y = old_grid_y
            end if
            Population = 0
            do x = 1, max_x
                do y = 1, max_y
                    CellArray( x, y ) = oldarray( x, y )
                    if( CellArray( x, y ).alive ) Population = Population + 1
                end do
            end do
            call SetCaption()
            deallocate( oldarray )
            call free( oldmem )
        end if
        deallocate( CellArray )
        ReSizeArray = .TRUE.

        end



        subroutine InitArray
c
c       Initialize the cell array
c
        include         'life.fi'

        ArrayMem = 0

        end



        subroutine FreeArray
c
c       Free the life array
c
        include         'life.fi'

        if( ArrayMem .ne. 0 ) call free( ArrayMem )

        end



        subroutine PutIntoRange( x, maxx )
        integer x,maxx
c
c       Wrap a point around the edge of space if it's out of range.
c
        while( x .lt. 1 ) do
           x = x + maxx
        end while
        while( x .gt. maxx ) do
            x = x - maxx
        end while

        end



        subroutine WrapAround( x, y )
        integer x,y
c
c       Wrap an (x,y) around the edge of space if it's out of range.
c

        include         'life.fi'

        call PutIntoRange( x, ArraySizeX )
        call PutIntoRange( y, ArraySizeY )

        end



        function SumNeighbours( x, y )
        integer x,y
c
c       Calculate the number of neighbours of cell at (x,y)
c
        include         'life.fi'
        integer         i
        integer         newx,newy

        integer Adjust(16) /
     +                        -1,-1,
     +                        -1, 0,
     +                        -1, 1,
     +                         0,-1,
     +                         0, 1,
     +                         1,-1,
     +                         1, 0,
     +                         1, 1
     +                     /
        include 'initarr.fi'
        SumNeighbours = 0
        do i = 1, 16, 2
            newx = x + Adjust(i)
            newy = y + Adjust(i+1)
            if( CurvedSpace ) then
                call WrapAround( newx, newy )
            end if
            if( CellArray( newx, newy ).alive ) then
                SumNeighbours = SumNeighbours + 1
            end if
        end do
        include 'finiarr.fi'

        end


        subroutine NextGeneration
c
c       Calculate the nextgeneration of cells
c
        include         'life.fi'
        integer         x,y
        integer         neighbours
        integer         ps

        include 'initarr.fi'
        Generation = Generation + 1

        do x = 1, ArraySizeX
            do y = 1, ArraySizeY
                if( y .eq. 0 ) call RelinquishControl()
                neighbours = SumNeighbours( x, y )
                CellArray( x, y ).next_alive = CellArray( x, y ).alive
                if( CellArray( x, y ).alive ) then
                    if( Deaths( neighbours ) ) then
                        CellArray( x, y ).next_alive = .FALSE.
                    end if
                else
                    if( Births( neighbours ) ) then
                        CellArray( x, y ).next_alive = .TRUE.
                    end if
                end if
            end do
        end do

        call GetPS( ps, .TRUE. )
        do x = 1, ArraySizeX
            do y = 1, ArraySizeY
                if( CellArray( x, y ).next_alive ) then
                    call TurnOnCell( ps, CellArray( x,y ), x, y )
                else
                    call TurnOffCell( ps, CellArray( x,y ), x, y )
                end if
                CellArray( x, y ).next_alive = .FALSE.
            end do
        end do

        call GetPS( ps, .FALSE. )

        call SetCaption()
        include 'finiarr.fi'

        end
