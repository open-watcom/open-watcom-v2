.* (c) Copyright 1987, 1990 by WATCOM Publications Limited
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* WATCOM Documentation Macros & Variables
.*
.*
:set symbol="headtxt0$" value=" ".
:set symbol="headtxt1$" value=" ".
:set symbol="headtext$" value=" ".
:set symbol="WDWlvl"	value="10".
:set symbol="SCTlvl"	value="0".
:set symbol="NTEpr2"	value=" ".
:set symbol="NTEpr1"	value=" ".
:set symbol="NTEset"	value=" ".
:set symbol="NTEnhi"	value="termhi=3".
:set symbol="NTEphi"	value="termhi=2".
:set symbol="ANTset"	value=" ".
:set symbol="APTset"	value="compact".
.if '&format' eq '6x8' .do begin
:set symbol="INDlvl"	value="8".
.do end
.el .if '&format' eq '8.5x11a' .do begin
:set symbol="INDlvl"	value="5".
.do end
.el .if '&format' eq 'help' .do begin
:set symbol="INDlvl"	value="0".
.do end
.el .do begin
:set symbol="INDlvl"	value="7".
.do end
.*
.dm ix begin
.* The next 2 lines correct problems with ".ix NAME="
.se *iw=&*
.if '&*1' ne '' .se *iw=&*1
.se *iw="&'strip(&*iw.,'T',',')"
.se *iw="&'strip(&*iw.,'T','.')"
.if '&'substr(&*iw.,1,1)' eq '_' .do begin
.   .sr *iw2="&'strip(&*iw.,'L','_') "
.   :IH1 print='&*iw.'.&*iw2.
.   .if '&*2' eq '' .do begin
.   .	:I1.&*iw2.
.   .do end
.do end
.el .do begin
.   :IH1 print='&*iw.'.&*iw.
.   .if '&*2' eq '' .do begin
.   .	:I1.&*iw.
.   .do end
.do end
.if '&*2' ne '' .do begin
.   .ix2 &*2
.do end
.if '&*3' ne '' .do begin
.   :I3.&*3
.do end
.dm ix end
.*
.dm ix2 begin
.se *iw="&'strip(&*,'T',',')"
.se *iw="&'strip(&*iw.,'T','.')"
.if '&'substr(&*iw,1,1)' eq '_' .do begin
.   .sr *iw2=&'strip(&*iw,'L','_')
.   :IH2 print='&*iw.'.&*iw2.
.   :I2.&*iw2.
.do end
.el .do begin
.   :IH2 print='&*iw.'.&*iw.
.   :I2.&*iw.
.do end
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
.dm us begin
:SF font=1.&*:eSF.
.dm us end
.*
.dm bd begin
:SF font=2.&*:eSF.
.dm bd end
.*
.dm bi begin
:SF font=3.&*:eSF.
.dm bi end
.*
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
:XMP.:SF font=4.~b
.do end
.el .if '&*' eq 'end' .do begin
:eSF.:eXMP.
.do end
.el .if '&*' eq 'break' .do begin
:eSF.:eXMP.:XMP.:SF font=4.
.do end
.el .do begin
:XMP.:SF font=4.~b
&*
:eSF.:eXMP.:P.
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
:eSF.:eXMP. :XMP.:SF font=5.
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
.se TMPlvl = &INDlvl-3
.in &TMPlvl
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
.dm autonote begin
.if '&*' ne '' .do begin
.  .sr tmplvl=&WDWlvl-3
.  .cp &tmplvl
.  :P.:HP1.&*:eHP1.
.do end
:OL &ANTset .
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
.dm autonote end
.*
.dm autopoint begin
.if '&*' ne '' .do begin
.  .sr tmplvl=&WDWlvl-3
.  .cp &tmplvl
.  :P.:HP1.&*:eHP1.
.do end
:OL &APTset .
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="ol".
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
:DL &NTEnhi &NTEop1 &NTEop2 &NTEop3.
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:DTHD.&*
:DDHD.~b
.do end
.do end
:set symbol="NTEpr2" value="&NTEpr1".
:set symbol="NTEpr1" value="&NTEset".
:set symbol="NTEset" value="dl".
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
:DL &NTEphi &NTEop1 &NTEop2 &NTEop3.
.if '&NTEop1' eq '' and '&NTEop2' eq '' and '&NTEop3' eq '' .do begin
.if '&*1' ne '' .do begin
:DTHD.&*
:DDHD.~b
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
:eOL.
.dm endOL end
.*
.dm endDL begin
:eDL.
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
:LI.
.dm noteOL end
.*
.dm noteDL begin
.sr tmplvl=&WDWlvl-3
.cp &tmplvl
.if '&*1' eq '.mono' .do begin
:DT.&*2 &*3 &*4 &*5 &*6
.do end
.el .do begin
:DT.&*
.do end
:DD.
.dm noteDL end
.*
.dm begbull begin
.sr tmplvl=&WDWlvl-3
.cp &tmplvl
.if '&*1' eq '$compact' .do begin
:UL compact
.do end
.el .do begin
:UL
.do end
.dm begbull end
.*
.dm bull begin
:LI
.dm bull end
.*
.dm endbull begin
:eUL.
.dm endbull end
.*
.dm keep begin
.if '&*' eq 'begin' .do begin
.  .cp &WDWlvl
.do end
.el .if '&*' eq 'break' .do begin
.  .br;.cp &WDWlvl
.do end
.el .if '&*' eq 'end' .do begin
.do end
.el .if '&*' eq '' .do begin
.  .cp &WDWlvl
.do end
.el .do begin
.  .cp &*
.do end
.dm keep end
.*
.dm sepsect begin
:H0.&*
:set symbol="headtext$" value=" ".
:set symbol="headtxt0$" value="&*".
.pa odd
.dm sepsect end
.*
.dm chap begin
:H1.&*
:set symbol="SCTlvl" value="0".
.if '&headtxt0$' eq ' ' .do begin
:set symbol="headtxt0$" value="Chapter &amp.$hnum1".
.do end
:set symbol="headtxt1$" value="&*".
:set symbol="headtext$" value="&*".
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
:BINCLUDE file='rule.eps' reposition=start depth='0'.
.do end
.dm chap end
.*
.dm chapnum begin
.dm chapnum end
.*
.dm beglevel begin
.if '&SCTlvl' eq '3' .ty too many section levels
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "3".
.el .if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "2".
.el .if '&SCTlvl' eq '0' :set symbol="SCTlvl" value = "1".
.dm beglevel end
.*
.dm endlevel begin
.if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "0".
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "1".
.el .if '&SCTlvl' eq '3' :set symbol="SCTlvl" value = "2".
.dm endlevel end
.*
.dm section begin
.cp &WDWlvl
.in 0
.if '&SCTlvl' eq '0' .do begin
.  :H2.&*
.  .se headtext$=&*
.do end
.el .if '&SCTlvl' eq '1' .do begin
.  :H3.&*
.do end
.el .if '&SCTlvl' eq '2' .do begin
.  :H4.&*
.do end
.el .if '&SCTlvl' eq '3' .do begin
.  :H5.&*
.do end
.in &INDlvl
.dm section end
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
.  :P.
.  :SF font=10.&*:eSF.
.  :P.
.do end
.el .if '&SCTlvl' eq '1' .do begin
.  :P.
.  :SF font=9.&*:eSF.
.  :P.
.do end
.el .if '&SCTlvl' eq '2' .do begin
.  :P.
.  :SF font=8.&*:eSF.
.  :P.
.do end
.el .if '&SCTlvl' eq '3' .do begin
.  :P.
.  :SF font=8.&*:eSF.
.  :P.
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
:BINCLUDE file='rule.eps' reposition=start depth='0'.
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
