.*
.*
.option MAP
.*
.np
The "MAP" option controls the generation of a map file.
The format of the "MAP" option (short form "M") is as follows.
.mbigbox
    OPTION MAP[=map_file]
.embigbox
.synote
.mnote map_file
is a file specification for the name of the map file.
If no file extension is specified, a file extension of "map" is
assumed.
.esynote
.np
By default, no map file is generated.
.ix 'map file'
Specifying this option causes the &lnkname to generate a map file.
The map file is simply a memory map of your program.
That is, it specifies the relative location of all global symbols in
your program.
The map file also contains the size of your program.
.np
If no file name is specified, the map file will have a default file
extension of "map" and the same file name as the executable file.
Note that the map file will be created in the current directory even
if the executable file name specified in the "NAME" directive contains
a path specification.
.np
Alternatively, a file name can be specified.
The following directive instructs the linker to generate a map file
and call it "myprog.map" regardless of the name of the executable
file.
.millust begin
option map=myprog
.millust end
.pc
You can also specify a path and/or file extension when using the
"MAP=" form of the "MAP" option.
