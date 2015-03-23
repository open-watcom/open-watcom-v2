.func sysconf
.synop begin
#include <unistd.h>
long sysconf( int name ); 
.synop end
.desc begin
The
.id &funcb.
function retrieves a current system configuration parameter
depending on the value of system information from the kernel via the
.arg name
passed.  This implementation currently supports the following inputs:
.begterm 16
.termhd1 Member
.termhd2 Meaning
.term _SC_ARG_MAX
The maximum number of arguments supported by 
.id execv 
and related arguments
.term _SC_CHILD_MAX
The maximum number of per-user child processes
.term _SC_CLK_TCK
The number of clock ticks per second
.term _SC_NGROUPS_MAX
The maximum number of groups to which a user may be a member
.term _SC_OPEN_MAX
The maximum number of file that may be opened by a user
.term _SC_JOB_CONTROL
If job control is supported, the value of 1 is returned
.term _SC_SAVED_IDS
If saved identifiers are supported, the value of 1 is returned
.term _SC_VERSION
The release year and month of the POSIX.1 standard attempting to be supported, in the format YYYYMM
.term _SC_STREAM_MAX
The maximum number of per-process streams supported
.term _SC_TZNAME_MAX
The maximum length of a time zone name
.term _SC_PAGESIZE
The size, in bytes, of one page of memory
.term _SC_NPROCESSORS_CONF
The number of processors currently configured on the system
.term _SC_NPROCESSORS_ONLN
The number of processors currently online and functioning on the system
.term _SC_PHYS_PAGES
The total number of physical pages of memory for the system
.term _SC_AVPHYS_PAGES
The available number of physical pages of memory for the system
.term _SC_REALTIME_SIGNALS
If real-time signals are supported, the value of 1 is returned
.endterm
In this implementation, requesting either _SC_NPROCESSORS_CONF or
_SC_NPROCESSORS_ONLN will return the same value.
.np
Some inputs, specifically _SC_OPEN_MAX and _SC_CHILD_MAX, are configurable
via calls to
.id setrlimit
and may change between subsequent
.id &funcb.
calls.
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
.arg name
is unsupported on this implementation
.endterm
.error end
.class POSIX
.system
