.*
.*
.option LINEARRELOCS
.*
.np
The "LINEARRELOCS" option instructs the linker to generate offset
fixups in addition to the normal segment fixups.
The offset fixups allow the system to move pieces of code and data
that were loaded at a particular offset within a segment to another
offset within the same segment.
.np
The format of the "LINEARRELOCS" option (short form "LI") is as
follows.
.mbigbox
    OPTION LINEARRELOCS
.embigbox
