.*
.*
.option OLDLIBRARY
.*
.np
The "OLDLIBRARY" option is used to preserve the export ordinals for
successive versions of a Dynamic Link Library.
This ensures that any application that references functions in a
Dynamic Link Library by ordinal will continue to execute correctly.
The format of the "OLDLIBRARY" option (short form "OLD") is as
follows.
.mbigbox
    OPTION OLDLIBRARY=dll_name
.embigbox
.synote
.mnote dll_name
is a file specification for the name of a Dynamic Link Library.
If no file extension is specified, a file extension of "DLL" is
assumed.
.esynote
.np
Only the current directory or a specified directory will be searched
for Dynamic Link Libraries specified in the "OLDLIBRARY" option.
