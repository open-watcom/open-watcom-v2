.*
.*
.dirctv NEWSEGMENT
.*
.np
This directive is intended for 16-bit segmented applications.
By default, the &lnkname automatically groups logical code segments
into physical segments.
By default, these segments are 64K bytes in size.
However, the "PACKCODE" option can be used to specify a maximum size
for all physical segments that is smaller than 64K bytes.
.np
The "NEWSEGMENT" directive provides an alternate method of grouping
code segments into physical segments.
By placing this directive after a sequence of "FILE" directives, all
code segments appearing in object modules specified by the sequence of
"FILE" directives will be packed into a physical segment.
Note that the size of a physical segment may vary in size.
The format of the "NEWSEGMENT" directive (short form "NEW") is as
follows.
.mbigbox
    NEWSEGMENT
.embigbox
.np
Consider the following example.
.millust begin
file file1, file2, file3
newsegment
file file4
file file5
.millust end
.pc
Code segments from
.sy file1,
.sy file2
and
.sy file3
will be grouped into one physical segment.
Code segments from
.sy file4
and
.sy file5
will be grouped into another physical segment.
.np
Note that code segments extracted from library files will be grouped
into physical segments as well.
The size of these physical segments is determined by the "PACKCODE"
option and is 64k by default.
