#include <stdio.h>
#include <dos.h>

print_attribute()
  {
    unsigned attribute;

    _dos_getfileattr( "file", &attribute );
    printf( "File attribute is %x\n", attribute );
    if( attribute & _A_RDONLY ) {
        printf( "This is a read-only file\n" );
    } else {
        printf( "This is not a read-only file\n" );
    }
  }

void main()
  {
    int      handle;

    if( _dos_creat( "file", _A_RDONLY, &handle ) != 0 ){
      printf( "Error creating file\n" );
    }
    print_attribute();
    _dos_setfileattr( "file", _A_NORMAL );
    print_attribute();
    _dos_close( handle );
  }
