
        function About( win_handle, msg, mp1, mp2 )
        integer win_handle
        integer msg
        integer mp1
        integer mp2
c
c       Process messages for the About dialogue.
c
        include 'gui.fi'

        select case ( msg )
        case ( WM_INITDLG )
            About = 0
            return
        case ( WM_COMMAND )
            if( SHORT1FROMMP( mp1 ) .eq. IDOK ) then
                call WinDismissDlg( win_handle, 0 )
                About = 0
                return
            end if
        end select
        About = WinDefDlgProc( win_handle, msg, mp1, mp2 )

        end
