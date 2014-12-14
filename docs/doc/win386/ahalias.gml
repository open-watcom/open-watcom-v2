.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="dword"      value="INTEGER*4".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="dword"      value="DWORD".
.do end
.func AllocHugeAlias16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function AllocHugeAlias16( ptr, size )
       integer*4 ptr, size
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
DWORD AllocHugeAlias16( void *ptr, DWORD size );
.do end
.synop end
.desc begin
The &func function obtains a 16-bit far pointer to a 32-bit memory object
that is
.arg size
bytes in size.
This is similar to the function
.kw AllocAlias16
.ct , except that
.kw AllocAlias16
will only give 16-bit far pointers to 32-bit memory objects of up to
64K in size.
To get 16-bit far pointers to 32-bit memory objects larger than 64K,
.kw AllocHugeAlias16
should be used.
.desc end
.return begin
The &func function returns a 16-bit far pointer
.if '&lang' eq 'FORTRAN 77' .do begin
(as an INTEGER*4)
.do end
usable by Windows,
or returns 0 if the alias cannot be allocated.
.return end
.see begin
.seelist AllocAlias16 FreeAlias16 FreeHugeAlias16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.exmp begin
integer ierr, SIZE
integer*4 alias
parameter (SIZE=300000)
integer*1 tmp(SIZE)


allocate( tmp(SIZE), stat=ierr )
if( ierr .ne. 0 )then
    alias = AllocHugeAlias16( loc( tmp ), SIZE )

    ! Windows calls using the alias ...

    call FreeHugeAlias16( alias, SIZE )
endif
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>
#include <malloc.h>
#define SIZE 300000

  DWORD alias;
  void *tmp;

  tmp = malloc( SIZE );
  alias = AllocHugeAlias16( tmp, SIZE );

  /* Windows calls using the alias ... */

  FreeHugeAlias16( alias, SIZE );
.exmp end
.do end
.class WIN386
