.*
.*
.dirctv LIBPATH
.*
.np
The "LIBPATH" directive is used to specify the directories that are to
be searched for library files appearing in subsequent "LIBRARY"
directives and object files appearing in subsequent "LIBFILE"
directives.
The format of the "LIBPATH" directive (short form "LIBP") is as
follows.
.mbigbox
    LIBPATH [path_name{&ps.path_name}]
.embigbox
.synote
.mnote path_name
is a path name.
.esynote
.np
Consider a directive file containing the following linker directives.
.millust begin
file test
libpath &pc.math
library trig
libfile newsin
.millust end
.np
First, the &lnkname will process the object file "test.&obj" from the
current working directory.
The object file "newsin.&obj" will then be processed, searching the
current working directory first.
If "newsin.&obj" is not in the current working directory, the "&pc.math"
directory will be searched.
If any unresolved references remain after processing the object files,
the library file "trig.lib" will be searched.
If the file "trig.lib" does not exist in the current working directory, the
"&pc.math" directory will be searched.
.np
It is also possible to specify a list of paths in a "LIBPATH"
directive.
Consider the following example.
.millust begin
libpath &pc.newmath&ps.&pc.math
library trig
.millust end
.pc
When processing undefined references, the &lnkname will attempt to
process the library file "trig.lib" in the current working directory.
If "trig.lib" does not exist in the current working directory, the
"&pc.newmath" directory will be searched.
If "trig.lib" does not exist in the "&pc.newmath" directory, the
"&pc.math" directory will be searched.
.np
If the name of a library file appearing in a "LIBRARY" directive or
the name of an object file appearing in a "LIBFILE" directive contains
a path specification, only the specified path will be searched.
.np
Note that
.millust begin
libpath path1
libpath path2
.millust end
.pc
is equivalent to the following.
.millust begin
libpath path2&ps.path1
.millust end
