.*
.*
.option PACKCODE
.*
.np
This option is intended for 16-bit segmented applications.
By default, the &lnkname automatically groups logical code segments
into physical segments.
The "PACKCODE" option is used to specify the size of the physical
segment.
The format of the "PACKCODE" option (short form "PACKC") is as
follows.
.mbigbox
    OPTION PACKCODE=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the size of the physical segments into which code segments
are packed.
The default value of
.sy n
is 64K for 16-bit applications.
Note that this is also the maximum size of a physical segment.
To suppress automatic grouping of code segments, specify a value of 0
for
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
