.func cwait _cwait
#include <process.h>
int cwait( int *status, int process_id, int action );
.ixfunc2 '&OS2Func' &func
.ixfunc2 '&NTFunc' &func
.if &'length(&_func.) ne 0 .do begin
int _cwait( int *status, int process_id, int action );
.ixfunc2 '&OS2Func' &_func
.ixfunc2 '&NTFunc' &_func
.do end
.funcend
.desc begin
The &func function suspends the calling process until the specified
process terminates.
.im waitstat
.np
The
.arg process_id
argument specifies which process to wait for.
Under Win32, any process can wait for any other process for which the
process id is known.
Under OS/2, a process can wait for any of its child processes.
For example, a process id is returned by certain forms of the
.kw spawn
function that is used to start a child process.
.np
The
.arg action
argument specifies when the parent process resumes execution.
This argument is ignored in Win32, but is accepted for compatibility
with OS/2 (although Microsoft handles the
.arg status
value differently from OS/2!).
The possible values are:
.begterm 17
.termhd1 Value
.termhd2 Meaning
.term WAIT_CHILD
Wait until the specified child process has ended.
.term WAIT_GRANDCHILD
Wait until the specified child process and all of the child processes
of that child process have ended.
.endterm
.np
Under Win32, there is no parent-child relationship.
.desc end
.return begin
The &func function returns the (child's) process id if the (child)
process terminated normally.
Otherwise, &func returns &minus.1 and sets
.kw errno
to one of the following values:
.begterm 10
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
Invalid action code
.term ECHILD
Invalid process id, or the child does not exist.
.term EINTR
The child process terminated abnormally.
.endterm
.return end
.see begin
.seelist cwait exit _exit spawn... wait
.see end
.exmp begin
#include <stdio.h>
#include <process.h>
.exmp break
void main()
  {
     int   process_id;
     int   status;
.exmp break
     process_id = spawnl( P_NOWAIT, "child.exe",
                "child", "parm", NULL );
     cwait( &status, process_id, WAIT_CHILD );
  }
.exmp end
.class WATCOM
.system
