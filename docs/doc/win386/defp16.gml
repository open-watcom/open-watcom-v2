.func DefineUserProc16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
*$include 'winapi.fi'
       integer*4 function DefineUserProc16(typ, routine, ...)
       integer*4 typ
       external routine
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
int DefineUserProc16( int typ, PROCPTR routine, ... );
.do end
.synop end
.desc begin
The &funcb function defines the arguments accepted by the user defined
callback procedure
.arg routine
.ct .li .
There may be up to 32 user defined callbacks.
The parameter
.arg typ
indicates which one of
.kw GETPROC_USERDEFINED_1
through
.kw GETPROC_USERDEFINED_32
is being defined
(see
.kw GetProc16
.ct ).
The callback routine must be declared as FAR PASCAL, or as FAR cdecl.
The variable argument list defines the types of parameters that will be
received by the user defined callback procedure
.arg routine
.ct .li .
Valid parameter types are:
.begterm
.term UDP16_PTR
16-bit far pointer
.term UDP16_DWORD
32-bits
.term UDP16_WORD
16-bits
.term UDP16_CHAR
8-bits
.term UDP16_CDECL
callback routine will be declared as type
.kw cdecl
rather than as type
.kw PASCAL
.ct .li .
This keyword may be placed anywhere before the
.kw UDP16_ENDLIST
keyword.
.term UDP16_ENDLIST
Marks the end of the variable argument list.
.endterm
.np
Once the &funcb function has been used to declare the user callback
routine, then
.kw GetProc16
may be used to get a 16-bit function pointer that may be used by Windows.
.if '&lang' eq 'FORTRAN 77' .do begin
.np
In the above synopsis, "..." in the argument list is not valid FORTRAN 77
syntax; it is used only to indicate a variable number of arguments.
.do end
.desc end
.return begin
The &funcb function returns zero if it succeeds; and non-zero if it fails.
.return end
.see begin
.seelist GetProc16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

c$pragma aux TestProc parm( value )

      subroutine TestProc( i )
      integer i
      character*128 str
      write( str, '(2hi=, i10, a)' ) i, char(0)
      call MessageBox( NULL, str, 'TEST'c, MB_OK )
      end

      integer function DefineTest()
      integer*4 cb
      external TestProc
      call DefineUserProc16( GETPROC_USERDEFINED_1,
     &                       TestProc,
     &                       UDP16_DWORD,
     &                       UDP16_ENDLIST )
      cb = GetProc16( TestProc, GETPROC_USERDEFINED_1 )
      ! cb may then be used whenever a pointer to the
      ! callback is required by 16-bit Windows
      end
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

WORD FAR PASCAL Test( DWORD a, WORD b )
{
  char foo[128];

  sprintf( foo, "a=%lx, b=%hx", a, b );
  MessageBox( NULL, foo, "TEST", MB_OK );
  return( 0x123 );
}
.exmp break
int DefineTest( void )
{
  FARPROC cb;

  DefineUserProc16( GETPROC_USERDEFINED_1,
                    (PROCPTR) Test,
                    UDP16_DWORD,
                    UDP16_WORD,
                    UDP16_ENDLIST );

  cb = GetProc16( (PROCPTR) Test, GETPROC_USERDEFINED_1 );

  /*
   * cb may then be used whenever a pointer to the
   * callback is required by 16-bit Windows
   */
}
.exmp end
.do end
.class WIN386
