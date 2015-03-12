.func timer_settime
.synop begin
#include <time.h>
int timer_settime(timer_t timerid, int flags, 
                  struct itimerspec *new_value,
                  struct itimerspec *old_value );

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
    int             notify_type;
    int             timer_type;
    long            data;
    
.synop end
.desc begin
The
.id &funcb.
function arms or resets the timer
.arg timerid
using the interval and value specified in 
.arg new_value
pointer.  The previous interval and value is returned in the
.arg oldvalue
pointer.  
.np
The structure pointed to by the "it_value" member of
.arg new_value
specifies the time in the future when the timer will expire,
and effectively arms the timer.  If the it_value member of 
.arg new_value
specifies a time of zero, the timer is disarmed.  The structure
pointed to by the "it_interval" member of
.arg new_value
specifies the interval after the initial timer expiration when
the timer would repeat expiration.  If the it_interval member's
components are set to zero, the timer will expire at the time
specified by the "it_value" member of
.arg new_value
and the timer will not automatically rearm.
.np
The "it_value" member of
.arg new_value
is regarded, by default, as a time relative to the system clock
at the time of the function call.  If flags incorporates the
TIMER_ABSTIME constant, the time in "it_value" is regarded as 
an absolute system time as opposed to a time relative to 
calling this function.  
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
return value is -1, and
.kw errno
will be set appropriately.
.return end
.error begin
.begterm 1
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg timerid
is NULL or invalid
.endterm
.error end
.see begin
.seelist timer_create timer_gettime timer_delete timer_getoverrun
.see end
.class POSIX
.system
