.func sem_getvalue
.synop begin
#include <semaphore.h>
int sem_getvalue(sem_t *semaphore, int *dest);    
.synop end
.desc begin
The
.id &funcb.
function returns the current value of
.arg semaphore
in the memory pointed to by the
.arg dest
pointer.
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
.arg value
is
.kw NULL
.endterm
.error end
.see begin
.seelist sem_destroy sem_init sem_post sem_trywait sem_wait
.see end
.class POSIX
.system
