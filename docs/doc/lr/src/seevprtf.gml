.* printf FAMILY
.seelist _bprintf cprintf fprintf printf sprintf term_printf
.if "&'substr(&function,1,1)" eq "v" .do begin
.seelist va_arg va_end va_start
.do end
.if "&'substr(&function,1,2)" eq "_v" .do begin
.seelist va_arg va_end va_start
.do end
.seelist _vbprintf vcprintf vfprintf vprintf vsprintf
