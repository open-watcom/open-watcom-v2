.func wait
.synop begin
#include <process.h>
int wait( int *status );
.ixfunc2 '&OS2Func' &func
.ixfunc2 '&NTFunc' &func
.synop end
.desc begin
The &func function suspends the calling process until any of the caller's
immediate child processes terminate.
.np
Under Win32, there is no parent-child relationship amongst
processes so the &func function cannot and does not wait for child
processes to terminate.
To wait for any process, you must specify its process id.
For this reason, the
.kw cwait
function should be used (one of its arguments is a process id).
.im waitstat
.desc end
.return begin
The &func function returns the child's process id if the child process
terminated normally.
Otherwise, &func returns &minus.1 and sets
.kw errno
to one of the following values:
.begterm 10
.termhd1 Constant
.termhd2 Meaning
.term ECHILD
No child processes exist for the calling process.
.term EINTR
The child process terminated abnormally.
.endterm
.return end
.see begin
.seelist wait cwait exit _exit spawn...
.see end
.exmp begin
#include <stdlib.h>
#include <process.h>

void main()
  {
     int   process_id, status;
.exmp break
     process_id = spawnl( P_NOWAIT, "child.exe",
                "child", "parm", NULL );
     wait( &status );
  }
.exmp end
.class WATCOM
.system
