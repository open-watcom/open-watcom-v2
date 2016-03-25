.func munlockall
.synop begin
#include <sys/mman.h>
int munlockall( )
.synop end
.desc begin
The
.id &funcb.
function unlocks all memory for a process.  If, in the preceding
call to 
.kw mlockall
function, the flag
.kw MCL_FUTURE
was specified, all future pages mapped for the process will also
not be locked unless another call to 
.kw mlockall
specifies such behavior.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.see begin
.seelist mlock mlockall munlock
.see end
.class POSIX
.system
