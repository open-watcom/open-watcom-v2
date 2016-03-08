.func sem_destroy
.synop begin
#include <semaphore.h>
int sem_destroy(sem_t *semaphore);    
.synop end
.desc begin
The
.id &funcb.
function destroys a semaphore pointed to by the
.arg semaphore
argument.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
function returns -1 and
.kw errno
is set appropriately.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EBUSY
The semaphore is currently unavailable.
.endterm
.error end
.see begin
.seelist sem_init sem_getvalue sem_post sem_trywait sem_wait
.see end
.class POSIX
.system
