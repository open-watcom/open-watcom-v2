
        subroutine CreatePatternMenu()
c
c       Create the "Pattern" menu, based upon MenuPatterns()
c
        include                 'gui.fi'

        integer                 ps
        integer                 menu_height
        integer                 menu_type
        integer                 i
        integer                 dc
        integer                 x,y
        integer                 dim_x, dim_y
        integer                 grid_x, grid_y
        integer                 size_x, size_y
        integer*1               pattern(:,:)
        record /SIZEL/          sizel
        record /MENUITEM/       mi
        record /DEVOPENSTRUC/   Dop/NULL,NULL,NULL,NULL,
     +                              NULL,NULL,NULL,NULL,NULL/
        record /BITMAPINFOHEADER/ MenuBitInfo
        record /BITMAPINFOHEADER/ bit_info
        record /POINTL/         aptl(5)
        integer                 text

        Dop.pszDriverName = loc( 'DISPLAY'c )

        if( MenuBitMap .eq. NULL ) then
            ps = WinGetPS( WinHandle )
            MenuBitMap = GpiLoadBitmap( ps, NULL, MENU_SMALL_BIT, 0, 0 )
            MenuDC = DevOpenDC( AnchorBlock, OD_MEMORY, '*'c,
     +                          3, Dop, NULL )
            sizel.cx = 1
            sizel.cy = 1
            MenuPS = GpiCreatePS( AnchorBlock, MenuDC, sizel,
     +                            PU_PELS .or. GPIA_ASSOC .or. GPIT_MICRO )
            call GpiSetBitmap( MenuPS, MenuBitMap )
            call GpiQueryBitmapParameters( MenuBitMap, MenuBitInfo )
            call WinReleasePS( ps )

            MenuMemDC = DevOpenDC( AnchorBlock, OD_MEMORY, '*'c, 8, Dop, NULL )
            sizel.cx = 1
            sizel.cy = 1


            MenuMemPS = GpiCreatePS( AnchorBlock, MenuMemDC,
     +                               sizel, GPIA_ASSOC .or. PU_PELS )
        end if
        call WinSendMsg( MenuHandle, MM_QUERYITEM,
     +              MPFROM2SHORT( SUBMENU_PATTERN, .TRUE. ), loc( mi ) )
        PatternMenu = mi.hwndSubMenu

        mi.iPosition = MIT_END
        mi.afAttribute = 0
        mi.hwndSubMenu = NULL

        if( .not. MenuAlreadyCreated ) then
            mi.id = MENU_ROTATE_P90
            text = loc( '~Rotate +90'c )
            execute AppendText

            mi.id = MENU_ROTATE_M90
            text = loc( '~Rotate -90'c )
            execute AppendText

            mi.id = MENU_ROTATE_180
            text = loc( '~Rotate 180'c )
            execute AppendText

            mi.id = MENU_REFLECT_X
            text = loc( '~Flip Top/Bottom'c )
            execute AppendText

            mi.id = MENU_REFLECT_Y
            text = loc( '~Flip Left/Right'c )
            execute AppendText

        end if
        menu_height = 0
        menu_type = MIS_BREAK
        if( MenuAlreadyCreated ) then
            do i = NumberPatterns, 1, -1
                call WinSendMsg( PatternMenu, MM_DELETEITEM,
     +                           MPFROM2SHORT( MENU_PATTERN+i, FALSE ), 0 )
            end do
            do i = 1, NumberPatterns
                call GpiDeleteBitmap( MenuPatterns(i) )
            end do
        end if
        do i = 1, NumberPatterns
            dim_x = PatternDimX( i )
            dim_y = PatternDimY( i )
            allocate( pattern( 1:dim_x, 1:dim_y ), location = Patterns(i) )
            size_x = MenuBitInfo.cx * dim_x
            size_y = MenuBitInfo.cy * dim_y
            if( size_x .gt. MAX_MENU_SIZE_X ) size_x = MAX_MENU_SIZE_X
            if( size_y .gt. MAX_MENU_SIZE_Y ) size_y = MAX_MENU_SIZE_Y
            grid_x = size_x / dim_x
            grid_y = size_y / dim_y
            if( grid_x .eq. 0 ) grid_x = 1
            if( grid_y .eq. 0 ) grid_y = 1
            if( size_x .lt. MIN_MENU_SIZE_X ) size_x = MIN_MENU_SIZE_X
            if( size_y .lt. MIN_MENU_SIZE_Y ) size_y = MIN_MENU_SIZE_Y

            bit_info.cbFix = isizeof(bit_info)
            bit_info.cx = size_x
            bit_info.cy = size_y
            bit_info.cPlanes = 1
            bit_info.cBitCount = 24

            MenuPatterns(i) = GpiCreateBitmap( MenuMemPS, bit_info,
     +                                         0, NULL_POINTER, NULL_POINTER )

            call GpiSetBitmap( MenuMemPS, MenuPatterns(i) )

            aptl(1).x = 0
            aptl(1).y = 0
            aptl(2).x = size_x
            aptl(2).y = size_y

            call GpiBitBlt( MenuMemPS, MenuPS, 2, aptl, ROP_ONE, 0 )

            do x = 1, dim_x
                do y = 1, dim_y
                    if( pattern(x,y) ) then
                        aptl(1).x = x*grid_x-grid_x
                        aptl(1).y = y*grid_y-grid_y
                        aptl(2).x = x*grid_x
                        aptl(2).y = y*grid_y
                        aptl(3).x = 0
                        aptl(3).y = 0
                        aptl(4).x = MenuBitInfo.cx
                        aptl(4).y = MenuBitInfo.cy
                        call GpiBitBlt( MenuMemPS, MenuPS, 4,
     +                                  aptl, ROP_SRCCOPY, 0 )
                    end if
                end do
            end do

            deallocate( pattern )

            menu_height = menu_height + size_y
            mi.id = MENU_PATTERN+i
            mi.hItem = MenuPatterns(i)
            mi.afStyle = MIS_BITMAP+menu_type
            call WinSendMsg( PatternMenu, MM_INSERTITEM, loc( mi ), loc( ''c ) )
            if( menu_height .gt. ScreenHeight / 2 ) then
                menu_type = MIS_BREAK
                menu_height = 0
            else
                menu_type = 0
            end if
        end do
        MenuAlreadyCreated = .TRUE.

        remote block AppendText
        mi.afStyle = MIS_TEXT
        mi.hItem = NULL
        call WinSendMsg( PatternMenu, MM_INSERTITEM, loc( mi ), text )
        end block

        end
