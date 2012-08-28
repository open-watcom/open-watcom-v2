
        subroutine Error( str )
        character*(*)   str
c
c       Pop up an error message box
c
        include 'gui.fi'
        call WinMessageBox( HWND_DESKTOP, WinHandle, str,
     +                      'Life Error'c, 0, MB_NOICON .or. MB_OK )
        end



        subroutine NoMemory()
c
c       out of memory
c
        call Error( 'Out of memory'c )

        end
