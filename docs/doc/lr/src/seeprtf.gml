.* printf FAMILY
.seelist _bprintf cprintf fprintf printf sprintf
.if '&machsys' eq 'QNX' .do begin
.seelist term_printf
.do end
.seelist _vbprintf vcprintf vfprintf vprintf vsprintf
