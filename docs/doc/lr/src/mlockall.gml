.func mlockall
.synop begin
#include <sys/mman.h>
int mlockall(int flags)
.synop end
.desc begin
The
.id &funcb.
function causes all memory in the current process's address
space to reside in physical memory until unlocked.  The
.arg flags
argument may be one or a combination of the following:
.begterm 2
.term MCL_CURRENT
Lock all pages currently mapped for the process
.term MCL_FUTURE
Lock all pages that may be mapped in the future for this process
.endterm
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
.arg flags
is zero or invalid
.term ENOMEM
The amount of memory requested for locking exceeds an allowable
limit
.term EAGAIN
Some portion of the memory could not be locked
.term EPERM
The calling process lacks the approriate permissions
.endterm
.error end
.see begin
.seelist mlock munlock munlockall
.see end
.class POSIX
.system
