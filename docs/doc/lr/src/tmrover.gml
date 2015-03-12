.func timer_getoverrun
.synop begin
#include <time.h>
int timer_getoverrun( timer_t timerid );
.synop end
.desc begin
The
.id &funcb.
function returns the number of intervals for the given
.arg timerid
since expiration.
.desc end
.return begin
If successful, the function will return the number of
elapsed intervals since the latest timer expiration. If the 
call fails, the
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
.seelist timer_create timer_settime timer_delete timer_overrun
.see end
.class POSIX
.system
