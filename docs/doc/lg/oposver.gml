.*
.*
.option OSVERSION
.*
.np
The "OSVERSION" option specifies that the linker should apply the given major
and minor version numbers to the PE format image header. This specifies the
major and minor versions of the operating system required to load this image.
If a version number is not specified, then the built-in value of 1.11 is used.
The format of the "OSVERSION" option (short form "OSV") is as follows.
.mbigbox
    OPTION OSVERSION = major[.minor]
.embigbox
