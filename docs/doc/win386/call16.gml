.func _Call16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function _Call16( lpFunc, fmt, ... )
       integer*4 lpFunc
       character*(*) fmt
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
DWORD _Call16( FARPROC lpFunc, char *fmt, ... );
.do end
.synop end
.desc begin
The &func function performs the same function as
.kw GetIndirectFunctionHandle
.ct,
.kw InvokeIndirectFunctionHandle
and
.kw FreeIndirectFunctionHandle
but is much easier to use.
The first argument
.arg lpFunc
is the address of the 16-bit function to be called.
This address is usually obtained by calling
.kw GetProcAddress
with the name of the desired function.
The second argument
.id fmt
is a string identifying the types of the parameters to be passed to
the 16-bit function.
.begnote $compact
:DTHD.Character
:DDHD.Parameter Type
.note c
call a 'cdecl' function as opposed to a 'pascal' function
(if specified, it must be listed first)
.note b
unsigned BYTE
.note w
16-bit WORD
.if '&lang' eq 'FORTRAN 77' .do begin
(INTEGER*2)
.do end
.note d
32-bit DWORD
.if '&lang' eq 'FORTRAN 77' .do begin
(INTEGER*4, REAL*4)
.do end
.note f
double precision floating-point
.if '&lang' eq 'FORTRAN 77' .do begin
(DOUBLE PRECISION, REAL*8)
.do end
.note p
32-bit flat pointer (converted to 16:16 far pointer)
.if '&lang' eq 'FORTRAN 77' .do begin
(LOC(x))
.do end
.endnote
.np
The 16-bit function must use either the
.kw PASCAL
or
.kw CDECL
calling convention.
.kw PASCAL
calling convention is the default.
If the function uses the
.kw CDECL
calling convention, then you must specify the letter "c" as the
first character of the argument type string.
.np
Pointer types will automatically be converted from 32-bit near
pointers to 16-bit far pointers before the function is invoked.
Note that this pointer is only valid over the period of the call;
after control returns to the 32-bit application, the 16-bit pointer
created by the Supervisor is no longer valid.
.desc end
.return begin
The &func function returns a 32-bit DWORD
.if '&lang' eq 'FORTRAN 77' .do begin
(as an INTEGER*4)
.do end
which represents the return value from the 16-bit function that was
called.
.return end
.see begin
.seelist GetIndirectFunctionHandle InvokeIndirectFunctionHandle FreeIndirectFunctionHandle
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

      include 'windows.fi'
      integer*2 hlib
      integer*4 dll_1, cb
      character*128 str

      hlib = LoadLibrary( 'windllv.dll'c )
      dll_1 = GetProcAddress( hlib, 'DLL1'c )

      cb = _Call16( dll_1, 'ddd'c, 111, 22222, 3333 )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>
HANDLE hDrv;
FARPROC lpfn;
int cb;

    if( (hDrv = LoadLibrary ("foo.dll")) < 32 )
        return FALSE;
    if( !(lpfn = GetProcAddress (hDrv, "ExtDeviceMode")) )
        return FALSE;

    /*
     * now, invoke the function
     */
    cb = (WORD) _Call16(
           lpfn,                // address of function
           "wwdppddw",          // parameter type info
           hwnd,                // parameters ...
           hDrv,
           NULL,
           "POSTSCRIPT PRINTER",
           "LPT1",
           NULL,
           NULL,
           0
            );
.exmp end
.do end
.class WIN386
