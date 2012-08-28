
        subroutine GetPS( ps, get )
        integer ps
        logical get
c
c
c
        include 'gui.fi'

        if( get ) then
            ps = WinGetPS( WinHandle )
        else
            call WinReleasePS( ps )
        end if

        end
