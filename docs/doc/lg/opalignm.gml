.*
.*
.option ALIGNMENT
.*
.np
The "ALIGNMENT" option specifies the alignment for segments in the
executable file.
The format of the "ALIGNMENT" option (short form "A") is as follows.
.mbigbox
    OPTION ALIGNMENT=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the alignment for segments in the executable file and must
be a power of 2.
.np
In 16-bit applications,
segments in the executable file are pointed to by a segment table.
An entry in the segment table contains a 16-bit value which is
a multiple of the alignment value.
Together they form the offset of the segment from the start of
the segment table.
Note that the smaller the value of
.sy n
the smaller the executable file.
.np
By default, the &lnkname will automatically choose the smallest value
of
.sy n
possible.
You need not specify this option unless you want padding between
segments in the executable file.
