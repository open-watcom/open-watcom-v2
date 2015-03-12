.func timer_create
.synop begin
#include <time.h>
int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);

struct sigevent {
    int          sigev_signo;
    union sigval sigev_value;
    int          sigev_notify;
};
.synop end
.desc begin
The
.id &funcb.
function creates a new timer using the clock specified by
.arg clockid
as supported by the underlying operating system. The
.arg evp
argument can be NULL or may specify a handler for when an
event of interest occurs.  This implementation currently
only supports responding using SIGEV_SIGNAL implementations.
The pointer
.arg timerid
will contain the unique, per-process timer id if the call
is successful.
.desc end
.return begin
If successful, the function will return zero, and the
.arg timerid
argument will contain the timer id.  If the call fails, the
return value is -1, and
.kw errno
will be set appropriately.
.return end
.error begin
.begterm 2
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The value of
.arg timerid
is NULL or an invalid
.arg clockid
is specified
.term EAGAIN
The system was unable to allocate resources for a new timer
.endterm
.error end
.see begin
.seelist timer_gettime timer_settime timer_delete timer_getoverrun
.see end
.class POSIX
.system
