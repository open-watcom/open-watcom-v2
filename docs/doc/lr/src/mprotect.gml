.func mprotect
.synop begin
#include <sys/mman.h>
int mprotect(void *address, size_t len, int flags)
.synop end
.desc begin
The
.id &funcb.
function writes changes the protections for the pages
encompassing the memory mapping at
.arg address
and measuring 
.arg len
bytes based on the 
.arg flags
argument as specified.
.np
The
.arg flags
argument may be one or a combination of the following:
.begterm 5
.termhd1 Constant
.termhd2 Meaning
.term PROT_NONE
No protections
.term PROT_READ
Read only
.term PROT_WRITE
Write only
.term PROT_EXEC
Allow execution
.endterm
The flags may be either exclusively
.kw PROT_NONE
or a combination of one or more of the remaining three
values.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.error begin
.begterm 5
.termhd1 Constant
.termhd2 Meaning
.term EACCES
The value of
.arg flags
violates a the permissions that a process has to the
mapped data.
.term EAGAIN
Write access was requested when the memory was originally
mapped with the 
.kw MAP_PRIVATE
flag and the system lacks the resources to create a private
page.
.term EINVAL
The argument
.arg address
is not a multiple of the page size or the
.arg flags
argument is invalid
.term ENOMEM
The arguments do not correspond to a region of the process's
address space or exceed the process's address space
.term ENOTSUP
The specified combination of
.arg flags
is not supported on the underlying system
.endterm
.error end
.see begin
.seelist mmap
.see end
.class POSIX
.system
