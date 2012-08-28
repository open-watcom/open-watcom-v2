! rxsutils.c -- REXX String utilities.  Various utility functions to do
!               string to type conversions and back.  You don't need to
!               use these, but they may be useful to you.

c$define INCL_DOS
c$include os2.fap
c$define INCL_REXXSAA
c$include rexxsaa.fap


! CopyResult -- Copies a string into a result, allocating space for it
!               if necessary.  If you pass it an RXSTRING with a non-null
!               buffer and a non-zero length, it will try to copy the
!               data into that buffer.  Otherwise is uses DosAllocMem
!               to allocate a new one.

        subroutine CopyResult( src, len, dest )
        character*(*) src
        integer len
        record /RXSTRING/ dest

        include 'rxsutils.fi'

        integer mem
        character*(*) dst

        if( len .eq. 0  )then
            call SetNullRXString( dest )
        else if( dest.strptr .ne. 0 .and. len .lt. dest.strlength )then
            dest.strlength = len
            allocate( dst*len, location=dest.strptr )
            dst = src
            dst(len+1:len+1) = char(0)
        else
            ! OK, at this point we have determined the buffer is too small
            ! for us to use, so we will allocate a new one
            call setNullRXString( dest )
            if( DosAllocMem( mem, len + 1,
     &          PAG_COMMIT .or. PAG_WRITE .or. PAG_READ ) .ne. 0 )then
                return
            endif

            dest.strptr = mem
            dest.strlength = len

            allocate( dst*len, location=dest.strptr )
            dst = src
            dst(len+1:len+1) = char(0)

        endif

        end


! SetNullRXString -- Sets an RXSTRING to a null string.

        subroutine SetNullRXString( str )
        record /RXSTRING/ str

        include 'rxsutils.fi'

        character*(*) dst

        if( str.strptr )then
            allocate( dst*1, location=str.strptr )
            dst(1:1) = char(0)
        endif
        str.strlength = 0

        end


        integer function strlen( str )
        character*(*) str
        integer i
        i = 1
        do
            if( str(i:i) .eq. char(0) )then
                strlen = i - 1
                return
            endif
            i = i + 1
        enddo
        end
