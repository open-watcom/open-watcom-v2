.*
.*
.option MIXED1632
.*
.np
The "MIXED1632" option specifies that 16-bit and 32-bit logical segments may
be grouped into a single physical segment. This applies to both code and
data segments.
.np
The format of the "MIXED1632" option (short form "MIX") is as
follows.
.mbigbox
    OPTION MIXED1632
.embigbox
.np
This option is useful certain specialized applications, such as OS/2 physical
device drivers. In most cases, mixing of 16-bit and 32-bit segments should
be avoided.
