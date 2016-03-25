.func gettid
.synop begin
#include <process.h>
int gettid(void);
.synop end
.desc begin
The
.id &funcb.
function returns the thread id for the current thread on
Linux systems.  It provides a wrapper around the appropriate
kernel system call.
.desc end
.return begin
The
.id &funcb.
function returns the thread id for the current thread.
.return end
.see begin
.seelist getpid
.see end
.class WATCOM
.system
