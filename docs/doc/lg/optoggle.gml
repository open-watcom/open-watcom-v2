.*
.*
.option TOGGLERELOCS
.*
.np
The "TOGGLERELOCS" option is used with LX format executables under
32-bit DOS/4G only.
.ix 'internal relocation'
.ix 'relocation' 'internal'
The "INTERNALRELOCS" option causes the &lnkname to include internal
relocation information in DOS/4G LX format executables.
Having done so, the linker normally clears the "internal fixups
done" flag in the LX executable header (bit 0x10).
The "TOGGLERELOCS" option causes the linker to toggle the value of
the "internal fixups done" flag in the LX executable header (bit
0x10).
This option is used with DOS/4G non-zero based executables.
Contact Tenberry Software for further explanation.
.np
The format of the "TOGGLERELOCS" option (short form "TOG") is as
follows.
.mbigbox
    OPTION TOGGLERELOCS
.embigbox
