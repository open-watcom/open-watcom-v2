
        subroutine RelinquishControl

        include         'gui.fi'

        record /QMSG/   peek

        call WinPeekMsg( AnchorBlock, peek, WinHandle,
     +                   WM_TIMER, WM_TIMER, PM_NOREMOVE )
        end
