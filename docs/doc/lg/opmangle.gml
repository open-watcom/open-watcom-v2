.*
.*
.option MANGLEDNAMES
.*
.np
The "MANGLEDNAMES" option should only be used if you are developing a
&cmppname application.
.ix 'mangled names in C++'
Due to the nature of C++, the &cmppname compiler generates mangled
names for symbols.
A mangled name for a symbol includes the following.
.autopoint
.point
symbol name
.point
scoping information
.point
typing information
.endpoint
.np
This information is stored in a cryptic form with the symbol.
When the linker encounters a mangled name in an object file, it formats
the above information and produces this name in the map file.
.np
If you would like the linker to produce the mangled name as it appeared
in the object file, specify the "MANGLEDNAMES" option.
.np
The format of the "MANGLEDNAMES" option (short form "MANG") is as follows.
.mbigbox
    OPTION MANGLEDNAMES
.embigbox
