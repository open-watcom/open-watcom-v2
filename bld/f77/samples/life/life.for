
        program life
c
c
c
        include         'gui.fi'
        integer         style
        record /QMSG/   qmsg
        integer         screen_x
        integer         screen_y


        call InitBD( Births, Deaths )
        CurvedSpace = .TRUE.
        Mode = MENU_RESUME
        MouseMode = MENU_FLIP_PATTERNS
        BitSize = MENU_MED_BIT
        CellBitMap = NULL
        MenuAlreadyCreated = .FALSE.
        RegionIsSelected = .FALSE.
        Button1Down = .FALSE.
        Button2Down = .FALSE.
        call InitArray()

        AnchorBlock = WinInitialize( 0 )
        if( AnchorBlock .eq. 0 ) stop

        hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 )
        if( hMessageQueue .eq. 0 ) stop
        if( WinRegisterClass( AnchorBlock, 'watcom'c, MainDriver,
     +                        CS_SIZEREDRAW, 0 ) .eq. 0 ) stop

        style = FCF_TITLEBAR .or. FCF_SYSMENU .or. FCF_SIZEBORDER .or.
     +          FCF_MINMAX .or. FCF_SHELLPOSITION .or. FCF_TASKLIST .or.
     +          FCF_MENU
        FrameHandle = WinCreateStdWindow( HWND_DESKTOP, 0,
     +                                    style, 'watcom'c,
     +                                    char(0), 0, NULL,
     +                                    RES_MAIN_MENU, WinHandle )
        if( FrameHandle .eq. 0 ) stop

        MenuHandle = WinWindowFromID( FrameHandle, FID_MENU )

        screen_x = WinQuerySysValue( HWND_DESKTOP, SV_CXFULLSCREEN )
        screen_y = WinQuerySysValue( HWND_DESKTOP, SV_CYFULLSCREEN )
        ScreenHeight = screen_y

        call WinSetWindowPos( FrameHandle, NULL,
     +                        screen_x / 6, screen_y / 6,
     +                        2 * screen_x / 3, 2 * screen_y / 3,
     +                        SWP_MOVE .or. SWP_SIZE .or. SWP_SHOW )

        call InitTimer()
        if( .not. ReadPatterns() ) stop
        call CreatePatternMenu()


        while( WinGetMsg( AnchorBlock, qmsg, NULL, 0, 0 ) ) do
            call WinDispatchMsg( AnchorBlock, qmsg )
        end while


        call FreeArray()
        call FiniTimer()

        call WinDestroyWindow( FrameHandle )
        call WinDestroyMsgQueue( hMessageQueue )
        call WinTerminate( AnchorBlock )

        end
