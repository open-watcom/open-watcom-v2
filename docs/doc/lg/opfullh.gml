.*
.*
.option FULLHEADER
.*
.np
This option is valid for 16-bit DOS "EXE" files.
By default, the &lnkname writes a "MZ" executable header which is just large
enough to contain all necessary data. 
The "FULLHEADER" option may be used to force the header size to 64 bytes, plus
the size of relocation records.
The format of the "FULLHEADER" option (short form "FULLH") is as
follows.
.mbigbox
    OPTION FULLHEADER
.embigbox
.np
.autonote Notes:
.note
This option may be useful when creating a 16-bit executable which is to be
used as a stub program for a non-DOS executable.
.note
This option is not required when using the &lnkname.. It is only needed when
the non-DOS executable is created using a third-party linker which does not
automatically extend the header size.
.endnote
