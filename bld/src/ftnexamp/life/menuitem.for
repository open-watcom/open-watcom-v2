
        subroutine MenuItem( item )
c
c       Handle a menu item which has been selected by the user.
c
        include 'gui.fi'
        integer item
        integer ps

        if( item .ge. MENU_PATTERN .and.
     +      item .le. MENU_PATTERN+NumberPatterns ) then
            call SelectOff()
            MouseMode = MENU_FLIP_PATTERNS
            Cursor = item-MENU_PATTERN
            return
        end if
        select case ( item )
        case ( MENU_TINY_BIT : MENU_HUGE_BIT )
            BitSize = item
            call InitBitMap()
            call SelectOff()
            call ReSizeArray( WindowWidth, WindowHeight )
            call GetPS( ps, .TRUE. )
            call RePaint( ps )
            call GetPS( ps, .FALSE. )

        case ( MENU_NEW_RULES )
            call WinDlgBox( HWND_DESKTOP, FrameHandle, Rules,
     +                      NULL, RES_RULES_DLG, NULL )

        case ( MENU_ABOUT )
            call WinDlgBox( HWND_DESKTOP, FrameHandle, About,
     +                      NULL, RES_ABOUT_DLG, NULL )

        case ( MENU_WRAP_AROUND )
            CurvedSpace = .TRUE.

        case ( MENU_BOUNDED_EDGES )
            CurvedSpace = .FALSE.

        case ( MENU_SINGLE_STEP )
            execute ToSingleStepMode

        case ( MENU_PAUSE )
            execute ToPauseMode

        case ( MENU_RESUME )
            execute ToResumeMode

        case ( MENU_FLIP_PATTERNS )
            execute ToPatternFlipMode

        case ( MENU_SELECT )
            execute ToSelectMode

        case ( MENU_SAVE )
            if( WinDlgBox( HWND_DESKTOP, FrameHandle, FileName,
     +                    NULL, RES_NAME_DLG, NULL ) .ne. 0 ) then
                call WritePatternFile()
            endif

        case ( MENU_LOAD )
            if( WinDlgBox( HWND_DESKTOP, FrameHandle, FileName,
     +                     NULL, RES_NAME_DLG, NULL ) .ne. 0 ) then
                call LoadPatternFile()
            endif

        case ( MENU_FASTEST )
            call TimerTurbo()
            execute ToResumeMode

        case ( MENU_CLEAR )
            call Clear()
            if( .not. RegionIsSelected ) execute ToPauseMode

        case ( MENU_GRID )
            DrawGrid = .not. DrawGrid
            call ReDrawScreen()

        case ( MENU_RANDOM )
            call Randomize()

        case ( MENU_ROTATE_M90 )
            call ReflectX()
            call ReflectXY()
            call CreatePatternMenu()

        case ( MENU_ROTATE_180 )
            call ReflectX()
            call ReflectY()
            call CreatePatternMenu()

        case ( MENU_ROTATE_P90 )
            call ReflectXY()
            call ReflectX()
            call CreatePatternMenu()

        case ( MENU_REFLECT_X )
            call ReflectX()
            call CreatePatternMenu()

        case ( MENU_REFLECT_Y )
            call ReflectY()
            call CreatePatternMenu()

        end select

        remote block ToPauseMode
        Mode = MENU_PAUSE
        end block

        remote block ToResumeMode
        Mode = MENU_RESUME
        call SelectOff()
        MouseMode = MENU_FLIP_PATTERNS
        end block


        remote block ToSelectMode
        MouseMode = MENU_SELECT
        Mode = MENU_PAUSE
        end block

        remote block ToPatternFlipMode
        call SelectOff()
        MouseMode = MENU_FLIP_PATTERNS
        end block


        remote block ToSingleStepMode
        call SelectOff()
        Mode = MENU_SINGLE_STEP
        MouseMode = Mode
        end block


        end
