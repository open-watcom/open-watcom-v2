! file copying program using the file dialog box

*$include winapi.fi

        integer*2 function FWINMAIN( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )

        integer*2 hInstance
        integer*2 hPrevInstance
        integer*4 lpszCmdLine
        integer*2 nCmdShow
        logical   loaded
        common /perm/ loaded

        include 'wincreat.fi'
        include 'wincurs.fi'
        include 'windefn.fi'
        include 'winmsg.fi'
        include 'windtool.fi'
        include 'winutil.fi'
        include 'winmenu.fi'
        include 'winapi.fi'
        include 'fwcopy.fi'

        external WndProc

        integer*2               hWnd
        integer*2               hMenu
        integer*2               flags
        integer*2               rc
        record /MSG/            msg
        record /WNDCLASS/       wndclass
        character*7             AppName

        data AppName/'FWCopy'/ AppName(7:7)/'00'x/

        FWinMain = 0
        loaded = .FALSE.

        if( hPrevInstance .eq. NULL_HANDLE )then
            wndclass.style = CS_HREDRAW .or. CS_VREDRAW
            wndclass.lpfnWndProc = Loc( WndProc )
            wndclass.cbClsExtra = 0
            wndclass.cbWndExtra = 0
            wndclass.hInstance = hInstance
            wndclass.hIcon = NULL_HANDLE
            wndclass.hCursor = LoadCursor( NULL_HANDLE, IDC_ARROW )
            wndclass.hbrBackground = GetStockObject( WHITE_BRUSH )
            wndclass.lpszMenuName = NULL
            wndclass.lpszClassName = Loc( AppName )
            if( RegisterClass( wndclass ) .eq. 0 )then
                return
            end if
        end if

        hMenu = CreateMenu()
        if( hMenu .eq. 0 ) then
            return
        else
            flags = MF_STRING .or. MF_ENABLED
            rc = AppendMenu( hMenu, flags, DID_LOAD_FILE,
     &                       loc( STR_LOAD_FILE ) )
            if( rc .eq. 0 )then
                return
            end if
            rc = AppendMenu( hMenu, flags, DID_SAVE_FILE,
     &                       loc( STR_SAVE_FILE ) )
            if( rc .eq. 0 ) then
                return
            end if
        end if

        hWnd = CreateWindow( AppName,
     &                       AppName,
     &                       WS_OVERLAPPEDWINDOW,
     &                       CW_USEDEFAULT,
     &                       0,
     &                       CW_USEDEFAULT,
     &                       0,
     &                       NULL_HANDLE,
     &                       hMenu,
     &                       hInstance,
     &                       NULL )
        call ShowWindow( hWnd, nCmdShow )
        call UpdateWindow( hWnd )

        while( GetMessage( msg, NULL_HANDLE, 0, 0 ) .ne. 0 )do
            call TranslateMessage( msg )
            call DispatchMessage( msg )
        end while

        FWINMAIN = msg.wParam

        end


*$pragma aux (callback) WndProc parm( value, value, value, value )

        integer*4 function WndProc( HWnd, iMessage, wParam, lParam )

        integer*2 hWnd
        integer*2 iMessage
        integer*2 wParam
        integer*4 lParam
        logical   loaded
        character*10 note
        character*256 GetFilename
        character*256 PutFilename
        character*256 filename
        common /perm/ loaded
        save filename

        include 'windefn.fi'
        include 'wincreat.fi'
        include 'winmsgs.fi'
        include 'winapi.fi'
        include 'winerror.fi'
        include 'fwcopy.fi'

        note = 'Note'//CHAR(0)

        select( iMessage )
        case( WM_COMMAND )
            select( wParam )
            case( DID_LOAD_FILE )
                filename = GetFilename( hWnd )
            case( DID_SAVE_FILE )
                if( loaded ) then
                    call PutFilename(  hWnd, filename )
                else
                    call MessageBox( NULL, 'Load file first!'//CHAR(0),
     &                               note, MB_OK .or. MB_ICONHAND .or.
     &                               MB_SYSTEMMODAL )
                end if
            end select
            WndProc = DefWindowProc( hWnd, iMessage, wParam, lParam )
        case( WM_DESTROY )
            call PostQuitMessage( 0 )
        case default
            WndProc = DefWindowProc(hWnd, iMessage, wParam, lParam)
            return
        end select

        WndProc = 0

        end


        character*(*) function GetFilename( hWnd )
        integer*2       hWnd

        include 'windefn.fi'
        include 'winmsgs.fi'
        include 'winapi.fi'
        include 'winerror.fi'
        include 'commdlg.fi'

        logical   loaded
        common /perm/ loaded
        record /OPENFILENAME/ file
        character*256 filename

        file.lStructSize = isizeof( file )
        file.hwndOwner = hWnd
        file.hInstance = 0
        file.lpstrFilter = loc( 'all'//CHAR(0)//'*.*'//CHAR(0) )
        file.lpstrCustomFilter = 0
        file.nMaxCustFilter = 0
        file.nFilterIndex = 0
        filename = CHAR( 0 )
        file.lpstrFile = loc( filename )
        file.nMaxFile = 256
        file.lpstrFileTitle = 0
        file.nMaxFileTitle = 0
        file.lpstrInitialDir = 0
        file.lpstrTitle = 0
        file.Flags = OFN_PATHMUSTEXIST .or. OFN_FILEMUSTEXIST
        file.nFileOffset = 0
        file.nFileExtension = 0
        file.lpstrDefExt = 0
        file.lCustData = 0
        file.lpfnHook = 0
        file.lpTemplateName = 0

        if( GetOpenFileName( file ) .eq. 0 ) then
            GetFilename = char( 0 )
        else
            GetFilename = filename
            loaded = .TRUE.
        end if
        end


        character*(*) function PutFilename( hWnd, src )
        character*(*)   src
        integer*2       hWnd

        include 'windefn.fi'
        include 'winmsgs.fi'
        include 'winapi.fi'
        include 'winerror.fi'
        include 'commdlg.fi'

        record /OPENFILENAME/ file
        character*256 filename

        file.lStructSize = isizeof( file )
        file.hwndOwner = hWnd
        file.hInstance = 0
        file.lpstrFilter = loc( 'all'//char(0)//'*.*'//char(0) )
        file.lpstrCustomFilter = 0
        file.nMaxCustFilter = 0
        file.nFilterIndex = 0
        filename = char(0)
        file.lpstrFile = loc( filename )
        file.nMaxFile = 256
        file.lpstrFileTitle = 0
        file.nMaxFileTitle = 0
        file.lpstrInitialDir = 0
        file.lpstrTitle = 0
        file.Flags = OFN_PATHMUSTEXIST .or. OFN_FILEMUSTEXIST
        file.nFileOffset = 0
        file.nFileExtension = 0
        file.lpstrDefExt = 0
        file.lCustData = 0
        file.lpfnHook = 0
        file.lpTemplateName = 0

        if( GetSaveFileName( file ) .eq. 0 )then
            PutFilename = char( 0 )
        else
            call CopyFile( filename, src )
            PutFilename = filename
        endif

        end


        subroutine CopyFile( dst, src )
        character*(*)   dst
        character*(*)   src

        include 'fsublib.fi'
        integer                 len
        integer                 BUFFSIZE
        parameter               (BUFFSIZE=512)
        character*(BUFFSIZE)    buffer

        open( unit=1, file=src, access='sequential',
     &        form='unformatted', recl=1, action='read',
     &        recordtype='fixed' )
        open( unit=2, file=dst, access='sequential',
     &        form='unformatted', recl=1, action='write',
     &        recordtype='fixed' )

        len = filesize( 1 )
        while( len .gt. BUFFSIZE )do
            read( 1 ) buffer
            write( 2 ) buffer
            len = len - BUFFSIZE
        end while
        read( 1 ) buffer(1:len)
        write( 2 ) buffer(1:len)

        close( 1 )
        close( 2 )

        end
