*$include winapi.fi

* GEN32.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 gen32 -explicit -d2 -l=win386
* Bind:             wbind gen32 -n -D "32-bit DLL Test"

      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hlib
      integer*4 dll_1, dll_2, cb
      character*128 str

      structure /args_1/
          integer     w1
          integer     w2
          integer     w3
          integer     sum
      end structure

      structure /args_2/
          real        w1
          real        w2
          real        sum
      end structure

      record /args_1/ args_1/111, 22222, 3333, 0/
      record /args_2/ args_2/714.3, 35.7, 0.0/

      hlib = LoadLibrary( 'windll.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLL'c,
     &                     'Gen32'c, MB_OK )
          stop
      endif

      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

      cb = _Call16( dll_1, 'p'c, loc(args_1) )
      write( str, '(15hDLL 1 returned , i10, a)' ) args_1.sum,
     &                                             char(0)
      call MessageBox( NULL, str, 'Gen32 Test 1'c, MB_OK )

      cb = _Call16( dll_2, 'p'c, loc(args_2) )
      write( str, '(15hDLL 2 returned , f10.2, a)' ) args_2.sum,
     &                                               char(0)
      call MessageBox( NULL, str, 'Gen32 Test 2'c, MB_OK )

      FWinMain = 0

      end
