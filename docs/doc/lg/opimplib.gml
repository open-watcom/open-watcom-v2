.*
.*
.option IMPLIB
.*
.np
.ix 'import library'
The "IMPLIB" option requests the linker to produce an import library
that corresponds to the DLL that is being generated.
The format of the "IMPLIB" option (short form "IMPL") is as follows.
.mbigbox
    OPTION IMPLIB[=imp_lib]
.embigbox
.synote
.mnote imp_lib
is a file specification for the name of the import library file.
If no file extension is specified, a file extension of "lib" is
assumed.
.esynote
.np
By default, no library file is generated.
.ix 'library file'
Specifying this option causes the &lnkname to generate an import
library file.
The import library file contains a list of the entry points in your
DLL.
.np
If no file name is specified, the import library file will have a
default file extension of "lib" and the same file name as the DLL
file.
Note that the import library file will be created in the same
directory as the DLL file.
The DLL file path and name can be specified in the "NAME" directive.
.np
Alternatively, a library file path and name can be specified.
The following directive instructs the linker to generate a library
file and call it "mylib.imp" regardless of the name of the executable
file.
.millust begin
option implib=mylib.imp
.millust end
.pc
You can also specify a path and/or file extension when using the
"IMPLIB=" form of the "IMPLIB" option.
.remark
At present, the linker spawns the &libname to create the import
library file.
.eremark
