.*
.*
.option RWRELOCCHECK
.*
.np
The "RWRELOCCHECK" option causes the linker to check for segment
relocations to a read/write data segment and issue a warning if any
are found.
This option is useful if you are building a 16-bit Windows application
that may have more than one instance running at a given time.
.np
The format of the "RWRELOCCHECK" option (short form "RWR") is as
follows.
.mbigbox
    OPTION RWRELOCCHECK
.embigbox
