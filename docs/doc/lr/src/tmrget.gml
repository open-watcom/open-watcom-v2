.func timer_gettime
.synop begin
#include <time.h>
int timer_gettime(timer_t timerid,
                  struct itimerspec *value );

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
function retrieves time remaining in the timer
.arg timerid
until expiration.
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
.seelist timer_create timer_settime timer_delete timer_getoverrun
.see end
.class POSIX
.system
