.func remove _wremove
.synop begin
#include <stdio.h>
int remove( const char *filename );
.ixfunc2 '&FileOp' &funcb
.if &'length(&wfunc.) ne 0 .do begin
int _wremove( const wchar_t *filename );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function deletes the file whose name is the string pointed
to by
.arg filename
.ct .li .
.im widefunc
.desc end
.return begin
The
.id &funcb.
function returns zero if the operation succeeds, non-zero if
it fails.
.im errnoref
.return end
.exmp begin
#include <stdio.h>

void main()
  {
    remove( "vm.tmp" );
  }
.exmp end
.class ISO C
.system
