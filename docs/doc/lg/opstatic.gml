.*
.*
.option STATICS
.*
.np
The "STATICS" option should only be used if you are developing a
&cmpcname or C++ application.
The &cmpcname and C++ compilers produce definitions for static symbols
in the object file.
By default, these static symbols do not appear in the map file.
If you want static symbols to be displayed in the map file, use the
"STATICS" option.
.np
The format of the "STATICS" option (short form "STAT") is as follows.
.mbigbox
    OPTION STATICS
.embigbox
