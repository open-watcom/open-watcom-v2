! Clover drawing program using regions.

*$include winapi.fi

        integer*2 function FWINMAIN( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )

        integer*2 hInstance
        integer*2 hPrevInstance
        integer*4 lpszCmdLine
        integer*2 nCmdShow

        include 'wincreat.fi'
        include 'wincurs.fi'
        include 'windefn.fi'
        include 'windisp.fi'
        include 'winmsg.fi'
        include 'windtool.fi'
        include 'winutil.fi'

        external WndProc

        integer*2               hWnd
        record /MSG/            msg
        record /WNDCLASS/       wndclass
        character*7             AppName

        data AppName/'Clover'/ AppName(7:7)/'00'x/

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
                FWINMAIN = 0
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
     &                       NULL_HANDLE,
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

        include 'windefn.fi'
        include 'wincurs.fi'
        include 'wincreat.fi'
        include 'winline.fi'
        include 'winmap.fi'
        include 'winmsg.fi'
        include 'winmsgs.fi'
        include 'winpaint.fi'
        include 'winrgn.fi'
        include 'windtool.fi'
        include 'winutil.fi'

        double precision TWO_PI
        parameter ( TWO_PI = 2 * 3.14159 )

        integer*2               hRgnClip
        integer*2               xClient, yClient
        double precision        fAngle, fRadius
        integer*2               hCursor
        integer*2               hDC
        integer*2               hRgnTemp(6)
        record /PAINTSTRUCT/    ps
        integer                 i
        double precision        hypot, x, y

        save hRgnClip, xClient, yClient

        data hRgnClip/NULL_HANDLE/

        hypot( x, y ) = sqrt( x**2 + y**2 )

        select( iMessage )
        case( WM_SIZE )
            xClient = LOWORD( lParam )
            yClient = HIWORD( lParam )

            hCursor = SetCursor( LoadCursor( NULL_HANDLE, IDC_WAIT ) )
            call ShowCursor( 1 )

            if( hRgnClip .ne. NULL_HANDLE )then
                call DeleteObject( hRgnClip )
            end if

            hRgnTemp(1) = CreateEllipticRgn( 0,
     &                                       yClient / 3,
     &                                       xClient / 2,
     &                                       2 * yClient / 3 )
            hRgnTemp(2) = CreateEllipticRgn( xClient / 2,
     &                                       yClient / 3,
     &                                       xClient,
     &                                       2 * yClient / 3 )
            hRgnTemp(3) = CreateEllipticRgn( xClient / 3,
     &                                       0,
     &                                       2 * xClient / 3,
     &                                       yClient / 2 )
            hRgnTemp(4) = CreateEllipticRgn( xClient / 3,
     &                                       yClient / 2,
     &                                       2 * xClient / 3,
     &                                       yClient )
            hRgnTemp(5) = CreateRectRgn( 0, 0, 1, 1 )
            hRgnTemp(6) = CreateRectRgn( 0, 0, 1, 1 )
            hRgnClip = CreateRectRgn( 0, 0, 1, 1 )

            call CombineRgn( hRgnTemp(5), hRgnTemp(1), hRgnTemp( 2 ),
     &                       RGN_OR )
            call CombineRgn( hRgnTemp(6), hRgnTemp(3), hRgnTemp( 4 ),
     &                       RGN_OR )
            call CombineRgn( hRgnClip, hRgnTemp(5), hRgnTemp( 6 ),
     &                       RGN_XOR )

            do i = 1, 6
                call DeleteObject( hRgnTemp(i) )
            end do

            call SetCursor( hCursor )
            call ShowCursor( 0 )
        case( WM_PAINT )
            hDC = BeginPaint( hWnd, ps )

            call SetViewportOrg( hDC, xClient / 2, yClient / 2 )
            call SelectClipRgn( hDC, hRgnClip )

            fRadius = hypot( xClient / 2d0, yClient / 2d0 )

            fAngle = 0.0
            while( fAngle .lt. TWO_PI )do
                call MoveTo( hDC, 0, 0 )
                call LineTo( hDC,
     &                 hfix( real( fRadius * cos( fAngle ) + .5 ) ),
     &                 hfix( real( (-fRadius) * sin( fAngle ) + .5 ) ) )
                fAngle = fAngle + TWO_PI / 360
            end while

            call EndPaint( hWnd, ps )
        case( WM_DESTROY )
            call DeleteObject( hRgnClip )
            call PostQuitMessage( 0 )
        case default
            WndProc = DefWindowProc( hWnd, iMessage, wParam, lParam )
            return
        end select

        WndProc = 0

        end
