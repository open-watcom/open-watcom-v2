.*
.*
.dirctv OPTLIB
.*
.np
The "OPTLIB" directive is used to specify the library files to be
searched when unresolved symbols remain after processing all specified
input object files.
The format of the "OPTLIB" directive (no short form) is as follows.
.mbigbox
    OPTLIB library_file{,library_file}
.embigbox
.synote
.mnote library_file
is a file specification for the name of a library file.
If no file extension is specified, a file extension of "lib" is
assumed.
.esynote
.np
This directive is similar to the "LIBRARY" directive except that the
linker will not issue a warning message if the library file cannot be
found.
.np
Consider the following example.
.exam begin
&sysprompt.&lnkcmd &syst_drctv file trig optlib &pc.math&pc.trig, &pc.cmplx&pc.trig
.exam end
.pc
The &lnkname is instructed to process the following object file:
.millust begin
trig.&obj
.millust end
.np
If any unresolved symbol references remain after all object files have
been processed, the following library files will be searched:
.millust begin
&pc.math&pc.trig.lib
&pc.cmplx&pc.trig.lib
.millust end
.np
More than one "OPTLIB" directive may be used.
The following example is equivalent to the preceding one.
.exam begin
&sysprompt.&lnkcmd &syst_drctv f trig optlib &pc.math&pc.trig optlib &pc.cmplx&pc.trig
.exam end
.pc
Thus other directives may be placed between lists of library files.
.*
.beglevel
.*
.section Searching for Optional Libraries Specified in Environment Variables
.*
.np
.ix 'LIB environment variable'
.ix 'environment variables' 'LIB'
The "LIB" environment variable can be used to specify a list of paths
that will be searched for library files.
The "LIB" environment variable can be set using the "&setcmd" command
as follows:
.millust begin
&sysprompt.&setcmd &libvarname=&pc.graphics&pc.lib&ps.&pc.utility
.millust end
.pc
Consider the following "OPTLIB" directive and the above definition of
the "LIB" environment variable.
.millust begin
optlib &pc.mylibs&pc.util, graph
.millust end
.pc
If undefined symbols remain after processing all object files
specified in all "FILE" directives, the &lnkname will resolve these
references by searching the following libraries in the specified order.
.autopoint
.point
the library file "&pc.mylibs&pc.util.lib"
.point
the library file "graph.lib" in the current directory
.point
the library file "&pc.graphics&pc.lib&pc.graph.lib"
.point
the library file "&pc.utility&pc.graph.lib"
.endpoint
.autonote Notes:
.note
If a library file specified in a "OPTLIB" directive contains an
absolute path specification, the &lnkname will not search any of the
paths specified in the "LIB" environment string for the library file.
On UNIX platforms, an absolute path specification is one that
begins the "/" character.
On all other hosts, an absolute path specification is one that begins
with a drive specification or the "\" character.
.note
Once a library file has been found, no further elements of the "LIB"
environment variable are searched for other libraries of the same
name.
That is, if the library file "&pc.graphics&pc.lib&pc.graph.lib"
exists, the library file "&pc.utility&pc.graph.lib" will not be
searched even though unresolved references may remain.
.endnote
.*
.endlevel
