.func timer_delete
.synop begin
#include <time.h>
int timer_delete(timer_t timerid );
.synop end
.desc begin
The
.id &funcb.
function disarms, if necessary, and deletes the timer
.arg timerid
immediately.
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
.seelist timer_create timer_settime timer_gettime timer_delete
.see end
.class POSIX
.system
