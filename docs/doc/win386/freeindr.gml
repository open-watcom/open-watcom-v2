.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="dword"      value="INTEGER*4".
:set symbol="word"       value="INTEGER*2".
:set symbol="char"       value="INTEGER*1".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="dword"      value="DWORD".
:set symbol="word"       value="WORD".
:set symbol="char"       value="char".
.do end
.func FreeIndirectFunctionHandle
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
      subroutine FreeIndirectFunctionHandle( handle )
      integer*4 handle
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void FreeIndirectFunctionHandle( HINDIR handle );
.do end
.funcend
.desc begin
&func frees a handle that was obtained using
.kw GetIndirectFunctionHandle
.ct .li .
This is important to do when there is no further use for the pointer
since there are a limited number of 16-bit aliases available (due to
limited space in the local descriptor table).
.desc end
.return begin
.if '&lang' eq 'FORTRAN 77' .do begin
&func is a subroutine.
.do end
.el .do begin
The &func function returns nothing.
.do end
.return end
.see begin
.seelist &function. _Call16 GetIndirectFunctionHandle InvokeIndirectFunction
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.keep 13
.exmp begin
c$include winapi.fi

      integer*2 hDrv
      integer*4 lpfn

      hDrv = LoadLibrary( 'your.lib'c )
      if( hDrv .lt. 32 ) return
      lpfn = GetProcAddress( hDrv,
     &                       'ExtDeviceMode'c )
      if( lpfn .eq 0 ) return
.exmp break
      hIndir = GetIndirectFunctionHandle(
     &             lpfn,
     &             INDIR_WORD,
     &             INDIR_WORD,
     &             INDIR_DWORD,
     &             INDIR_PTR,
     &             INDIR_PTR,
     &             INDIR_DWORD,
     &             INDIR_DWORD,
     &             INDIR_WORD,
     &             INDIR_ENDLIST )
.exmp break
      cb = InvokeIndirectFunction(
     &             hIndir,
     &             hwnd,
     &             hDrv,
     &             NULL,
     &             'POSTSCRIPT PRINTER'c,
     &             'LPT1'c,
     &             NULL,
     &             NULL,
     &             0 )
      call FreeIndirectFunctionHandle( hIndir )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  HANDLE hDrv;
  FARPROC lpfn;

  if( (hDrv = LoadLibrary( "foo.lib" )) < 32 )
      return FALSE;
  if( !(lpfn = GetProcAddress( hDrv, "ExtDeviceMode" )) )
      return FALSE;
.exmp break
#ifdef __WINDOWS_386__
  hIndir = GetIndirectFunctionHandle(
              lpfn,
              INDIR_WORD,
              INDIR_WORD,
              INDIR_DWORD,
              INDIR_PTR,
              INDIR_PTR,
              INDIR_DWORD,
              INDIR_DWORD,
              INDIR_WORD,
              INDIR_ENDLIST );
.exmp break
  cb = (WORD) InvokeIndirectFunction(
                hIndir,
                hwnd,
                hDrv,
                NULL,
                "POSTSCRIPT PRINTER",
                "LPT1",
                NULL,
                NULL,
                0 );
  FreeIndirectFunctionHandle( hIndir );
.exmp break
#else
  cb = lpfn( hwnd,
             hDrv,
             NULL,
             "POSTSCRIPT PRINTER",
             "LPT1",
             NULL,
             NULL,
             0 );
#endif
.exmp end
.do end
.class WIN386
