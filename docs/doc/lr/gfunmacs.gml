.*
.* fd... macros are used for function definitions
.*
.dm fdbeg begin
The following &routines are defined:
.if &e'&dohelp eq 0 .do begin
:DL compact termhi=1 tsize=20 break.
.do end
.el .do begin
:ZDL compact termhi=1 tsize=20 break.
.do end
.dm fdbeg end
.*
.dm fd begin
.se *fnd=&'vecpos(&*.,fnclst)
.if &*fnd. ne 0 .do begin
.   .se *lib=&'vecpos(&imblst(&*fnd.).,imblst)
.   .se *ent=&fnclst(&*lib.)
.   .if &e'&dohelp eq 0 .do begin
:DT.&*
:DD.
.   .do end
.   .el .do begin
.   .   .if '&*' ne '&*ent.' .do begin
.   .   .   .sr *text1='&* (see '
.   .   .   .sr *text2=&*ent.
.   .   .   .sr *text3=')'
.   .   .do end
.   .   .el .do begin
.   .   .   .sr *text1=''
.   .   .   .sr *text2=&*
.   .   .   .sr *text3=''
.   .   .do end
:ZDT.&*text1.:QREF str='&*text2.'.&*text3.
:ZDD.
.   .do end
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
