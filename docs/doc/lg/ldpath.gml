.*
.*
.dirctv PATH
.*
.np
The "PATH" directive is used to specify the directories that are to be
searched for object files appearing in subsequent "FILE" directives.
When the "PATH" directive is specified, the current directory will no
longer be searched unless it appears in the "PATH" directive.
The format of the "PATH" directive (short form "P") is as follows.
.mbigbox
    PATH path_name{&ps.path_name}
.embigbox
.synote
.mnote path_name
is a path name.
.esynote
.np
Consider a directive file containing the following linker directives.
.millust begin
path &pc.math
file sin
path &pc.stats
file mean, variance
.millust end
.np
It instructs the &lnkname to process the following object files:
.millust begin
&pc.math&pc.sin.&obj
&pc.stats&pc.mean.&obj
&pc.stats&pc.variance.&obj
.millust end
.np
It is also possible to specify a list of paths in a "PATH" directive.
Consider the following example.
.millust begin
path &pc.math&ps.&pc.stats
file sin
.millust end
.pc
First, the linker will attempt to load the file "&pc.math&pc.sin.&obj".
If unsuccessful, the linker will attempt to load the file
"&pc.stats&pc.sin.&obj".
.np
It is possible to override the path specified in a "PATH" directive
by preceding the object file name in a "FILE" directive with an absolute
path specification.
On UNIX platforms, an absolute path specification is one that
begins the "/" character.
On all other hosts, an absolute path specification is one that begins
with a drive specification or the "\" character.
.millust begin
path &pc.math
file sin
path &pc.stats
file mean, &pc.mydir&pc.variance
.millust end
.pc
The above directive file instructs the linker to process the
following object files:
.millust begin
&pc.math&pc.sin.&obj
&pc.stats&pc.mean.&obj
&pc.mydir&pc.variance.&obj
.millust end
