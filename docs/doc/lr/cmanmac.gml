:set symbol="sysrb" value="~b".
:set symbol="XMPset" value="of".
:set symbol="grpsfx" value="...".
.*
.dm funcinit begin
.sr funcb=''
.sr funcn=''
.sr _func=''
.sr __func=''
.sr ffunc=''
.sr wfunc=''
.sr fwfunc=''
.sr mfunc=''
.sr fmfunc=''
.sr func64=''
.sr wfunc64=''
.sr mathfunc=''
.sr fmathfunc=''
.sr lmathfunc=''
.*
.sr fncttl=''
.sr __fnx=0
.sr __cltxt=''
.sr __clnam=''
.sr __clatr=0
.sr __clx=0
.sr funcgrp=''
.sr groupfun=0
.sr iswidefn=0
.dm funcinit end
.*
.* Define these here otherwise &_func. expands to &_func. and not ''
.*
.funcinit
.*
.dm prelim begin
.dm prelim end
.*
.dm dos begin
.if '&machsys' eq 'DOS' .do begin
.im &*
.do end
.dm dos end
.*
.dm qnx begin
.if '&machsys' eq 'QNX' .do begin
.im &*
.do end
.dm qnx end
.*
.dm debug begin
.* .ty &*
.dm debug end
.*
.dm widefunc begin
.sr iswidefn=1
.if '&*1(1:2)' eq '_w' and '&*1(1:7)' ne '_wrapon' and '&*1(1:6)' ne '_write' .me
.if '&*1(1:2)' eq 'wc' or '&*1(1:4)' eq 'wmem' .me
.if '&*1(1:3)' eq 'tow' .me
.if '&*1(1:3)' eq 'isw' .me
.if '&*1(1:4)' eq 'wasc' .me
.if '&*1(1:5)' eq '__isw' .me
.if &'pos('wprintf',&*1) ne 0 .me
.if &'pos('wscanf',&*1) ne 0 .me
.if '&'right(&*1,2)' eq 'wc' or '&'right(&*1,2)' eq 'ws' .me
.if '&'right(&*1,3)' eq 'tow' .me
.if '&'right(&*1,5)' eq 'wchar' .me
.sr iswidefn=0
.dm widefunc end
.*
.dm addclinf begin
.sr *cltxt=''
.sr *clatr=0
.sr *cls=''
.if &*0 gt 1 .do begin
.   .se *cltxt=is &'substr(&*.,&'length(&*1)+1)
.do end
.widefunc &*1
.if &iswidefn. ne 0 .do begin
.   .if '&*1(1:1).' ne '_' .sr *clatr=1
.do end
.se __clx=&__clx.+1
.se __clnam(&__clx.)=&*1
.se __clatr(&__clx.)=&*clatr
.se __cltxt(&__clx.)=&*cltxt
.dm addclinf end
.*
.dm addsyinf begin
.se __fnx=&__fnx.+1
.se $$fnc(&__fnx.)=&*1
.dm addsyinf end
.*
.dm funkw begin
.se *fnd=&'vecpos(&*1,fnclst)
.if &*fnd. eq 0 .do begin
.   .ty ***WARNING*** &*1 not defined in libfuns.gml
.   .me
.do end
.if &__sysl(&*fnd.) eq 0 .ty ***WARNING*** &*1 not defined in liblist7.gml
.if |&fncttl.| eq || .do begin
.   .sr fncttl=&*1
.do end
.el .if '&funcgrp.' eq '' .do begin
.   .sr fncttl=&fncttl., &*1
.do end
.if '&funcb' eq '' .sr funcb=&*1
.if '&funcn' eq '' .sr funcn=&'strip(&*1,'L','_')
.addsyinfo &*1
.* try to classify type of function
.if &'pos('i64',&*1) ne 0 .do begin
.   .if '&*1(1:2).' eq '_w' .do begin
.   .   .sr wfunc64=&*1
.   .do end
.   .el .do begin
.   .   .sr func64=&*1
.   .do end
.do end
.widefunc &*1
.if &iswidefn ne 0 .do begin
.   .sr wfunc=&*1
.do end
.el .if '&*1' eq '_&funcn' .do begin
.   .sr _func=&*1
.do end
.el .if '&*1' eq '__&funcn' .do begin
.   .sr __func=&*1
.do end
.el .if '&*1' eq '_f&funcn' .do begin
.   .sr ffunc=&*1
.do end
.el .if '&*1(1:3).' eq '_mb' .do begin
.   .sr mfunc=&*1
.do end
.el .if '&*1(1:4).' eq '_fmb' .do begin
.   .sr fmfunc=&*1
.do end
.if &groupfun. eq 0 .do begin
.   .addclinf &*
.do end
.dm funkw end
.*
.*
.*  describe functions for c library
.*  type is derived from function name
.*  classification is supported by .func2 macro
.*  if omitted, default classification is WATCOM
.*
.*  .func begin
.*  .func2     norm  classification
.*  .func end
.*
.*  .func norm Functions
.*  .func norm
.*  .func norm _norm __norm _wnorm _fnorm _mbsnorm
.*
.dm func begin
.if |&*1| ne |end| .do begin
.   .sr *cnt=&*0
.   .sr *i=1
.   .funcinit
.   .if |&*1| eq |begin| .do begin
.   .   .sr *i=&*i.+1
.   .   .sr *cnt=&*cnt.-1
.   .do end
.   .if &'&*cnt ne 0 .do begin
.   .   .sr *j=&*i.+1
.   .   .sr *first=&*&*i.
.   .   .sr *second=&*&*j.
.   .   .if '&*second' eq 'Functions' .do begin
.   .   .   .sr fncttl=&*first. &*second.
.   .   .   .sr funcgrp=&'strip(&*first.,'T',',')
.   .   .   .if |&'right(&funcgrp,&'length(&grpsfx.))| eq |&grpsfx.| .do begin
.   .   .   .   .sr groupfun=1
.   .   .   .   .addclinf &funcgrp
.   .   .   .do end
.   .   .do end
.   .   .el .do begin
.   .   .   .pe &*cnt;.funkw &*&*i.;.sr *i=&*i.+1
.   .   .do end
.   .do end
.do end
.if |&*1| ne |begin| .do begin
.*  generate title and start of code (declaration)
.   .topsect &fncttl.
.   .if '&funcgrp.' ne '' .do begin
.   .   .sr function=&funcgrp.
.   .   .ixm '&funcgrp. Functions'
.   .do end
.   .el .sr function=&funcb.
.   .sr *i=0
.   .pe &__fnx.;.sr *i=&*i.+1;.ixm '&$$fnc(&*i.)'
.do end
.dm func end
.*
.*
.* func2 create an index entry for &* also
.*
.dm func2 begin
:cmt .if &e'&dohelp eq 1 .xmpoff
.funkw &*
:cmt. .if &e'&dohelp eq 1 .xmpon
.dm func2 end
.*
.dm fungroup begin
.if &groupfun. ne 0 .do begin
.   .addclinf &*
.do end
.dm fungroup end
.*
.* The funcw macro explicitly declares the "wide" name of a function
.*
.dm funcw begin
.if &version ge 107 .do begin
.  .sr wfunc=&*
.do end
.dm funcw end
.*
.dm func_ begin
.  .sr _func=&*
.dm func_ end
.*
.dm ixfunc begin
.if &'vecpos(&*1,fnclst) ne 0 .do begin
.ix '&*1'
.do end
.dm ixfunc end
.*
.dm ixfunc2 begin
.if &'vecpos(&*2,fnclst) ne 0 .do begin
.if &e'&dohelp eq 1 .xmpoff
.ix '&*1' '&*2'
.if &e'&dohelp eq 1 .xmpon
.do end
.dm ixfunc2 end
.*
.dm desc begin
.if '&*' eq 'begin' .newtext Description:
.el .if '&*' eq 'end' .oldtext
.dm desc end
.*
.dm exmp begin
.if '&*' eq 'begin' .do begin
.cp 8
.newcode Example:
.do end
.el .if '&*' eq 'end' .endcode
.el .if '&*' eq 'break' .brkcode
.el .if '&*' eq 'output' .outcode
.dm exmp end
.*
.dm synop begin
.if '&*' eq 'begin' .do begin
.   .cp 5
.   .newcode Synopsis:
.do end
.el .if '&*' eq 'end' .do begin
.   .endcode
.do end
.dm synop end
.*
.dm seexmp begin
.newtext Example:
See example provided with
.mono &*..
.oldtext
.dm seexmp end
.*
.dm newcode begin
.listnew &*
.xmpon
.sk -1
.sr XMPlin=&sysfnum.
.dm newcode end
.*
.dm endcode begin
.if '&XMPset' eq 'on' .xmpoff
.listend
.sr XMPlin=0
.dm endcode end
.*
.dm brkcode begin
.sk 1
.dm brkcode end
.*
.dm outcode begin
.sk 1
.if '&XMPset' eq 'on' .xmpoff
produces the following:
.xmpon
.sk 1
.dm outcode end
.*
.dm return begin
.if '&*' eq 'begin' .newtext Returns:
.el .if '&*' eq 'end' .oldtext
.dm return end
.*
.dm error begin
.if '&*' eq 'begin' .do begin
.newtext Errors:
When an error has occurred,
.kw errno
contains a value indicating the type of error that has been detected.
.do end
.el .if '&*' eq 'end' .oldtext
.dm error end
.*
.dm begterm begin
.if '&*1' ne '' .do begin
.begnote $setptnt &*.
.do end
.el .do begin
.begnote $setptnt 15
.do end
.dm begterm end
.*
.dm termhd1 begin
.if &e'&dohelp eq 0 .do begin
:DTHD.&*
.do end
.el .do begin
:cmt.   :ZDT.:SF font=2.&*:eSF.
.sr termhd1$='&*'
.do end
.dm termhd1 end
.*
.dm termhd2 begin
.if &e'&dohelp eq 0 .do begin
:DDHD.&*
.do end
.el .do begin
:cmt.   :ZDD.:SF font=2.&*:eSF.
:ZDT.&termhd1$.~b~b~b~b~b&*
.do end
.dm termhd2 end
.*
.dm term begin
.if '&*1' eq '.arg' .termhi &*2
.el .if '&*1' eq '.kw' .termhi &*2
.el .if '&*1' eq '.mono' .termhi &*2
.el .termhi &*
.dm term end
.*
.dm termhi begin
.note &*
.ix '&*'
.dm termhi end
.*
.dm termnx begin
.note &*
.dm termnx end
.*
.dm endterm begin
.endnote
.dm endterm end
.*
:cmt. .dm beglist begin
:cmt. :XMP.:SF font=0.~b
:cmt. .dm beglist end
:cmt. .*
:cmt. .dm endlist begin
:cmt. :eSF.:eXMP.
:cmt. .dm endlist end
.*
.dm filename begin
.mono &*
.dm filename end
.*
.dm hdrfile begin
.mono <&*>
.dm hdrfile end
.*
.dm proto begin
Prototype in
.mono <&*>.
.br
.dm proto end
.*
.dm id begin
:SF font=4.&*:eSF.
.dm id end
.*
.dm kw begin
.ix '&*'
:SF font=4.&*:eSF.
.dm kw end
.*
.dm arg begin
:SF font=1.&*:eSF.
.dm arg end
.*
.dm idbold begin
:SF font=2.&*:eSF.
.dm idbold end
.*
.dm clitm begin
.if |&*| ne || .do begin
.   .ct &*
.   .br
.do end
.dm clitm end
.*
.dm ansiname begin
.if '&*' eq '' .me
.sr *i=0
...loopa .sr *i=&*i.+1
.   .if '&*' eq '&__clnam(&*i.)' .do begin
.   .   .if &__clatr(&*i.) lt 2 .sr __clatr(&*i.)=&__clatr(&*i.).+2
.   .   .me
.   .do end
.if &*i. lt &__clx. .go loopa
.dm ansiname end
.*
.dm listclas begin
.sr *i=1
...loopc .se *i=&*i.+1
.   .sr *clatr=&__clatr(&*i.).
.   .if '&__cltxt(&*i.)' ne '' .do begin
.   .   .if &clatr ge 2 .do begin
.   .   .   .clitm &__clnam(&*i) &__cltxt(&*i.), conforms to ANSI naming conventions
.   .   .do end
.   .   .el .do begin
.   .   .   .clitm &__clnam(&*i) &__cltxt(&*i.)
.   .   .do end
.   .do end
.   .el .if &*clatr ge 2 .do begin
.   .   .clitm &__clnam(&*i) conforms to ANSI naming conventions
.   .do end
.   .el .if &*1 ne 0 .do begin
.   .   .if &*clatr eq 0 .do begin
.   .   .   .clitm &__clnam(&*i) is WATCOM
.   .   .do end
.   .do end
.if &*i. lt &__clx. .go loopc
.dm listclas end
.*
.dm class begin
.sr *extr=0
.if |&*1| ne |end| .do begin
.   .if |&*1| eq |begin| .do begin
.   .   .sr __class=&*2
.   .   .sr *all=&'strip(&'substr(&*,6),'L',' ')
.   .do end
.   .el .do begin
.   .   .sr __class=&*1
.   .   .sr *all=&*
.   .do end
.   .listnew Classification:
.   .if &__clx. gt 1 and '&all' ne '&grfun' .do begin
.   .   .sr *i=1
.   .   .pe &__clx.-1;.sr *i=&*i.+1;.if '&__cltxt(&*i.).' ne '' or &__clatr(&*i.). ge 2 .sr *extr=1
.   .   .if '&__class.' eq 'ISO' or '&__class.' eq 'POSIX' .do begin
.   .   .   .sr *i=1
.   .   .   .pe &__clx.-1;.sr *i=&*i.+1;.if '&__clnam(&*i.,1:1).' eq '_' .sr *extr=1
.   .   .do end
.   .do end
.   .if &*extr eq 0 .do begin
.   .   .clitm &*all
.   .do end
.   .el .do begin
.   .   .clitm &*all
.   .   .if '&__class.' eq 'ISO' or '&__class.' eq 'POSIX' .do begin
.   .   .   .listclas 1
.   .   .do end
.   .   .el .do begin
.   .   .   .listclas 0
.   .   .do end
.   .do end
.do end
.if |&*1| ne |begin| .do begin
.   .listend
.do end
.dm class end
.*
.dm system begin
.if '&__class' eq 'WIN386' .me
.if '&machsys' eq 'WIN32' .me
.se *stm=0
.se *flg=0
...loopsys
.se *stm=&*stm.+1
.sysstr &$$fnc(&*stm.)
.if '&$$str' ne '' .do begin
.   .if &*flg. eq 0 .do begin
.   .   .se *flg=1
.   .   .listnew Systems:
.   .do end
.   .if &__fnx. gt 1 .do begin
.   .   .mono &$$fnc(&*stm.) - &$$str
.   .   .br
.   .do end
.   .el .do begin
.   .    &$$str
.   .do end
.do end
.if &*stm. lt &__fnx. .go loopsys
.if &*flg. ne 0 .do begin
.   .listend
.do end
.dm system end
.*
.dm see begin
.se seecnt=0
.if '&*' eq 'begin' .newtext See Also:
.el .if '&*' eq 'end' .oldtext
.dm see end
.*
.dm seelist begin
.se *i=1
.se *j=2
.pe &*0.
.   .seekw '&function.' &*&*i. &*&*j.;.se *i=&*i.+1;.se *j=&*i.+1
.dm seelist end
.*
.dm seekw begin
.if '&*2' ne '' .do begin
.   .if '&*2' ne 'Functions' .do begin
.   .   .if '&*1' ne '&*2' .do begin
.   .   .   .if &'vecpos(&*2,fnclst) ne 0 .do begin
.   .   .   .   .if &seecnt. ne 0 .ct,
.   .   .   .   .se seecnt=1
.   .   .   .   .if &e'&dohelp eq 0 .do begin
.   .   .   .   .   .mono &*2
.   .   .   .   .do end
.   .   .   .   .el .do begin
:QREF str='&*2'.
.   .   .   .   .do end
.   .   .   .   .if '&*3' eq 'Functions' .do begin
&*3
.   .   .   .   .do end
.   .   .   .do end
.   .   .do end
.   .do end
.do end
.dm seekw end
.*
.dm seeall begin
.newtext See also:
All functions starting with &mn.&*&emn.~...
.oldtext
.dm seeall end
.*
.dm blkcode begin
.if '&*' eq 'begin' .do begin
.   .if '&XMPset' eq 'of' .do begin
.   .   .xmpon
.   .   .sk 1 c
.   .do end
.do end
.el .if '&*' eq 'end' .do begin
.   .if '&XMPset' ne 'on' .er ***error*** blkcode misuse
.   .xmpoff
.do end
.dm blkcode end
.*
.dm blktext begin
.if '&*' eq 'begin' .do begin
.   .if '&XMPset' eq 'on' .do begin
.   .   .xmpoff
.   .do end
.   .se BLKlin=&sysfnum.-1
.   .if &BLKlin. eq &XMPlin.   .do begin
.   .   .sk -1
.   .do end
.   .el .do begin
.   .   .sk 1 c
.   .do end
.   .co on
.do end
.el .if '&*' eq 'end' .do begin
.do end
.el .er ***error*** blktext misuse
.dm blktext end
.*
.dm emp begin
.if '&*' eq 'begin' .empon
.el .if '&*' eq 'end' .empoff
.dm emp end
.*
.dm empon begin
:P.:SF font=2.
.dm empon end
.*
.dm empoff begin
:eSF.:P.
.dm empoff end
.*
.dm xmpon begin
:set symbol="XMPset" value="on".:SF font=4.
.co off
.dm xmpon end
.*
.dm xmpoff begin
.co on
:eSF.:set symbol="XMPset" value="of".
.dm xmpoff end
.*
.dm listnew begin
.in 0
.if &e'&dohelp eq 0 .do begin
:DL &NTEphi..:DT.&*:DD.
.do end
.el .do begin
:ZDL &NTEphi..:ZDT.&*:ZDD.
.do end
.dm listnew end
.*
.dm listend begin
.if &e'&dohelp eq 0 .do begin
:eDL.
.do end
.el .do begin
:ZeDL.
.do end
.in &INDlvl
.dm listend end
.*
.dm newtext begin
.cp 3
.listnew &*
.dm newtext end
.*
.dm oldtext begin
.listend
.dm oldtext end
.*
.dm doscmd begin
DOS
.mono &*
command
.ix 'DOS command' '&*'
.dm doscmd end
.*
.dm qnxcmd begin
QNX
.mono &*
command
.ix 'QNX command' '&*'
.dm qnxcmd end
.*
:cmt. include 'Safer C Library' related macros
:INCLUDE file='safecmac'.
