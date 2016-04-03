.func sched_rr_get_interval
.synop begin
#include <sched.h>
#include <time.h>
int sched_rr_get_interval(pid_t pid, struct timespec *ts);

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

.synop end
.desc begin
The
.id &funcb.
function retrieves the execution time limit for the process
specified by the
.arg pid
argument.  The memory pointed to by
.arg ts
will be populated with this time limit if the call is successful.
.np
If
.arg pid
is zero, the execution time limit for the calling process will be
returned in the
.arg ts
argument.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
return value is -1 and 
.kw errno
is appropriately set.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term ESRCH
The process ID
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
.class POSIX
.system
