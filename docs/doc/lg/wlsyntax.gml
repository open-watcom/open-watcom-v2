.*
.np
.ix '&lnkcmdup' 'command line format'
.ix '&lnkcmdup command line' 'invoking &lnkcmdup'
.ix 'command line format' '&lnkcmdup'
.ix 'invoking &lnkname'
Input to the &lnkname is specified on the command line and can be
redirected to one or more files or environment strings.
The &lnkname command line format is as follows.
.mbigbox
.if '&opsys' eq 'QNX' .do begin
&lnkcmd {directive}
.do end
.el .do begin
&lnkcmdup {directive}
.do end
.embigbox
