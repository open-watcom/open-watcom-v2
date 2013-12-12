.func _bios_memsize
.synop begin
#include <bios.h>
unsigned short _bios_memsize( void );
.ixfunc2 '&BiosFunc' &func
.synop end
.desc begin
The &func function uses INT 0x12 to determine the total amount of
memory available.
.desc end
.return begin
The &func function returns the total amount of 1K blocks of memory
installed (maximum 640).
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    unsigned short memsize;
.exmp break
    memsize = _bios_memsize();
    printf( "The total amount of memory is: %dK\n",
               memsize );
  }
.exmp output
The total amount of memory is: 640K
.exmp end
.class BIOS
.system
