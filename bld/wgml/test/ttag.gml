:cmt. test for usertags
:GDOC.
:BODY.
.dm br /.ty br/
.ty &xxx
.se xxx = yyy
.*if a = b .do begin
.sr $$msgcntr=-1
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
.sr $$msgnumb=/&'right('&$$msgcntr.',3,'0')/
.dm msgsym end
.gt msgsym add msgsym att
.*
:cmt. :MSGTXT. internal compiler error
.dm msgtxt begin
.cw $
.se *ttl="&'strip(&*)"
.if !&*ttl.! ne !filler! .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .   .keep
.   .   .note &$$msgnumb
.   .   .us &*
.   .   .np
.   .do end
.   .el .do begin
:ZH2.&$$msgnumb. &*ttl.
.   .   .cntents &$$msgnumb. &*ttl.
.   .   .pu 1 .ixsect &$$msgnumb. &*ttl.
:ZI1.&*ttl.
.   .   .pu 1 .ixline `&*ttl.`
.   .   .sr *ctxn=&$$msgcntr.+1
.   .   .if '&dotarget' eq 'os2' .do begin
.   .   .   .se *ctxn=/20&'right('&$$msgcntr.',3,'0')/
.   .   .do end
.   .   .ctxstr &$$msgnumb. &*ttl.
.   .   .pu 2 #define HLP_&'upper(&ctx_str.) &*ctxn
.   .   .np
.   .do end
.do end
.cw ;
.dm msgtxt end
.gt msgtxt add msgtxt att
:msglvl.w1
:msgsym.errrr
:msgtxt.F„hler hier

.*
.gt * print

.cw 23;.ty willi#.cw ;
.cw #
.pa #.wi lli;#.cw ;
.*im whelp
:set symbol='trfrom1'   value=" -+*/\%<>[](){}=#~@$,.&|':?!".
.gt willi add @willi attributes cont textr
.ga * leer val '' def
.ga * abc val "noch ein"
.*ga * abc range  1 2 3
.ga * magic auto "eeeeeeõ"
.ga * place upper value float default
.*ga * muss req
.*ga * muss2 req
.*ga * xyz range 1 2 3 4
.*gt n234567890123456 add xyz
.gt * print
.dm @willi begin
.br
.ty Tag: &*_tag call: &*_n Text: &*
.br
.ty &*magic
.br
.ty &*place
.br
.ty &*abc
.br
.ty '&*leer'
.dm @willi end

 .* :set symbol="xxx" value="yyy" | delete
.gt see add @set attr texte
.ga * symbol req len 10
.* ga * * len 10
.ga * value req any

.gt see print
.gt see cha bset
.gt see print

.dm @set begin
.if ›&*value› = ›delete› .do
.   .se &*symbol off
.do end
.el .do
.   .se &*symbol=›&*value›
.do end
.ty >&&*symbol..<
.dm @set end

.dm bset begin
.if ›&*value› = ›delete› .se &*symbol off
.el .se &*symbol=›&*value›
.ty >&&*symbol..<
.dm bset end

:see symbol='trfrom1'   value=" -+*/\%<>[](){}=#~@$,.&|':?!".
:see symbol="seesym"
     value="acd".
.br
&seesym.
.br
:see symbol="seesym" value=delete.
&seesym.
.br
:cmt willi.
:willi abc="noch ein" abc="noch ein" place=float place=FLOAT.tagtext
.do end
:EGDOC.
