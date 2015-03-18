.func clock_getres
.synop begin
#include <time.h>
int clock_getres(clockid_t clockid, struct timespec *ts);

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
    
.synop end
.desc begin
The
.id &funcb.
function retrieves the minimum resolution of the  clock specified by
.arg clockid
in the
.arg ts
pointer.
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
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg clockid
is invalid.
.endterm
.error end
.see begin
.seelist clock_settime clock_gettime clock_nanosleep
.see end
.class POSIX
.system
