.func _os_handle
#include <&iohdr>
int _os_handle( int handle );
.funcend
.desc begin
The &func function takes a POSIX-style file handle specified by
.arg handle
.ct .li .
It returns the corresponding operating system level handle.
.desc end
.return begin
The &func function returns the operating system handle that
corresponds to the specified POSIX-style file handle.
.return end
.see begin
.seelist &function. close fdopen _get_osfhandle _hdopen
.seelist &function. open _open_osfhandle _os_handle
.see end
.exmp begin
#include <stdio.h>
#include <&iohdr>

void main()
  {
    int handle;
    FILE *fp;

    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      handle = _os_handle( fileno( fp ) );
      fclose( fp );
    }
  }
.exmp end
.class WATCOM
.system
