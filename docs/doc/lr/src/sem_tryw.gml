.func sem_trywait
.synop begin
#include <semaphore.h>
int sem_trywait(sem_t *semaphore);    
.synop end
.desc begin
The
.id &funcb.
function attempts to lock a semaphore pointed to by the 
.arg semaphore
argument, and returns immediately regardless of success.
.desc end
.return begin
If the semaphore was successfully locked, the function 
will return zero. If the call fails or the semaphore could
not be locked, the function returns -1 and
.kw errno
is set appropriately.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The pointer
.arg semaphore
is
.kw NULL
.term EAGAIN
The semaphore is currently locked.
.endterm
.error end
.see begin
.seelist sem_destroy sem_getvalue sem_init sem_post sem_wait
.see end
.class POSIX
.system
