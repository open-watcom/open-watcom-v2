.*
.*
.np
You can view all the directives specific to &opsys executable files
by simply typing the following:
.if '&exeformat' = 'win nt' .do begin
.millust begin
&sysprompt.&lnkcmd ? nt
.millust end
.do end
.el .do begin
.millust begin
&sysprompt.&lnkcmd ? &exeformat
.millust end
.do end
.pc
