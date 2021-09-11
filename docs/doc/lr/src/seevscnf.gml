.* SCAN FAMILY (including variable args)
.seelist cscanf fscanf scanf sscanf
.if "&'substr(&functiong,1,1)" eq "v" .do begin
.seelist va_arg va_end va_start
.do end
.seelist vcscanf vfscanf vscanf vsscanf
.seelist fwscanf wscanf swscanf
.seelist vfwscanf vwscanf vswscanf
