.*
.*
.option LONGLIVED
.*
.np
The "LONGLIVED" option specifies that the application being linked
will reside in memory, or be active, for a long period of time (e.g.,
background tasks).
The memory manager, knowing an application is "LONGLIVED", allocates
memory for the application so as to reduce fragmentation.
.np
The format of the "LONGLIVED" option (short form "LO") is as follows.
.mbigbox
    OPTION LONGLIVED
.embigbox
