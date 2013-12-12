.func _dos_read
.synop begin
#include <&doshdr>
unsigned _dos_read( int &fd, void __far *buffer,
                    unsigned count, unsigned *bytes );
.ixfunc2 '&DosFunc' &func
.ixfunc2 '&OsIo' &func
.synop end
.desc begin
The
.id &func.
function uses system call 0x3F to read
.arg count
bytes of data from the file specified by
.arg &fd
into the buffer pointed to by
.arg buffer
.ct .li .
The number of bytes successfully read will be stored in the
unsigned integer pointed to by
.arg bytes
.ct .li .
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
.seelist _dos_read _dos_close _dos_open _dos_write
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
#include <fcntl.h>
.exmp break
void main()
  {
    unsigned len_read;
    int      &fd;
    auto char buffer[80];
.exmp break
    if( _dos_open( "file", O_RDONLY, &amp.&fd ) != 0 ) {
      printf( "Unable to open file\n" );
    } else {
      printf( "Open succeeded\n" );
      _dos_read( &fd, buffer, 80, &len_read );
      _dos_close( &fd );
    }
  }
.exmp end
.class DOS
.system
