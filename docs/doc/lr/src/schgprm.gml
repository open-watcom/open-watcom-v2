.func sched_getparam
.synop begin
#include <sched.h>
int sched_getparam(pid_t pid, struct sched_param *sp);

struct sched_param {
    int sched_priority;
};
    
.synop end
.desc begin
The
.id &funcb.
function retrieves scheduling parameters for the process specified by
.arg pid
and returns the parameters in the memory pointed to by the
.arg sp
argument.
.np
If
.arg pid
is zero, the scheduling parameters for the calling process will be
returned in the
.arg sp
argument.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
return value is -1 and 
.kw errno
is appropriately set.
.return end
.error begin
.begterm 3
.termhd1 Constant
.termhd2 Meaning
.term ESRCH
The process id
.arg pid
is invalid or could not be found
.term EPERM
The calling process does not have permission to access the parameters
.term EFAULT
The memory at
.arg sp
could not be written successfully
.endterm
.error end
.see begin
.seelist sched_setparam
.see end
.class POSIX
.system
