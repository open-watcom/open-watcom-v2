.*
.* Open Watcom Documentation Macros & Variables
.*
.pu 1
.pu 3
:set symbol="cnt_ttl"   value=" ".
:set symbol="pubdate"   value="July, 1997".
:set symbol="headtxt0$" value=" ".
:set symbol="headtxt1$" value=" ".
:set symbol="headtext$" value=" ".
:set symbol="WDWlvl"    value="10".
:set symbol="SCTlvl"    value="1".
:set symbol="INDillust" value="2".
:set symbol="NTEpr2"    value=" ".
:set symbol="NTEpr1"    value=" ".
:set symbol="NTEset"    value=" ".
:set symbol="NTEnhi"    value="termhi=3".
:set symbol="NTEphi"    value="termhi=2".
:set symbol="ANTset"    value=" ".
:set symbol="APTset"    value="compact".
.if &e'&dohelp eq 0 .do begin
:set symbol="INDxmp"    value="0.3i".
.do end
.el .do begin
:set symbol="INDxmp"    value="0.5i".
.do end
.if '&format' eq '6x8' .do begin
:set symbol="INDlvl"    value="8".
.do end
.el .if '&format' eq '8.5x11a' .do begin
:set symbol="INDlvl"    value="5".
.do end
.el .if '&format' eq 'help' .do begin
:set symbol="INDlvl"    value="0".
.do end
.el .do begin
:set symbol="INDlvl"    value="7".
.do end
.*
:CMT. .dm im begin
:CMT. .ty ***Imbedding &*
:CMT. ..im &*
:CMT. .dm im end
.*
.dm ix begin
.se ixmajor=''
.ix1 &*
.dm ix end
.*
.dm ixm begin
.se ixmajor='pg=major'
.ix1 &*
.dm ixm end
.*
.dm ix1 begin
.pu 1 .ixline &*
.* The next 2 lines correct problems with ".ix NAME="
.se *iwrd=&*
.if '&*1' ne '' .do begin
.se *iwrd=&*1
.do end
.se *iwrd="&'strip('&*iwrd.','T',',')"
.if '&*iwrd.' ne '.' .do begin
.se *iwrd="&'strip('&*iwrd.','T','.')"
.do end
.se *iwrd2='&*iwrd.'
.if '&'left(&*iwrd2.,1)' eq '_' .do begin
.se *iwrd2=&'right(&*iwrd2.,&'length(&*iwrd2.)-1)_
.do end
.if '&'left(&*iwrd2.,1)' eq '_' .do begin
.se *iwrd2=&'right(&*iwrd2.,&'length(&*iwrd2.)-1)_
.do end
.if &e'&dohelp eq 0 .do begin
:IH1 print='&*iwrd.'.&*iwrd2.
.   .if '&*2' eq '' .do begin
.   .   .:I1 &ixmajor..&*iwrd2.
.   .do end
.   .el .do begin
.   .   .ix2 &*2
.   .do end
.   .if '&*3' ne '' .do begin
.   .   .:I3.&*3
.   .do end
.do end
.el .do begin
.   .if '&*3' ne '' .do begin
.   .   .sr *ixstr="&*iwrd., &*2, &*3"
.   .do end
.   .el .if '&*2' ne '' .do begin
.   .   .sr *ixstr="&*iwrd., &*2"
.   .do end
.   .el .do begin
.   .   .sr *ixstr="&*iwrd."
.   .do end
:ZI1.&*ixstr.
.do end
.dm ix1 end
.*
.dm ix2 begin
.se *iw='&*.'
.if '&*iw.' ne ',' .do begin
.se *iw="&'strip('&*iw.','T',',')"
.do end
.if '&*iw.' ne '.' .do begin
.se *iw="&'strip('&*iw.','T','.')"
.do end
.se *iw2='&*iw.'
.if '&'left(&*iw2.,1)' eq '_' .do begin
.se *iw2=&'right(&*iw2.,&'length(&*iw2.)-1)_
.do end
.if '&'left(&*iw2.,1)' eq '_' .do begin
.se *iw2=&'right(&*iw2.,&'length(&*iw2.)-1)_
.do end
:IH2 print='&*iw.'.&*iw2.:I2 &ixmajor..&*iw2.
.dm ix2 end
.*
.dm fn begin
.if '&*' eq 'begin' .do begin
:FN.
.do end
.el .if '&*' eq 'end' .do begin
:eFN.
.do end
.dm fn end
.*
:cmt. .dm us begin
:cmt. :SF font=1.&*:eSF.
:cmt. .dm us end
:cmt. .*
:cmt. .dm bd begin
:cmt. :SF font=2.&*:eSF.
:cmt. .dm bd end
:cmt. .*
:cmt. .dm bi begin
:cmt. :SF font=3.&*:eSF.
:cmt. .dm bi end
:cmt. .*
.dm pp begin
:P.
.dm pp end
.*
.dm np begin
:P.
.dm np end
.*
.dm pc begin
:PC.
.dm pc end
.*
.dm illust begin
.if '&*' eq 'begin' .do begin
:XMP.:SF font=0.~b
.do end
.el .if '&*' eq 'end' .do begin
:eSF.:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
:eSF.:eXMP.:XMP.:SF font=0.
.do end
.el .do begin
:XMP.:SF font=0.~b
&*
:eSF.:eXMP.:P.
.do end
.dm illust end
.*
.dm millust begin
.if '&*' eq 'begin' .do begin
:XMP.~b
.do end
.el .if '&*' eq 'end' .do begin
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
:eXMP.:XMP.
.do end
.el .do begin
:XMP.~b
&*
:eXMP.:P.
.do end
.dm millust end
.*
.dm code begin
.if '&*' eq 'begin' .do begin
:XMP.:SF font=5.~b
.do end
.el .if '&*' eq 'end' .do begin
:eSF.:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
:eSF.:eXMP.:XMP.:SF font=5.
.do end
.el .do begin
:XMP.:SF font=5.~b
&*
:eSF.:eXMP.:P.
.do end
.dm code end
.*
.dm listing begin
.if '&*' eq 'begin' .do begin
.se *lvl=&INDlvl-3
.in &*lvl.
:XMP.:SF font=5.~b
.do end
.el .if '&*' eq 'end' .do begin
:eSF.:eXMP.
.in &INDlvl
.do end
.el .if '&*' eq 'break' .do begin
:eSF.:eXMP. :XMP.:SF font=5.
.do end
.el .do begin
:XMP.:SF font=5.~b
&*
:eSF.:eXMP.:P.
.do end
.dm listing end
.*
.dm codeon begin
:SF font=5.
.dm codeon end
.*
.dm codeoff begin
:eSF.
.dm codeoff end
.*
.dm mono begin
:SF font=4.&*:eSF.
.dm mono end
.*
.dm monoon begin
:SF font=4.
.dm monoon end
.*
.dm monooff begin
:eSF.
.dm monooff end
.*
.dm smonoon begin
:SF font=5.
.dm smonoon end
.*
.dm smonooff begin
:eSF.
.dm smonooff end
.*
.dm autonote begin
.if '&*' ne '' .do begin
.   .sr tmplvl=&WDWlvl-3
.   .cp &tmplvl
.   :P.:HP1.&*:eHP1.
.do end
.if &e'&dohelp eq 0 .do begin
:OL &ANTset .
.do end
.el .do begin
:ZOL &ANTset .
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="oln".
.dm autonote end
.*
.dm autopoint begin
.if '&*' ne '' .do begin
.   .sr tmplvl=&WDWlvl-3
.   .cp &tmplvl
.   :P.:HP1.&*:eHP1.
.do end
.if &e'&dohelp eq 0 .do begin
:OL &APTset .
.do end
.el .do begin
:ZOL &APTset .
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="olp".
.dm autopoint end
.*
.dm begnote begin
.sr tmplvl=&WDWlvl-3
.cp &tmplvl
:set symbol="NTEop1" value="".
:set symbol="NTEop2" value="".
:set symbol="NTEop3" value="".
.if '&*1' eq '$break' or '&*2' eq '$break' or '&*3' eq '$break' .do begin
:set symbol="NTEop1" value="break".
.do end
.if '&*1' eq '$compact' or '&*2' eq '$compact' or '&*3' eq '$compact' .do begin
:set symbol="NTEop2" value="compact".
.do end
.if '&*1' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*2".
.do end
.if '&*2' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*3".
.do end
.if '&*3' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*4".
.do end
.if &e'&dohelp eq 0 .do begin
:DL &NTEnhi. &NTEop1. &NTEop2. &NTEop3..
.do end
.el .do begin
:ZDL &NTEnhi. &NTEop1. &NTEop2. &NTEop3..
.do end
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
.if &e'&dohelp eq 0 .do begin
:DTHD.&*
:DDHD.~b
.do end
.el .do begin
:ZDT.:SF font=2.&*:eSF.
:ZDD.~b
.do end
.do end
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dln".
.dm begnote end
.*
.dm begpoint begin
.sr tmplvl=&WDWlvl-3
.cp &tmplvl
:set symbol="NTEop1" value="".
:set symbol="NTEop2" value="".
:set symbol="NTEop3" value="".
.if '&*1' eq '$break' or '&*2' eq '$break' or '&*3' eq '$break' .do begin
:set symbol="NTEop1" value="break".
.do end
.if '&*1' eq '$compact' or '&*2' eq '$compact' or '&*3' eq '$compact' .do begin
:set symbol="NTEop2" value="compact".
.do end
.if '&*1' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*2".
.do end
.if '&*2' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*3".
.do end
.if '&*3' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize=&*4".
.do end
.if &e'&dohelp eq 0 .do begin
:DL &NTEphi. &NTEop1. &NTEop2. &NTEop3..
.do end
.el .do begin
:ZDL &NTEphi. &NTEop1. &NTEop2. &NTEop3..
.do end
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
.if &e'&dohelp eq 0 .do begin
:DTHD.&*
:DDHD.~b
.do end
.el .do begin
:ZDT.:SF font=2.&*:eSF.
:ZDD.~b
.do end
.do end
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dlp".
.dm begpoint end
.*
.dm setptnt begin
.dm setptnt end
.*
.dm notehd1 begin
.if &e'&dohelp eq 0 .do begin
:DTHD.&*
.do end
.el .do begin
:ZDT.:SF font=2.&*:eSF.
.do end
.dm notehd1 end
.*
.dm notehd2 begin
.if &e'&dohelp eq 0 .do begin
:DDHD.&*
.do end
.el .do begin
:ZDD.:SF font=2.&*:eSF.
.do end
.dm notehd2 end
.*
.dm endnote begin
.if '&NTEset' eq 'olp' .do begin
.   .ty ***ERROR*** endnote with autopoint &sysfile.(&sysfnum)
.   .endOL
.do end
.el .if '&NTEset' eq 'dlp' .do begin
.   .ty ***ERROR*** endnote with begpoint &sysfile.(&sysfnum)
.   .endDL
.do end
.el .if '&NTEset' eq 'oln' .endOL
.el .if '&NTEset' eq 'dln' .endDL
.el .ty ***ERROR*** endnote without autonote/begnote &sysfile.(&sysfnum)
:set symbol="NTEset" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEpr2".
:set symbol="NTEpr2" value=" ".
.dm endnote end
.*
.*
.dm endpoint begin
.if '&NTEset' eq 'oln' .do begin
.   .ty ***ERROR*** endpoint with autonote &sysfile.(&sysfnum)
.   .endOL
.do end
.el .if '&NTEset' eq 'dln' .do begin
.   .ty ***ERROR*** endpoint with begnote &sysfile.(&sysfnum)
.   .endDL
.do end
.el .if '&NTEset' eq 'olp' .endOL
.el .if '&NTEset' eq 'dlp' .endDL
.el .ty ***ERROR*** endpoint without autopoint/begpoint &sysfile.(&sysfnum)
:set symbol="NTEset" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEpr2".
:set symbol="NTEpr2" value=" ".
.dm endpoint end
.*
.dm endOL begin
.if &e'&dohelp eq 0 .do begin
:eOL.
.do end
.el .do begin
:ZeOL.
.do end
.dm endOL end
.*
.dm endDL begin
.if &e'&dohelp eq 0 .do begin
:eDL.
.do end
.el .do begin
:ZeDL.
.do end
.dm endDL end
.*
.dm note begin
.if '&NTEset' eq 'olp' .do begin
.   .ty ***ERROR*** note with autopoint &sysfile.(&sysfnum)
.   .noteOL
.do end
.el .if '&NTEset' eq 'dlp' .do begin
.   .ty ***ERROR*** note with begpoint &sysfile.(&sysfnum)
.   .noteDL &*
.do end
.el .if '&NTEset' eq 'oln' .noteOL &*
.el .if '&NTEset' eq 'dln' .noteDL &*
.el .ty ***ERROR*** note without autonote/begnote &sysfile.(&sysfnum)
.dm note end
.*
.dm point begin
.if '&NTEset' eq 'oln' .do begin
.   .ty ***ERROR*** point with autonote &sysfile.(&sysfnum)
.   .noteOL
.do end
.el .if '&NTEset' eq 'dln' .do begin
.   .ty ***ERROR*** point with begnote &sysfile.(&sysfnum)
.   .noteDL &*
.do end
.el .if '&NTEset' eq 'olp' .noteOL &*
.el .if '&NTEset' eq 'dlp' .noteDL &*
.el .ty ***ERROR*** point without autopoint/begpoint &sysfile.(&sysfnum)
.dm point end
.*
.dm noteOL begin
.if '&*' ne '' .ty ***ERROR*** argument specified with autonote/autopoint list item &sysfile.(&sysfnum)
.if &e'&dohelp eq 0 .do begin
:LI.
.do end
.el .do begin
:ZLI.
.do end
.dm noteOL end
.*
.dm noteDL begin
.sr tmplvl=3
.cp &tmplvl
.if '&*1' eq '.mono' .ty ***ERROR*** .mono with .point/.note &sysfile.(&sysfnum)
.if &e'&dohelp eq 0 .do begin
:DT.&*
:DD.
.do end
.el .do begin
:ZDT.&*
:ZDD.
.do end
.dm noteDL end
.*
.dm begbull begin
.sr tmplvl=&WDWlvl-5
.cp &tmplvl
.if &e'&dohelp eq 0 .do begin
.   .if '&*1' eq '$compact' .do begin
.   :UL compact
.   .do end
.   .el .do begin
.   :UL
.   .do end
.do end
.el .do begin
.   .if '&*1' eq '$compact' .do begin
.   :ZUL compact
.   .do end
.   .el .do begin
.   :ZUL
.   .do end
.do end
.dm begbull end
.*
.dm bull begin
.if &e'&dohelp eq 0 .do begin
:LI
.do end
.el .do begin
:ZLI
.do end
.dm bull end
.*
.dm endbull begin
.if &e'&dohelp eq 0 .do begin
:eUL.
.do end
.el .do begin
:ZeUL.
.do end
.dm endbull end
.*
.dm keep begin
.if '&*' eq 'begin' .do begin
.   .if &keeplvl ne 0 .me
.   .sr keeplvl=1
.   .cp &WDWlvl
.do end
.el .if '&*' eq 'break' .do begin
.   .br;.cp &WDWlvl
.do end
.el .if '&*' eq 'end' .do begin
.   .sr keeplvl=0
.do end
.el .if '&*' eq '' .do begin
.   .cp &WDWlvl
.do end
.el .do begin
.   .cp &*
.do end
.dm keep end
.*
.dm sepsect begin
.if &e'&dohelp eq 0 .do begin
:H0.&*
.do end
.el .do begin
:cmt. This screws up OS/2 online help files; don't do it
:cmt. :ZH0.&*
.do end
:set symbol="headtext$" value=" ".
:set symbol="headtxt0$" value="&*".
.pa odd
.dm sepsect end
.*
.dm chap begin
.se *secttl$=&*
.se *reftx=""
.if '&*refid.' ne '' .do begin
.   .se *secttl$=&'substr(&*,&'pos(&*refid.,&*)+&'length(&*refid.)+1)
.   .se *reftx="id='&*refid.'"
.do end
.if &e'&dohelp eq 0 .do begin
:H1 &*reftx..&*secttl$.
.do end
.el .do begin
:ZH1 &*reftx..&*secttl$.
.do end
.pu 1 .ixchap &*secttl$.
:set symbol="SCTlvl" value = "0".
.cntents &*secttl$.
:set symbol="SCTlvl" value = "1".
.if '&headtxt0$' eq ' ' .do begin
.se headtxt0$=Chapter &amp.$hnum1
.do end
.se headtxt1$=&*secttl$.
.se headtext$=&*secttl$.
.in &INDlvl
.if '&format' eq '7x9' .do begin
:BINCLUDE file='rule7x9.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '6x8' .do begin
:BINCLUDE file='rule6x8.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '8.5x11' .do begin
:BINCLUDE file='rule.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '8.5x11a' .do begin
:cmt. nothing for ASCII listing file
.do end
.dm chap end
.*
.dm chapnum begin
.dm chapnum end
.*
.dm beglevel begin
.if '&SCTlvl' eq '5' .ty too many section levels
.el .if '&SCTlvl' eq '4' :set symbol="SCTlvl" value = "5".
.el .if '&SCTlvl' eq '3' :set symbol="SCTlvl" value = "4".
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "3".
.el .if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "2".
.el .if '&SCTlvl' eq '0' :set symbol="SCTlvl" value = "1".
.pu 1 .beglevel
.dm beglevel end
.*
.dm endlevel begin
.if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "0".
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "1".
.el .if '&SCTlvl' eq '3' :set symbol="SCTlvl" value = "2".
.el .if '&SCTlvl' eq '4' :set symbol="SCTlvl" value = "3".
.el .if '&SCTlvl' eq '5' :set symbol="SCTlvl" value = "4".
.pu 1 .endlevel
.dm endlevel end
.*
.dm section begin
.se *secttl$=&*
.se *reftx=""
.if '&*refid.' ne '' .do begin
.   .se *secttl$=&'substr(&*,&'pos(&*refid.,&*)+&'length(&*refid.)+1)
.   .se *reftx="id='&*refid.'"
.do end
.in 0
.if '&SCTlvl' eq '1' .do begin
.   .cp &WDWlvl
.   .if &e'&dohelp eq 0 .do begin
.   :H2 &*reftx..&*secttl$.
.   .do end
.   .el .do begin
.   :ZH2 &*reftx..&*secttl$.
.   .do end
.   .se headtext$=&*secttl$.
.do end
.el .if '&SCTlvl' eq '2' .do begin
.   .sr TMPlvl=&WDWlvl.-2
.   .cp &TMPlvl
.   .if &e'&dohelp eq 0 .do begin
.   :H3 &*reftx..&*secttl$.
.   .do end
.   .el .do begin
.   :ZH3 &*reftx..&*secttl$.
.   .do end
.do end
.el .if '&SCTlvl' eq '3' .do begin
.   .sr TMPlvl=&WDWlvl.-4
.   .cp &TMPlvl
.   .if &e'&dohelp eq 0 .do begin
.   :H4 &*reftx..&*secttl$.
.   .do end
.   .el .do begin
.   :ZH4 &*reftx..&*secttl$.
.   .do end
.do end
.el .if '&SCTlvl' eq '4' .do begin
.   .sr TMPlvl=&WDWlvl.-6
.   .cp &TMPlvl
.   :H5 &*reftx..&*secttl$.
.do end
.el .if '&SCTlvl' eq '5' .do begin
.   .sect &*secttl$.
.do end
.in &INDlvl
.cntents &*secttl$.
.pu 1 .ixsect &*secttl$.
.dm section end
.*
.dm reptilde begin
.* replace all "~x" with "x"
.se *ind=1
...replace
.   .se *ind=&'index(›&cnt_ttl.›,'~',&*ind.)
.   .if &*ind. eq 0 .me
.   .sr cnt_ttl=&'delstr(&cnt_ttl.,&*ind.,1)
.   .se *ind=&*ind.+1
.   .go replace
.dm reptilde end
.*
.dm repchars begin
.* Microsoft Help Tool Kit says "# = > @ ! |" are bad characters
.* replace all ";" with "\;"
.* replace all "=" with "\="
.se *ind=1
...replace
.   .se *ind1=&'index(›&cnt_ttl.›,';',&*ind.)
.   .se *ind2=&'index(›&cnt_ttl.›,'=',&*ind.)
.   .if &*ind1. eq 0 .se *ind1=&*ind2.
.   .if &*ind1. eq 0 .me
.   .if &*ind2. eq 0 .se *ind2=&*ind1.
.   .se *ind=&'min(&*ind1.,&*ind2.)
.   .se *ind=&*ind.-1
.   .sr cnt_ttl=&'insert('\',›&cnt_ttl.›,&*ind.)
.   .se *ind=&*ind.+3
.   .go replace
.dm repchars end
.*
.dm cntents begin
.if &e'&dohelp eq 0 .me
.if ›&*› eq ›end_of_book› .do begin
:set symbol="SCTlvl" value = "0".
.do end
.sr *sct=0
.if ›&cnt_ttl› ne › › .do begin
.   .ctxstr &cnt_pfx.&cnt_ctx.
.   .reptilde
.   .repchars
.   .if &cnt_lvl lt &SCTlvl .do begin
.   .   .sr *sct=&cnt_lvl+2
.   .   .pu 3 &*sct &cnt_ttl.
.   .   .sr cnt_ttl='Introduction'
.   .do end
.   .el .do begin
.   .   .if &cnt_lvl eq 0 .do begin
.   .   .   .sr *sct=&cnt_lvl+2
.   .   .   .pu 3 &*sct &cnt_ttl.
.   .   .   .sr cnt_ttl='Introduction'
.   .   .do end
.   .   .el .do begin
.   .   .   .sr *sct=&cnt_lvl+1
.   .   .do end
.   .do end
.   .if &*sct. ge 5 .do begin
.   .   .if ›&cnt_ttl.› ne ›Introduction› .do begin
.   .   .   .ty ***WARNING*** nesting too deep for Contents file (.CNT)
.   .   .   .ty ***WARNING*** &*sct &cnt_ttl.
.   .   .do end
.   .do end
.   .if &'length(&ctx_str.) gt 65 .do begin
.   .   .pu 3 &*sct &cnt_ttl.=
.   .   .pu 3 &ctx_str.
.   .do end
.   .el .do begin
.   .   .pu 3 &*sct &cnt_ttl.=&ctx_str.
.   .do end
.do end
.sr cnt_lvl=&SCTlvl
.if '&*ctx.' ne '' .do begin
.   .sr cnt_pfx=''
.   .sr cnt_ctx=&*ctx.
.   .sr cnt_ttl=&'substr(&*,&'pos(&*ctx.,&*)+&'length(&*ctx.)+1)
.do end
.el .do begin
.   .sr cnt_pfx='&pfx$.'
.   .sr cnt_ctx=&*
.   .sr cnt_ttl=&*
.do end
.dm cntents end
.*
.se pfx$=''
.*
.dm npsection begin
.pa
.section &*
.dm npsection end
.*
.dm sect begin
.cp &WDWlvl
.in 0
.if '&SCTlvl' eq '0' .do begin
.   :P.
.   :SF font=11.&*:eSF.
.   :P.
.do end
.el .if '&SCTlvl' eq '1' .do begin
.   :P.
.   :SF font=10.&*:eSF.
.   :P.
.do end
.el .if '&SCTlvl' eq '2' .do begin
.   :P.
.   :SF font=9.&*:eSF.
.   :P.
.do end
.el .if '&SCTlvl' eq '3' .do begin
.   :P.
.   :SF font=8.&*:eSF.
.   :P.
.do end
.el .if '&SCTlvl' eq '4' .do begin
.   :P.
.   :SF font=8.&*:eSF.
.   :P.
.do end
.el .if '&SCTlvl' eq '5' .do begin
.   :P.
.   :SF font=8.&*:eSF.
.   :P.
.do end
.in &INDlvl
.dm sect end
.*
.dm npsect begin
.pa
.sect &*
.dm npsect end
.*
.dm preface begin
.in 0
:SF font=11.&*:eSF.
.if '&format' eq '7x9' .do begin
:BINCLUDE file='rule7x9.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '6x8' .do begin
:BINCLUDE file='rule6x8.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '8.5x11' .do begin
:BINCLUDE file='rule.eps' reposition=start depth='0'.
.do end
.el .if '&format' eq '8.5x11a' .do begin
:cmt. nothing for ASCII listing file
.do end
:P.
.in &INDlvl
.dm preface end
.*
.dm csect begin
:H6.&*
.dm csect end
.*
.dm endfmt begin
.dm endfmt end
.*
.dm rev1 begin
.dm rev1 end
.dm rev2 begin
.dm rev2 end
.dm rev3 begin
.dm rev3 end
.*
