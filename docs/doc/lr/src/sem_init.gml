.func sem_init
.synop begin
#include <semaphore.h>
int sem_init(sem_t *semaphore, int flags, int value);    
.synop end
.desc begin
The
.id &funcb.
function initializes a semaphore pointed to by
.arg semaphore
using
.arg value
as its initial value.  On Open Watcom, the
.arg flags
argument must be zero as this runtime does not currently support
sharing semaphores across processes.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
function returns -1 and
.kw errno
is set appropriately.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of the 
.arg value
argument exceeds
.kw SEM_VALUE_MAX
.term ENOSYS
The value of
.arg flags
was non-zero or semaphores are not supported on this CPU.
.endterm
.error end
.see begin
.seelist sem_destroy sem_getvalue sem_post sem_trywait sem_wait
.see end
.class POSIX
.system
