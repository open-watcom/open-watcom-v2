.* printf FAMILY
.seelist _bprintf cprintf fprintf printf sprintf
.if '&machsys' eq 'QNX' .do begin
.seelist term_printf
.do end
.if "&'substr(&functiong,1,1)" eq "v" .do begin
.seelist va_arg va_end va_start
.do end
.if "&'substr(&functiong,1,2)" eq "_v" .do begin
.seelist va_arg va_end va_start
.do end
.seelist _vbprintf vcprintf vfprintf vprintf vsprintf
