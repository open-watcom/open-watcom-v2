
        function ReadAPatternFile( name, size, i )
        integer name,size,i
c
c       Read the pattern found in file "name", into pattern position "i"
c
        include         'life.fi'

        integer         io
        integer         ch
        integer         pattern_mem
        integer         xdim,ydim
        integer         x,y
        integer         j
        integer*1       array(:,:)

        io = fopen( loc( name ), 'r'c )
        if( io .eq. 0 ) then
            ReadAPatternFile = .FALSE.
            return
        end if
        xdim = 0
        loop
            ch = fgetc( io )
            if( ch .eq. -1 ) quit
            if( ch .eq. 10 ) quit
            xdim = xdim + 1
        end loop
        ydim = 1
        loop
            ch = fgetc( io )
            if( ch .eq. -1 ) quit
            if( ch .eq. 10 ) then
                ydim = ydim + 1
            end if
        end loop
        call fclose( io )
        PatternDimX(i) = xdim
        PatternDimY(i) = ydim
        pattern_mem = malloc( xdim*ydim )
        if( pattern_mem .eq. 0 ) call NoMemory()
        Patterns(i) = pattern_mem
        io = fopen( loc( name ), 'r'c )
        allocate( array( 1:xdim, 1:ydim ), location=pattern_mem )
        do y = ydim, 1, -1
            do x = 1, xdim
                ch = fgetc( io )
                if( ch .eq. ichar( '_' ) .or. ch .eq. ichar( ' ' ) ) then
                    array( x, y ) = 0
                else
                    array( x, y ) = 1
                end if
            end do
            ch = fgetc( io )
        end do
        call fclose( io )
        deallocate( array )

        ReadAPatternFile = .TRUE.

        end
