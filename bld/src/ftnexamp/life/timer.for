
c*
c* Code to handle the timer callbacks and adjust the rate as necessary
c*



        subroutine InitTimer

        include 'life.fi'

        NeedNewTimer = .FALSE.
        FastEnoughCount = 0
        TimerDelay = ONE_SECOND/4
        call DoTimer( .TRUE. )
        end


        subroutine FiniTimer

        include 'life.fi'

        call DoTimer( .FALSE. )
        end


        subroutine TimerTurbo

        include 'life.fi'

        TimerDelay = ONE_SECOND/100
        NeedNewTimer = .TRUE.
        call NewTimer()

        end


        subroutine NewTimer

        include 'life.fi'

        if( NeedNewTimer ) then
            call DoTimer( .FALSE. )
            call DoTimer( .TRUE. )
            NeedNewTimer = .FALSE.
        end if

        end
