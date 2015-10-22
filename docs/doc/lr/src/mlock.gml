.func mlock
.synop begin
#include <sys/mman.h>
int mlock(void *address, size_t len)
.synop end
.desc begin
The
.id &funcb.
function causes memory located at
.arg address
measuring 
.arg len
bytes to be held in physical memory until unlocked or
the process terminates.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.error begin
.begterm 4
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
address space or the requested lock region exceeds an allowable
limit
.term EAGAIN
Some portion of the memory could not be locked
.term EPERM
The calling process lacks the approriate permissions
.endterm
.error end
.see begin
.seelist mlockall munlock munlockall
.see end
.class POSIX
.system
