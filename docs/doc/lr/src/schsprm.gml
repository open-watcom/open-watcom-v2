.func sched_setparam
.synop begin
#include <sched.h>
int sched_setparam(pid_t pid, const struct sched_param *sp);

struct sched_param {
    int sched_priority;
};
    
.synop end
.desc begin
The
.id &funcb.
function sets the scheduling parameters for the process specified
by the
.arg pid
argument.
.np
If
.arg pid
is zero, the scheduling parameters for the calling process will be
set.
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
.term ESRCH;
The process id
.arg pid
is invalid or could not be found
.term EINVAL
The value of 
.arg pid
or
.arg sp
is invalid
.term EFAULT
The memory at 
.arg sp
could not be read
.endterm
.error end
.see begin
.seelist sched_getparam
.see end
.class POSIX
.system
