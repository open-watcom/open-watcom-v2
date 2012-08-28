!-------------------------------------------------------------------------------
!
!  PROGRAM    : clock.for
!
!  PURPOSE    : To give a demonstration on the use of a timer in a windows
!               application.
!
!  PROCEDURES : HourHandPos  - Computes the hour hand position based on
!                              both the hour and minute values.
!
!               VertEquiv    - Computes the raster line equivalent to the
!                              given pixel value.
!
!               HorzEquiv    - Computes the pixel equivalent to the given
!                              raster line value.
!
!               About        - Dialog function for the About Dialog.
!
!               ClockWndProc - Window function for the application.
!
!               CreateTools  - Creates brushes and pens to coincide with
!                              the current system colors.
!
!               DeleteTools  - Destroys the brushes and pens created by
!                              CreateTools.
!
!               ClockCreate  - Performs the necessary initialization for
!                              drawing the clock correctly and gets the
!                              initial time to be displayed by the clock.
!
!               ClockSize    - Resize the clock to the largest possible
!                              circle that will fit in the client area.
!
!               ClockTimer   - Update the clock to reflect the most recent
!                              time.
!
!               ClockPaint   - Paint the clock to display the most recent
!                              time.
!
!               DrawFace     - Draws the clock face.
!
!               DrawHand     - Draws a thin hand with the specified brush
!                              in the specified hand position.
!
!               DrawFatHand  - Draws a fat hand with the specified brush
!                              in the specified hand position.
!
!               CircleClock  - Resizes clock rectangle to keep clock
!                              circular.
!
!               WinMain      - Calls the initialization function, creates
!                              the main application window, and enters the
!                              message loop.
!
!               ClockInit    - Registers the application window class and
!                              initializes the circle values for the clock
!                              face.
!
!-------------------------------------------------------------------------------

c$include winapi.fi

!-------------------------------------------------------------------------------
!
!  FUNCTION : strlen
!
!  PURPOSE  : length of NULL-terminated string
!
!-------------------------------------------------------------------------------

        integer*4 function strlen( str )

        character*(*) str

        do strlen = 1, len( str )
            if( str(strlen:strlen) .eq. char(0) ) quit
        end do

        strlen = strlen - 1

        end

!-------------------------------------------------------------------------------
!
!  SUBROUTINE : GetTime
!
!  PURPOSE    : Get current time from system
!
!-------------------------------------------------------------------------------

        subroutine GetTime( time )

        include 'clock.fi'

        record /time/ time
        integer*2 tics

        call gettim( time.hour, time.minute, time.second, tics )
        if( time.hour .ge. 12 ) time.hour = time.hour - 12

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : HourHandPos
!
!  PURPOSE  : Computes the hour hand position based on both the hour and
!             minute values in the given time record.
!
!-------------------------------------------------------------------------------

        integer*2 function HourHandPos( time )

        include 'clock.fi'

        record /time/ time

        HourHandPos = ( time.hour * 5)  + ( time.minute / 12 )

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : VertEquiv
!
!  PURPOSE  : Computes the raster line (vertical) equivalent to the given
!             pixel (horizontal) value.
!
!-------------------------------------------------------------------------------

        integer*2 function VertEquiv( lengthH )

        integer*2 lengthH

        include 'clock.fi'

        VertEquiv = ( lengthH * AspectV ) / AspectH

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : HorzEquiv
!
!  PURPOSE  : Computes the pixel (horizontal) equivalent to the given
!             raster line (vertical) value.
!
!-------------------------------------------------------------------------------

        integer*2 function HorzEquiv( lengthV )

        integer*2 lengthV

        include 'clock.fi'

        HorzEquiv = ( lengthV * AspectH ) / AspectV

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : About
!
!  PURPOSE  : Dialog function for the "About..." menu item dialog.
!
!-------------------------------------------------------------------------------

c$pragma aux (callback) About parm( value, value, value, value )

        integer*4 function About( hDlg, message, wParam, lParam )

        integer*2 hDlg
        integer*2 message
        integer*2 wParam
        integer*4 lParam

        include 'clock.fi'
        include 'winmsgs.fi'
        include 'windlg.fi'

        select case( message )
        case( WM_COMMAND )
            call EndDialog( hDlg, 1 )
            About = 1
        case( WM_INITDIALOG )
            About = 1
        case default
            About = 0
        end select

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : ClockWndProc
!
!  PURPOSE  : Window function for the application.
!
!-------------------------------------------------------------------------------

c$pragma aux (callback) ClockWndProc parm( value, value, value, value )

        integer*4 function ClockWndProc( hWnd, msg, wParam, lParam )

        include 'clock.fi'
        include 'win386.fi'
        include 'winmsgs.fi'
        include 'winmsg.fi'
        include 'winpaint.fi'
        include 'windlg.fi'
        include 'wincreat.fi'
        include 'wininput.fi'
        include 'windisp.fi'
        include 'winutil.fi'
        include 'winmodul.fi'

        integer*2       hWnd
        integer*2       msg
        integer*2       wParam
        integer*4       lParam

        integer*4               cbp
        record /RECT/           rc
        record /PAINTSTRUCT/    ps
        external                About
        integer*4               About

        select case( msg )
        case( WM_SYSCOMMAND )
            if( wParam .eq. IDM_ABOUT )then
                ! Draw and handle messages for the "About..." Dialog
                cbp = GetProc16( About, GETPROC_CALLBACK )
                call DialogBox( hInst, MAKEINTRESOURCE( 1 ),
     &                          hWnd, MakeProcInstance( cbp, hInst ) )
                call ReleaseProc16( cbp )
            else
                ! Perform the default window processing
                ClockWndProc = DefWindowProc( hWnd, msg, wParam,lParam )
                return
            end if
        case( WM_SIZE )
            ! Resize clock based on window size and redraw
            call ClockSize( hWnd, LOWORD( lParam ), HIWORD( lParam ),
     &                      wParam )
            call UpdateWindow( hWnd )
        case( WM_DESTROY )
            ! Destroy clock's timer and tools before exiting
            call KillTimer( hWnd, TimerID )
            call DeleteTools()
            call PostQuitMessage( 0 )
        case( WM_PAINT )
            ! Paint clock displaying current time
            call InvalidateRect( hWnd, NULL_POINTER, .TRUE. )
            call BeginPaint( hWnd, ps )
            call ClockPaint( hWnd, ps.hdc, PAINTALL )
            call EndPaint( hWnd, ps )
        case( WM_TIMECHANGE, WM_TIMER )
            ! Update clock to display new time
            call ClockTimer( hWnd )
        case( WM_SYSCOLORCHANGE )
            ! Change tools to coincide with system window colors
            call DeleteTools()
            call CreateTools()
        case( WM_ERASEBKGND )
            ! Paint over the entire client area
            call GetClientRect( hWnd, rc )
            call FillRect( wParam, rc, hbrBackgnd )
        case default
            ! Perform the default window processing
            ClockWndProc = DefWindowProc( hWnd, msg, wParam, lParam )
            return
        end select

        ClockWndProc = 0

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : CreateTools
!
!  PURPOSE    : Creates brushes and pens to coincide with the current
!               system colors.
!
!-------------------------------------------------------------------------------

        subroutine CreateTools()

        include 'clock.fi'
        include 'windtool.fi'
        include 'winsysm.fi'

        hbrForegnd = CreateSolidBrush( GetSysColor( COLOR_WINDOWTEXT ) )
        hbrBackgnd = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) )
        hpenForegnd = CreatePen( 0, 1, GetSysColor( COLOR_WINDOWTEXT ) )
        hpenBackgnd = CreatePen( 0, 1, GetSysColor( COLOR_WINDOW ) )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : DeleteTools
!
!  PURPOSE    : Destroys the brushes and pens created by CreateTools.
!
!-------------------------------------------------------------------------------

        subroutine DeleteTools()

        include 'clock.fi'
        include 'windtool.fi'

        call DeleteObject( hbrForegnd )
        call DeleteObject( hbrBackgnd )
        call DeleteObject( hpenForegnd )
        call DeleteObject( hpenBackgnd )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : ClockCreate
!
!  PURPOSE    : First, for drawing the clock, ClockCreate computes the
!               aspect ratio and creates the necessary pens and brushes.
!               Then, if this is the first instance of the app running,
!               ClockCreate scales the circle table values according to the
!               aspect ratio. Finally, ClockCreate gets the initial time.
!
!-------------------------------------------------------------------------------

        subroutine ClockCreate()

        include 'clock.fi'
        include 'winpaint.fi'
        include 'windev.fi'
        include 'winmem.fi'

        integer         pos       ! hand position index into the circle table
        integer         _vertSize ! height of the display in millimeters
        integer         _horzSize ! width of the display in millimeters
        integer*2       hDC
        record /POINT/  lpCirTab(:)
        integer*2       VertEquiv
        integer*2       prev_vert

c$pragma array lpCirTab far

        ! Get display size in (pixels X raster lines)
        ! and in (millimeters X millimeters)
        hDC = GetDC( NULL )
        _VertRes = GetDeviceCaps( hDC, VERTRES )
        _HorzRes = GetDeviceCaps( hDC, HORZRES )
        _vertSize = GetDeviceCaps( hDC, VERTSIZE )
        _horzSize = GetDeviceCaps( hDC, HORZSIZE )
        call ReleaseDC( NULL, hDC )

        ! Compute (raster lines / decimeter) and (pixels / decimeter)
        AspectV = (_VertRes * MMPERDM) / _vertSize
        AspectH = (_HorzRes * MMPERDM) / _horzSize

        call CreateTools()

        ! Scale cosines for aspect ratio if this is the first instance
        if( bFirst )then
            allocate( lpCirTab(0:HANDPOSITIONS-1),
     &                location=GlobalLock( hCirTab ) )
            do pos = 0, HANDPOSITIONS - 1
                prev_vert = lpCirTab(pos).y
                lpCirTab(pos).y = VertEquiv( prev_vert )
            end do
            call GlobalUnlock( hCirTab )
            deallocate( lpCirTab )
        end if

        call GetTime( oTime )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : ClockSize
!
!  PURPOSE    : Resize the clock to the largest possible circle that will
!               fit in the client area. If switching from not iconic to
!               iconic, alter the timer to update every minute.  And if
!               switching back to non iconic, restore the timer to update
!               every second.
!
!-------------------------------------------------------------------------------

        subroutine ClockSize( hWnd, newWidth, newHeight, sizeType )
        integer*2       hWnd
        integer*2       newWidth
        integer*2       newHeight
        integer*2       sizeType

        include 'clock.fi'
        include 'winrect.fi'
        include 'wininput.fi'
        include 'winmsgs.fi'

        ! Set ClockRect to bound the largest possible circle in the window
        call SetRect( ClockRect, 0, 0, newWidth, newHeight )
        call CircleClock( newWidth, newHeight )

        if( sizeType .eq. SIZEICONIC )then
            ! Update once every minute in the iconic state
            call KillTimer( hWnd, TimerID )
            call SetTimer( hWnd, TimerID, ICON_TLEN, 0 )
            bIconic = .TRUE.
        else if( bIconic )then
            ! Update every second in the opened state (ignore tiling)
            call KillTimer( hWnd, TimerID )
            call SetTimer( hWnd, TimerID, OPEN_TLEN, 0 )
            bIconic = .FALSE.
        end if

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : ClockTimer
!
!  PURPOSE    : Update the clock to reflect the most recent time.
!
!-------------------------------------------------------------------------------

        subroutine ClockTimer( hWnd )
        integer*2       hWnd

        include 'clock.fi'
        include 'winpaint.fi'

        record /TIME/   nTime
        integer*2       hDC

        call GetTime( nTime )

        ! It's possible to change any part of the system at any time through
        ! the Control Panel. Check for any change in second, minute, or hour
        if( ( nTime.second .ne. oTime.second) .or.
     &      ( nTime.minute .ne. oTime.minute) .or.
     &      ( nTime.hour   .ne. oTime.hour ) )then
            ! The time has changed -- update the clock
            hDC = GetDC( hWnd )
            call ClockPaint( hWnd, hDC, HANDPAINT )
            call ReleaseDC( hWnd, hDC )
        end if

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : ClockPaint
!
!  PURPOSE    : Paint the clock to display the most recent time.
!
!-------------------------------------------------------------------------------

        subroutine ClockPaint( hWnd, hDC, paintType )
        integer*2       hWnd
        integer*2       hDC
        integer         paintType

        include 'clock.fi'
        include 'winmem.fi'
        include 'winpaint.fi'
        include 'windattr.fi'

        record /TIME/   nTime
        integer*2       HourHandPos

        call SetBkMode( hDC, TRANSPARENT )

        CirTab = GlobalLock( hCirTab )
        if( paintType .eq. PAINTALL )then
            ! Paint entire clock -- face and hands
            call FillRect( hDC, ClockRect, hbrBackgnd )
            call DrawFace( hDC )
            call DrawFatHand( hDC, HourHandPos( oTime ), hpenForegnd,
     &                        HHAND )
            call DrawFatHand( hDC, oTime.minute, hpenForegnd, MHAND )
            if( .not. bIconic )then
                ! Erase old second hand
                call DrawHand( hDC, oTime.second, hpenBackgnd,
     &                         SECONDTIP, R2_NOT )
            end if
        else if( paintType .eq. HANDPAINT )then
            call GetTime( nTime )

            if( ( .not. bIconic ) .and.
     &          ( nTime.second .ne. oTime.second ) )then
                ! Second has changed -- erase old second hand
                call DrawHand( hDC, oTime.second, hpenBackgnd,
     &                         SECONDTIP, R2_NOT )
            end if

            if( ( nTime.minute .ne. oTime.minute ) .or.
     &          ( nTime.hour .ne. oTime.hour ) )then
                ! Hour and/or minute have changed -- update hands
                if( bIconic )then
                    ! Erase old minute and hour hands
                    call DrawHand( hDC, oTime.minute, hpenBackgnd,
     &                             MINUTETIP, R2_COPYPEN )
                    call DrawHand( hDC, HourHandPos( oTime ),
     &                             hpenBackgnd, HOURTIP, R2_COPYPEN )

                    ! Draw new minute and hour hands
                    call DrawHand( hDC, nTime.minute, hpenForegnd,
     &                             MINUTETIP, R2_COPYPEN )
                    call DrawHand( hDC, HourHandPos( nTime ),
     &                             hpenForegnd, HOURTIP, R2_COPYPEN )
                else
                    ! Erase old minute and hour fat hands
                    call DrawFatHand( hDC, oTime.minute,
     &                                hpenBackgnd, MHAND )
                    call DrawFatHand( hDC, HourHandPos( oTime ),
     &                                hpenBackgnd, HHAND )

                    ! Draw new minute and hour fat hands
                    call DrawFatHand( hDC, nTime.minute,
     &                                hpenForegnd, MHAND )
                    call DrawFatHand( hDC, HourHandPos( nTime ),
     &                                hpenForegnd, HHAND )
                end if
            end if

            if( ( .not. bIconic ) .and.
     &          ( nTime.second .ne. oTime.second ) )then
                ! second has changed -- draw new second hand
                call DrawHand( hDC, nTime.second, hpenBackgnd,
     &                         SECONDTIP, R2_NOT )
            end if

            ! Store most recent time
            oTime.minute = nTime.minute
            oTime.hour   = nTime.hour
            oTime.second = nTime.second
        end if
        call GlobalUnlock( hCirTab )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : DrawFace
!
!  PURPOSE    : Draws the clock face.
!
!-------------------------------------------------------------------------------

        subroutine DrawFace( hDC )
        integer*2       hDC ! device context to be used when drawing face

        include 'clock.fi'
        include 'winpaint.fi'

        integer*2       pos       ! hand position index into the circle table
        integer*2       dotHeight ! height of the hour-marking dot
        integer*2       dotWidth  ! width of the hour-marking dot
        record /POINT/  dotCenter ! center point of the hour-marking dot
        record /RECT/   rc
        record /POINT/  lpCirTab(:)
        integer*2       VertEquiv

c$pragma array lpCirTab far

        ! Compute hour-marking dot width, height, and center point
        dotWidth = (MAXDOTWIDTH * (ClockRect.right - ClockRect.left)) /
     &              _HorzRes
        dotHeight = VertEquiv( dotWidth )

        if( dotHeight .lt. MINDOTHEIGHT )then
            dotHeight = MINDOTHEIGHT
        end if

        if( dotWidth .lt. MINDOTWIDTH )then
            dotWidth = MINDOTWIDTH
        end if

        dotCenter.x = dotWidth / 2
        dotCenter.y = dotHeight / 2

        ! Compute the clock center and radius
        call InflateRect( ClockRect, -dotCenter.y, -dotCenter.x )
        ClockRadius = ( ClockRect.right - ClockRect.left ) / 2
        ClockCenter.x = ClockRect.left + ClockRadius
        ClockCenter.y = ClockRect.top +
     &                  ( ClockRect.bottom - ClockRect.top ) / 2
        call InflateRect( ClockRect, dotCenter.y, dotCenter.x )

        ! Draw the large hour-marking dots and small minute-marking dots
        allocate( lpCirTab(0:HANDPOSITIONS-1), location=CirTab )
        do pos = 0, HANDPOSITIONS - 1
            rc.top = (lpCirTab(pos).y * ClockRadius) / CIRTABSCALE +
     &               ClockCenter.y
            rc.left = (lpCirTab(pos).x * ClockRadius) / CIRTABSCALE +
     &                ClockCenter.x
            if( mod( pos, 5 ) .ne. 0 )then
                if( ( dotWidth .gt. MINDOTWIDTH ) .and.
     &              ( dotHeight .gt. MINDOTHEIGHT ) )then
                    ! Draw small minute-marking dot
                    rc.right = rc.left + 1
                    rc.bottom = rc.top + 1
                    call FillRect( hDC, rc, hbrForegnd )
                end if
            else
                ! Draw large hour-marking dot
                rc.right = rc.left + dotWidth
                rc.bottom = rc.top + dotHeight
                call OffsetRect( rc, -dotCenter.x, -dotCenter.y )
                call FillRect( hDC, rc, hbrForegnd )
            end if
        end do
        deallocate( lpCirTab )

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : DrawHand
!
!  PURPOSE  : Draws a thin hand with the specified pen in the specified
!             hand position.
!
!-------------------------------------------------------------------------------

        subroutine DrawHand( hDC, pos, hPen, scale, patMode )
        integer*2       hDC     ! device context to be used when drawing hand
        integer*2       pos     ! hand position index into the circle table
        integer*2       hPen    ! pen to be used when drawing hand
        integer*4       scale   ! ClockRadius percentage to scale drawing to
        integer*4       patMode ! pattern mode to be used when drawing hand

        include 'clock.fi'
        include 'windattr.fi'
        include 'windtool.fi'
        include 'winline.fi'

        record /POINT/  lpCirTab(:)
        integer*4       radius

c$pragma array lpCirTab far

        ! scale length of hand
        radius = (ClockRadius * scale) / 100

        ! set pattern mode for hand
        call SetROP2( hDC, patMode )

        ! select pen for hand
        call SelectObject( hDC, hPen )

        ! Draw thin hand
        call MoveTo( hDC, ClockCenter.x, ClockCenter.y )
        allocate( lpCirTab(0:HANDPOSITIONS-1), location=CirTab )
        call LineTo( hDC, ClockCenter.x +
     &               ((lpCirTab(pos).x * radius) / CIRTABSCALE),
     &               ClockCenter.y +
     &               ((lpCirTab(pos).y * radius) / CIRTABSCALE) )
        deallocate( lpCirTab )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTEINE : DrawFatHand
!
!  PURPOSE     : Draws a fat hand with the specified pen in the specified
!                hand position.
!
!-------------------------------------------------------------------------------

        subroutine DrawFatHand( hDC, pos, hPen, _hHand )
        integer*2       hDC     ! device context to be used when drawing hand
        integer*2       pos     ! hand position index into the circle table
        integer*2       hPen    ! pen to be used when drawing hand
        logical         _hHand  ! TRUE if drawing hour hand; FALSE otherwise

        include 'clock.fi'
        include 'windattr.fi'
        include 'windtool.fi'
        include 'winline.fi'

        record /POINT/  lpCirTab(:)
        record /POINT/  ptTip   ! coordinates for the tip of the hand
        record /POINT/  ptTail  ! coordinates for the tail of the hand
        record /POINT/  ptSide  ! coordinates for the side of the hand
        integer         index   ! position index into the circle table
        integer*4       scale   ! ClockRadius percentage to scale drawing to

c$pragma array lpCirTab far

        ! set pattern mode for hand
        call SetROP2( hDC, 13 )

        ! select pen for hand
        call SelectObject( hDC, hPen )

        allocate( lpCirTab(0:HANDPOSITIONS-1), location=CirTab )

        ! compute coordinates for the side of the hand
        if( _hHand )then
            scale = ( ClockRadius * HOURSIDE ) / 100
        else
            scale = ( ClockRadius * MINUTESIDE ) / 100
        end if
        index = mod( pos + SIDESHIFT, HANDPOSITIONS )
        ptSide.y = (lpCirTab(index).y * scale) / CIRTABSCALE
        ptSide.x = (lpCirTab(index).x * scale) / CIRTABSCALE

        ! compute coordinates for the tip of the hand
        if( _hHand )then
            scale = ( ClockRadius * HOURTIP ) / 100
        else
            scale = ( ClockRadius * MINUTETIP ) / 100
        end if
        ptTip.y = (lpCirTab(pos).y * scale) / CIRTABSCALE
        ptTip.x = (lpCirTab(pos).x * scale) / CIRTABSCALE

        ! compute coordinates for the tail of the hand
        if( _hHand )then
            scale = ( ClockRadius * HOURTAIL ) / 100
        else
            scale = ( ClockRadius * MINUTETAIL ) / 100
        end if
        index = mod( pos + TAILSHIFT, HANDPOSITIONS )

        ptTail.y = (lpCirTab(index).y * scale) / CIRTABSCALE
        ptTail.x = (lpCirTab(index).x * scale) / CIRTABSCALE

        deallocate( lpCirTab )

        ! Draw tip of hand
        call MoveTo( hDC, ClockCenter.x + ptSide.x,
     &               ClockCenter.y + ptSide.y )
        call LineTo( hDC, ClockCenter.x +  ptTip.x,
     &               ClockCenter.y +  ptTip.y )
        call MoveTo( hDC, ClockCenter.x - ptSide.x,
     &               ClockCenter.y - ptSide.y )
        call LineTo( hDC, ClockCenter.x +  ptTip.x,
     &               ClockCenter.y +  ptTip.y )

        ! Draw tail of hand
        call MoveTo( hDC, ClockCenter.x + ptSide.x,
     &               ClockCenter.y + ptSide.y )
        call LineTo( hDC, ClockCenter.x + ptTail.x,
     &               ClockCenter.y + ptTail.y )
        call MoveTo( hDC, ClockCenter.x - ptSide.x,
     &               ClockCenter.y - ptSide.y )
        call LineTo( hDC, ClockCenter.x + ptTail.x,
     &               ClockCenter.y + ptTail.y )

        end


!-------------------------------------------------------------------------------
!
!  SUBROUTINE : CircleClock
!
!  PURPOSE    : Resizes the clock rectangle to keep the face circular.
!
!-------------------------------------------------------------------------------

        subroutine CircleClock( maxWidth, maxHeight )
        integer*2 maxWidth    ! the maximum width of the clock face
        integer*2 maxHeight   ! the maximum height of the clock face

        include 'clock.fi'

        integer*2 clockHeight ! tallest height that will keep face circular
        integer*2 clockWidth  ! widest width that will keep face circular
        integer*2 VertEquiv
        integer*2 HorzEquiv

        if( maxWidth .gt. HorzEquiv( maxHeight ) )then
            ! too wide -- decrease width to keep face circular
            clockWidth = HorzEquiv( maxHeight )
            ClockRect.left = ClockRect.left +
     &                       ( maxWidth - clockWidth ) / 2
            ClockRect.right = ClockRect.left + clockWidth
        else
            ! too tall -- decrease height to keep face circular
            clockHeight = VertEquiv( maxWidth )
            ClockRect.top = ClockRect.top +
     &                      ( maxHeight - clockHeight ) / 2
            ClockRect.bottom = ClockRect.top + clockHeight
        end if

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : FWinMain
!
!  PURPOSE  : Calls the initialization function, creates the main application
!             window, and enters the message loop.
!
!-------------------------------------------------------------------------------

        integer*2 function FWINMAIN( hInstance,
     &                               hPrev,
     &                               lpszCmdLine,
     &                               cmdShow )
        integer*2 hInstance
        integer*2 hPrev
        integer*4 lpszCmdLine
        integer*2 cmdShow

        include 'clock.fi'
        include 'winresrc.fi'
        include 'winmodul.fi'
        include 'winsysm.fi'
        include 'wincreat.fi'
        include 'wininput.fi'
        include 'winerror.fi'
        include 'winmenu.fi'
        include 'winmsg.fi'
        include 'windisp.fi'

        integer*2     hWnd
        record /MSG/  msg
        integer*2     hMenu
        record /TIME/ nTime
        integer*2     sysWidth  ! width of left and right frames
        integer*2     sysHeight ! height of caption bar and top & bottom frames
        integer*2     width     ! width of entire clock window
        integer*2     height    ! height of entire clock window
        character*40  szTooMany
        logical       ClockInit
        integer*2     VertEquiv

        data bFirst/.TRUE./
        data bIconic/.FALSE./
        data TimerId/1/

        hInst = hInstance

        call LoadString( hInst, IDS_APPNAME, szBuffer, BUFLEN )

        if( .not. ClockInit() )then
            FWinMain = 0
            return
        end if
        if( hPrev .ne. NULL )then
            ! Not first instance -- get circle table and reset bFirst flag
            call GetInstanceData( hPrev, hCirTab, 2 )
            bFirst = .FALSE.
        end if

        call ClockCreate()

        ! compute window height and width
        sysWidth  = GetSystemMetrics( SM_CXFRAME ) * 2
        sysHeight = GetSystemMetrics( SM_CYCAPTION ) +
     &              2 * GetSystemMetrics( SM_CYFRAME )
        width = (_HorzRes / 3) + sysWidth
        height = VertEquiv( width ) + sysHeight

        hWnd = CreateWindow( _class,        ! class name
     &               'Clock'//char(0),      ! window name
     &               WS_TILEDWINDOW,        ! window style
     &               CW_USEDEFAULT,         ! use default positioning
     &               0,                     ! y not used
     &               width,                 ! window width
     &               height,                ! window height
     &               NULL,                  ! NULL parent handle
     &               NULL,                  ! NULL menu/child handle
     &               hInst,                 ! program instance
     &               NULL )                 ! NULL data structure reference

        call GetTime( nTime )
        call GetTime( oTime )
        while( ( nTime.second .eq. oTime.second ) .and.
     &         ( nTime.minute .eq. oTime.minute ) .and.
     &         ( nTime.hour   .eq. oTime.hour ) )do
            call GetTime( oTime )
        end while

        if( SetTimer( hWnd, TimerID, OPEN_TLEN, 0 ) .eq. 0 )then
            ! 16 public timers already in use -- post error and exit
            call LoadString( hInst, IDS_TOOMANY, szTooMany, 40 )
            call MessageBox( NULL, szTooMany, szBuffer,
     &                       MB_OK .or. MB_ICONHAND .or.
     &                       MB_SYSTEMMODAL )
            call DeleteTools()
            FWinMain = 0
            return
        end if

        ! Add the "About..." menu item to the bottom of the system menu
        call LoadString( hInst, IDS_ABOUTMENU, szBuffer, BUFLEN )
        hMenu = GetSystemMenu( hWnd, .FALSE. )
        call ChangeMenu( hMenu, 0, szBuffer, IDM_ABOUT,
     &                   MF_APPEND .or. MF_STRING )

        call ShowWindow( hWnd, cmdShow )

        ! Process messages until program termination
        while( GetMessage( msg, NULL, 0, 0 ) .ne. 0 )do
            call TranslateMessage( msg )
            call DispatchMessage( msg )
        end while


        FWinMain = msg.wParam

        end


!-------------------------------------------------------------------------------
!
!  FUNCTION : ClockInit
!
!  PURPOSE  : Registers the applicatoin window class and initializes the
!             circle values for the clock face.
!
!-------------------------------------------------------------------------------

        logical function ClockInit()

        include 'clock.fi'
        include 'win386.fi'
        include 'wincreat.fi'
        include 'winresrc.fi'
        include 'winerror.fi'
        include 'wincurs.fi'

        record /WNDCLASS/       ClockClass
        integer*2               hRes
        character*5             szData
        integer*4               cbp

        external                ClockWndProc
        integer*4               ClockWndProc

        integer*4               strlen
        external                strlen

        cbp = GetProc16( ClockWndProc, GETPROC_CALLBACK )

        write( _class, '(a,i5.5,a)' ) szBuffer(1:strlen(szBuffer)),
     &                                hinst, char(0)
        ClockClass.cbClsExtra    = 0
        ClockClass.cbWndExtra    = 0
        ClockClass.lpszMenuName  = 0
        ClockClass.lpszClassName = loc( _class )
        ClockClass.hbrBackground = NULL
        ClockClass.style         = CS_VREDRAW .or. CS_HREDRAW .or.
     &                             CS_BYTEALIGNCLIENT
        ClockClass.hInstance     = hInst
        ClockClass.lpfnWndProc   = cbp
        ClockClass.hCursor       = LoadCursor( NULL, IDC_ARROW )
        ClockClass.hIcon         = NULL

        if( RegisterClass( ClockClass ) .eq. 0 )then
            ! Error registering class -- return
            ClockInit = .FALSE.
            return
        endif

        ! Load in pre-computed circle table cosine values from resource file
        call LoadString( hInst, IDS_DATA, szData, 5 )
        hRes = FindResource( hInst, szBuffer, szData )
        if( hRes .eq. NULL )then
            ! Could not find circle table resource data -- return
            call MessageBox( NULL, 'No Circle Class'//char(0), char(0),
     &                       MB_OK .or. MB_ICONHAND .or.
     &                       MB_SYSTEMMODAL )
            ClockInit = .FALSE.
            return
        endif

        hCirTab = LoadResource( hInst, hRes )
        call LockResource( hCirTab )

        ClockInit = .TRUE.

        end
