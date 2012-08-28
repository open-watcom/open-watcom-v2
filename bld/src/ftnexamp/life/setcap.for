
        subroutine SetCaption
c
c       Set the caption to indicate generation number, etc.
c
        include 'gui.fi'

        write( Buffer, 100 ) Population, Generation, TimerDelay
        call WinSetWindowText( FrameHandle, loc( Buffer ) )

100     format( '32-Bit Life (FORTRAN) Pop: ', I4, ' Gen: ', I4,
     +          ' Rate: ', I4 )
        end
