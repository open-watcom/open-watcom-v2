.*
.*
.option FILLCHAR
.*
.np
The "FILLCHAR" option (short form "FILL") specifies the byte value used to
fill gaps in the output image.
.mbigbox
    OPTION FILLCHAR=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the value to be used in blank areas of the output image.
The value must be in the range of 0 to 255, inclusive.
.np
This option is most useful for raw binary output that will be programmed
into an (E)EPROM where a value of 255 (0xff) is preferred. The default
value of
.sy n
is zero.
