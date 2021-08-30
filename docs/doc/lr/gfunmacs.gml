.*
.* fd... macros are used for function definitions
.*
.dm fdbeg begin
The following &routines are defined:
.if &e'&dohelp eq 0 .do begin
:DL compact termhi=1 tsize='20' break.
.do end
.el .do begin
:ZDL compact termhi=1 tsize='20' break.
.do end
.dm fdbeg end
.*
.dm fd begin
.funcref &*1.
.if &e'&dohelp eq 0 .do begin
:DT.&*
:DD.
.do end
.el .do begin
.   .if '&freffnd.' eq '0' .do begin
:ZDT.&*
.   .do end
.   .el .do begin
:ZDT.:QREF top='&frefid.' str='&*.'.
.   .do end
:ZDD.
.do end
.dm fd end
.*
.dm fdend begin
.if &e'&dohelp eq 0 .do begin
:eDL.
.do end
.el .do begin
:ZeDL.
.do end
.dm fdend end
