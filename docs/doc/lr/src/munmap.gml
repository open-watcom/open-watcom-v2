.func munmap
.synop begin
#include <sys/mman.h>
int munmap(void *address, size_t len)
.synop end
.desc begin
The
.id &funcb.
function removes any mapping at the address
.arg address
measuring 
.arg len
bytes from the process address space.
.desc end
.return begin
If successful, the function will return 0.  Upon failure,
the function will return -1, and errno will be set appropriately.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
Either one or both of the arguments was invalid.
.endterm
.error end
.see begin
.seelist mmap
.see end
.class POSIX
.system
