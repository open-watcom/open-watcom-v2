.func munlock
.synop begin
#include <sys/mman.h>
int munlock(void *address, size_t len)
.synop end
.desc begin
The
.id &funcb.
function causes memory located at
.arg address
measuring 
.arg len
bytes to be released regardless of the number of prior calls
to lock the referenced memory.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The argument
.arg address
is not a multiple of the page size or the
.arg len
is zero or causes an overflow
.term ENOMEM
The arguments do not correspond to a region of the process's
address space
.endterm
.error end
.see begin
.seelist mlock mlockall munlockall
.see end
.class POSIX
.system
