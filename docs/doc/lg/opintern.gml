.*
.*
.option INTERNALRELOCS
.*
.np
The "INTERNALRELOCS" option is used with LX format executables under
32-bit OS/2.
.ix 'internal relocation'
.ix 'relocation' 'internal'
By default, OS/2 executables do not contain internal relocation
information and OS/2 Dynamic Link Libraries do contain internal
relocation information.
This option causes the &lnkname to include internal relocation
information in OS/2 LX format executables.
.np
The format of the "INTERNALRELOCS" option (short form "INT") is as
follows.
.mbigbox
    OPTION INTERNALRELOCS
.embigbox
