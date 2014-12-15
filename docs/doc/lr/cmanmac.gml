:set symbol="sysrb" value="~b".
:set symbol="XMPset" value="of".
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
.sr _func64=''
.sr wfunc64=''
.sr mathfunc=''
.sr fmathfunc=''
.sr lmathfunc=''
.*
.sr fncttl=''
.sr __fnx=0
.sr __cltxt=''
.sr funcgrp=''
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
.dm funkw begin
.se *fnd=&'vecpos(&*,fnclst)
.if &*fnd. eq 0 .do begin
.   .ty ***WARNING*** &* not defined in libfuns.gml
.   .me
.do end
.if &__sysl(&*fnd.) eq 0 .ty ***WARNING*** &* not in library
.if |&fncttl.| eq || .do begin
.   .sr fncttl=&*
.do end
.el .if '&funcgrp.' eq '' .do begin
.   .sr fncttl=&fncttl., &*
.do end
.if '&funcn' eq '' .sr funcn=&'strip(&*,'L','_')
.se __fnx=&__fnx.+1
.se $$fnc(&__fnx.)=&*
.* try to classify type of function
.if &'pos('_&funcn',&*) eq 1 .do begin
.   .if "&'right(&*,3)" eq "i64" .do begin
.   .   .sr _func64=&*
.   .do end
.   .el .do begin
.   .   .sr _func=&*
.   .do end
.do end
.el .if '&*' eq '__&funcn' .do begin
.   .sr __func=&*
.do end
.el .if '&*' eq '_f&funcn' .do begin
.   .sr ffunc=&*
.do end
.el .if &'pos('_w',&*) eq 1 .do begin
.   .if &'pos('_wrapon',&*) ne 1 .do begin
.   .   .if "&'right(&*,3)" eq "i64" .do begin
.   .   .   .sr wfunc64=&*
.   .   .do end
.   .   .el .do begin
.   .   .   .sr wfunc=&*
.   .   .do end
.   .do end
.do end
.el .if &'pos('wc',&*) eq 1 .do begin
.   .sr wfunc=&*
.do end
.el .if &'pos('wmem',&*) eq 1 .do begin
.   .sr wfunc=&*
.do end
.el .if &'pos('wasc',&*) eq 1 .do begin
.   .sr wfunc=&*
.do end
.el .if &'pos('_mb',&*) eq 1 .do begin
.   .sr mfunc=&*
.do end
.el .if &'pos('_fmb',&*) eq 1 .do begin
.   .sr fmfunc=&*
.do end
.dm funkw end
.*
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
.   .   .sr funcb=&'strip(&*first.,'T',',')
.   .   .if '&*second' eq 'Functions' .do begin
.   .   .   .sr fncttl=&*first. &*second.
.   .   .   .sr funcgrp=&'strip(&*first.,'T',',')
.   .   .do end
.   .   .el .do begin
.   .   .   .pe &*cnt
.   .   .   .   .funkw &*&*i.;.sr *i=&*i.+1
.   .   .do end
.   .do end
.do end
.if |&*1| ne |begin| .do begin
.*  generate title and start of code (declaration)
.   .topsect &fncttl.
.   .if '&funcgrp.' ne '' .do begin
.   .   .ixm '&funcgrp.'
.   .do end
.   .sr *i=1
.   .pe &__fnx.
.   .   .if '&funcgrp.' ne '&$$fnc(&*i.)' .ixm '&$$fnc(&*i.)';.sr *i=&*i.+1
.   .sr function=&funcb.
.do end
.dm func end
.*
.*
.* func2 create an index entry for &* also
.*
.dm func2 begin
:cmt .if &e'&dohelp eq 1 .xmpoff
.funkw &*1
:cmt. .if &e'&dohelp eq 1 .xmpon
.dm func2 end
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
.   .if &clitmc eq 0 .do begin
.   .   .ct &*
.   .   .sr clitmc=1
.   .do end
.   .el .do begin
.   .   .br .ct &*
:cmt. .   .   .ct , &*
.   .do end
.do end
.dm clitm end
.*
.dm ansiname begin
.if '&*' ne '' .do begin
.br &*. conforms to ANSI/ISO naming conventions
.do end
.dm ansiname end
.*
.dm class begin
.sr *extr=0
.if |&*1| ne |end| .do begin
.   .if |&*1| eq |begin| .sr __class=&*2
.   .el .sr __class=&*1
.   .listnew Classification:
.   .if &'length(&_func.) ne 0 .do begin
.   .   .if '&_func.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&_func64.) ne 0 .do begin
.   .   .if '&_func64.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&__func.) ne 0 .do begin
.   .   .if '&__func.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&ffunc.) ne 0 .do begin
.   .   .if '&ffunc.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&wfunc.) ne 0 .do begin
.   .   .if '&wfunc.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&wfunc64.) ne 0 .do begin
.   .   .if '&wfunc64.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&mfunc.) ne 0 .do begin
.   .   .if '&mfunc.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if &'length(&fmfunc.) ne 0 .do begin
.   .   .if '&fmfunc.' ne '&funcb.' :set symbol="*extr" value=1.
.   .do end
.   .if |&*1| eq |begin| .sr *all=&'strip(&'substr(&*,6),'L',' ')
.   .el .sr *all=&*
.   .if &*extr eq 0 .do begin
&*all
.   .do end
.   .el .if |&*all| eq |WATCOM| .do begin
&*all
.   .do end
.   .el .if &'words(|&*all|) gt 6 .do begin
&*all
.   .do end
.   .el .do begin
.   .   .sr clitmc=0
.   .   .clitm &function. is &*all
.   .   .if |&*1| eq |begin| .sr *first=&*2
.   .   .el .sr *first=&*1
.   .   .if |&*first| eq |ISO| OR |&*first| eq |TR| .do begin
.   .   .   .sr *cls=&'strip(&*all,'T',',')
.   .   .   .if |&*all| eq |ISO C90| .do begin
.   .   .   .   .sr *wcls='ISO C95'
.   .   .   .do end
.   .   .   .el .do begin
.   .   .   .   .sr *wcls=&'strip(&*all,'T',',')
.   .   .   .do end
.   .   .do end
.   .   .el .do begin
.   .   .   .sr *cls=&'strip(&*first,'T',',')
.   .   .   .sr *wcls=&'strip(&*first,'T',',')
.   .   .do end
.   .   .if &'length(&_func.) ne 0 .do begin
.   .   .   .clitm &_func. is not &*cls
.   .   .do end
.   .   .if &'length(&_func64.) ne 0 .do begin
.   .   .   .clitm &_func64. is not &*cls
.   .   .do end
.   .   .if &'length(&__func.) ne 0 .do begin
.   .   .   .clitm &__func. is not &*cls
.   .   .do end
.   .   .if &'length(&ffunc.) ne 0 .do begin
.   .   .   .clitm &ffunc. is not &*cls
.   .   .do end
.   .   .if &'length(&wfunc.) ne 0 .do begin
.   .   .   .if "&'substr(&wfunc.,1,1)" eq "_" .do begin
.   .   .   .   .clitm &wfunc. is not &*cls
.   .   .   .do end
.   .   .   .el .do begin
.   .   .   .   .if '&wfunc.' ne '&funcb.' .do begin
.   .   .   .   .   .if '&wfunc.' ne '&function.' .do begin
.   .   .   .   .   .   .clitm &wfunc. is &*wcls
.   .   .   .   .   .do end
.   .   .   .   .do end
.   .   .   .do end
.   .   .do end
.   .   .if &'length(&wfunc64.) ne 0 .do begin
.   .   .   .clitm &wfunc64. is not &*cls
.   .   .do end
.   .   .if &'length(&mfunc.) ne 0 .do begin
.   .   .   .clitm &mfunc. is not &*cls
.   .   .do end
.   .   .if &'length(&fmfunc.) ne 0 .do begin
.   .   .   .clitm &fmfunc. is not &*cls
.   .   .do end
.   .do end
.do end
.if |&*1| ne |begin| .do begin
.   .listend
.   .sr clitmc=0
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
.*       describe functions for c library  2006-03-16
.*  new version with explicit type and classification
.*  if omitted, default classification is WATCOM
.*   .func begin
.*   .funct     norm              classification
.*   .funct_    _norm                  "
.*   .funct_f   farnorm                "
.*   .funct_m   multibytenorm          "
.*   .funct_fm  farmultibytenorm       "
.*   .funct_w   widenorm               "
.*   .funct_fw  farwidewnorm           "
.*
.*   .functm    doublemathfunc         "
.*   .functm_f  floatmathfunc          "
.*   .functm_l  longdoublemathfunc     "
.*   .func end
.*
.*    description
.*
.*   .classt
.*
.*
.*  .func norm Functions
.*  .func norm
.*  .func norm _norm __norm _wnorm _fnorm _mbsnorm
.*
.*  functii internal macro for funct_xxx
.*
.dm functii begin
.if '&funcb' eq '' .sr funcb=&*1.
.if '&funcn' eq '' .sr funcn=&'strip(&*1,'L','_')
.se *fnd=&'vecpos(&*1,fnclst)
.if &*fnd. eq 0 .me
.if &__sysl(&*fnd.) eq 0 .ty ***WARNING*** &* not in library
.if |&fncttl.| eq || .do begin
.   .sr fncttl=&*1
.do end
.el .do begin
.   .sr fncttl=&fncttl., &*1
.do end
.se __fnx=&__fnx.+1
.se $$fnc(&__fnx.)=&*1
.if |&*2| eq || .se __cl=WATCOM
.el .se __cl=&*2. &*3.
.se __cltxt(&__fnx.)=&*1 is &__cl.
.dm functii end
.*
.*  macros for the different function types
.*
.dm funct  begin
.sr funcb=&*1
.functii &*
.dm funct  end
.*
.dm funct_  begin
.sr _func=&*1
.functii &*
.dm funct_  end
.*
.dm funct__  begin
.sr __func=&*1
.functii &*
.dm funct__  end
.*
.dm funct_f begin
.sr ffunc=&*1
.functii &*
.dm funct_f end
.*
.dm funct_m begin
.sr mfunc=&*1
.functii &*
.dm funct_m end
.*
.dm funct_w begin
.sr wfunc=&*1
.functii &*
.dm funct_w end
.*
.dm funct_fm begin
.sr fmfunc=&*1
.functii &*
.dm funct_fm end
.*
.dm funct_fw begin
.sr fwfunc=&*1
.functii &*
.dm funct_fw end
.*
.dm functm begin
.sr mathfunc=&*1
.functii &*
.dm functm end
.*
.dm functm_f begin
.sr fmathfunc=&*1
.functii &*
.dm functm_f end
.*
.dm functm_l begin
.sr lmathfunc=&*1
.functii &*
.dm functm_l end
.*
.*  classt
.*    output classifications for the defined functions
.*
.dm classt begin
.if |&*1| ne |end| .do begin
.   .if |&*1| eq |begin| .sr __class=&*2
.   .el .sr __class=&*1
.   .listnew Classification:
.   .sr clitmc=0
.   .sr *i=1
.   .pe &__fnx.
.   .   .clitm &__cltxt(&*i).;.sr *i=&*i+1;
.do end
.if |&*1| ne |begin| .do begin
.   .listend
.   .sr clitmc=0
.do end
.dm classt end
.*
:cmt. include 'Safer C Library' related macros
:INCLUDE file='safecmac'.
