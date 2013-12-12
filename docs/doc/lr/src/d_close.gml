.func _dos_close
.synop begin
#include <&doshdr>
unsigned _dos_close( int &fd );
.ixfunc2 '&DosFunc' &func
.ixfunc2 '&OsIo' &func
.synop end
.desc begin
The
.id &func.
function uses system call 0x3E to close the file indicated by
.arg &fd
.ct .li .
The value for
.arg &fd
is the one returned by a function call that created or last opened the file.
.desc end
.return begin
The
.id &func.
function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_close creat _dos_creat _dos_creatnew _dos_open dup fclose open
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
#include <fcntl.h>
.exmp break
void main()
  {
    int &fd;
.exmp break
    /* Try to open "stdio.h" and then close it */
    if( _dos_open( "stdio.h", O_RDONLY, &amp.&fd ) != 0 ){
      printf( "Unable to open file\n" );
    } else {
      printf( "Open succeeded\n" );
      if( _dos_close( &fd ) != 0 ) {
        printf( "Close failed\n" );
      } else {
        printf( "Close succeeded\n" );
      }
    }
  }
.exmp end
.class DOS
.system
