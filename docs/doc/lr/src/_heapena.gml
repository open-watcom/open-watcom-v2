.func _heapenable
#include <malloc.h>
int _heapenable( int enabled );
.ixfunc2 '&Heap' _heapenable
.funcend
.desc begin
The &func function is used to control attempts by the heap allocation
manager to request more memory from the operating system's memory
pool.
If
.arg enabled
is 0 then all further allocations which would normally go to the
operating system for more memory will instead fail and return NULL.
If
.arg enabled
is 1 then requests for more memory from the operating system's memory
pool are re-enabled.
.np
This function can be used to impose a limit on the amount of system
memory that is allocated by an application.
For example, if an application wishes to allocate no more than 200K
bytes of memory, it could allocate 200K and immediately free it.
It can then call &func to disable any further requests from the system
memory pool.
After this, the application can allocate memory from the 200K pool
that it has already obtained.
.desc end
.return begin
The return value is the previous state of the system allocation flag.
.return end
.see begin
.seelist &function. _heapchk _heapenable _heapgrow _heapmin _heapset _heapshrink _heapwalk
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>

void main()
  {
    char *p;
.exmp break
    p = malloc( 200*1024 );
    if( p != NULL ) free( p );
    _heapenable( 0 );
    /*
      allocate memory from a pool that
      has been capped at 200K
    */
  }
.exmp end
.class WATCOM
.system
