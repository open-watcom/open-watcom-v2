.*
.*
.section The &lnkname Memory Requirements
.*
.np
.ix 'memory requirements'
The &lnkname uses all available memory when linking an application.
.if '&target' ne 'QNX' .do begin
For DOS-hosted versions of the &lnkname, this includes expanded memory
(EMS) and extended memory.
.do end
It is possible for the size of the image being linked to exceed the
amount of memory available in your machine, particularly if the image
file is to contain debugging information.
For this reason, a temporary disk file is used when all available
memory is used by the &lnkname..
.np
Normally, the temporary file is created in the current working directory.
.ix '&tmpvarname. environment variable'
.ix 'environment variables' '&tmpvarname.'
However, by defining the "&tmpvarname." environment variable to be a
directory, you can tell the &lnkname where to create the temporary
file.
This can be particularly useful if you have a RAM disk.
Consider the following definition of the "&tmpvarname." environment
variable.
.millust begin
&sysprompt.&setcmd &tmpvarname=&pc.tmp
.millust end
.pc
The &lnkname will create the temporary file in the directory
"&pc.tmp".
