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
.dm chkwfunc begin
.sr *fx=&'strip(&*1,'L','_')
.sr iswidefn=1
.* far function
.if '&*fx(1:1)' eq 'w' and '&*fx(1:6)' ne 'wrapon' and '&*fx(1:5)' ne 'write' .me
.if '&*fx(1:2)' eq 'wc' or '&*fx(1:4)' eq 'wmem' .me
.if '&*fx(1:3)' eq 'tow' .me
.if '&*fx(1:3)' eq 'isw' .me
.if '&*fx(1:4)' eq 'wasc' .me
.if &'pos('wprintf',&*fx) ne 0 .me
.if &'pos('wscanf',&*fx) ne 0 .me
.if '&*fx(1:3)' ne 'fmb' and '&*fx(1:2)' ne 'mb' and '&'right(&*fx,2)' eq 'wc' .me
.if '&*fx(1:3)' ne 'fmb' and '&*fx(1:2)' ne 'mb' and '&'right(&*fx,2)' eq 'ws' .me
.if '&'right(&*fx,4)' eq 'itow' or '&'right(&*fx,4)' eq 'ltow' .me
.if '&'right(&*fx,5)' eq 'wchar' .me
.sr iswidefn=0
.dm chkwfunc end
.*
.dm addclinf begin
.sr *cltxt=''
.sr *clatr=0
.sr *cls=''
.if &*0 gt 1 .do begin
.   .se *cltxt=is &'substr(&*.,&'length(&*1)+1)
.do end
.chkwfunc &*1
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
.dm ftopsect begin
.cp 10
.if &e'&dohelp eq 0 .do begin
.   .sr *ban=&headtext$.
.   .if &syslc. gt 3 .do begin
.   .   .in
.   .   .tb 1 _/&syscl.
.   .   .tb set $
$$
.   .   .tb set
.   .   .tb
.   .do end
.   .sr headtext$=&*ban.
:H5.&*
.   .sr *attrs=''
.do end
.el .do begin
.   .funcref &'strip(&*1.,'T',',')
.   .sr *top='&'translate(&frefid.,`&trto.`,`&trfrom.`)'
:ZH2 ctx='&*top.'.&*
.   .pu 1 .ixsect &*
.   .pu 1 .ixsectid &*top.
.   .sr *attrs='*ctx='&*top.''
.do end
.sr headtxt0$=&*
.sr headtxt1$=&*
.sr SCTlvl=1
.cntents &*attrs. &*
.dm ftopsect end
.*
.dm funkw begin
.se *fnd=&'vecpos(&*1,fnclst)
.if &*fnd. eq 0 .do begin
.   .ty ***WARNING*** &*1 not defined in libfuns.gml
.   .me
.do end
.if &__sysl(&*fnd.) eq 0 .ty ***WARNING*** &*1 not defined in liblist7.gml
.if &'length(&fncttl.) eq 0 .do begin
.   .sr fncttl=&*1
.do end
.el .if &'length(&funcgrp.) eq 0 .do begin
.   .sr fncttl=&fncttl., &*1
.do end
.if &'length(&funcb.) eq 0 .sr funcb=&*1
.if &'length(&funcn.) eq 0 .sr funcn=&'strip(&*1,'L','_')
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
.chkwfunc &*1
.if &iswidefn ne 0 .do begin
.   .if &'length(&wfunc.) eq 0 .do begin
.   .   .sr wfunc=&*1
.   .do end
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
.if &'compare(&*1.,'end') ne 0 .do begin
.   .sr *cnt=&*0
.   .sr *i=1
.   .funcinit
.   .if &'compare(&*1.,'begin') eq 0 .do begin
.   .   .sr *i=&*i.+1
.   .   .sr *cnt=&*cnt.-1
.   .do end
.   .if &'max(&*cnt.,0) ne 0 .do begin
.   .   .sr *j=&*i.+1
.   .   .sr *first=&*&*i.
.   .   .sr *second=&*&*j.
.   .   .if &'compare(&*second.,'Functions') eq 0 .do begin
.   .   .   .sr fncttl=&*first. &*second.
.   .   .   .sr funcgrp=&'strip(&*first.,'T',',')
.   .   .   .if &'compare(&'right(&funcgrp,&'length(&grpsfx.)).,&grpsfx.) eq 0 .do begin
.   .   .   .   .sr groupfun=1
.   .   .   .   .addclinf &funcgrp
.   .   .   .do end
.   .   .do end
.   .   .el .do begin
.   .   .   .pe &*cnt;.funkw &*&*i.;.sr *i=&*i.+1
.   .   .do end
.   .do end
.do end
.if &'compare(&*1.,'begin') ne 0 .do begin
.*  generate title and start of code (declaration)
.   .if &'length(&funcgrp.) ne 0 .do begin
.   .   .sr functiong=&funcgrp.
.   .   .ixm '&funcgrp. Functions'
.   .do end
.   .el .sr functiong=&funcb.
.   .ftopsect &fncttl.
.   .sr *i=0
.   .pe &__fnx.;.sr *i=&*i.+1;.ixm '&$$fnc(&*i.)'
.do end
.dm func end
.*
.*
.* func2 create an index entry for &* also
.*
.dm func2 begin
:cmt .if &e'&dohelp ne 0 .xmpoff
.funkw &*
:cmt. .if &e'&dohelp ne 0 .xmpon
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
.if &vermacro ge 1070 .do begin
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
.if &e'&dohelp ne 0 .xmpoff
.ix '&*1' '&*2'
.if &e'&dohelp ne 0 .xmpon
.do end
.dm ixfunc2 end
.*
.dm desc begin
.if &'compare(&*.,'begin') eq 0 .newtext Description:
.el .if &'compare(&*.,'end') eq 0 .oldtext
.dm desc end
.*
.dm exmp begin
.if &'compare(&*.,'begin') eq 0 .do begin
.cp 8
.newcode Example:
.do end
.el .if &'compare(&*.,'end') eq 0 .endcode
.el .if &'compare(&*.,'break') eq 0 .brkcode
.el .if &'compare(&*.,'output') eq 0 .outcode
.dm exmp end
.*
.dm synop begin
.if &'compare(&*.,'begin') eq 0 .do begin
.   .cp 5
.   .newcode Synopsis:
.do end
.el .if &'compare(&*.,'end') eq 0 .do begin
.   .endcode
.do end
.dm synop end
.*
.dm seexmp begin
.newtext Example:
See example provided with
.mono &*
.period
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
.if &'compare(&XMPset.,'on') eq 0 .xmpoff
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
.if &'compare(&XMPset.,'on') eq 0 .xmpoff
produces the following:
.xmpon
.sk 1
.dm outcode end
.*
.dm return begin
.if &'compare(&*.,'begin') eq 0 .newtext Returns:
.el .if &'compare(&*.,'end') eq 0 .oldtext
.dm return end
.*
.dm error begin
.if &'compare(&*.,'begin') eq 0 .do begin
.newtext Errors:
When an error has occurred,
.kw errno
contains a value indicating the type of error that has been detected.
.do end
.el .if &'compare(&*.,'end') eq 0 .oldtext
.dm error end
.*
.dm begterm begin
.if &'length(&*1.) ne 0 .do begin
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
:ZDTHD3.&termhd1$.~b~b~b~b~b&*
.do end
.dm termhd2 end
.*
.dm term begin
.if &'compare(&*1.,'.arg') eq 0 .termhi &*2
.el .if &'compare(&*1.,'.kw') eq 0 .termhi &*2
.el .if &'compare(&*1.,'.mono') eq 0 .termhi &*2
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
.if &'length(&*.) ne 0 .do begin
.   .ct &*
.   .br
.do end
.dm clitm end
.*
.dm ansiname begin
.if &'length(&*.) eq 0 .me
.sr *i=0
...loopa .sr *i=&*i.+1
.   .if &'compare(&*.,&__clnam(&*i.).) eq 0 .do begin
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
.   .if &'length(&__cltxt(&*i.).) ne 0 .do begin
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
.if &'compare(&*1.,'end') ne 0 .do begin
.   .if &'compare(&*1.,'begin') eq 0 .do begin
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
.   .   .pe &__clx.-1;.sr *i=&*i.+1;.if &'length(&__cltxt(&*i.).) ne 0 or &__clatr(&*i.). ge 2 .sr *extr=1
.   .   .if &'compare(&__class.,'ISO') eq 0 or &'compare(&__class.,'POSIX') eq 0 .do begin
.   .   .   .sr *i=1
.   .   .   .pe &__clx.-1;.sr *i=&*i.+1;.if '&__clnam(&*i.,1:1).' eq '_' .sr *extr=1
.   .   .do end
.   .do end
.   .if &*extr eq 0 .do begin
.   .   .clitm &*all
.   .do end
.   .el .do begin
.   .   .clitm &*all
.   .   .if &'compare(&__class.,'ISO') eq 0 or &'compare(&__class.,'POSIX') eq 0 .do begin
.   .   .   .listclas 1
.   .   .do end
.   .   .el .do begin
.   .   .   .listclas 0
.   .   .do end
.   .do end
.do end
.if &'compare(&*1.,'begin') ne 0 .do begin
.   .listend
.do end
.dm class end
.*
.dm system begin
.if &'compare(&__class.,'WIN386') eq 0 .me
.if '&machsys' eq 'WIN32' .me
.se *stm=0
.se *flg=0
...loopsys
.se *stm=&*stm.+1
.sysstr &$$fnc(&*stm.)
.if &'length(&$$str.) ne 0 .do begin
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
.if &'compare(&*.,'begin') eq 0 .newtext See Also:
.el .if &'compare(&*.,'end') eq 0 .oldtext
.dm see end
.*
.dm ofuncref begin
.if &e'&dohelp eq 0 .do begin
.   .mono &frefid.
.do end
.el .do begin
:QREF top='&frefid.' str='&*'.
.do end
.dm ofuncref end
.*
.dm reffunc begin
.funcref &*1.
.if '&freffnd.' ne '0' .do begin
.   .ix '&*'
.   .ofuncref &*.
.do end
.dm reffunc end
.*
.dm seeref begin
(see the
.reffunc &*1.
&routine)
.dm seeref end
.*
.dm seekw begin
.if &'length(&*2.) ne 0 .do begin
.   .if &'compare(&*2.,'Functions') ne 0 .do begin
.   .   .if &'compare(&*1.,&*2.) ne 0 .do begin
.   .   .   .funcref &*2.
.   .   .   .if '&freffnd.' ne '0' .do begin
.   .   .   .   .if &seecnt. ne 0 .ct ,
.   .   .   .   .se seecnt=1
.   .   .   .   .ofuncref &*2.
.   .   .   .   .if &'compare(&*3.,'Functions') eq 0 .do begin
&*3
.   .   .   .   .do end
.   .   .   .do end
.   .   .do end
.   .do end
.do end
.dm seekw end
.*
.dm seelist begin
.se *i=1
.se *j=2
.pe &*0.
.   .seekw '&functiong.' &*&*i. &*&*j.;.se *i=&*i.+1;.se *j=&*i.+1
.dm seelist end
.*
.dm seeall begin
.newtext See also:
All functions starting with &mn.&*&emn.~...
.oldtext
.dm seeall end
.*
.dm blkcode begin
.if &'compare(&*.,'begin') eq 0 .do begin
.   .if &'compare(&XMPset.,'of') eq 0 .do begin
.   .   .xmpon
.   .   .sk 1 c
.   .do end
.do end
.el .if &'compare(&*.,'end') eq 0 .do begin
.   .if &'compare(&XMPset.,'on') ne 0 .er ***error*** blkcode misuse
.   .xmpoff
.do end
.dm blkcode end
.*
.dm blktext begin
.if &'compare(&*.,'begin') eq 0 .do begin
.   .if &'compare(&XMPset.,'on') eq 0 .do begin
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
.el .if &'compare(&*.,'end') eq 0 .do begin
.do end
.el .er ***error*** blktext misuse
.dm blktext end
.*
.dm emp begin
.if &'compare(&*.,'begin') eq 0 .empon
.el .if &'compare(&*.,'end') eq 0 .empoff
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
.dm period begin
.ct .li .
.dm period end
.*
:cmt. include 'Safer C Library' related macros
:INCLUDE file='safecmac'.
.*
.dm farfunc begin
.if &farfnc ne 0 .do begin
.pc
The
.id &*1.
function is a data model independent form of the
.id &*2.
function.
It accepts far pointer arguments and returns a far pointer.
It is most useful in mixed memory model applications.
.do end
.dm farfunc end
.*
.dm farfuncp begin
.if &farfnc ne 0 .do begin
.pc
The
.id &*1.
function is a data model independent form of the
.id &*2.
function that accepts far pointer arguments.
It is most useful in mixed memory model applications.
.do end
.dm farfuncp end
.*
.dm mbcsfunc begin
.pc
The
.id &*1.
function is a multi-byte character version of
.id &*2.
that operates with multi-byte character strings.
.dm mbcsfunc end
.*
.dm sbcsfunc begin
.pc
The
.id &*1.
function is a single-byte character version of
.id &*2.
that operates with single-byte character strings.
.dm sbcsfunc end
.*
.dm widefunc begin
.pc
The
.id &*1.
function is a wide character version of
.id &*2.
.if &'length(&*3.) eq 0 .do begin
that operates with wide character strings.
.do end
.el .if &'compare(&*3.,'<char>') eq 0 .do begin
that operates with wide character argument.
.do end
.el .if &'compare(&*3.,'<form>') eq 0 .do begin
.period
It accepts a wide character string argument for
.arg format
and produces wide character output.
.do end
.el .if &'compare(&*3.,'<ret>') eq 0 .do begin
except that it produces a wide character string.
.do end
.el .do begin
&*3..
.do end
.dm widefunc end
.*
.dm widegrp begin
.pc
The
.id &*1.&grpsfx.
functions are similar to their counterparts but operate on
wide character strings.
.dm widegrp end
.*
.dm tcshdr begin
The header file
.hdrfile tchar.h
defines the generic-text
.if &'length(&*2.) eq 0 .do begin
macro
.kw &*1.
.do end
.el .do begin
macros
.kw &*1.
and
.kw &*2.
.do end
.period
.dm tcshdr end
.*
.dm tcsbody begin
.br
The
.id &*1.
macro maps to
.id &*2.
if
.kw _MBCS
has been defined, or to the
.id &*4.
macro if
.kw _UNICODE
has been defined, otherwise it maps to
.id &*3.
macro.
.dm tcsbody end
.*
.dm tcsfoot begin
.br
The
.id &*1.
and
.id &*2.
macros are provided only for this mapping and
should not be used otherwise.
.dm tcsfoot end
.*
.dm tcsfunc begin
.pc
.tcshdr &*1.
.tcsbody &*1. &*2. &*3. &*4.
.br
.id &*3.
and
.id &*4.
are single-byte character string and wide character
string versions of
.id &*2.
.period
.tcsfoot &*3. &*4.
.dm tcsfunc end
.*
.dm deprec begin
** deprecated **
.dm deprec end
.*
.dm deprfunc begin
The
.id &*1.
function is deprecated, use
.reffunc &*2.
instead.
.dm deprfunc end
.*
