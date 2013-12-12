.func _dos_getfileattr
#include <&doshdr>
unsigned _dos_getfileattr( const char *path,
                           unsigned *attributes );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function uses system call 0x43 to get the current attributes
of the file or directory that
.arg path
points to.
The possible attributes are:
.im dosattr
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_getfileattr _dos_setfileattr
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
print_attribute()
  {
    unsigned attribute;
.exmp break
    _dos_getfileattr( "file", &attribute );
    printf( "File attribute is %d\n", attribute );
    if( attribute & _A_RDONLY ) {
        printf( "This is a read-only file.\n" );
    } else {
        printf( "This is not a read-only file.\n" );
    }
  }
.exmp break
void main()
  {
    int      &fd;
.exmp break
    if( _dos_creat( "file", _A_RDONLY, &amp.&fd ) != 0 ) {
      printf( "Error creating file\n" );
    }
    print_attribute();
    _dos_setfileattr( "file", _A_NORMAL );
    print_attribute();
    _dos_close( &fd );
  }
.exmp end
.class DOS
.system
