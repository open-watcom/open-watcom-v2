.dm errhead begin
.if &e'&dohelp eq 0 .do begin
.   .begnote
.   :DTHD.&*
.   :DDHD.~b
.do end
.dm errhead end
.*
.dm errnote begin
.if &e'&dohelp eq 0 .do begin
.   .note &*
.   .se *ctxn=&'substr(&*,1,&'pos(' ',&*)-1)
.   .ix '&*ctxn'
.do end
.el .do begin
.   .se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
:ZH2.&*
.   .pu 1 .ixsect &*
:ZI1.&*ttl$
.   .pu 1 .ixline `&*ttl$`
.   .se *ctxc=&'substr(&*,1,2)
.   .if '&*ctxc' ne '&ctxcls$' .do begin
.   .   .se ctxcls$=&*ctxc
.   .   .se ctxcln$=&ctxcln$+100
.   .   .if &ctxcln$ gt 4200 .er ***ERROR*** Extra class numbers - See David Wellstood
.   .do end
.   .se *ctxn=&'substr(&*,4,2)+&ctxcln$
.   .if '&dotarget' eq 'os2' .do begin
.   .   .se *ctxn=&*ctxn+20000
.   .do end
.   .ctxstr &*
.   .pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.   .np
.   .us &*
.do end
.dm errnote end
.*
.dm errtail begin
.if &e'&dohelp eq 0 .do begin
.   .endnote
.do end
.dm errtail end
.*
.* initialization
.* close the header file
.*
.if &e'&dohelp eq 1 .do begin
.   .se ctxcls$='##'
.   .se ctxcln$=0
.   .pu 2
.do end
.*
