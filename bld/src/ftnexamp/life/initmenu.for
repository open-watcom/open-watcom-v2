
c$define INCL_WINMENUS
c$include pmwin.fap

        subroutine InitMenu
c
c   Initialize the menu display. Disable any items which are not applicable.
c
        include         'os2.fi'
        include         'life.fi'
        integer         i

        call WinCheckMenuItem( MenuHandle, MENU_WRAP_AROUND,
     +                         CurvedSpace )
        call WinCheckMenuItem( MenuHandle, MENU_BOUNDED_EDGES,
     +                         .not. CurvedSpace )
        call WinCheckMenuItem( MenuHandle, MENU_PAUSE,
     +                         Mode .eq. MENU_PAUSE )
        call WinCheckMenuItem( MenuHandle, MENU_RESUME,
     +                         Mode .eq. MENU_RESUME )
        call WinCheckMenuItem( MenuHandle, MENU_SINGLE_STEP,
     +                         MouseMode .eq. MENU_SINGLE_STEP)
        call WinCheckMenuItem( MenuHandle, MENU_SELECT,
     +                         MouseMode .eq. MENU_SELECT)
        call WinCheckMenuItem( MenuHandle, MENU_FLIP_PATTERNS,
     +                         MouseMode .eq. MENU_FLIP_PATTERNS )
        call WinEnableMenuItem( MenuHandle, MENU_SAVE,
     +                          RegionIsSelected )
        call WinCheckMenuItem( MenuHandle, MENU_GRID, DrawGrid )
        do i = 1, NumberPatterns
            call WinCheckMenuItem( MenuHandle, MENU_PATTERN+i, Cursor .eq. i )
        enddo

        end
