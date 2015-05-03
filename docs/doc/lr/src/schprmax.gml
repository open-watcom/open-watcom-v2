.func sched_get_priority_max
.synop begin
#include <sched.h>
int sched_get_priority_max(pid_t pid, int policy);
.synop end
.desc begin
The
.id &funcb.
function returns the maximum priority for the scheduling policy
specified by the
.arg policy
argument.
.desc end
.return begin
If successful, the function will return the maximum priority allowed
for the given scheduling policy. If the call fails, the
return value is -1 and 
.kw errno
is appropriately set.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg policy
does not represent a valid scheduling policy
.endterm
.error end
.see begin
.seelist sched_get_priority_min
.see end
.class POSIX
.system
