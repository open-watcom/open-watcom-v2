.* printf FAMILY
.seelist &*1 _bprintf cprintf fprintf printf sprintf term_printf
.if "&'substr(&*1,1,1)" eq "v" .do begin
.seelist &*1 va_arg va_end va_start
.do end
.if "&'substr(&*1,1,2)" eq "_v" .do begin
.seelist &*1 va_arg va_end va_start
.do end
.seelist &*1 _vbprintf vcprintf vfprintf vprintf vsprintf
