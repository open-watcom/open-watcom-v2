.* SCAN FAMILY (including variable args)
.seelist &*1 cscanf fscanf scanf sscanf
.if "&'substr(&*1,1,1)" eq "v" .do begin
.seelist &*1 va_arg va_end va_start
.do end
.seelist &*1 vcscanf vfscanf vscanf vsscanf
