*$include winapi.fi

* GEN16V.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl gen16v -explicit -d2 -windows -l=windows
*                              -"op desc '16-bit DLL Test'"

*$pragma aux (dll32_call) indirect_1 \
*            parm( value*4, value*4, value*4 )
*$pragma aux (dll32_call) indirect_2 \
*            parm( value*4, value*4 )

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
      integer*4 indirect_1, indirect_2
      integer*4 dll_1, dll_2, cb
      character*128 str

      hlib = LoadLibrary( 'windllv.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLLV'c,
     &                     'Gen16V'c, MB_OK )
          stop
      endif

      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

      cb = indirect_1( 111, 22222, 3333, dll_1 )
      write( str, '(15hDLL 1 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen16V Test 1'c, MB_OK )

      cb = indirect_2( 4444, 55, dll_2 )
      write( str, '(15hDLL 2 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen16V Test 2'c, MB_OK )

      FWinMain = 0

      end
