.*
.*
.option LINKVERSION
.*
.np
The "LINKVERSION" option specifies that the linker should apply the given major
and minor version numbers to the PE format image header. If a version number is
not specified, then the built-in value of 2.18 is used.
The format of the "LINKVERSION" option (short form "LINKV") is as follows.
.mbigbox
    OPTION LINKVERSION = major[.minor]
.embigbox
