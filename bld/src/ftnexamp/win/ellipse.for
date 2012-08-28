! Display ellipses

*$include winapi.fi

c$noreference
        integer*2 function FWINMAIN( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )
c$reference

        integer*2 hInstance
        integer*2 hPrevInstance
        integer*2 nCmdShow
        integer*4 lpszCmdLine

        include 'win386.fi'
        include 'wincreat.fi'
        include 'wincurs.fi'
        include 'windefn.fi'
        include 'windisp.fi'
        include 'winmsg.fi'
        include 'winmsgs.fi'
        include 'windtool.fi'
        include 'winutil.fi'

        external WndProc

        integer*2               hWnd
        record /MSG/            msg
        record /WNDCLASS/       wndclass
        character*14            class

        wndclass.style = CS_HREDRAW .or. CS_VREDRAW
        wndclass.lpfnWndProc = loc( WndProc )
        wndclass.cbClsExtra = 0
        wndclass.cbWndExtra = 0
        wndclass.hInstance = hInstance
        wndclass.hIcon = NULL_HANDLE
        wndclass.hCursor = LoadCursor( NULL_HANDLE, IDC_ARROW )
        wndclass.hbrBackground = GetStockObject( WHITE_BRUSH )
        wndclass.lpszMenuName = NULL
        write( class, '(''Ellipses'',i5.5,a)' ) hInstance, char(0)
        wndclass.lpszClassName = Loc( class )
        if( RegisterClass( wndclass ) .eq. 0 )then
            FWINMAIN = 0
            return
        end if

        hWnd = CreateWindow( class,
     &                       class,
     &                       WS_OVERLAPPEDWINDOW,
     &                       CW_USEDEFAULT,
     &                       0,
     &                       CW_USEDEFAULT,
     &                       0,
     &                       NULL_HANDLE,
     &                       NULL_HANDLE,
     &                       hInstance,
     &                       NULL )
        call ShowWindow( hWnd, nCmdShow )
        call UpdateWindow( hWnd )

        loop
            if( PeekMessage( msg, NULL_HANDLE, 0, 0,
     &                       PM_REMOVE ) .ne. 0 )then
                if( msg.message .eq. WM_QUIT ) exit
                call TranslateMessage( msg )
                call DispatchMessage( msg )
            else
                call DrawEllipse( hWnd )
            endif
        endloop

        FWINMAIN = msg.wParam

        end


        subroutine DrawEllipse( hWnd )

        integer*2 hWnd

        include 'win386.fi'
        include 'windefn.fi'
        include 'winellps.fi'
        include 'windtool.fi'
        include 'winpaint.fi'
        include 'winutil.fi'

        external                irand
        integer*2               irand

        integer*2               hBrush
        integer*2               hDC
        integer*2               xLeft, xRight, yTop, yBottom
        integer*2               nRed, nGreen, nBlue
        integer*2               xClient, yClient

        common /client/ xClient, yClient

        xLeft   = mod( irand(), xClient )
        xRight  = mod( irand(), xClient )
        yTop    = mod( irand(), yClient )
        yBottom = mod( irand(), yClient )
        nRed    = irand() .and. 255
        nGreen  = irand() .and. 255
        nBlue   = irand() .and. 255

        hDC = GetDC( hWnd )
        hBrush = CreateSolidBrush( RGB( nRed, nGreen, nBlue ) )
        call SelectObject( hDC, hBrush )

        call Ellipse( hDC, min( xLeft, xRight ),
     &                     min( yTop, yBottom ),
     &                     max( xLeft, xRight ),
     &                     max( yTop, yBottom ) )

        call ReleaseDC( hWnd, hDC )
        call DeleteObject( hBrush )

        end


*$pragma aux (callback) WndProc parm( value, value, value, value )

        integer*4 function WndProc( HWnd, iMessage, wParam, lParam )

        integer*2 hWnd
        integer*2 iMessage
        integer*2 wParam
        integer*4 lParam

        include 'win386.fi'
        include 'windefn.fi'
        include 'winmsg.fi'
        include 'winmsgs.fi'
        include 'winutil.fi'
        include 'wincreat.fi'

        integer*2       xClient, yClient

        common /client/ xClient, yClient

        select( iMessage )
        case( WM_SIZE )
            xClient = LOWORD( lParam )
            yClient = HIWORD( lParam )
        case( WM_DESTROY )
            call PostQuitMessage( 0 )
        case default
            WndProc = DefWindowProc( hWnd, iMessage, wParam, lParam )
            return
        end select

        WndProc = 0

        end


        integer*2 function irand()

        integer         seed
        real            urand

        data seed/75347/

        irand = int( urand( seed ) * 32767 )

        end
