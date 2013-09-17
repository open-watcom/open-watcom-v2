.*
.*
.option NOLARGEADDRESSAWARE
.*
.np
The "NOLARGEADDRESSAWARE" option specifies that the application can not handle
addresses larger than 2 gigabytes. The linker reset appropriate flag to the
PE format image header.
.np
The format of the "NOLARGEADDRESSAWARE" option (short form "NOLARGE") is as follows.
.mbigbox
    OPTION NOLARGEADDRESSAWARE
.embigbox
.np
