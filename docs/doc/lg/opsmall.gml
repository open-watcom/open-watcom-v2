.*
.*
.option SMALL
.*
.np
The "SMALL" option tells the &lnkname to use the standard overlay
manager (as opposed to the dynamic overlay manager) and that near
calls can be generated to overlay vectors corresponding to routines
defined in the overlayed portion of your program.
The format of the "SMALL" option (short form "SM") is as follows.
.mbigbox
    OPTION SMALL
.embigbox
.np
This option should only be specified in the following circumstances.
.autonote
.note
Your program has been compiled for a small code memory model.
.note
You are creating an overlayed application.
.note
The code in your program, including overlay areas, does not exceed
64K.
.endnote
.np
If the "SMALL" option is not specified and you are creating an
overlayed application, the linker will generate far calls to overlay
vectors.
In this case, your application must have been compiled using a big
code memory model.
