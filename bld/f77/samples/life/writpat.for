
        subroutine WritePatternFile
c
c       Write the selected region to a pattern file (Prompt for name)
c
        include         'life.fi'

        integer         start_x, end_x
        integer         start_y, end_y
        integer         x, y
        integer         io

        include 'initarr.fi'
        call GetSelectedCoords( start_x, end_x, start_y, end_y )
        io = fopen( Buffer, 'w'c )
        if( io .ne. 0 ) then
            do y = end_y, start_y, -1
                do x = start_x, end_x
                    if( CellArray( x, y ).alive ) then
                        call fputc( ichar( 'X' ), io )
                    else
                        call fputc( ichar( '_' ), io )
                    end if
                end do
                call fputc( 10, io )
            end do
            call fclose( io )
        end if
        call LoadPatternFile()
        include 'finiarr.fi'

        end
