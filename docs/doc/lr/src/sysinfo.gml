.func sysinfo
.synop begin
#include <sys/sysinfo.h>
int sysinfo( struct sysinfo *info );

struct sysinfo {
    unsigned long uptime;
    unsigned long loads[3];
    unsigned long totalram;
    unsigned long freeram;
    unsigned long sharedram;
    unsigned long bufferram;
    unsigned long totalswap;
    unsigned long freeswap;
    unsigned short procs, pad;
    unsigned long totalhigh;
    unsigned long freehigh;
    unsigned mem_unit;
    char __reserved[];
};
   
.synop end
.desc begin
The
.id &funcb.
function retrieves system information from the kernel via the
.arg info
structure.  The members of the structure are defined below:
.begterm 12
.termhd1 Member
.termhd2 Meaning
.term uptime
The number of seconds since the system booted
.term loads
The 1 minute, 5 minute, and 15 minute load averages
.term totalram
The total usable amount of system memory in bytes
.term freeram
The amount of memory currently unused and unallocated
.term sharedram
The amount of memory currently being shared
.term bufferram
The amount of memory allocated to buffering
.term totalswap
The amount of swap space available in bytes
.term freeswap
The amount of unused swap space
.term procs
The amount of processes currently running
.term pad
Padding for alignment; can be ignored
.term totalhigh
Total amount of "high" memory
.term mem_unit
The memory unit size in bytes
.endterm
.desc end
.return begin
If successful, the function will return zero. If the call fails, the
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
.arg info
is NULL
.term EFAULT
The value of
.arg info
is invalid
.endterm
.error end
.class WATCOM
.system
