.*
.* Open Watcom Documentation Macros & Variables
.*
.*
.pu 1
:set symbol="pubdate"   value="January, 1995".
:set symbol="headtxt0$" value=" ".
:set symbol="headtxt1$" value=" ".
:set symbol="headtext$" value=" ".
:set symbol="WDWlvl"    value="10".
:set symbol="SCTlvl"    value="0".
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
.dm ix1 end
.*
.dm ix2 begin
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
.dm br begin
..br
<BR>
.dm br end
.*
.dm us begin
<EM>&*</EM>
.dm us end
.*
.dm bd begin
<STRONG>&*</STRONG>
.dm bd end
.*
.dm bi begin
<EM><STRONG>&*</STRONG></EM>
.dm bi end
.*
.dm pp begin
:P.<P>
.ct
.dm pp end
.*
.dm np begin
:P.<P>
.ct
.dm np end
.*
.dm pc begin
:PC.<P>
.ct
.dm pc end
.*
.dm illust begin
.if '&*' eq 'begin' .do begin
:XMP.
<P><PRE>
.do end
.el .if '&*' eq 'end' .do begin
</PRE>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</PRE>
:eXMP.:XMP.
<PRE>
.do end
.el .do begin
:XMP.
<P><PRE>
&*
</PRE>
:eXMP.:P.<P>
.do end
.dm illust end
.*
.dm millust begin
.if '&*' eq 'begin' .do begin
:XMP.
<P><PRE>
.do end
.el .if '&*' eq 'end' .do begin
</PRE>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</PRE>:eXMP.:XMP.<PRE>
.do end
.el .do begin
:XMP.
<P><PRE>
&*
</PRE>
:eXMP.:P.<P>
.do end
.dm millust end
.*
.dm code begin
.if '&*' eq 'begin' .do begin
:XMP.
<P><PRE>
.do end
.el .if '&*' eq 'end' .do begin
</PRE>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</PRE>
:eXMP.:XMP.
<PRE>
.do end
.el .do begin
:XMP.
<P><PRE>
&*
</PRE>
:eXMP.:P.<P>
.do end
.dm code end
.*
.dm listing begin
.if '&*' eq 'begin' .do begin
:XMP.
<P><PRE>
.do end
.el .if '&*' eq 'end' .do begin
</PRE>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</PRE>
:eXMP.:XMP.
<PRE>
.do end
.el .do begin
:XMP.
<P><PRE>
&*
</PRE>
:eXMP.:P.<P>
.do end
.dm listing end
.*
.dm codeon begin
<CODE><PRE>
.dm codeon end
.*
.dm codeoff begin
</PRE></CODE>
.dm codeoff end
.*
.dm mono begin
<TT>&*</TT>
.dm mono end
.*
.dm monoon begin
<TT>
.dm monoon end
.*
.dm monooff begin
</TT>
.dm monooff end
.*
.dm autonote begin
.if '&*' ne '' .do begin
.  .sr tmplvl=&WDWlvl-3
.  :P.<P><I>&*</I>
.do end
:P.<OL>
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
.dm autonote end
.*
.dm autopoint begin
.if '&*' ne '' .do begin
.  :P.<P><I>&*</I>
.do end
:P.<OL>
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
.dm autopoint end
.*
.dm begnote begin
.sr tmplvl=&WDWlvl-3
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
:P.<DL>
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:P.<DT>&*<DD>
.do end
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dl".
.dm begnote end
.*
.dm begpoint begin
.sr tmplvl=&WDWlvl-3
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
:P.<DL>
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:P.<DT>&*<DD>
.do end
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dl".
.dm begpoint end
.*
.dm setptnt begin
.dm setptnt end
.*
.dm notehd1 begin
:P.<DT><B>&*</B>
.dm notehd1 end
.*
.dm notehd2 begin
:P.<DD><B>&*</B>
.dm notehd2 end
.*
.dm endnote begin
.if '&NTEset' eq 'ol' .endOL
.el .if '&NTEset' eq 'dl' .endDL
:set symbol="NTEset" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEpr2".
:set symbol="NTEpr2" value=" ".
.dm endnote end
.*
.dm endpoint begin
.endnote
.dm endpoint end
.*
.dm endOL begin
..br
</OL>
.dm endOL end
.*
.dm endDL begin
..br
</DL>
.dm endDL end
.*
.dm note begin
.if '&NTEset' eq 'ol' .noteOL
.el .if '&NTEset' eq 'dl' .noteDL &*
.dm note end
.*
.dm point begin
.if '&NTEset' eq 'ol' .noteOL
.el .if '&NTEset' eq 'dl' .noteDL &*
.dm point end
.*
.dm noteOL begin
:P.<LI>
.dm noteOL end
.*
.dm noteDL begin
.sr tmplvl=3
.if '&*1' eq '.mono' .do begin
:P.<DT> &*2 &*3 &*4 &*5 &*6
.do end
.el .do begin
:P.<DT> &*
.do end
<DD>
.dm noteDL end
.*
.dm begbull begin
.sr tmplvl=&WDWlvl-5
.if '&*1' eq '$compact' .do begin
:P.<UL>
.do end
.el .do begin
:P.<UL>
.do end
.dm begbull end
.*
.dm bull begin
:P.<LI>
.dm bull end
.*
.dm endbull begin
..br
</UL>
.dm endbull end
.*
.dm keep begin
.dm keep end
.*
.dm sepsect begin
:P.<HEAD>
..br
<TITLE>&*
..br
</TITLE>
..br
</HEAD>
..br
<BODY>
:set symbol="headtext$" value=" ".
:set symbol="headtxt0$" value="&*".
.pa odd
.dm sepsect end
.*
.dm chap begin
.se *secttl$=&*
.se *reftx=""
.se *reftxb=""
.se *reftxe=""
.if '&*refid.' ne '' .do begin
.  .se *secttl$=&'substr(&*,&'pos(&*refid.,&*)+&'length(&*refid.)+1)
.  .se *reftx="id='&*refid.'"
.  .se *reftxb='<A NAME = "&*refid.">'
.  .se *reftxe='</A>'
.do end
:P.<HR>
..br
<H1>&*reftxb.:H1 &*reftx..&*secttl$.
&*reftxe.</H1>
.pu 1 .ixchap &*secttl$.
:set symbol="SCTlvl" value = "0".
.if '&headtxt0$' eq ' ' .do begin
.se headtxt0$=Chapter &amp.$hnum1
.do end
.se headtxt1$=&*secttl$.
.se headtext$=&*secttl$.
..br
<HR>
.dm chap end
.*
.dm chapnum begin
.dm chapnum end
.*
.dm beglevel begin
.if '&SCTlvl' eq '4' .ty too many section levels
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
.pu 1 .endlevel
.dm endlevel end
.*
.dm section begin
.se *secttl$=&*
.se *reftx=""
.se *reftxb=""
.se *reftxe=""
.if '&*refid.' ne '' .do begin
.   .se *secttl$=&'substr(&*,&'pos(&*refid.,&*)+&'length(&*refid.)+1)
.   .se *reftxb='<A NAME = "&*refid.">'
.   .se *reftxe='</A>'
.   .se *reftx="id='&*refid.'"
.do end
:P.
.if '&SCTlvl' eq '0' .do begin
.   :P.<H2>&*reftxb.:H2 &*reftx..&*secttl$.
.   &*reftxe.</H2>
.   .se headtext$=&*secttl$.
.do end
.el .if '&SCTlvl' eq '1' .do begin
.   :P.<H3>&*reftxb.:H3 &*reftx..&*secttl$.
.   &*reftxe.</H3>
.do end
.el .if '&SCTlvl' eq '2' .do begin
.   :P.<H4>&*reftxb.:H4 &*reftx..&*secttl$.
.   &*reftxe.</H4>
.do end
.el .if '&SCTlvl' eq '3' .do begin
.   :P.<H5>&*reftxb.:H5 &*reftx..&*secttl$.
.   &*reftxe.</H5>
.do end
.el .if '&SCTlvl' eq '4' .do begin
.   :P.<H6>&*reftxb.&*secttl$.&*reftxe.</H6>
.do end
.pu 1 .ixsect &*secttl$.
.dm section end
.*
.dm npsection begin
.section &*
.dm npsection end
.*
.dm sect begin
:P.
.if '&SCTlvl' eq '0' .do begin
.  <P><EM>&*</EM>
.  ..br
.do end
.el .if '&SCTlvl' eq '1' .do begin
.  <P><EM>&*</EM>
.  ..br
.do end
.el .if '&SCTlvl' eq '2' .do begin
.  <P><EM>&*</EM>
.  ..br
.do end
.el .if '&SCTlvl' eq '3' .do begin
.  <P><EM>&*</EM>
.  ..br
.do end
.el .if '&SCTlvl' eq '4' .do begin
.  <P><EM>&*</EM>
.  ..br
.do end
.dm sect end
.*
.dm npsect begin
.sect &*
.dm npsect end
.*
.dm preface begin
:P.
<TITLE>&*</TITLE>
..br
<HR>
:P.
<P>
.dm preface end
.*
.dm csect begin
:P.
<H1>&*</H1>
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
.* ------------------------------------------------------------------------
.*
.* WATCOM Documentation Macros & Variables
.*
:INCLUDE file='SYMBOLS'.
.se lt off
.se gt off
:set symbol="lc" value="&lt.".
:set symbol="rc" value="&gt.".
.*
.if '&format' eq '7x9' .do begin
:set symbol='rmargin' value='60'.
.do end
.el .if '&format' eq '8.5x11' .do begin
:set symbol='rmargin' value='68'.
.do end
.el .if '&format' eq '8.5x11a' .do begin
:set symbol='rmargin' value='78'.
.do end
.el .if '&format' eq 'help' .do begin
:set symbol='rmargin' value='78'.
.do end
.el .do begin
:set symbol='rmargin' value='60'.
.do end
.*
.dm sy begin
<VAR>&*</EVAR>
.dm sy end
.*
.dm ev begin
<DFN>&*</DFN>
.ix 'environment variables' '&*'
.ix '&* environment variable'
.dm ev end
.*
.dm kw begin
<DFN>&*</DFN>
.ix '&*'
.dm kw end
.*
.dm kwm begin
<KBD>&*</KBD>
.ix '&*'
.dm kwm end
.*
.dm id begin
<CODE>&*</CODE>
.dm id end
.*
.dm book begin
<CITE>&*</CITE>
.dm book end
.*
.dm fie begin
<CODE>&*
.dm fie end
.*
.dm fi begin
.fie &*.
</CODE>
.dm fi end
.*
.dm fname begin
.fie &*.
.dm fname end
.*
.dm efname begin
.if &l'&*. eq 0 .do begin
.   ..ct
</CODE>
.do end
.el .do begin
</CODE>&*.
.do end
.dm efname end
.*
.gt fname add fname cont
.gt efname add efname cont
.*
.dm uindex begin
.ix '&*'
.dm uindex end
.*
.dm figure begin
.se *figttl=&*
.if '&*depth.' eq '' or '&*depth.' eq '1.xx' .do begin
.   .ty *** Missing picture file '&*figttl.'
.   .me
.do end
.se *figttl=&'substr(&*,&'pos(&*file.,&*)+&'length(&*file.)+1)
:P.
<IMG ALIGN=MIDDLE SRC="&*file..bmp" ALT="&*figttl.">
:P.
<P>&*figttl.
.dm figure end
.gt figure add figure att
.ga * depth any
.ga * file any
.*
.dm pict begin
:P.
<IMG ALIGN=MIDDLE SRC="&*file..bmp" ALT="&*text.">
:P.
<P>&*text.
.dm pict end
.if &$cmacpass. eq 'ugh' .do begin
.gt picture delete
.do end
.gt picture add pict att
.ga * depth any
.ga * file any
.ga * text any
.*
.dm image begin
:P.
<IMG ALIGN=MIDDLE SRC="&*file..bmp" ALT="&*text.">
:P.
<P>&*text.
.dm image end
.if &$cmacpass. eq 'ugh' .do begin
.gt image delete
.do end
.gt image add image att
.ga * xoff any
.ga * depth any
.ga * file any
.ga * text any
.*
.dm @hdref begin
<A HREF="#&*refid.">:HDREF refid='&*refid.' page=no.</A>&*
.dm @hdref end
.gt HDREF add @HDREF continue attributes
.ga * REFID any
.ga * PAGE uppercase value NO
.ga * *    value YES
.*
.dm box begin
.if '&*' eq 'begin' .do begin
.   :P.
.   <P>
.   <HR>
.   :P.
.do end
.el .if '&*' eq 'end' .do begin
.   :P.
.   <P>
.   <HR>
.   :P.
.do end
.el .do begin
.   :P.
.   <P>
.   <HR>
.   :P.
.do end
.dm box end
.*
.dm mbox begin
.if '&*1' eq 'on' .do begin
.   :XMP.
.   <P>
.   <HR>
.   <PRE>
.do end
.el .if '&*' eq 'off' .do begin
.   </PRE>
.   <HR>
.   :eXMP.
.do end
.el .if '&*' eq 'begin' .do begin
.   :XMP.
.   <P>
.   <HR>
.   <PRE>
.do end
.el .if '&*' eq 'end' .do begin
.   </PRE>
.   <HR>
.   :eXMP.
.do end
.el .do begin
.   <HR>
.do end
.dm mbox end
.*
.dm cbox begin
.mbox &*
.dm cbox end
.*
.dm mbigbox begin
:XMP.
<P>
<HR>
<PRE>
.dm mbigbox end
.*
.dm embigbox begin
</PRE>
<HR>
:eXMP.
.dm embigbox end
.*
.dm syntax begin
:XMP.
<P><PRE>
.dm syntax end
.*
.dm esyntax begin
</PRE>
:eXMP.
.dm esyntax end
.*
.dm syntaxbrk begin
.esyntax
.syntax
.dm syntaxbrk end
.*
.dm list begin
:XMP.
<P><PRE>
.dm list end
.*
.dm elist begin
</PRE>
:eXMP.
.dm elist end
.*
.dm synote begin
.begnote
:P.
<DT>where:
<DD>description:
..br
.dm synote end
.*
.dm esynote begin
.endnote
.dm esynote end
.*
.dm mnote begin
.note &*
.dm mnote end
.*
.dm optlist begin
.sr OLDlvl=&SCTlvl.
.sr SCTlvl=3
.dm optlist end
.gt optlist add optlist
.*
.dm opt begin
.if '&*refid.' ne '' .do begin
.section *refid=&*refid. &*name.&*
.do end
.el .do begin
.section &*name.&*
.do end
.dm opt end
.gt opt add opt attr
.ga * refid any
.ga * name any
.*
.dm eoptlist begin
.sr SCTlvl=&OLDlvl.
.dm eoptlist end
.gt eoptlist add eoptlist
.*
.dm optref begin
:P.
(see <A HREF="#&*refid.">:HDREF refid='&*refid.' page=no.</A>)
.dm optref end
.gt optref add optref attr
.ga * refid any
.*
.dm contents begin
:P.
<P>
..br
<PRE>
.dm contents end
.*
.dm econtents begin
..br
</PRE>
.dm econtents end
.*
.dm hint begin
:P.
<BLOCKQUOTE>
<STRONG>Hint:</STRONG>
.dm hint end
.*
.dm ehint begin
..br
</BLOCKQUOTE>
.dm ehint end
.*
.dm remark begin
:P.
<BLOCKQUOTE>
<STRONG>Note:</STRONG>
.dm remark end
.*
.dm eremark begin
..br
</BLOCKQUOTE>
.dm eremark end
.*
.dm warn begin
:P.
<BLOCKQUOTE>
<STRONG>WARNING!</STRONG>
.dm warn end
.*
.dm ewarn begin
..br
</BLOCKQUOTE>
.dm ewarn end
.*
.dm exam begin
.  .if '&*1' eq 'begin' .do begin
.  .  :P.
.  .  <P><I>Example:</I>
.  .  :XMP.
.  .  <PRE>
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  </PRE>
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  </PRE>
.  .  :eXMP.
.  .  :XMP.
.  .  <PRE>
.  .do end
.  .el .do begin
.  .  :P.
.  .  <P><I>Example:</I>
.  .  :XMP.
.  .  <PRE>
.  .  &*
.  .  </PRE>
.  .  :eXMP.:P.<P>
.  .do end
.dm exam end
.*
.dm tinyexam begin
.  .if '&*1' eq 'begin' .do begin
.  .  :P.
.  .  <P><I>Example:</I>
.  .  :XMP.
.  .  <PRE>
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  </PRE>
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  </PRE>
.  .  :eXMP.
.  .  :XMP.
.  .  <PRE>
.  .do end
.  .el .do begin
.  .  :P.
.  .  <P><I>Example:</I>
.  .  :XMP.
.  .  <PRE>
.  .  &*
.  .  </PRE>
.  .  :eXMP.:P.<P>
.  .do end
.dm tinyexam end
.*
.* for a sequence of steps in which there is only one step
.*
.dm onestep begin
.initstep &*
.sr stplvl=-1
.dm onestep end
.*
.* for a sequence of steps in which there are more than one step
.*
.dm begstep begin
.initstep &*
.sr stplvl=0
.dm begstep end
.*
.dm initstep begin
:P.<DL>
.if '&*1' ne '' .do begin
:P.<DT>&*<DD>
.do end
.dm initstep end
.*
.dm step begin
.sr stplvl=&stplvl.+1
.if &stplvl. eq 0 .do begin
:P.<DT>
.do end
.el .do begin
:P.<DT>(&stplvl.)
.do end
<DD>
<B>&*</B>
.dm step end
.*
.dm orstep begin
:P.<P>
or
:P.<DT>
<DD>
<B>&*</B>
.dm orstep end
.*
.dm result begin
:P.
<P>
.dm result end
.*
.dm endstep begin
..br
</DL>
.dm endstep end
.*
.dm topsect begin
.pa
.sect &*
.dm topsect end
.*
.dm helppref begin
.if &e'&dohelp eq 1 .do begin
.if '&*' ne '' .do begin
:helppfx pfx='&* '.
.se pfx$='&* '
.do end
.el .do begin
:helppfx.
.se pfx$=''
.do end
.pu 1 .helppref &*
.do end
.dm helppref end
.*
.dm ixbook begin
.ty ***We don't do ixbook
.dm ixbook end
.*
.dm ixhelp begin
.ty ***We don't do ixhelp
.dm ixhelp end
.*
.dm ixchap begin
.ty ***We don't do ixchap
.dm ixchap end
.*
.dm ixsect begin
.ty ***We don't do ixsect
.dm ixsect end
.*
.dm ixsectid begin
.* placeholder, pass info for external HTML Help processing
.dm ixsectid end
.*
.dm ixline begin
.ty ***We don't do ixline
.dm ixline end
.*
.* The following symbol set corrects a macro definition problem
.*
:SET symbol='$cmacpass' value='ugh'.
.*
.* Start Useful macros created by John for the debugger book.
.*
.dm begmenu begin
.begnote $break
.sr currmenu='&*'
.dm begmenu end

.dm menuitem begin
.ix '&currmenu. menu' '&*'
.ix '&* menu item'
.note &*
.dm menuitem end

.dm endmenu begin
.endnote
.dm endmenu end
.*
.dm seealso begin
See <A HREF="#&*">:HDREF refid='&*' page=no.</A>
.dm seealso end
.*
.dm mi begin
<B><I>&*</I></B>
.dm mi end
.*
.dm mm begin
<B><I>&*</I></B>
.ix '&* menu'
.dm mm end
.*
.dm menuref begin
.mi &*1
from the
.mi &*2
menu
.dm menuref end
.*
.dm popref begin
.mi &*
.ix '&* menu item'
from the pop-up menu
.dm popref end
.*
.dm dot begin
.ct .li .
.dm dot end
.*
.dm cmddef begin
.section &*
.ix 'command' '&*'
.ix '&* command'
.dm cmddef end
.*
.dm wnddef begin
&*
.ix 'window' '&*'
.ix '&*' 'window'
.dm wnddef end
.*
.dm optdef begin
.note &*
.ix 'options' '&*'
.ix '&* option'
.dm optdef end
.*
.* End Useful macros created by John for the debugger book.
.*
