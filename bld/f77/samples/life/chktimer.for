
        subroutine CheckTimerRate
c
c       Check to see if we should speed up or slow down the timer, based upon
c       whether there's already another WM_TIMER event in the queue. We speed
c       up if we made it before the new timer event n times, and slow down
c       the first time we don't make it.
c
        include         'gui.fi'
        record /QMSG/   peek
        logical         removed

        removed = .FALSE.
        while( WinPeekMsg( AnchorBlock, peek, WinHandle,
     +                     WM_TIMER, WM_TIMER, PM_REMOVE ) ) do
            removed = .TRUE.
        end while

        if( removed ) then
            FastEnoughCount = 0
            TimerDelay = TimerDelay + TimerDelay / 8
            NeedNewTimer = .TRUE.
        else
            FastEnoughCount = FastEnoughCount + 1
            if( FastEnoughCount .gt. 4 ) then
                if( FastEnoughCount*TimerDelay .gt. 4*ONE_SECOND ) then
                    TimerDelay = TimerDelay - TimerDelay / 8
                    NeedNewTimer = .TRUE.
                    FastEnoughCount = 0
                end if
            end if
        end if

        end
