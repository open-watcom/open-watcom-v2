.*
.*
.dirctv LIBFILE
.*
.np
The "LIBFILE" directive is used to specify the object files that the
&lnkname is to process.
The format of the "LIBFILE" directive (short form "LIBF") is as
follows.
.mbigbox
    LIBFILE obj_spec{,obj_spec}

    obj_spec ::= obj_file | library_file
.embigbox
.synote
.im objfile
.mnote library_file
is a file specification for the name of a library file.
Note that the file extension of the library file (usually "lib") must
be specified; otherwise an object file will be assumed.
When a library file is specified, all object files in the library are
included (whether required or not).
.esynote
.np
The difference between the "LIBFILE" directive and the "FILE"
directive is as follows.
.autopoint
.point
When searching for an object or library file specified in a "LIBFILE"
directive, the current working directory will be searched first, followed by
the paths specified in the "LIBPATH" directive, and finally the paths
specified in the "LIB" environment variable.
Note that if the object or library file name contains a path, only the
specified path will be searched.
.point
Object or library file names specified in a "LIBFILE" directive will
not be used to create the name of the executable file when no "NAME"
directive is specified.
.endpoint
.np
Essentially, object files that appear in "LIBFILE" directives are
viewed as components of a library that have not been explicitly placed
in a library file.
.np
Consider the following linker directive file.
.millust begin
libpath &pc.libs
libfile mystart
path &pc.objs
file file1, file2
.millust end
.pc
The &lnkname is instructed to process the following object files:
.millust begin
&pc.libs&pc.mystart.&obj
&pc.objs&pc.file1.&obj
&pc.objs&pc.file2.&obj
.millust end
.np
Note that the executable file will have file name "file1" and not
"mystart".
