.*
.*
.dirctv RESOURCE
.*
.np
The "RESOURCE" directive is used to specify
resource files to add to the executable file being generated.
.*
The format of the "RESOURCE" directive (short form "RES") is as follows.
.mbigbox
    RESOURCE resource_file{,resource_file}
.embigbox
.synote
.mnote resource_file
is a file specification for the name of the resource file that to
be added to the executable file.
If no file extension is specified, a file extension of "res" is assumed.
.esynote
.*
