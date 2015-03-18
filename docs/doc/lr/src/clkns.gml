.func clock_nanosleep
.synop begin
#include <time.h>
int clock_nanosleep(clockid_t clockid, int flags,
                    const struct timespec *request,
                    struct timespec *remains);

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
    
.synop end
.desc begin
The
.id &funcb.
function causes the current thread to be suspended until
the time period specified by 
.arg request
has elapsed according to the clock specified by
.arg clockid
if the 
.arg flags
 do not contain the value TIMER_ABSTIME.
.np
If
.arg flags
does contain TIMER_ABSTIME, the
.id &funcb.
function causes the current thread to be suspended until
the absolute time specified by
.arg request
has been reached according to the clock specified by
.arg clockid
if the flags.
.np
If
.arg remains
is not NULL, the structure will contain the amount of time
remaining to be slept if
.id &funcb.
is interrupted by a signal.
.desc end
.return begin
If successful and uninterrupted, the function will return zero. If the call fails
or is interrupted, the return value is the negation of the appropriate
.kw errno
value as specified below.  This implementation will also set
.kw errno
appropriately on failures.
.return end
.error begin
.begterm 3
.termhd1 Constant
.termhd2 Meaning
.term EINTR
The sleep was interrupted by a signal.
.term EINVAL
The value of
.arg clockid
,
.arg request
, or
.arg flags
is invalid.
.term ENOTSUP
The specified clock does not support this function.
.endterm
.error end
.see begin
.seelist clock_gettime clock_getres clock_settime
.see end
.class POSIX
.system
