
        subroutine DoTimer( start )
        logical start

        include 'gui.fi'

        if( start ) then
            call WinStartTimer( AnchorBlock, WinHandle, TIMER_ID, TimerDelay )
        else
            call WinStopTimer( AnchorBlock, WinHandle, TIMER_ID )
        end if

        end
