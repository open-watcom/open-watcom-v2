.dm topsect begin
.cp 10
.ctxstr &'strip(&*1,'T',',')
.sr *tag=&*
.if '&ctx_str.' ne '&*tag.' .do begin
.   .sr *tag=&ctx_str.
.do end
:ZH2 ctx='&*tag.'.&*
.pu 1 .ixsect &*
.pu 1 .ixsectid &*tag.
.se headtxt0$=&*
.se headtxt1$=&*
.se SCTlvl=1
.cntents *ctx='&*tag.' &*
.dm topsect end
