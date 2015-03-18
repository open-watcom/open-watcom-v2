.func clock_settime
.synop begin
#include <time.h>
int clock_settime(clockid_t clockid, const struct timespec *ts);

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
    
.synop end
.desc begin
The
.id &funcb.
function sets the time for the clock specified by
.arg clockid
to the time contained in the
.arg ts
pointer.  Depending on the value of
.arg clockid
, the function may fail if the user has insufficient
privileges.
.np
Values of
.arg ts
that specify significance beyond the clock's resolution will
be truncated.
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
return value is the negation of the appropriate
.kw errno
value as specified below.  This implementation will also set
.kw errno
appropriately on failures.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg clockid
is invalid, or the value of
.arg ts
exceeds the range of this clock.
.term EPERM
The user does not have sufficient privileges to modify the specified clock.
.endterm
.error end
.see begin
.seelist clock_gettime clock_getres clock_nanosleep
.see end
.class POSIX
.system
