.*
.*
.option ARTIFICIAL
.*
.np
The "ARTIFICIAL" option should only be used if you are developing a
&cmppname application.
A &cmppname application contains many compiler-generated symbols.
By default, the linker does not include these symbols in the map file.
The "ARTIFICIAL" option can be used if you wish to include these
compiler-generated symbols in the map file.
.np
The format of the "ARTIFICIAL" option (short form "ART") is as follows.
.mbigbox
    OPTION ARTIFICIAL
.embigbox
