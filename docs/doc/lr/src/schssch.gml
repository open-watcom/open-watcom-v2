.func sched_setscheduler
.synop begin
#include <sched.h>
int sched_setscheduler(pid_t pid, int policy,
    const struct sched_param *sp);

struct sched_param {
    int sched_priority;
};
    
.synop end
.desc begin
The
.id &funcb.
function sets the scheduling policy and parameters for the 
process specified by the
.arg pid
argument.
.np
If
.arg pid
is zero, the policy and parameters for the calling process 
will be set.
.desc end
.return begin
If successful, the function will return the kernel's former
scheduling policy for the specified process. If the call fails, 
the return value is -1 and 
.kw errno
is appropriately set.
.return end
.error begin
.begterm 3
.termhd1 Constant
.termhd2 Meaning
.term ESRCH;
The process id
.arg pid
could not be found
.term EINVAL
The process id
.arg pid
, policy value
.arg policy
, or the pointer
.arg sp
is invalid
.term EFAULT
The memory at 
.arg sp
could not be read
.endterm
.error end
.see begin
.seelist sched_getscheduler sched_setparam sched_getparam
.see end
.class POSIX
.system
