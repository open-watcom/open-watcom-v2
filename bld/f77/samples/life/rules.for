
        function Rules( win_handle, msg, mp1, mp2 )
        integer win_handle,msg,mp1,mp2
c
c       Process messages for the rules dialogue.
c

        include         'gui.fi'

        logical         WorkBirths(0:8), WorkDeaths(0:8)
        integer         i

        select case ( msg )
        case ( WM_INITDLG )
            do i = 0, 8
                WorkBirths( i ) = Births( i )
                WorkDeaths( i ) = Deaths( i )
            end do
            Rules = 0

        case ( WM_COMMAND )
            execute Command

        case ( WM_CONTROL )
            execute Command

        otherwise
            Rules = WinDefDlgProc( win_handle, msg, mp1, mp2 )
            return

        endselect

        do i = 0, 8
            call WinSendDlgItemMsg( win_handle, BM_BIRTH_0+i, BM_SETCHECK,
     +                              MPFROMSHORT( WorkBirths( i ) ), 0 )
            call WinSendDlgItemMsg( win_handle, BM_DEATH_0+i, BM_SETCHECK,
     +                              MPFROMSHORT( WorkDeaths( i ) ), 0 )
        end do


        remote block Command

        msg = SHORT1FROMMP( mp1 )
        select case( msg )
        case ( IDOK )
            do i = 0, 8
                Births( i ) = WorkBirths( i )
                Deaths( i ) = WorkDeaths( i )
            end do
            call WinDismissDlg( win_handle, 0 )
            Rules = 0

        case ( IDCANCEL )
            call WinDismissDlg( win_handle, 0 )
            Rules = 0


        case ( BM_BIRTH_0 : BM_BIRTH_8 )
            i = msg - BM_BIRTH_0
            WorkBirths( i ) = .not. WorkBirths( i )
            Rules = 0

        case ( BM_DEATH_0 : BM_DEATH_8 )
            i = msg - BM_DEATH_0
            WorkDeaths( i ) = .not. WorkDeaths( i )
            Rules = 0

        case ( BM_DEATH_NEVER )
            do i = 0, 8
                WorkDeaths( i ) = .FALSE.
            end do
            Rules = 0

        case ( BM_BIRTH_DEFAULT )
            call InitBD( WorkBirths, WorkDeaths )
            Rules = 0

        otherwise
            Rules = WinDefDlgProc( win_handle, msg, mp1, mp2 )

        end select

        end block

        end
