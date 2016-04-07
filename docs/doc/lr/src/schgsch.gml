.func sched_getscheduler
.synop begin
#include <sched.h>
int sched_getscheduler(pid_t pid);
.synop end
.desc begin
The
.id &funcb.
function retrieves scheduling policy for the process specified by the
.arg pid
argument.
.np
If
.arg pid
is zero, the policy for the calling process will be returned.
.desc end
.return begin
If successful, the function will return the kernel's scheduling
policy for the specified process. If the call fails, the
return value is -1 and 
.kw errno
is appropriately set.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term ESRCH;
The process ID
.arg pid
could not be found
.term EINVAL
The process ID
.arg pid
is invalid
.endterm
.error end
.see begin
.seelist sched_setscheduler
.see end
.class POSIX
.system
