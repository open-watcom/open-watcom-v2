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
.autonote Notes:
.note
.if '&opsys' eq 'QNX' .do begin
If the file
.id /etc/wlink.hlp
exists,
.do end
.el .do begin
If the file "wlink.hlp" is located in one of the paths specified in the
"PATH" environment variable,
.do end
the contents of that file will be displayed when the following command
is issued.
.millust begin
&sysprompt.&lnkcmd ?
.millust end
.note
If all of the directive information does not fit on the command line,
type the following.
.millust begin
&sysprompt.&lnkcmd
.millust end
.pc
The prompt "WLINK>" will appear on the next line.
You can enter as many lines of directive information as required.
.if '&target' eq 'QNX' .do begin
Press "Ctrl/D" to terminate the input of directive information.
.do end
.el .do begin
Press "Ctrl/Z" followed by the "Enter" key to terminate the input of
directive information if you are running a DOS, ZDOS, OS/2 or Windows
NT-hosted version of the &lnkname..
Press "Ctrl/D" to terminate the input of directive information if you
are running a UNIX-hosted version of the &lnkname..
.do end
.endnote
