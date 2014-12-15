.func DefineDLLEntry
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function DefineDLLEntry(index, routine, ...)
       integer*4 index
       external routine
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
int DefineDLLEntry( int index, void * routine, ... );
.do end
.synop end
.desc begin
The &funcb function defines an
.arg index
number for the 32-bit DLL procedure
.arg routine
.ct .li .
The parameter
.arg index
defines the index number that must be used in order to invoke the
32-bit FAR procedure
.arg routine
.ct .li .
The variable argument list defines the types of parameters that will be
received by the 32-bit DLL
.arg routine
.ct .li .
Valid parameter types are:
.begterm 16 $compact
.term DLL_PTR
16-bit far pointer
.term DLL_DWORD
32-bits
.term DLL_WORD
16-bits
.term DLL_CHAR
8-bits
.term DLL_ENDLIST
Marks the end of the variable argument list.
.endterm
.if '&lang' eq 'FORTRAN 77' .do begin
.np
In the above synopsis, "..." in the argument list is not valid FORTRAN 77
syntax; it is used only to indicate a variable number of arguments.
.do end
.np
Note that all pointers are received as 16-bit far pointers.
To access the data from the 32-bit DLL,
.if '&lang' eq 'FORTRAN 77' .do begin
a dynamically allocatable array must be mapped to the memory pointed to
by the 16-bit far pointer using the
.kw LOCATION
specifier of the
.kw ALLOCATE
statement and assigning the
.kw FAR
attribute to the array using the array pragma.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
the
.kw MK_FP32
macro must be applied.
The data can then be accessed with the resulting 32-bit far pointer.
.do end
.desc end
.return begin
The &funcb function returns zero if successful, and a non-zero value otherwise.
.return end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

c$pragma aux (dll_function) DLL_1

        integer function DLL_1( w1, w2, w3 )
        integer*4 w1, w2, w3
        include 'win386.fi'
        include 'windefn.fi'
        include 'winerror.fi'
        character*128 str
        write( str, '(16hDLL 1 arguments:, 3i10, a)' ) w1,
     &                                     w2, w3, char(0)
        call MessageBox( NULL, str,
     &                  'DLL Function 1'c, MB_OK )
        DLL_1 = w1 + w2 + w3
        end
.exmp break

        integer*2 function FWINMAIN( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )
        integer*2 hInstance, hPrevInstance, nCmdShow
        integer*4 lpszCmdLine
        include 'win386.fi'
        include 'windefn.fi'
        include 'winerror.fi'
        external DLL_1
        integer rc
        rc = DefineDLLEntry( 1, DLL_1, DLL_DWORD,
     &                       DLL_DWORD, DLL_DWORD,
     &                       DLL_ENDLIST )
        if( rc .ne. 0 )then
            FWinMain = 0
            return
        end if
        call MessageBox( NULL,
     &                   '32-bit DLL started'c,
     &                   '32-bit DLL'c, MB_OK )
        FWinMain = 1
        end
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>
int FAR PASCAL FooMe( WORD w1, DWORD w2, WORD w3 )
{
  char str[128];

  sprintf( str, "w1=%hx, w2=%lx, w3=%hx", w1, w2, w3 );
  MessageBox( NULL, str, "DLL Test", MB_OK );
  return( w1 + w2 );
}
.exmp break
int PASCAL WinMain( HANDLE hInstance, HANDLE x1,
        LPSTR lpCmdLine, int x2 )
{
  DefineDLLEntry( 1, (PROCPTR) FooMe, DLL_WORD,
                  DLL_DWORD, DLL_WORD, DLL_ENDLIST );
  MessageBox( NULL, "32-bit DLL Started", "Test", MB_OK );
  return( 1 );
}
.exmp end
.do end
.class WIN386
