
        subroutine HandleMouse( x, y )
        integer x,y
c
c       Process a mouse event at (pixel_x,pixel_y)
c

        include 'life.fi'

        if( Button1Down ) then
            if( MoveSelect( x, y ) ) return
        end if
        if( .not. Button1Down .and. .not. Button2Down ) return
        if( MouseMode .ne. MENU_FLIP_PATTERNS ) return
        if( x .gt. WindowWidth .or. x .lt. 0 ) return
        if( y .gt. WindowHeight .or. y .lt. 0 ) return
        call DrawPattern( x, y, Button1Down )

        end

        function MainDriver( hwnd, msg, mp1, mp2 )
        integer hwnd
        integer msg
        integer mp1
        integer mp2

        integer         ps
        record /RECTL/  rcl
        integer         height
        integer         width
        record /POINTL/ pt
        integer         x, y

        include 'gui.fi'


        x = SHORT1FROMMP( mp1 )
        y = SHORT2FROMMP( mp1 )
        select case ( msg )
        case ( WM_INITMENU )
            call InitMenu()

        case ( WM_COMMAND )
            call MenuItem( x )

        case ( WM_BUTTON2DOWN )
            call WinSetCapture( HWND_DESKTOP, WinHandle )
            Button2Down = .TRUE.

        case ( WM_BUTTON2UP )
            call WinSetCapture( HWND_DESKTOP, NULL )
            Button2Down = .FALSE.

        case ( WM_BUTTON1UP )
            Button1Down = .FALSE.
            call EndSelect( x, y )
            call WinSetCapture( HWND_DESKTOP, NULL )

        case ( WM_BUTTON1DOWN )
            call WinSetCapture( HWND_DESKTOP, WinHandle )
            Button1Down = .TRUE.
            if( .not. SingleStep() ) then
                if( .not. StartSelect( x, y ) ) then
                    call HandleMouse( x, y )
                end if
            end if

        case ( WM_MOUSEMOVE )
            call HandleMouse( x, y )

        case ( WM_CHAR )
            if( x .and. KC_KEYUP ) then
                call SingleStep()
            end if
        case ( WM_TIMER )
            if( Mode .eq. MENU_RESUME ) then
                call NewTimer()
                call NextGeneration()
                call CheckTimerRate()
                MainDriver = 0
                return
            end if
        case ( WM_PAINT )
            ps = WinBeginPaint( WinHandle, NULL, NULL_POINTER )
            call RePaint( ps )
            call WinEndPaint( ps )
            MainDriver = 0
            return
        case ( WM_SIZE )
            call SelectOff()
            if( ReSizeArray( SHORT1FROMMP( mp2 ), SHORT2FROMMP( mp2 ), 0 ) ) then
                call ReDrawScreen()
                MainDriver = 0
                return
            end if
        case ( WM_CREATE )
            WinHandle = hwnd
            BitSize = MENU_MED_BIT
            call InitBitMap()
        end select

        MainDriver = WinDefWindowProc( WinHandle, msg, mp1, mp2 )


        end



        function SingleStep()

        include 'life.fi'

        if( Mode .ne. MENU_SINGLE_STEP ) then
            SingleStep = .FALSE.
            return
        end if
        call NextGeneration()
        SingleStep = .TRUE.

        end
