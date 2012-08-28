
c$define INCL_WINFRAMEMGR
c$define INCL_WINMESSAGEMGR
c$define INCL_WINWINDOWMGR
c$define INCL_WINTIMER
c$define INCL_GPIPRIMITIVES
c$include os2.fap

        program fshapes

        integer         style
        record /QMSG/   qmsg

        character*7     watcom
        parameter       (watcom='WATCOM'//char(0))

        include 'fshapes.fi'


        AnchorBlock = WinInitialize( 0 )
        if( AnchorBlock .eq. 0 ) stop
        hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 )
        if( hMessageQueue .eq. 0 ) stop
        if( WinRegisterClass( AnchorBlock, watcom, MainDriver,
     +                        CS_SIZEREDRAW, 0 ) .eq. 0 ) stop
        style = FCF_TITLEBAR .or. FCF_SYSMENU .or. FCF_SIZEBORDER .or.
     +          FCF_MINMAX .or. FCF_SHELLPOSITION .or. FCF_TASKLIST
        FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
     +                                    style, watcom,
     +                                    char(0), 0, NULL,
     +                                    0, WinHandle )
        if( FrameHandle .eq. 0 ) stop

        while( WinGetMsg( AnchorBlock, qmsg, NULL, 0, 0 ) ) do
            call WinDispatchMsg( AnchorBlock, qmsg )
        end while

        call WinDestroyWindow( FrameHandle )
        call WinDestroyMsgQueue( hMessageQueue )
        call WinTerminate( AnchorBlock )

        end

        function MainDriver( hwnd, msg, mp1, mp2 )

        integer hwnd
        integer msg
        integer mp1
        integer mp2

        include 'fshapes.fi'

        integer         ps
        record /RECTL/  rcl

        select case ( msg )
        case ( WM_CREATE )
            WinHandle = hwnd
            call WinStartTimer( AnchorBlock, WinHandle, 1, 150 )
        case ( WM_TIMER )
            call DrawEllipse()
            MainDriver = 0
            return
        case ( WM_SIZE )
            SizeX = SHORT1FROMMP( mp2 )
            SizeY = SHORT2FROMMP( mp2 )
            MainDriver = 0
            return
        case ( WM_PAINT )
            ps = WinBeginPaint( WinHandle, NULL, NULL_POINTER )
            call WinQueryWindowRect( WinHandle, rcl )
            call WinFillRect( ps, rcl, CLR_WHITE )
            call WinEndPaint( ps )
            MainDriver = 0
            return
        end select

        MainDriver = WinDefWindowProc( WinHandle, msg, mp1, mp2 )
        return

        end

        subroutine DrawEllipse

        record /POINTL/         ptl
        integer                 ps
        integer                 Odd /0/
        integer                 parm1
        integer                 parm2

        include 'fshapes.fi'

        ps = WinGetPS( WinHandle )
        ptl.x = Random( SizeX )
        ptl.y = Random( SizeY )
        call GpiMove( ps, ptl )
        ptl.x = Random( SizeX )
        ptl.y = Random( SizeY )
        parm1 = Random( 32767 )
        parm2 = Random( 32767 )
        if( Random( 10 ) .ge. 5 ) then
            execute NewColor
            call GpiBox( ps, DRO_FILL, ptl, 0, 0 )
            execute NewColor
            call GpiBox( ps, DRO_OUTLINE, ptl, 0, 0 )
        else
            execute NewColor
            call GpiBox( ps, DRO_FILL, ptl, parm1, parm2 )
            execute NewColor
            call GpiBox( ps, DRO_OUTLINE, ptl, parm1, parm2 )
        end if

        Odd = Odd + 1
        Odd = Odd .and. 1
        call WinReleasePS( ps )

        remote block NewColor
        call GpiSetColor( ps, Random( 15 ) + 1 )
        end block

        end

        integer function Random( high )
        integer         high

        external        urand
        real            urand
        integer         seed /75347/
        Random = urand( seed ) * high

        end
