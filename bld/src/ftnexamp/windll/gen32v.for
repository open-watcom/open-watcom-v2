*$include winapi.fi

* GEN32V.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 gen32v -explicit -d2 -l=win386
* Bind:             wbind gen32v -n -D "32-bit DLL Test"


      integer*2 function FWinMain( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hlib
      integer*4 dll_1, dll_2, cb
      character*128 str

      hlib = LoadLibrary( 'windllv.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLLV'c,
     &                     'Gen32V'c, MB_OK )
          stop
      endif

      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

      cb = _Call16( dll_1, 'ddd'c, 111, 22222, 3333 )
      write( str, '(15hDLL 1 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen32V Test 1'c, MB_OK )

      cb = _Call16( dll_2, 'dd'c, 4444, 55 )
      write( str, '(15hDLL 2 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen32V Test 2'c, MB_OK )

      FWinMain = 0

      end
