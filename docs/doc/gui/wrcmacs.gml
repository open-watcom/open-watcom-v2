.dm errsect begin
.if &e'&dohelp eq 0 .do begin
.   .begnote
.do end
.dm errsect end
.*
.dm eerrsect begin
.if &e'&dohelp eq 0 .do begin
.   .endnote
.do end
.dm eerrsect end
.*
.dm errnote begin
.if &e'&dohelp eq 0 .do begin
.   .se *ctxn=&'substr(&*,1,&'pos(' ',&*)-1)
.   .se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
.   .note &*ctxn
.   .us &*ttl$
.do end
.el .do begin
.   .se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
:ZH2.&*
.   .cntents &*
.   .pu 1 .ixsect &*
:ZI1.&*ttl$
.   .pu 1 .ixline `&*ttl$`
.   .se *ctxn=&*1
.   .ctxstr &*
.   .pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.*  .np
.*  .us &*
.do end
.dm errnote end
.*
.* close the header file
.*
.pu 2
