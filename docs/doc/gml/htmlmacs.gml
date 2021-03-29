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
<br>
.dm br end
.*
.dm us begin
<em>&*</em>
.dm us end
.*
.dm bd begin
<strong>&*</strong>
.dm bd end
.*
.dm bi begin
<em><strong>&*</strong></em>
.dm bi end
.*
.dm pp begin
:P.<p>
.ct
.dm pp end
.*
.dm np begin
:P.<p>
.ct
.dm np end
.*
.dm pc begin
:PC.<p>
.ct
.dm pc end
.*
.dm illust begin
.if '&*' eq 'begin' .do begin
:XMP.
<p><pre>
.do end
.el .if '&*' eq 'end' .do begin
</pre>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</pre>
:eXMP.:XMP.
<pre>
.do end
.el .do begin
:XMP.
<p><pre>
&*
</pre>
:eXMP.:P.<p>
.do end
.dm illust end
.*
.dm millust begin
.if '&*' eq 'begin' .do begin
:XMP.
<p><pre>
.do end
.el .if '&*' eq 'end' .do begin
</pre>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</pre>:eXMP.:XMP.<pre>
.do end
.el .do begin
:XMP.
<p><pre>
&*
</pre>
:eXMP.:P.<p>
.do end
.dm millust end
.*
.dm code begin
.if '&*' eq 'begin' .do begin
:XMP.
<p><pre>
.do end
.el .if '&*' eq 'end' .do begin
</pre>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</pre>
:eXMP.:XMP.
<pre>
.do end
.el .do begin
:XMP.
<p><pre>
&*
</pre>
:eXMP.:P.<p>
.do end
.dm code end
.*
.dm listing begin
.if '&*' eq 'begin' .do begin
:XMP.
<p><pre>
.do end
.el .if '&*' eq 'end' .do begin
</pre>
:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
</pre>
:eXMP.:XMP.
<pre>
.do end
.el .do begin
:XMP.
<p><pre>
&*
</pre>
:eXMP.:P.<p>
.do end
.dm listing end
.*
.dm codeon begin
<code><pre>
.dm codeon end
.*
.dm codeoff begin
</pre></code>
.dm codeoff end
.*
.dm mono begin
<tt>&*</tt>
.dm mono end
.*
.dm monoon begin
<tt>
.dm monoon end
.*
.dm monooff begin
</tt>
.dm monooff end
.*
.dm autonote begin
.if '&*' ne '' .do begin
.  .sr tmplvl=&WDWlvl-3
.  :P.<p><i>&*</i>
.do end
:P.<ol>
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
.dm autonote end
.*
.dm autopoint begin
.if '&*' ne '' .do begin
.  :P.<p><i>&*</i>
.do end
:P.<ol>
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
:set symbol="NTEop3" value="tsize='&*2'".
.do end
.if '&*2' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize='&*3'".
.do end
.if '&*3' eq '$setptnt' .do begin
:set symbol="NTEop3" value="tsize='&*4'".
.do end
:P.<dl>
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:P.<dt>&*<dd>
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
:P.<dl>
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:P.<dt>&*<dd>
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
:P.<dt><b>&*</b>
.dm notehd1 end
.*
.dm notehd2 begin
:P.<dd><b>&*</b>
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
</ol>
.dm endOL end
.*
.dm endDL begin
..br
</dl>
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
:P.<li>
.dm noteOL end
.*
.dm noteDL begin
.sr tmplvl=3
.if '&*1' eq '.mono' .do begin
:P.<dt> &*2 &*3 &*4 &*5 &*6
.do end
.el .do begin
:P.<dt> &*
.do end
<dd>
.dm noteDL end
.*
.dm begbull begin
.sr tmplvl=&WDWlvl-5
.if '&*1' eq '$compact' .do begin
:P.<ul>
.do end
.el .do begin
:P.<ul>
.do end
.dm begbull end
.*
.dm bull begin
:P.<li>
.dm bull end
.*
.dm endbull begin
..br
</ul>
.dm endbull end
.*
.dm keep begin
.dm keep end
.*
.dm sepsect begin
:P.<head>
..br
<title>&*
..br
</title>
..br
</head>
..br
<body>
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
.  .se *reftxb='<a name="&*refid.">'
.  .se *reftxe='</a>'
.do end
:P.<hr>
..br
<h1>&*reftxb.:H1 &*reftx..&*secttl$.
&*reftxe.</h1>
.pu 1 .ixchap &*secttl$.
:set symbol="SCTlvl" value = "0".
.if '&headtxt0$' eq ' ' .do begin
.se headtxt0$=Chapter &amp.$hnum1
.do end
.se headtxt1$=&*secttl$.
.se headtext$=&*secttl$.
..br
<hr>
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
.   .se *reftxb='<a name="&*refid.">'
.   .se *reftxe='</a>'
.   .se *reftx="id='&*refid.'"
.do end
:P.
.if '&SCTlvl' eq '0' .do begin
.   :P.<h2>&*reftxb.:H2 &*reftx..&*secttl$.
.   &*reftxe.</h2>
.   .se headtext$=&*secttl$.
.do end
.el .if '&SCTlvl' eq '1' .do begin
.   :P.<h3>&*reftxb.:H3 &*reftx..&*secttl$.
.   &*reftxe.</h3>
.do end
.el .if '&SCTlvl' eq '2' .do begin
.   :P.<h4>&*reftxb.:H4 &*reftx..&*secttl$.
.   &*reftxe.</h4>
.do end
.el .if '&SCTlvl' eq '3' .do begin
.   :P.<h5>&*reftxb.:H5 &*reftx..&*secttl$.
.   &*reftxe.</h5>
.do end
.el .if '&SCTlvl' eq '4' .do begin
.   :P.<h6>&*reftxb.&*secttl$.&*reftxe.</h6>
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
.  <p><em>&*</em>
.  ..br
.do end
.el .if '&SCTlvl' eq '1' .do begin
.  <p><em>&*</em>
.  ..br
.do end
.el .if '&SCTlvl' eq '2' .do begin
.  <p><em>&*</em>
.  ..br
.do end
.el .if '&SCTlvl' eq '3' .do begin
.  <p><em>&*</em>
.  ..br
.do end
.el .if '&SCTlvl' eq '4' .do begin
.  <p><em>&*</em>
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
<title>&*</title>
..br
<hr>
:P.
<p>
.dm preface end
.*
.dm csect begin
:P.
<h1>&*</h1>
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
<var>&*</var>
.dm sy end
.*
.dm ev begin
<dfn>&*</dfn>
.ix 'environment variables' '&*'
.ix '&* environment variable'
.dm ev end
.*
.dm kw begin
.ix '&*'
<dfn>&*</dfn>
.dm kw end
.*
.dm kwm begin
.ix '&*'
<kbd>&*</kbd>
.dm kwm end
.*
.dm id begin
<code>&*</code>
.dm id end
.*
.dm book begin
<cite>&*</cite>
.dm book end
.*
.dm fie begin
<code>&*
.dm fie end
.*
.dm fi begin
.fie &*.
</code>
.dm fi end
.*
.dm fname begin
.fie &*.
.dm fname end
.*
.dm efname begin
.if &l'&*. eq 0 .do begin
.   ..ct
</code>
.do end
.el .do begin
</code>&*.
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
<img align=middle src="&*file..bmp" alt="&*figttl.">
:P.
<p>&*figttl.
.dm figure end
.gt figure add figure att
.ga * depth any
.ga * file any
.*
.dm pict begin
:P.
<img align=middle src="&*file..bmp" alt="&*text.">
:P.
<p>&*text.
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
<img align=middle src="&*file..bmp" alt="&*text.">
:P.
<p>&*text.
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
<a href="#&*refid.">:HDREF refid='&*refid.' page=no.</a>&*
.dm @hdref end
.gt HDREF add @HDREF continue attributes
.ga * REFID any
.ga * PAGE uppercase value NO
.ga * *    value YES
.*
.dm box begin
.if '&*' eq 'begin' .do begin
.   :P.
.   <p>
.   <hr>
.   :P.
.do end
.el .if '&*' eq 'end' .do begin
.   :P.
.   <p>
.   <hr>
.   :P.
.do end
.el .do begin
.   :P.
.   <p>
.   <hr>
.   :P.
.do end
.dm box end
.*
.dm mbox begin
.if '&*1' eq 'on' .do begin
.   :XMP.
.   <p>
.   <hr>
.   <pre>
.do end
.el .if '&*' eq 'off' .do begin
.   </pre>
.   <hr>
.   :eXMP.
.do end
.el .if '&*' eq 'begin' .do begin
.   :XMP.
.   <p>
.   <hr>
.   <pre>
.do end
.el .if '&*' eq 'end' .do begin
.   </pre>
.   <hr>
.   :eXMP.
.do end
.el .do begin
.   <hr>
.do end
.dm mbox end
.*
.dm cbox begin
.mbox &*
.dm cbox end
.*
.dm mbigbox begin
:XMP.
<p>
<hr>
<pre>
.dm mbigbox end
.*
.dm embigbox begin
</pre>
<hr>
:eXMP.
.dm embigbox end
.*
.dm syntax begin
:XMP.
<p><pre>
.dm syntax end
.*
.dm esyntax begin
</pre>
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
<p><pre>
.dm list end
.*
.dm elist begin
</pre>
:eXMP.
.dm elist end
.*
.dm synote begin
.begnote
:P.
<dt>where:
<dd>description:
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
(see <a href="#&*refid.">:HDREF refid='&*refid.' page=no.</a>)
.dm optref end
.gt optref add optref attr
.ga * refid any
.*
.dm contents begin
:P.
<p>
..br
<pre>
.dm contents end
.*
.dm econtents begin
..br
</pre>
.dm econtents end
.*
.dm hint begin
:P.
<blockquote>
<strong>Hint:</strong>
.dm hint end
.*
.dm ehint begin
..br
</blockquote>
.dm ehint end
.*
.dm remark begin
:P.
<blockquote>
<strong>Note:</strong>
.dm remark end
.*
.dm eremark begin
..br
</blockquote>
.dm eremark end
.*
.dm warn begin
:P.
<blockquote>
<strong>WARNING!</strong>
.dm warn end
.*
.dm ewarn begin
..br
</blockquote>
.dm ewarn end
.*
.dm exam begin
.  .if '&*1' eq 'begin' .do begin
.  .  :P.
.  .  <p><i>Example:</i>
.  .  :XMP.
.  .  <pre>
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  </pre>
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  </pre>
.  .  :eXMP.
.  .  :XMP.
.  .  <pre>
.  .do end
.  .el .do begin
.  .  :P.
.  .  <p><i>Example:</i>
.  .  :XMP.
.  .  <pre>
.  .  &*
.  .  </pre>
.  .  :eXMP.:P.<p>
.  .do end
.dm exam end
.*
.dm tinyexam begin
.  .if '&*1' eq 'begin' .do begin
.  .  :P.
.  .  <p><i>Example:</i>
.  .  :XMP.
.  .  <pre>
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  </pre>
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  </pre>
.  .  :eXMP.
.  .  :XMP.
.  .  <pre>
.  .do end
.  .el .do begin
.  .  :P.
.  .  <p><i>Example:</i>
.  .  :XMP.
.  .  <pre>
.  .  &*
.  .  </pre>
.  .  :eXMP.:P.<p>
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
:P.<dl>
.if '&*1' ne '' .do begin
.   .stephdr &*.
.do end
.dm initstep end
.*
.dm stephdr begin
:P.<dt>&*<dd>
.dm stephdr end
.*
.dm step begin
.sr stplvl=&stplvl.+1
.if &stplvl. eq 0 .do begin
:P.<dt>
.do end
.el .do begin
:P.<dt>(&stplvl.)
.do end
<dd>
<b>&*</b>
.dm step end
.*
.dm orstep begin
:P.<p>
or
:P.<dt>
<dd>
<b>&*</b>
.dm orstep end
.*
.dm result begin
:P.
<p>
.dm result end
.*
.dm endstep begin
..br
</dl>
.dm endstep end
.*
.dm topsect begin
.pa
.sect &*
.dm topsect end
.*
.dm helppref begin
.if &e'&dohelp ne 0 .do begin
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
See <a href="#&*">:HDREF refid='&*' page=no.</a>
.dm seealso end
.*
.dm mi begin
<b><i>&*</i></b>
.dm mi end
.*
.dm mm begin
<b><i>&*</i></b>
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
.dm period begin
.ct .li .
.dm period end
.gt period add period
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
