
        subroutine UpdateCell( ps, cell, x, y )
c
c       Update the cell at location (x,y)
c
        include         'life.fi'
        integer                 ps
        record /cell_type/      cell
        integer                 x,y

        if( cell.alive .and. .not. cell.drawn ) then
            call BlitBitMap( ps, x, y, .TRUE. )
            cell.drawn = .TRUE.
        else if( .not. cell.alive .and. cell.drawn ) then
            call BlitBitMap( ps, x, y, .FALSE. )
            cell.drawn = .FALSE.
        end if

        end



        function TurnOffCell( ps, cell, x, y )
c
c       Turn off the "cell" at grid location (x,y) if needed
c
        include 'life.fi'

        integer                 ps
        record /cell_type/      cell
        integer                 x,y

        if( .not. cell.alive ) then
            TurnOffCell = .FALSE.
            return
        end if
        Population = Population - 1
        cell.alive = .FALSE.

        call UpdateCell( ps, cell, x, y )
        TurnOffCell = .TRUE.

        end



        function TurnOnCell( ps, cell, x, y )
c
c       Turn On the "cell" at grid location (x,y) if needed
c
        include 'life.fi'

        integer                 ps
        record /cell_type/      cell
        integer                 x,y

        if( cell.alive ) then
            TurnOnCell = .FALSE.
            return
        end if
        Population = Population + 1
        cell.alive = .TRUE.

        call UpdateCell( ps, cell, x, y )
        TurnOnCell = .TRUE.

        end
