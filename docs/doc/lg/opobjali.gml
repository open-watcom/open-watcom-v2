.*
.*
.option OBJALIGN
.*
.np
The "OBJALIGN" option specifies the alignment for objects in the
executable file.
The format of the "OBJALIGN" option (short form "OBJA") is as follows.
.mbigbox
    OPTION OBJALIGN=n
.embigbox
.synote
.im lnkvalue
.np
.id n
must be a value that is a power of 2 and is between 16 bytes and 256
megabytes inclusive.
The default is 64k.
.esynote
