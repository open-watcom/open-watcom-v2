.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="dword"      value="INTEGER*4".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="dword"      value="DWORD".
.do end
.func MapAliasToFlat
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function MapAliasToFlat( alias )
       integer*4 alias
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void *MapAliasToFlat( DWORD alias );
.do end
.funcend
.desc begin
The &func function returns a 32-bit near pointer equivalent of a pointer
allocated previously with
.kw AllocAlias16
or
.kw AllocHugeAlias16
.ct .li .
This is useful if you are communicating with a 16-bit application
that is returning pointers that you previously gave it.
.desc end
.return begin
The &func function returns a 32-bit near pointer
.if '&lang' eq 'FORTRAN 77' .do begin
(as an INTEGER*4)
.do end
usable by the 32-bit application.
.return end
.see begin
.seelist &function. AllocAlias16 AllocHugeAlias16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
c$include winapi.fi

       integer alias
       integer ptr

       alias = AllocAlias16( loc( alias ) )
       alias += 5
       ptr = MapAliasToFlat( alias )
       if( ptr .eq. loc( alias ) + 5 )then
           call MessageBox( NULL, 'It Worked'c, ''c, MB_OK )
       else
           call MessageBox( NULL, 'It Failed'c, ''c, MB_OK )
       end if
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  DWORD alias;
  void  *ptr;

  alias = (DWORD) AllocAlias16( &alias );
  alias += 5;
  ptr = MapAliasToFlat( alias );
  if( ptr == ((char *)&alias + 5) ) {
     MessageBox( NULL,"It Worked","",MB_OK );
  } else {
     MessageBox( NULL,"It Failed","",MB_OK );
  }
.exmp end
.do end
.class WIN386
