.func _bios_memsize for NEC PC98
.synop begin
#include <bios.h>
unsigned short _bios_memsize( void );
.ixfunc2 '&BiosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function determines the total amount of real-mode memory
available.
.desc end
.return begin
The
.id &funcb.
function returns the total amount of 1K blocks of memory
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
