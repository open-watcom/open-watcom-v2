
        subroutine      ReDrawScreen
c
c
c
        integer         ps

        call GetPS( ps, .TRUE. )
        call RePaint( ps )
        call GetPS( ps, .FALSE. )

        end
