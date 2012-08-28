
        subroutine      ReflectXY()
c
c
c
        include         'life.fi'

        integer*1       array(:,:)
        integer*1       newarray(:,:)
        integer         dim_x, dim_y
        integer         new_pattern
        integer         x, y
        integer         i

        do i = 1, NumberPatterns
            dim_x = PatternDimX( i )
            dim_y = PatternDimY( i )
            new_pattern = malloc( dim_x*dim_y )
            if( new_pattern .eq. 0 ) then
                call NoMemory()
            end if

            allocate( array( 1:dim_x, 1:dim_y ), location = Patterns(i) )
            allocate( newarray( 1:dim_y, 1:dim_x ), location = new_pattern )
            do x = 1, dim_x
                do y = 1, dim_y
                    newarray( y, x ) = array( x, y )
                end do
            end do
            deallocate( array )
            deallocate( newarray )
            call free( Patterns( i ) )
            Patterns( i ) = new_pattern
            PatternDimX( i ) = dim_y
            PatternDimY( i ) = dim_x
        end do

        end


        subroutine      ReflectX()
c
c
c
        include         'life.fi'

        integer*1       array(:,:)
        integer         dim_x, dim_y
        integer         x, y
        integer*1       temp
        integer         i

        do i = 1, NumberPatterns
            dim_x = PatternDimX( i )
            dim_y = PatternDimY( i )
            allocate( array( 1:dim_x, 1:dim_y ), location = Patterns(i) )
            do x = 1, dim_x
                do y = 1, dim_y / 2
                    temp = array( x, y )
                    array( x, y ) = array( x, dim_y - y + 1 )
                    array( x, dim_y - y + 1 ) = temp
                end do
            end do
            deallocate( array )
        end do

        end


        subroutine      ReflectY()
c
c
c
        include         'life.fi'

        integer*1       array(:,:)
        integer         dim_x, dim_y
        integer         x, y
        integer*1       temp
        integer         i

        do i = 1, NumberPatterns
            dim_x = PatternDimX( i )
            dim_y = PatternDimY( i )
            allocate( array( 1:dim_x, 1:dim_y ), location = Patterns(i) )
            do x = 1, dim_x / 2
                do y = 1, dim_y
                    temp = array( x, y )
                    array( x, y ) = array( dim_x - x + 1, y )
                    array( dim_x - x + 1, y ) = temp
                end do
            end do
            deallocate( array )
        end do

        end
