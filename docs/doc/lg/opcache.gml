.*
.*
.option CACHE
.*
.np
The "CACHE" and "NOCACHE" options can be used to control caching of
object and library files in memory by the linker.
When neither the "CACHE" nor "NOCACHE" option is specified, the
linker will only cache small libraries.
Object files and large libraries are not cached.
The "CACHE" and "NOCACHE" options can be used to alter this default
behaviour.
The "CACHE" option enables the caching of object files and large
library files while the "NOCACHE" option disables all caching.
.np
The format of the "CACHE" option (short form "CAC") is as follows.
.mbigbox
    OPTION CACHE
.embigbox
.np
The format of the "NOCACHE" option (short form "NOCAC") is as follows.
.mbigbox
    OPTION NOCACHE
.embigbox
.np
When linking large applications with many object files, caching object
files will cause extensive use of memory by the linker.
On virtual memory systems such as OS/2, Windows NT or Windows 95, this
can cause extensive page file activity when real memory resources have
been exhausted.
This can degrade the performance of other tasks on your system.
For this reason, the OS/2 and Windows-hosted versions of the linker do
not perform object file caching by default.
This does not imply that object file caching is not beneficial.
If your system has lots of real memory or the linker is running as the
only task on the machine, object file caching can certainly improve
the performance of the linker.
.np
On single-tasking environments such as DOS, the benefits of improved
linker performance outweighs the memory demands associated with object
file caching.
For this reason, object file caching is performed by default on these
systems.
If the memory requirements of the linker exceed the amount of memory
on your system, the "NOCACHE" option can be specified.
.np
The QNX operating system is a multi-tasking real-time operating
system.
However, it is not a virtual memory system.
Caching object files can consume large amounts of memory.
This may prevent other tasks on the system from running, a problem
that may be solved by using the "NOCACHE" option.
