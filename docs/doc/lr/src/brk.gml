.func brk
.synop begin
#include <stdlib.h>
int brk( void __near *addr );
.ixfunc2 '&Memory' &funcb
.synop end
.desc begin
Change data segment size, the "break" value.
The "break" value is the address of the first byte of unallocated
memory.
When a program starts execution, the break value is placed following
the code and constant data for the program.
As memory is allocated, this pointer will advance when there is no
freed block large enough to satisfy an allocation request.
The
.id &funcb.
function can be used to set a new "break" value for the program.
.np
This value may be changed by a program at any time.
.desc end
.return begin
If the call to
.id &funcb.
succeeds, "break" value is set to new value and 0 is returned.
If the call to
.id &funcb.
fails, &minus.1 is returned.
.im errnoref
.return end
.see begin
.im seealloc
.see end
.exmp begin
.exmp end
.class WATCOM
.system
