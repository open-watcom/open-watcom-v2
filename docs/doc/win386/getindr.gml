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
.func GetIndirectFunctionHandle
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
      integer*4 function GetIndirectFunctionHandle( prc, ... )
      integer*4 prc
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
HINDIR GetIndirectFunctionHandle( FARPROC prc, ... );
.do end
.synop end
.desc begin
The &funcb function gets a handle for a 16-bit procedure that is to be
invoked indirectly.
The procedure is assumed to have PASCAL calling convention, unless
the
.kw INDIR_CDECL
parameter is used, to indicate that Microsoft C calling convention
is to be used.
The 16-bit far pointer
.arg prc
is supplied to &funcb,
and a list of the type of each parameter (in the order that they will
be passed to the 16-bit function).
The parameter types are:
.begterm
.term INDIR_DWORD
A &dword will be passed.
.term INDIR_WORD
A &word will be passed.
.term INDIR_CHAR
A &char will be passed.
.term INDIR_PTR
A pointer will be passed.
This is only used if pointer conversion from 32-bit to 16-bit is
required, otherwise; INDIR_DWORD is specified.
.term INDIR_CDECL
This option may be included anywhere in the list before the
.kw INDIR_ENDLIST
keyword.
When this is used, the calling convention used to invoke the 16-bit
function will be the Microsoft C calling convention.
.term INDIR_ENDLIST
Marks the end of the parameter list.
.endterm
.np
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
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
.np
This handle is a data structure that was created using the
.kw malloc
function.  To free the handle, just use one of the
.kw FreeIndirectFunctionHandle
or
.kw free
functions.
.np
You may find it easier to use
.kw _Call16
rather than &funcb followed by a call to
.kw InvokeIndirectFunction
.ct .li .
.desc end
.return begin
The &funcb function returns a handle to the indirect function, or NULL
if a handle could not be allocated.
This handle is used in conjunction with
.kw InvokeIndirectFunction
to call the 16-bit procedure.
.return end
.see begin
.seelist _Call16 FreeIndirectFunctionHandle InvokeIndirectFunction
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
