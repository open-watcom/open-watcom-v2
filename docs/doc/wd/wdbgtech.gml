.chap Techniques for Tracking Down Bugs in a Program
.*
.np
A common execution error is the "Null Assignment". The C programming
language makes use of the concept of a NULL pointer. The NULL pointer
cannot be dereferenced (i.e., used as a pointer) according to the ISO/ANSI
standard. In "real-mode" 80x86 systems, the compiler's run-time system
cannot signal the programmer when the NULL address has been written to
or read from. The best that the run-time system can do is help
programmers find these sorts of errors through indirect means. The
lower 32 bytes of NEAR memory have been seeded with 32 bytes of the
value 0x01. The run-time function "_exit" checks these 32 bytes to
ensure that they have not been written over. Any modification of these
32 bytes results in the "NULL assignment error" being printed before
the program terminates.
.np
Here is an overview of a good debugging technique for this sort of error:
.autopoint
.point
Use the &dbgname. to debug the program.
.point
Let the program execute.
.point
When and if the program exits with "NULL assignment error" check the
NULL area to see which address has been modified.
.point
Set a "break-on-write" breakpoint on the modified address.
.point
Restart the application.
.point
When the memory location is modified, execution will be suspended.
.point
You should be able to easily spot the bug from this point.
.endpoint
