.*
.*
.option NORELOCS
.*
.np
The "NORELOCS" option specifies that no relocation information is to
be written to the executable file.
When the "NORELOCS" option is specified, the executable file can only
be run in protected mode and will not run in real mode.
In real mode, the relocation information is required; in protected
mode, the relocation information is not required unless your
application is running at privilege level 0.
.np
The format of the "NORELOCS" option (short form "NOR") is as follows.
.mbigbox
    OPTION NORELOCS
.embigbox
.synote
.mnote NORELOCS
tells the &lnkname not to generate relocation information.
.esynote
