
        subroutine XORSelectedRegion
c
c       XOR the select rectangle with a black pen
c
        include         'gui.fi'

        integer         ps
        integer         old_rop
        record /POINTL/ aptl(2)

        ps = WinGetPS( WinHandle )
        aptl(1).x = SelectStartX
        aptl(1).y = SelectStartY
        aptl(2).x = SelectEndX
        aptl(2).y = SelectEndY
        call GpiBitBlt( ps, NULL, 2, aptl, ROP_DSTINVERT, 0 )
        call WinReleasePS( ps )

        end
