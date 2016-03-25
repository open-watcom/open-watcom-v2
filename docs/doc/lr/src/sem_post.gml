.func sem_post
.synop begin
#include <semaphore.h>
int sem_post(sem_t *semaphore);    
.synop end
.desc begin
The
.id &funcb.
function unlocks a semaphore pointed to by the 
.arg semaphore
argument.  Unlocking releases the semaphore and signals
any waiting threads appropriately.
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
.term EINVAL
The pointer
.arg semaphore
is
.kw NULL
.endterm
.error end
.see begin
.seelist sem_destroy sem_init sem_post sem_trywait sem_wait
.see end
.class POSIX
.system
