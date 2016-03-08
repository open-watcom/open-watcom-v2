.func sem_wait
.synop begin
#include <semaphore.h>
int sem_wait(sem_t *semaphore);    
.synop end
.desc begin
The
.id &funcb.
function attempts to lock a semaphore pointed to by the 
.arg semaphore
argument, and blocks until the semaphore is successfully
locked.
.desc end
.return begin
If the semaphore was successfully locked, the function 
will return zero. If the call fails or the semaphore could
not be locked, the function returns -1 and
.kw errno
is set appropriately.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The pointer
.arg semaphore
is
.kw NULL
.endterm
.error end
.see begin
.seelist sem_destroy sem_getvalue sem_init sem_post sem_trywait
.see end
.class POSIX
.system
