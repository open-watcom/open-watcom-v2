*$include winapi.fi

* WINDLLV.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 windllv -explicit -d2 -bd -l=win386
* Bind:             wbind windllv -d -n

*$pragma aux (dll_function) Add3

      integer function Add3( w1, w2, w3 )
      integer*4 w1, w2, w3

      include 'windows.fi'

      character*128 str

      write( str, '(16hDLL 1 arguments:, 3i10, a)' ) w1, w2, w3,
     &                                               char(0)
      call MessageBox( NULL, str, 'DLL Function 1'c, MB_OK )
      Add3 = w1 + w2 + w3

      end

*$pragma aux (dll_function) Add2

      integer function Add2( w1, w2 )
      integer*4 w1, w2

      include 'windows.fi'

      character*128 str

      write( str, '(16hDLL 2 arguments:, 2i10, a)' ) w1, w2, char(0)
      call MessageBox( NULL, str, 'DLL Function 2'c, MB_OK )
      Add2 = w1 + w2

      end

      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      external Add3, Add2
      integer rc

      rc = DefineDLLEntry( 1, Add3, DLL_DWORD, DLL_DWORD, DLL_DWORD,
     &                        DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      rc = DefineDLLEntry( 2, Add2, DLL_DWORD, DLL_DWORD,
     &                        DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      call MessageBox( NULL, '32-bit DLL started'c,
     &                 'WINDLLV'c, MB_OK )
      FWinMain = 1

      end
