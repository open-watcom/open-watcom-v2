.*
.*
.option PSEUDOPREEMPTION
.*
.np
The "PSEUDOPREEMPTION" option specifies that an additional set of
system calls will yield control to other processes.
Multitasking in current NetWare operating systems is non-preemptive.
That is, a process must give up control in order for other processes
to execute.
Using the "PSEUDOPREEMPTION" option increases the probability that all
processes are given an equal amount of CPU time.
.np
The format of the "PSEUDOPREEMPTION" option (short form "PS") is as
follows.
.mbigbox
    OPTION PSEUDOPREEMPTION
.embigbox
