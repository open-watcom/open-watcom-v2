.func PASS_WORD_AS_POINTER
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function PASS_WORD_AS_POINTER( dw )
       integer*4 dw
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void *PASS_WORD_AS_POINTER( DWORD dw );
.do end
.synop end
.desc begin
Some Windows API functions have pointer parameters that do not always
take pointers.
Sometimes these parameters are pure data.
In order to stop the supervisor from trying to convert the data into a
16-bit far pointer, the &func function is used.
.desc end
.return begin
The &func returns a 32-bit "near" pointer, that is really the parameter
.arg dw
.ct .li .
.return end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

      call Func( PASS_WORD_AS_POINTER(1) )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  Func( PASS_WORD_AS_POINTER( 1 ) );
.exmp end
.do end
.class WIN386
