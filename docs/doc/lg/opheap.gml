.*
.*
.option HEAPSIZE
.*
.np
The "HEAPSIZE" option specifies the size of the heap required by the
application.
The format of the "HEAPSIZE" option (short form "H") is as follows.
.mbigbox
    OPTION HEAPSIZE=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the size of the heap.
The default heap size is 0 bytes.
The maximum value of
.sy n
is 65536 (64K) for 16-bit applications and 4G for 32-bit applications
which is the maximum size of a physical segment.
Actually, for a particular application, the maximum value of
.sy n
is 64K or 4G less the size of group "DGROUP".
