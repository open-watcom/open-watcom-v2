.func sched_yield
.synop begin
#include <sched.h>
int sched_yield( );
.synop end
.desc begin
The
.id &funcb.
function causes the calling thread to yield the processor to other
threads until the kernel assigns it to be the current thread once 
again.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
return value is -1.
.return end
.class POSIX
.system
