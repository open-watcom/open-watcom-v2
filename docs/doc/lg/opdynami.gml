.*
.*
.option DYNAMIC
.*
.np
The "DYNAMIC" option tells the &lnkname to use the dynamic overlay
manager.
The format of the "DYNAMIC" option (short form "DYN") is as follows.
.mbigbox
    OPTION DYNAMIC
.embigbox
.np
Note that the dynamic overlay manager can only be used with
applications that have been compiled using the "of" option and a big
code memory model.
The "of" option generates a special prologue/epilogue sequence for
procedures that is required by the dynamic overlay manager.
See the compiler User's Guide for more information on the "of"
option.
.np
For more information on the dynamic overlay manager,
see the section entitled :HDREF refid='useover'..
