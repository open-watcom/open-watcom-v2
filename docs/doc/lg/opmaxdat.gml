.*
.*
.option MAXDATA
.*
.np
The format of the "MAXDATA" option (short form "MAXD") is as follows.
.mbigbox
    OPTION MAXDATA=n
.embigbox
.synote
.im lnkvalue
.esynote
.np
.sy n
specifies the maximum number of bytes, in addition to the memory
required by executable image, that may be allocated by
386|DOS-Extender at the end of the loaded executable image.
No more than
.sy n
bytes will be allocated.
.np
If the "MAXDATA" option is not specified, a default value of
hexadecimal ffffffff is assumed.
This means that 386|DOS-Extender will allocate all available memory to
the program at load time.
