.func FreeHugeAlias16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       subroutine FreeHugeAlias16( fp16, size )
       integer*4 fp16, size
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
void FreeHugeAlias16( DWORD fp16, DWORD size );
.do end
.synop end
.desc begin
&func frees a 16-bit far pointer alias that was allocated with
.kw AllocHugeAlias16
.ct .li .
The size of the original 32-bit memory object must be specified.
It is important to use
.kw FreeHugeAlias16
when there is no further use for the pointer,
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
.seelist &function. AllocHugeAlias16 AllocAlias16 FreeAlias16
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

  /* windows calls using the alias ... */

  FreeHugeAlias16( alias, SIZE );
.exmp end
.do end
.class WIN386
