.*
.*
.option MINDATA
.*
.np
The format of the "MINDATA" option (short form "MIND") is as follows.
.mbigbox
    OPTION MINDATA=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the minimum number of bytes, in addition to the memory
required by executable image, that must be allocated by
386|DOS-Extender at the end of the loaded executable image.
If
.sy n
bytes are not available, the program will not be executed.
.np
If the "MINDATA" option is not specified, a default value of zero is
assumed.
This means that 386|DOS-Extender will load the program as long as
there is enough memory for the load image; no extra memory is
required.
