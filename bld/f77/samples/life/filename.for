
        function FileName( win_handle, msg, mp1, mp2 )
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
            FileName = 0
            return
        case ( WM_COMMAND )
            msg = SHORT1FROMMP( mp1 )
            if( msg .eq. IDOK .or. msg .eq. IDCANCEL ) then
                call WinQueryDlgItemText( win_handle, IDNAME,
     +                                    isizeof( Buffer ), loc( Buffer ) )
                call WinDismissDlg( win_handle, msg .eq. IDOK )
                FileName = 0
                return
            end if
        end select

        FileName = WinDefDlgProc( win_handle, msg, mp1, mp2 )
        end
