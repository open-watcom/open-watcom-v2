.func InvokeIndirectFunction
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function InvokeIndirectFunction(handle, ...)
       integer*4 handle
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
long InvokeIndirectFunction( HINDIR handle, ... );
.do end
.synop end
.desc begin
The &funcb function invokes the 16-bit function pointed to by the
specified handle.
The handle must have been previously allocated using the
.kw GetIndirectFunctionHandle
function.
The handle is followed by the list of parameters to be passed to the
16-bit function.
.np
If you specified
.kw INDIR_PTR
as a parameter when allocating the handle, then a 16-bit pointer is
allocated for a 32-bit pointer that you pass.  However, this pointer
is freed when the 16-bit function being invoked returns.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
There is no substitute for this function when compiling for 16-bit Windows.
In order to make the code 16-bit Windows compatible, conditional code
(based on the __WINDOWS_386__ macro)
should be placed around the &funcb usage (see the example).
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.np
In the above synopsis, "..." in the argument list is not valid FORTRAN 77
syntax; it is used only to indicate a variable number of arguments.
.do end
.desc end
.return begin
The &funcb function returns the value which the 16-bit function
returned.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
If the 16-bit function returns a short rather than a long, the result
must be typecast.
.do end
.return end
.see begin
.seelist _Call16 FreeIndirectFunctionHandle GetIndirectFunctionHandle
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
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
  HINDIR hIndir;
  int cb;

  if( (hDrv = LoadLibrary( "foo.lib" )) < 32 )
      return FALSE;
  if( !(lpfn = GetProcAddress( hDrv, "ExtDeviceMode" )) )
      return FALSE;
#ifdef __WINDOWS_386__
.exmp break
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
#else
.exmp break
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
