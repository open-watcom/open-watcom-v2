.func _dos_write
#include <&doshdr>
unsigned _dos_write( int &fd, void const __far *buffer,
                     unsigned count, unsigned *bytes );
.ixfunc2 '&DosFunc' &func
.ixfunc2 '&OsIo' &func
.funcend
.desc begin
The &func function uses system call 0x40 to write
.arg count
bytes of data from the buffer pointed to by
.arg buffer
to the file specified by
.arg &fd
.ct .li .
The number of bytes successfully written will be stored in the
unsigned integer pointed to by
.arg bytes
.ct .li .
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_write _dos_close _dos_open _dos_read
.see end
.exmp begin
#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
.exmp break
char buffer[] = "This is a test for _dos_write.";
.exmp break
void main()
  {
    unsigned len_written;
    int      &fd;
.exmp break
    if( _dos_creat( "file", _A_NORMAL, &amp.&fd ) != 0 ) {
      printf( "Unable to create file\n" );
    } else {
      printf( "Create succeeded\n" );
      _dos_write( &fd, buffer, sizeof(buffer),
                  &len_written );
      _dos_close( &fd );
    }
  }
.exmp end
.class DOS
.system
