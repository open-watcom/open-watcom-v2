.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 30-jul-92	Craig Eisler	initial draft
.* 20-oct-92	Craig Eisler	online help support
.*

.if &ishelp ne 1 .do begin
.se ishelp = 0
:INCLUDE file='nohelp.gml'
.do end

.*
.*** macros for these are defined in the layouts
.*
.gt H_0 add H_0 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.gt H_1 add H_1 att nocont
.ga * ID 
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.gt H_2 add H_2 att nocont
.ga * ID 
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.gt H_3 add H_3 att nocont
.ga * ID 
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.gt H_4 add H_4 att nocont
.ga * ID 
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.gt H_5 add H_5 att nocont
.ga * ID 
.ga * * VALUE '' DEFAULT
.ga * * ANY 
.ga * STITLE any

.'se cmdmode='.kwd command mode.ct '
.'se tinsmode='.kwd text insertion mode.ct '
.'se cmdline='.kwd command line.ct '
.'se copybuffer='.kwd copy buffer.ct '

.dm @sepsect begin
:H_0.&*
:set symbol="headtext$" value=" ".
:set symbol="headtxt0$" value="&*".
.pa odd
.dm @sepsect end

.dm @section begin
.cp &WDWlvl
.in 0
.if '&SCTlvl' eq '0' .do begin
.  .if '&*id.' ne '' .do begin
.      :H_2 id='&*id'.&*
.  .do end
.  .el .do begin
.      :H_2.&*
.  .do end
.  .se headtext$=&amp.$htext2.
.do end
.el .if '&SCTlvl' eq '1' .do begin
.  .if '&*id.' ne '' .do begin
.      :H_3 id='&*id'.&*
.  .do end
.  .el .do begin
.      :H_3.&*
.  .do end
.do end
.el .if '&SCTlvl' eq '2' .do begin
.  .if '&*id.' ne '' .do begin
.      :H_4 id='&*id'.&*
.  .do end
.  .el .do begin
.      :H_4.&*
.  .do end
.do end
.el .if '&SCTlvl' eq '3' .do begin
.  .if '&*id.' ne '' .do begin
.      :H_5 id='&*id'.&*
.  .do end
.  .el .do begin
.      :H_5.&*
.  .do end
.do end
.in &INDlvl
:P.
.dm @section end

.dm @chap begin
.if '&*id.' ne '' .do begin
:H_1 id='&*id.'.&*
.do end
.el .do begin
:H_1.&*
.do end
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
:P.
.dm @chap end

.gt chapter add @chap att
.ga * id any
.gt section add @section att
.ga * id any

.gt sepsect add @sepsect

.gt subsect add subsect att
.ga * id any
.gt esubsect add esubsect

.gt cmsect add cmsect att
.ga * id any
.gt ecmsect add esubsect

.gt sesect add sesect
.gt esesect add esubsect
.gt seref add seref
.gt serefer add @serefer
.gt eseref add eseref

.gt keyword add kwd att
.ga * pc any

.gt italics add i
.gt hilite add hi
.gt mono add mno
.gt cmdparm add cmdparm

.gt cont add cont

.dm cont begin
.ct &*.
.dm cont end

.gt keylist add keylist
.gt ekeylist add ekeylist
.gt key add @key

.gt deflist add deflist
.gt edeflist add edeflist
.gt defitem add defitem

.gt picture del
.dm @foo begin
.dm @foo end
.if &ishelp eq 1 .do begin
.gt picture add @foo att
.ga * depth any
.ga * file any
.ga * text any
.do end
.el .do begin
.gt picture add @pict att
.ga * depth any
.ga * file any
.ga * text any
.do end

.gt period add @period cont
.gt addline add addline
.gt blankline add addline
.gt newpage add newpage
.gt break add break

.gt setlist add @setlist tagnext texterror
.gt esetlist add @esetlist tagnext texterror
.gt setcmd add @setcmd attr
.ga * short any
.ga * parm any

.gt fnlist add fnlist
.gt efnlist add efnlist
.gt func add @func
.gt efunc add efunc
.gt syntax add @syntax
.gt exmode add exmode
.gt seealso add @seealso
.gt see add @see attr
.ga * key any
.gt returns add returns
.gt ereturns add ereturns
.gt retval add retval attr
.gt eseealso add @eseealso

.gt rxxmp add rxxmp
.gt erxxmp add erxxmp
.gt rxres add @rxres
.gt rxrep add rxrep
.gt rxorig add rxorig

.gt illust add sillust
.gt eillust add eillust
.gt billust add billust

.gt sexmp add sexmp
.gt seline add seline
.gt esexmp add esexmp

.gt example add xmp
.gt eexample add exmp

.gt seecmd add seecmd

.gt exercises add exercises
.gt eexercises add eexercises

.dm eexercises begin
.dm eexercises end

.dm exercises begin
:P.
:SF font=9.
Exercises
:eSF.
:P.
.dm exercises end

.dm break begin
.br
.dm break end

.dm sillust begin
.if &ishelp eq 1 .do begin
:XMP.:SF font=5.
.do end
.el .do begin
:XMP.:SF font=5.~b
.do end
.dm sillust end

.dm eillust begin
:eSF.:eXMP.
.dm eillust end

.dm billust begin
:eSF.:eXMP.:XMP.:SF font=5.
.dm billust end

.dm sexmp begin
:ILLUST.
.se currline = 1
.dm sexmp end

.dm seline begin
.if &currline le 9 .do begin
.se *tmp2 = ' &currline.) &*.'
.do end
.el .do begin
.se *tmp2 = '&currline.) &*.'
.do end
.se *tmp = '> &*.'
&*tmp2
.se line(&currline) = &*tmp.
.sr currline = &currline + 1
:BILLUST.
.dm seline end

.dm esexmp begin
:eILLUST.
.dm esexmp end

.dm seref begin
.se sein = 0
.dm seref end

.dm eseref begin
.if &sein. eq 1 .do begin
.in -0.1i
.do end
.dm eseref end

.dm @serefer begin
.serefer &*.
.dm @serefer end

.dm segrp begin
&line(&s)
:BILLUST.
.sr s = &s + 1
.dm segrp end

.dm serefer begin
.if &sein. eq 1 .do begin
.in -0.1i
.do end
.se sein = 1
:P.
.se s = &*1
.if '&*2.' ne '' .do begin
:ITALICS.Lines &*1.-&*2.
.se *e = &*2
.do end
.el .do begin
:ITALICS.Line &*1.
.se *e = &*1
.do end
:ILLUST.
.pe on.segrp.if &s gt &*e .pe delete
:eILLUST.
.in +0.1i
.sp
.dm serefer end

.dm subsect begin
.beglevel
.if '&*id' ne '' .do begin
:SECTION id='&*id.'.&*.
.do end
.el .do begin
:SECTION.&*.
.do end
:P.
.dm subsect end

.dm esubsect begin
.endlevel
.dm esubsect end


.dm i begin
:HP1.&*.:eHP1.
.dm i end

.dm hi begin
:SF font=2.&*.:eSF.
.dm hi end

.dm mno begin
:SF font=4.&*:eSF.
.dm mno end

.dm kwd begin
.if '*pc' eq '' .do begin
.se *lc = &'lower(&*.)
.do end
.el .do begin
.se *lc = &*.
.do end
.ct
:SF font=3.&*.:eSF.
:IH1 print='&*lc.'.&*lc.
:I1.&*lc.
.dm kwd end

.dm cmdparm begin
:SF font=3.&*:eSF.
.dm cmdparm  end


.dm newpage begin
.pa
.dm newpage end

.dm addline begin
.sp
.dm addline end

.dm @period begin
.ct .li .
.dm @period end

.*
.*** definition list (script-like macros)
.*
.dm deflist begin
:DL break.
.dm deflist end

.dm defitem begin
:DT.&*.
:DD.
.dm defitem end

.dm edeflist begin
:eDL.
.dm edeflist end

.*
.*** SETLIST definitions
.*
.dm @setlist begin
.in +0.6i
.dm @setlist end
.dm @esetlist begin
.in -0.6i
.dm @esetlist end

.dm @setcmd begin
.sp
.in -0.6i
.se cmd_short='&*short.'
:cmt .dc cw ;
.'se cmd_parm='.cmdparm <&*parm.>'
.'se nocmd_long='.cmdparm no&*.'
.'se cmd_long='.cmdparm &*.'
:cmt .dc cw off
.if '&*short.' <> '' .th .do begin
.i [&*short.] &*.
.do end
.el .do begin
.if '&*parm.' <> '' .th .do begin
.i &*. <&*parm.>
.do end
.el .do begin
.i &*.
.do end
.do end
:IH1 print=Settings.Settings
:IH2 print='&*.'.&*.
:I2.&*.
:IH1 print='&*.'.&*.
:I1.&*.
.in +0.6i
.dm @setcmd end


.dm @pict begin
:FIG place=inline frame=none.
..if '&*depth' eq '' ..do begin
:GRAPHIC depth='2.73i' file='&*file..ps'.
..do end
..if '&*depth' ne '' ..do begin
:GRAPHIC depth='&*depth' file='&*file..ps'.
..do end
:FIGCAP.&*text (&*file..ps)
:eFIG.
.dm @pict end

.*
.*** editor function description
.*
.dm fnlist begin
.se fn_topic = &*.
.beglevel
.dm fnlist end

.dm efnlist begin
.se fn_topic =
.endlevel
.dm efnlist end

.dm @func begin
.func &*.
.dm @func end

.dm func begin
.@section &*.
.se func_name='&*1.'
.se *lc1 = &'lower(&*1.)
.se has_xmp=0
.if '&fn_topic' ne '' .do begin
:IH1 print='&*lc1.'.&*lc1.
:I1.&*1.
:IH1 print='&fn_topic.'.&fn_topic
:IH2 print='&*lc1.'.&*lc1.
:I2.&*lc1.
.do end
.el .do begin
:IH1 print='&*lc1.'.&*lc1.
:I1.&*lc1.
.do end
.dm func end

.dm exmp begin
.if '&keysp' eq '1' .do begin
.sp
.se keysp = 0
.do end
.if '&fneed_edl' eq '1' .do begin
:eDL
.se fneed_edl = 0
.do end
.if '&fis_indent' eq '1' .do begin
.se fis_indent = 0
.in -0.8i
.sp
.do end
.if '&*.' eq '' .se has_xmp = 0
.dm exmp end

.dm efunc begin
.if '&fis_indent' ne '1' .do begin
.sp
.do end
.exmp 1
.dm efunc end

.dm @syntax begin
.syntax &*.
.dm @syntax end

.dm syntax begin
:DL termhi=0.
:DT.Syntax:
.if '&*1' eq '*' .do begin
:DD.&*2. &*3. &*4. &*5. &*6. &*7. &*8.
.do end
.el .do begin
:DD .&*
.do end
:eDL
.'se range ='.cmdparm &*1..ct '
.'se parm1 ='.cmdparm &*3..ct '
.'se parm2 ='.cmdparm &*4..ct '
.'se parm3 ='.cmdparm &*5..ct '
.'se parm4 ='.cmdparm &*6..ct '
.'se parm5 ='.cmdparm &*7..ct '
.'se parm6 ='.cmdparm &*8..ct '
:DL termhi=0 break.
:DT.Description:
:DD.
.se fneed_edl=1
.dm syntax end

.dm xmp begin
.exmp 1
.se fis_indent = 1
.if '&has_xmp.' ne '1' .do begin
.se has_xmp = 1
Example(s):
.do end
.in +0.6i
:SF font=6.&*.:eSF.
.in +0.2i
.dm xmp end

.dm returns begin
.exmp 1
.sp
:DL termhi=0 break.
:DT.Returns:
:DD.:DL break.
.dm returns end

.dm retval begin
:DT.&*.
:DD.
.dm retval end

.dm ereturns begin
:eDL.
:eDL.
.dm ereturns end


.dm @seealso begin
.exmp 1
.se see = 0
:DL termhi=0.
:DT.See Also:
:DD.
.dm @seealso end

.dm @see begin
.if '&see.' eq '1' .do begin
.ct ,
.do end
.se see = 1
.if '&*key' ne '' .do begin
:SF font=3.
'&*.' key (Command Mode)
:eSF.
.do end
.el .do begin
:KEYWORD.&*.
.do end
.dm @see end

.dm @eseealso begin
:eDL.
.dm @eseealso end

.dm exmode begin
.exmp 1
:DL termhi=0.
:DT.Notes:
:DD.
Only valid in
:KEYWORD pc=1.EX mode
:PERIOD.
:eDL.
.dm exmode end

.*
.*** Command mode macros
.*
.dm seecmd begin
.sp
Also see the
.kwd command line
command
.kwd &*.
:PERIOD.
.dm seecmd end

.dm cmidx begin
.se cindex = '&*.'
:IH1 print='Command Mode'.Command Mode
:IH2 print='&*.'.&*.
:I2.&*.
:cmt :IH1 print='&*.'.&*.
:cmt :I1.&*.
.dm cmidx end

.dm sesect begin
:CMT .if '&sexcnt.' eq '' .do begin
:CMT .se sexcnt = 1
:CMT .do end
:CMT .el .do begin
:CMT .sr *tmp = &sexcnt + 1
:CMT .se sexcnt = &*tmp
:CMT .do end
:CMT :SUBSECT.Example &*tmp. - "&*."
:SUBSECT.Example - "&*."
.dm sesect end

.dm cmsect begin
.if '&*id' ne '' .do begin
:SUBSECT id='&*id'.&*.
.do end
.el .do begin
:SUBSECT.&*.
.do end
.cmidx &*.
.dm cmsect end

.dm keylist begin
.sp
.se fneed_edl = 0
.se done_key = 0
.dm keylist end

.dm ekeylist begin
.in -0.6i
.exmp 1
.dm ekeylist end

.dm @key begin
.key &*.
.dm @key end

.dm key begin
.se key_name='&*1.'
.se *tmp = &'lower(&*1.)

.if '&*tmp.' eq '&*1.' .do begin
.se *kn = &*1.
.do end
.el .do begin
.se *kn = &*1.z
.do end

.se has_xmp=0
.se *key = ''&*kn.' key'
.if '&*1.' = '"' .th .do begin
. . .se *key2=''""' key'
.do end
.el .do begin
. . .se *key2=''&*1.' key'
.do end
.if '&done_key.' eq '1' .do begin
.in -0.6i
.exmp 1
.do end
.se done_key = 1
:SF font=6.&*2.:eSF.
.if '&*3' ne '' .do begin
:SF font=1.&*3. &*4. &*5. &*6. &*7. &*8.:eSF.
.do end
:IH1 print='Command Mode'.Command Mode
:IH2 print='&cindex.'.&cindex.
:IH3 print="&*key2.".&*key
:I3.&*key.
:IH1 print='&cindex. (Command Mode)'.&cindex. (Command Mode)
:IH2 print="&*key2.".&*key
:I2.&*key.
.in +0.6i
.se keysp = 1
.dm key end

.*
.*** regular expression example macros
.*
.dm rxxmp begin
:MONO.&*.
:cmt .in +0.2i
:OL.
.dm rxxmp end

.dm rxorig begin
:LI.
.'se orig = '&*.'
:DL termhi=0.
:DT.String:
:DD.
:SF font=0.
&*.
:eSF.
:eDL.
.dm rxorig end

.dm rxres begin
:DL termhi=0.
:DT.Matches:
:DD.
.if '&*3.' eq '' .do begin
.if '&*2.' eq '' .do begin
&*.
.do end
.el .do begin
.if '&*1.' eq '*' .do begin
:SF font=13.&*2.:eSF
.do end
.el .do begin
:SF font=13.&*1.:eSF
.ct &*2.
.do end
.do end
.do end
.el .do begin
.if '&*1.' eq '*' .do begin
&*2.
.ct :SF font=13.&*3.:eSF
.do end
.el .do begin
&*1.
.ct :SF font=13.&*2.:eSF
.ct &*3.
.do end
.do end
:eDL.
.dm rxres end

.dm @rxres begin
.rxres &*.
.dm @rxres end

.dm rxrep begin
:DL termhi=0.
:DT.Replacement:
:DD.
.dm rxrep end
.dm erxrep begin
:eDL.
.dm erxrep end

.dm erxxmp begin
:cmt .in -0.2i
:eOL.
.sp
.dm erxxmp end
