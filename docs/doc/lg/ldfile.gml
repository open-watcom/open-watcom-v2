.*
.*
.dirctv FILE
.*
.np
The "FILE" directive is used to specify the object files and library
modules that the &lnkname is to process.
The format of the "FILE" directive (short form "F") is as follows.
.mbigbox
    FILE obj_spec{,obj_spec}

    obj_spec ::= obj_file[(obj_module)]
                       | library_file[(obj_module)]
.embigbox
.synote
.im objfile
.mnote library_file
is a file specification for the name of a library file.
Note that the file extension of the library file (usually "lib") must
be specified; otherwise an object file will be assumed.
When a library file is specified, all object files in the library are
included (whether required or not).
.mnote obj_module
is the name of an object module defined in an object or library file.
.esynote
.np
Consider the following example.
.exam begin
&sysprompt.&lnkcmd &syst_drctv f &pc.math&pc.sin, mycos
.exam end
.pc
The &lnkname is instructed to process the following object files:
.millust begin
&pc.math&pc.sin.&obj
mycos.&obj
.millust end
.pc
The object file "mycos.&obj" is located in the current directory since
no path was specified.
.np
More than one "FILE" directive may be used.
The following example is equivalent to the preceding one.
.exam begin
&sysprompt.&lnkcmd &syst_drctv f &pc.math&pc.sin f mycos
.exam end
.pc
Thus, other directives may be placed between lists of object files.
.np
The "FILE" directive can also specify object modules from a library file
or object file.
Consider the following example.
.exam begin
&sysprompt.&lnkcmd &syst_drctv f &pc.math&pc.math.lib(sin)
.exam end
.pc
The &lnkname is instructed to process the object module "sin" contained in
the library file "math.lib" in the directory "&pc.math".
.np
In the following example, the &lnkname will process the object module
"sin" contained in the object file "math.&obj" in the directory "&pc.math".
.exam begin
&sysprompt.&lnkcmd &syst_drctv f &pc.math&pc.math(sin)
.exam end
.np
In the following example, the &lnkname will include all object modules
contained in the library file "math.lib" in the directory "&pc.math".
.exam begin
&sysprompt.&lnkcmd &syst_drctv f &pc.math&pc.math.lib
.exam end
