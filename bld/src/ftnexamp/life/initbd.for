
        subroutine InitBD( births, deaths )

        logical births( 0:8 )
        logical deaths( 0:8 )
        integer i

        do i = 0,8
            births( i ) = .FALSE.
            deaths( i ) = .TRUE.
        end do

        births(3) = .TRUE.

        deaths(2) = .FALSE.
        deaths(3) = .FALSE.

        end
