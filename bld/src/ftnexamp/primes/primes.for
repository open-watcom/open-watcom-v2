c$define INCL_WIN
c$include os2.fap

        program primes

        include 'os2.fi'
        include 'primes.fi'

        integer         hmq
        integer         anchor
        external        MainDialogProc
        integer         MainDialogProc

        anchor = WinInitialize( 0 )
        if( anchor .eq. 0 ) stop

        hmq = WinCreateMsgQueue( anchor, 0 )
        if( hmq .ne. 0 )then
            call WinDlgBox( HWND_DESKTOP, 0, MainDialogProc,
     &                       0, DLG_PRIME_NUMBER, 0 )
            call WinDestroyMsgQueue( hmq )
        endif

        call WinTerminate( anchor )

        end


        subroutine Error( hwndDlg, msg )
        integer         hwndDlg
        character*(*)   msg

        include 'os2.fi'

        call WinMessageBox( HWND_DESKTOP, hwndDlg,
     &              msg,
     &              'Error!'//char(0), 0,
     &              MB_APPLMODAL .or. MB_NOICON .or.
     &              MB_OK .or. MB_MOVEABLE )
        end


        logical function GetIntegerValue( hwndDlg, id, val )
        integer         hwndDlg, val
        integer         id

        include 'os2.fi'

        integer         ios, len
        character*9     buff

        guess
            len = WinQueryDlgItemText( hwndDlg, id, 9, buff )
            if( len .eq. 0 ) quit
            read( buff(1:len), *, iostat=ios ) val
            if( ios .ne. 0 ) quit
            GetIntegerValue = .true.
        admit
            GetIntegerValue = .false.
        endguess

        end


        logical function ChkIntegerValue( hwndDlg, id, val )
        integer         hwndDlg, val
        integer         id

        logical         GetIntegerValue

        if( GetIntegerValue( hwndDlg, id, val ) )then
            ChkIntegerValue = .true.
        else
            call Error( hwndDlg, 'Bad integer'//char(0) )
            ChkIntegerValue = .false.
        endif

        end


c$pragma aux (FNWP)             MainDialogProc

        integer function MainDialogProc( hwnd, msg, mp1, mp2 )
        integer         hwnd, mp1, mp2
        integer*2       msg

        include 'os2.fi'
        include 'primes.fi'

        logical         ChkIntegerValue
        integer*2       dlg_id
        integer         nprimes, upper_bound
        logical         prime_count
        external        prime_count, ChkIntegerValue

        character*32    text
        character*9     buff

        character*(*)   caption
        parameter       (caption = 'Number of Prime Numbers'//char(0))

        select( msg )
        case( WM_INITDLG )
            call WinSendDlgItemMsg( hwnd, DLG_UPPER_BOUND,
     &          EM_SETTEXTLIMIT, MPFROMSHORT( 8 ), 0 )
            write( buff, '(i8,a)' ) 1000, char(0)
            call WinSetDlgItemText( hwnd, DLG_UPPER_BOUND, buff )
        case( WM_COMMAND )
            dlg_id = SHORT1FROMMP( mp1 )
            select( dlg_id )
            case( DLG_GENERATE_PRIMES )
                if( ChkIntegerValue( hwnd, DLG_UPPER_BOUND,
     &                  upper_bound ) )then
                    if( prime_count( hwnd, upper_bound, nprimes ) )then
                        write( text, '(i8,a)' ) nprimes, char(0)
                        call WinMessageBox( HWND_DESKTOP, hwnd, text,
     &                      caption, 0, MB_NOICON .or. MB_OK )
                    endif
                endif
            case( DLG_PRIME_CLOSE )
                call WinDismissDlg( hwnd, 0 )
            endselect
        case default
            MainDialogProc = WinDefDlgProc( hwnd, msg, mp1, mp2 )
            return
        endselect
        MainDialogProc = 0

        end


        logical function prime_count( hwndDlg, upbound, nprimes )
        integer upbound, hwndDlg, nprimes

        integer i, k, ios, numbers(:)

        prime_count = .true.
        nprimes = 0
        if( upbound .le. 1 ) return
        allocate( numbers(upbound), stat = ios )
        if( ios .ne. 0 )then
            call Error( hwndDlg, 'No memory'//char(0) )
            prime_count = .false.
            return
        endif
        numbers( 1 ) = 0
        do i = 2, upbound
            numbers( i ) = i
        enddo
        do i = 2, upbound
            if( numbers( i ) .ne. 0 )then
                nprimes = nprimes + 1
                do k = i + i, upbound, i
                    numbers( k ) = 0
                enddo
            endif
        enddo
        deallocate( numbers )

        end
