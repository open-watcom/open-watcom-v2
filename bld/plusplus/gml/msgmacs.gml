.sr $$msgcntr=-1
.sr $$msgpfx=''
.*
:cmt. :MSGGRP. Warn1
.dm msggrp begin
.sr $$msgcntr=-1
.sr $$msgpfx=''
.dm msggrp end
.gt msggrp add msggrp att
.*
:cmt. :eMSGGRP. Warn1
.dm emsggrp begin
.if &e'&dohelp eq 0 .do begin
.   .endnote
.do end
.sr $$msgcntr=-1
.sr $$msgpfx=''
.dm emsggrp end
.gt emsggrp add emsggrp att
.*
:cmt. :MSGGRPSTR. W
.dm msggrpstr begin
.sr $$msgpfx=&*
.dm msggrpstr end
.gt msggrpstr add msggrpstr att
.*
:cmt. :MSGGRPNUM. 100
.dm msggrpnum begin
.sr $$msgcntr=&*
.sr $$msgcntr=&$$msgcntr.-1
.dm msggrpnum end
.gt msggrpnum add msggrpnum att
.*
:cmt. :MSGGRPTXT. Warning Level 1 Messages
.dm msggrptxt begin
.if &e'&dohelp eq 0 .do begin
.   .section &*
.   .begnote
.do end
.dm msggrptxt end
.gt msggrptxt add msggrptxt att
.*
:cmt. :MSGLVL W1
.dm msglvl begin
:cmt. .section &*
:cmt. .begnote
.dm msglvl end
.gt msglvl add msglvl att
.*
:cmt. :EMSGLVL.
.dm emsglvl begin
:cmt. .endnote
.dm emsglvl end
.gt emsglvl add emsglvl att
.*
:cmt. :MSGSYM. ERR_CALL_WATCOM
.dm msgsym begin
.sr $$msgcntr=&$$msgcntr.+1
.sr $$msgnumb=&$$msgcntr.
.if &$$msgcntr. lt 100 .do begin
.   .sr $$msgnumb=/&'right('&$$msgcntr.',3,'0')/
.do end
.dm msgsym end
.gt msgsym add msgsym att
.*
:cmt. :MSGTXT. internal compiler error
.dm msgtxt begin
.cw $
.se *ttl="&'strip(&*)"
.if !&*ttl.! ne !filler! .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .	.keep
.   .	.note &$$msgpfx.&$$msgnumb
.   .	.us &*
.   .	.np
.   .do end
.   .el .do begin
:ZH2.&$$msgpfx.&$$msgnumb. &*ttl.
.   .	.cntents &$$msgpfx.&$$msgnumb. &*ttl.
.   .	.pu 1 .ixsect &$$msgpfx.&$$msgnumb. &*ttl.
:ZI1.&*ttl.
.   .	.pu 1 .ixline `&*ttl.`
.   .	.sr *ctxn=&$$msgcntr.+1
.   .	.if '&dotarget' eq 'os2' .do begin
.   .	.   .sr *ctxn=&$$msgcntr.+20000
.   .	.do end
.   .	.ctxstr &$$msgpfx.&$$msgnumb. &*ttl.
.   .	.pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.   .	.np
.   .do end
.do end
.cw ;
.dm msgtxt end
.gt msgtxt add msgtxt att
.*
:cmt. :MSGJTXT. internal compiler error
.dm msgjtxt begin
.dm msgjtxt end
.gt msgjtxt add msgjtxt att
.*
:cmt. :ANSI
.dm ansi begin
.dm ansi end
.gt ansi add ansi att
:cmt. :ANSIERR
.dm ansierr begin
.dm ansierr end
.gt ansierr add ansierr att
:cmt. :ANSIWARN
.dm ansiwarn begin
.dm ansiwarn end
.gt ansiwarn add ansiwarn att
:cmt. :INFO
.dm info begin
.dm info end
.gt info add info att
:cmt. :WARNING
.dm warning begin
.dm warning end
.gt warning add warning att
.*
:cmt. :errgood.
.dm errgood begin
.exam begin
.dm errgood end
.gt errgood add errgood att
.*
:cmt. :eerrgood.
.dm eerrgood begin
.exam end
.dm eerrgood end
.gt eerrgood add eerrgood att
.*
:cmt. :errbad.
.dm errbad begin
.exam begin
.dm errbad end
.gt errbad add errbad att
.*
:cmt. :eerrbad.
.dm eerrbad begin
.exam end
.dm eerrbad end
.gt eerrbad add eerrbad att
.*
:cmt. :errbreak.
.dm errbreak begin
.exam break
.dm errbreak end
.gt errbreak add errbreak att
.*
.* close the header file
.*
.pu 2
