.*
.*
.option IMPFILE
.*
.np
.ix 'import library'
.ix 'import library command file'
The "IMPFILE" option requests the linker to produce a &libname command
file that can be used to create an import library that corresponds to
the DLL that is being generated.
This option is useful in situations where the &lnkname cannot create
an import library file when you have specified the "IMPLIB" option
(i.e., the linker fails to launch &libname.).
.np
The format of the "IMPFILE" option (short form "IMPF") is as follows.
.mbigbox
    OPTION IMPFILE[=imp_file]
.embigbox
.synote
.mnote imp_file
is a file specification for the name of the command file that can be
used to create the import library file using the &libname..
:CMT. If no file extension is specified, a file extension of "lbc" is
:CMT. assumed.
If no file extension is specified, no file extension is
assumed.
.esynote
.np
By default, no command file is generated.
.ix 'library file'
Specifying this option causes the linker to generate an import library
command file.
The import library command file contains a list of the entry points in
your DLL.
When this command file is processed by the &libname, an import library
file will be produced.
.np
If no file name is specified, the import library command file will
have a default file extension of "lbc" and the same file name as the
DLL file.
Note that the import library command file will be created in the same
directory as the DLL file.
The DLL file path and name can be specified in the "NAME" directive.
.np
Alternatively, a library command file path and name can be specified.
The following directive instructs the linker to generate a import
library command file and call it "mylib.lcf" regardless of the name of
the executable file.
.millust begin
option impfile=mylib.lcf
.millust end
.pc
You can also specify a path and/or file extension when using the
"IMPFILE=" form of the "IMPFILE" option.
