*$include winapi.fi

* WINDLL.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 windll -explicit -d2 -bd -l=win386
* Bind:             wbind windll -d -n

*$pragma aux (dll_function) Add3

      subroutine Add3( arg_list )
      integer*4 arg_list
      structure /argtypes/
          integer     w1
          integer     w2
          integer     w3
          integer     sum
      end structure
      record /argtypes/ args(:)
*$pragma array args far

      include 'windows.fi'

      character*128 str

      allocate( args(1), location=arg_list )
      write( str, '(16hDLL 1 arguments:, 3i10, a)' ) args(1).w1,
     &                                               args(1).w2,
     &                                               args(1).w3,
     &                                               char(0)
      call MessageBox( NULL, str, 'DLL Function 1'c, MB_OK )
      args(1).sum = args(1).w1 + args(1).w2 + args(1).w3
      deallocate( args )

      end

*$pragma aux (dll_function) Add2

      subroutine Add2( arg_list )
      integer*4 arg_list
      structure /argtypes/
          real        w1
          real        w2
          real        sum
      end structure
      record /argtypes/ args(:)
*$pragma array args far

      include 'windows.fi'

      character*128 str

      allocate( args(1), location=arg_list )
      write( str, '(16hDLL 2 arguments:, 2f10.2, a)' ) args(1).w1,
     &                                                 args(1).w2,
     &                                                 char(0)
      call MessageBox( NULL, str, 'DLL Function 2'c, MB_OK )
      args(1).sum = args(1).w1 + args(1).w2
      deallocate( args )

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

      rc = DefineDLLEntry( 1, Add3, DLL_PTR, DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      rc = DefineDLLEntry( 2, Add2, DLL_PTR, DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      call MessageBox( NULL, '32-bit DLL started'c,
     &                 'WINDLL'c, MB_OK )
      FWinMain = 1

      end
