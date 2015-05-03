.func sched_get_priority_min
.synop begin
#include <sched.h>
int sched_get_priority_min(pid_t pid, int policy);
.synop end
.desc begin
The
.id &funcb.
function returns the minimum priority for the scheduling policy
specified by the
.arg policy
argument.
.desc end
.return begin
If successful, the function will return the minimum priority allowed
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
.seelist sched_get_priority_max
.see end
.class POSIX
.system
