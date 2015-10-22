.func mmap
.synop begin
#include <sys/mman.h>
void *mmap(void *address, size_t len, int prot, int flags, int fd, off_t offset)
.synop end
.desc begin
The
.id &funcb.
function creates a mapping of a file specified by 
.arg fd
and the process address space, allowing the file to be accessed via the 
returned pointer.  
.np
The argument
.arg address
provides the kernel a "suggestion" as to where to locate the pointer
returned by this function.  The argmuent
.arg address
is normally set to NULL, however.  
.np
The argument
.arg len
specifies the number of bytes of the file to map. 
.np
The
.arg prot
argument specifies allowed access, being one of:
.begterm 4
.term PROT_READ
Data can be read
.term PROT_WRITE
Data can be written
.term PROT_EXEC
Data can be executed
.term PROT_NONE
No access allowed
.endterm
.np
The
.arg flags
argument specifies flags related to memory mapping, and may be one or a
combination of the following:
.begterm 7
.term MAP_ANONYMOUS
Don't use an actual file, ignoring
.arg fd
entirely
.term MAP_FIXED
Use
.arg address
exactly 
.term MAP_LOCKED
Lock the memory contents
.term MAP_NORESERVE
Memory for the file is not reserved in swap
.term MAP_POPULATE
Populate (prefault) page tables
.term MAP_PRIVATE
Changes to the file are private
.term MAP_SHARED
Share changes to the file
.endterm
An additional flag, MAP_ANON, is an alias for MAP_ANONYMOUS.
.np
The argument
.arg fd
is a file descriptor for mapping.  It is ignored if MAP_ANONYMOUS is specified.
.np
The
.arg offset
argument specifies the offset within the file or device to be mapped, in bytes,
at which mapping will start.
.desc end
.return begin
If successful, the function will return a pointer to mapped file.  Upon failure,
the function will return MAP_FAILED, and errno will be set appropriately.
.return end
.error begin
.begterm 6
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg address
could not be read or the
.arg len
is zero, or value of any argument was invalid
.term ENOMEM
Memory could not be allocated
.term EOVERFLOW
An overflow occurred due to the 
.arg len
value
.term EPERM
The memory could not be locked when requested
.term EACCES
Access was denied for the specified
.arg fd
file descriptor
.term .ENODEV
Memory mapping is unavailable for the requested device
.endterm
.error end
.see begin
.seelist munmap
.see end
.class POSIX
.system
