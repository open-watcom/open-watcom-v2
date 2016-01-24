.func clone
.synop begin
#include <sched.h>

pid_t  clone( int (*fn)(void *), void *child_stack,  
              int flags, void *args, ... );
            /* pid_t *ppid, void *tls, pid_t *ctid */
            
.synop end
.desc begin
The
.id &funcb.
function creates a "clone" of the currently running process that shares
the address space of the original, parent process, and starts execution
within the clone process at the
.arg fn
function.  The child function is called with 
.arg args
as its argument, and, once complete, with will terminate with the return
value of
.arg fn
as its exit code.  This function provides a wrapper around the Linux
kernel's
.kw SYS_clone
system call.
.np
The
.arg child_stack
argument points to a user-allocated memory block within the process's
heap for use as the cloned process's new stack space.  Because the stack
will grow in the direction of descending address, the
.arg child_stack
argument should refer to the highest address in the allocated memory.
.np
The
.arg flags
argument may be one of the following values:
.begterm 21
.termhd1 Constant
.termhd2 Meaning
.term CLONE_VM
Virtual memory should be shared across processes
.term CLONE_FS
File system information should be shared across processes
.term CLONE_FILES
File handles should be shared across processes
.term CLONE_SIGHAND
Signal handlers and blocked signals should be shared across processes
.term CLONE_PTRACE
Allow tracing within the child process (if enabled in the parent)
.term CLONE_VFORK
The child can wake the parent via 
.kw mm_release
.term CLONE_PARENT
The new child should share the same parent as the cloning process
.term CLONE_THREAD
The new process is to be considered a thread, and should be grouped in 
the same thread group as the parent
.term CLONE_NEWNS
The child process is provided a new mount namespace
.term CLONE_SYSVSEM
The child process shares all System V semaphores
.term CLONE_SETTLS
The thread-local storage, an optional argument, should be passed and set
appropriately.  The user must specify the
.arg tls
argument if this flag is set.
.term CLONE_PARENT_SETTID
Store the thread ID of the parent in the
.arg ptid
argument.  The user must specify the optional
.arg ptid
argument if this flag is set.
.term CLONE_CHILD_CLEARTID
Clear the thread ID stored in 
.arg ctid
argument.  The user must specify the optional
.arg ctid
argument if this flag is set.
.term CLONE_UNTRACED      
The parent process cannot force tracing on the child process
.term CLONE_CHILD_SETTID  
Store the thread ID of the child process in the
.arg ctid
argument.  The user must specify the optional
.arg ctid
argument if this flag is set.
.term CLONE_NEWUTS
The child process has a new UTS namespace
.term CLONE_NEWIPC
The child process has a new IPC namespace
.term CLONE_NEWUSER
The child process has a new user namespace
.term CLONE_NEWPID
The child process has a new PID namespace
.term CLONE_NEWNET
The child process has a new network namespace
.term CLONE_IO
The parent and child share io contexts
.endterm
.np
This function provides a low-level threading interface that is normally
meant for designing higher level thread libraries.
.desc end
.return begin
If successful, the function will return the process ID of the child
process.  If the system call fails, the return value will be -1, and
.kw errno
will be set appropriately.
.return end
.class WATCOM
.system
