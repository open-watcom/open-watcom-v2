.func msync
.synop begin
#include <sys/mman.h>
int msync(void *address, size_t len, int flags)
.synop end
.desc begin
The
.id &funcb.
function writes all modified data to permanent storage
corresponding to the memory mapping at
.arg address
measuring 
.arg len
bytes.  
.np
The
.arg flags
argument may be one or a combination of the following:
.begterm 3
.termhd1 Constant
.termhd2 Meaning
.term MS_ASYNC
Perform asynchronous writes
.term MS_SYNC
Perform synchronous writes
.term MS_INVALIDATE
Invalidate any cached data
.endterm
The flags
.kw MS_ASYNC
and
.kw MS_SYNC
may not be combined.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.error begin
.begterm 3
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The argument
.arg address
is not a multiple of the page size or the
.arg flags
argument is invalid
.term ENOMEM
The arguments do not correspond to a region of the process's
address space or exceed the process's address space
.term EBUSY
A portion of or all of the address space specified is
currently locked
.endterm
.error end
.see begin
.seelist mlock mlockall mmap munlock munlockall munmap
.see end
.class POSIX
.system
