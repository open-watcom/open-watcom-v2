.*
.*
.option PACKDATA
.*
.np
This option is intended for 16-bit segmented applications.
By default, the &lnkname automatically groups logical far data
segments into physical segments.
The "PACKDATA" option is used to specify the size of the physical
segment.
The format of the "PACKDATA" option (short form "PACKD") is as
follows.
.mbigbox
    OPTION PACKDATA=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the size of the physical segments into which far data
segments are packed.
The default value of
.sy n
is 64K for 16-bit applications.
:CMT.  and 4G for 32-bit applications.
Note that this is also the maximum size of a physical segment.
To suppress automatic grouping of far data segments, specify a value
of 0 for
.sy n.
.autonote Notes:
.note
Only adjacent segments are packed into a physical segment.
.note
Segments belonging to the same group are packed in a physical segment.
Segments belonging to different groups are not packed into a physical
segment.
.note
Segments with different attributes are not packed together unless they
are explicitly grouped.
.endnote
