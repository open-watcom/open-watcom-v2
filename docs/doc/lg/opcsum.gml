.*
.*
.option CHECKSUM
.*
.np
The "CHECKSUM" option specifies that the linker should create an MS-CRC32
checksum for the current image. This is primarily used for DLL's and device
drivers but can be applied to any PE format images.
The format of the "CHECKSUM" option (no short form) is as follows.
.mbigbox
    OPTION CHECKSUM
.embigbox
