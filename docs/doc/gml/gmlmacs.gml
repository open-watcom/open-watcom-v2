.*
.* Open Watcom Documentation Macros & Variables
.*
:INCLUDE file='SYMBOLS'.
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
.dm v105 begin
.if &version eq 105 .do begin
&*
.do end
.dm v105 end
.*
.dm v110 begin
.if &version eq 110 .do begin
&*
.do end
.dm v110 end
.*
.dm sy begin
:SF font=1.&*:eSF.
.dm sy end
.*
.dm ev begin
:SF font=2.&*:eSF.
.ix 'environment variables' '&*'
.ix '&* environment variable'
.dm ev end
.*
.dm kw begin
:SF font=3.&*:eSF.
.ix '&*'
.dm kw end
.*
.dm kwm begin
:SF font=4.&*:eSF.
.ix '&*'
.dm kwm end
.*
.dm id begin
:SF font=4.&*:eSF.
.dm id end
.*
.dm book begin
:SF font=3.&*:eSF.
.dm book end
.*
.dm fie begin
:SF font=4.&*.
.dm fie end
.*
.dm fi begin
.fie &*.
:eSF.
.dm fi end
.*
.dm fname begin
.fie &*.
.dm fname end
.*
.dm efname begin
.if &l'&*. eq 0 .do begin
.   ..ct
:eSF.
.do end
.el .do begin
:eSF.&*.
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
.if '&*scale.' eq '' .do begin
.   .se *scale=100
.do end
:FIG place=inline frame=none.
.if '&*file.' ne '' .do begin
.   .if &e'&dohelp eq 1 .do begin
:HBMP '&*file..bmp' c
.   .do end
.   .el .do begin
:GRAPHIC depth='&*depth.i' scale=&*scale. file='&*file..ps'.
.   .do end
.   .se *figttl=&'substr(&*,&'pos(&*file.,&*)+&'length(&*file.)+1)
.do end
:FIGCAP.&*figttl.
:eFIG.
.dm figure end
.gt figure add figure att
.ga * depth any
.ga * file any
.*
.dm pict begin
:FIG place=inline frame=none.
:cmt. .in +&INDlvl.
.if '&*depth' eq '' .do begin
:cmt. :GRAPHIC depth='2.73i' file='&*file..ps'.
:cmt. :GRAPHIC depth='2.70i' file='&*file..ps'.
:GRAPHIC depth='2.65i' file='&*file..ps'.
.do end
.if '&*depth' ne '' .do begin
:GRAPHIC depth='&*depth' file='&*file..ps'.
.do end
:FIGCAP.&*text
:cmt. .in -&INDlvl.
:eFIG.
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
.if &e'&dohelp eq 1 .do begin
    .imbmp &*xoff. &*depth. &*file. &*text.
.do end
.el .do begin
    .imposts &*xoff. &*depth. &*file. &*text.
.do end
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
.dm imbmp begin
:cmt. :HBMP '&*file..bmp' c
:P.
.ce;.us *** &*file..bmp GOES HERE ***
:P.
.if '&*text' ne '' .do begin
.   .us &*text.
.do end
:eFIG.
.dm imbmp end
.*
.dm imposts begin
:FIG place=inline frame=none.
.if '&*xoff' ne '' .do begin
.   .:GRAPHIC depth='&*depth.' xoff='&*xoff.' file='&*file..eps'.
.do end
.el .do begin
.   .:GRAPHIC depth='&*depth.' file='&*file..eps'.
.do end
.if '&*text' ne '' .do begin
.   .:FIGCAP.&*text.
.do end
:eFIG.
.dm imposts end
.*
.dm @hdref begin
.if '&format.' eq '8.5x11a' .do begin
:HDREF refid='&*refid.' page=no.&*
.do end
.el .do begin
.   .if '&*page.' eq '' .do begin
:HDREF refid='&*refid.'.&*
.   .do end
.   .el .do begin
:HDREF refid='&*refid.' page=&*page.&*
.   .do end
.do end
.dm @hdref end
.gt HDREF add @HDREF continue attributes
.ga * REFID any
.ga * PAGE uppercase value NO
.ga * *    value YES
.*
.dm abox begin
.if '&*1' eq 'begin' or '&*1' eq 'on' .do begin
.   .box
.do end
.el .if '&*1' eq 'end' or '&*1' eq 'off' .do begin
.   .ebox
.do end
.el .do begin
.   .ebox
.   .box
.do end
.dm abox end
.*
.dm mkbx begin
.if &e'&dohelp eq 0 .do begin
.   .bx &*
.do end
.el .do begin
.   .abox &*
.do end
.dm mkbx end
.*
.dm mbox begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &*tmplvl.
.   .mkbx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   :XMP.
.do end
.el .if '&*' eq 'off' .do begin
.   :eXMP.
.   .mkbx off
.do end
.el .if '&*' eq 'begin' .do begin
.   :P.
.   .cp &*tmplvl.
.   .se *lmargin=&sysin.+1
.   .mkbx on &*lmargin. &rmargin.
.   :XMP.
.do end
.el .if '&*' eq 'end' .do begin
.   :eXMP.
.   .mkbx off
.do end
.el .do begin
.   .mkbx
.do end
.dm mbox end
.*
.dm cbox begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &*tmplvl.
.   .mkbx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   :XMP.:SF font=5.
.do end
.el .if '&*' eq 'off' .do begin
.   :eSF.:eXMP.
.   .mkbx off
.do end
.el .if '&*' eq 'begin' .do begin
.   :P.
.   .cp &*tmplvl.
.   .se *lmargin=&sysin.+1
.   .mkbx on &*lmargin. &rmargin.
.   :XMP.:SF font=5.
.do end
.el .if '&*' eq 'end' .do begin
.   :eSF.:eXMP.
.   .mkbx off
.do end
.el .do begin
.   .mkbx
.do end
.dm cbox end
.*
.dm mbigbox begin
.if '&*1' eq 'end' .do begin
.   .if '&format' eq '8.5x11a' .do begin
.   :eFIG.
.   .do end
.   .el .if '&format' eq 'help' .do begin
.   :eFIG.
.   .do end
.   .el .do begin
.   :eSF.:eXMP.
.   .bx off
.   .do end
.do end
.el .do begin
.   :P.
.   .se *tmplvl=&WDWlvl-3
.   .cp &*tmplvl.
.   .if '&format' eq '8.5x11a' .do begin
.   :FIG frame=box place=inline.
.   .sk 1
.   .do end
.   .el .if '&format' eq 'help' .do begin
.   :FIG frame=box place=inline.
.   .sk 1
.   .do end
.   .el .do begin
.   .se *lmargin=&sysin.+1
.   .bx on &*lmargin. &rmargin.
.   :XMP.:SF font=3.
.   .do end
.do end
.dm mbigbox end
.*
.dm embigbox begin
.if '&format' eq '8.5x11a' .do begin
:eFIG.
.do end
.el .if '&format' eq 'help' .do begin
:eFIG.
.do end
.el .do begin
:eSF.:eXMP.
.bx off
.do end
.dm embigbox end
.*
.dm syntax begin
:XMP.:SF font=3.~b
.dm syntax end
.*
.dm esyntax begin
:eSF.:eXMP.
.dm esyntax end
.*
.dm syntaxbrk begin
.esyntax
.syntax
.dm syntaxbrk end
.*
.dm list begin
:XMP.:SF font=3.
.dm list end
.*
.dm elist begin
:eSF.:eXMP.
.dm elist end
.*
.dm synote begin
.begnote
.if &e'&dohelp eq 0 .do begin
:DTHD.where
:DDHD.description:
.do end
.el .do begin
:ZDT.:SF font=2.where:eSF.
:ZDD.:SF font=2.description:eSF.
.do end
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
.sr SCTlvl=4
:cmt. :DL termhi=3 tsize=10.
:cmt. :DTHD.Option:
:cmt. :DDHD.Description:
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
:cmt. :DT.&*name.&*
:cmt. :DD.
.dm opt end
.gt opt add opt attr
.ga * refid any
.ga * name any
.*
.dm eoptlist begin
.sr SCTlvl=&OLDlvl.
:cmt. :eDL.
.dm eoptlist end
.gt eoptlist add eoptlist
.*
.dm optref begin
.if &e'&dohelp eq 0 .do begin
(see :HDREF refid='&*refid'.)&*
.do end
.el .do begin
(see :ZHDREF refid='&*refid'.)&*
.do end
.sr $&*refid.=1
.dm optref end
.gt optref add optref attr
.ga * refid any
.*
.dm hotlink begin
.if &e'&dohelp eq 0 .do begin
is described :HDREF refid='&*refid'.
.do end
.el .do begin
:ZHDREF refid='&*refid'.
.do end
.sr $&*refid.=1
.dm hotlink end
.gt hotlink add hotlink attr
.ga * refid any
.*
.dm contents begin
:P.
:SF font=4.
.dm contents end
.*
.dm econtents begin
:eSF.
.dm econtents end
.*
.dm hint begin
:P.
.cp &WDWlvl
.se *lmargin=&sysin.+1
.bx on &*lmargin. &rmargin.
.in +2
:SF font=3.Hint::eSF.
.dm hint end
.*
.dm ehint begin
.in -2
.bx off
.dm ehint end
.*
.dm remark begin
:P.
.sr tmplvl=&WDWlvl-3
.cp &tmplvl
.se *lmargin=&sysin.+1
.bx on &*lmargin. &rmargin.
.in +2
:SF font=3.Note::eSF.
.dm remark end
.*
.dm eremark begin
.in -2
.bx off
.dm eremark end
.*
.dm warn begin
:P.
.cp &WDWlvl
.se *lmargin=&sysin.+1
.bx on &*lmargin. &rmargin.
.in +2
:SF font=3.WARNING!:eSF.
.dm warn end
.*
.dm ewarn begin
.in -2
.bx off
.dm ewarn end
.*
.dm exam begin
.  .if '&*1' eq 'begin' .do begin
.  .  .if '&*2' ne '' .do begin
.  .  .  .se *tmplvl=3+&*2
.  .  .  .cp &*tmplvl
.  .  .do end
.  .  .el .do begin
.  .  .  .cp &WDWlvl
.  .  .do end
.  .  :P.:HP1.Example&*3::eHP1.:XMP.
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  :eXMP.:XMP.~b
.  .do end
.  .el .do begin
.  .  .cp &WDWlvl;:P.:HP1.Example::eHP1.:XMP.
.  .  &*
.  .  :eXMP.:PC.
.  .do end
.dm exam end
.*
.dm tinyexam begin
.  .if '&*1' eq 'begin' .do begin
.  .  .if '&*2' ne '' .do begin
.  .  .  .cp &*2
.  .  .do end
.  .  .el .do begin
.  .  .  .cp &WDWlvl
.  .  .do end
.  .  :P.:HP1.Example&*3::eHP1.:XMP.:SF font=5.
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  :eSF.:eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  :eSF.:eXMP.:XMP.:SF font=5.
.  .do end
.  .el .do begin
.  .  .cp &WDWlvl;:P.:HP1.Example::eHP1.:XMP.:SF font=5.
.  .  &*
.  .  :eSF.:eXMP.:PC.
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
.sr tmplvl=&WDWlvl.-3
.cp &tmplvl
:DL tsize=3 termhi=2 break.
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
.dm initstep end
.*
.dm step begin
.sr stplvl=&stplvl.+1
.if &stplvl. eq 0 .do begin
:DT.~b
.do end
.el .do begin
:DT.(&stplvl.)
.do end
:DD.
:SF font=2.&*
:eSF.
.dm step end
.*
.dm orstep begin
.np
or
:DT.~b
:DD.
:SF font=2.&*
:eSF.
.dm orstep end
.*
.dm result begin
:P.
.dm result end
.*
.dm endstep begin
:eDL.
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
.   .if '&dotarget' eq 'os2' .do begin
.   .   .se pfx$='&* *'
.   .do end
.   .el .do begin
.   .   .se pfx$='&* '
.   .do end
.do end
.el .do begin
:helppfx.
.se pfx$=''
.do end
.pu 1 .helppref &*
.do end
.dm helppref end
.*
.se bookno=0
.se ixlvl=0
.*
.dm ixbook begin
.if &ixlvl. eq 2 .do begin
.   .endlevel
.   .se ixlvl=0
.do end
.se bookttl$=`&*`
.se bookno=&bookno.+1
.chap &bookttl$.
.np
The following pages contain cross-references to the
.book &*..
.dm ixbook end
.*
.dm ixhelp begin
.if &ixlvl. eq 2 .do begin
.   .endlevel
.   .se ixlvl=0
.do end
.se booknam$=`&*`
.im &booknam$..MIX
.dm ixhelp end
.*
.dm ixchap begin
.if &ixlvl. eq 2 .do begin
.   .endlevel
.do end
.se sectttl$=`&*`
.section &sectttl$.
.se ixlvl=1
.dm ixchap end
.*
.dm ixsect begin
.if &ixlvl. eq 1 .do begin
.   .beglevel
.   .se ixlvl=2
.do end
.se sectttl$=`&*`
.section &sectttl$. [&bookno.]
.dm ixsect end
.*
.dm ixsectid begin
.* placeholder, pass info for external HTML Help processing
.dm ixsectid end
.*
.dm ixline begin
.se *iwrd="&*1"
.se *iwrd="&'strip(&*iwrd.,'T',',')"
.se *iwrd="&'strip(&*iwrd.,'T','.')"
.if ›&*3› ne ›› .do begin
.   .sr *ixstr="&*iwrd., &*2, &*3"
.do end
.el .if ›&*2› ne ›› .do begin
.   .sr *ixstr="&*iwrd., &*2"
.do end
.el .do begin
.   .sr *ixstr="&*iwrd."
.do end
:P.
.ix &*
Information on
.if &'pos('"',&sectttl$) ne 0 .do begin
.   .if &'pos("'",&sectttl$) ne 0 .do begin
.   .   .se sectttl$=&'translate(&sectttl$.,'"',"'")
.   .do end
:FLINK file='&booknam$.' hd='&pfx$.&sectttl$.'.&*ixstr.:EFLINK.
.do end
.el .do begin
:FLINK file="&booknam$." hd="&pfx$.&sectttl$.".&*ixstr.:EFLINK.
.do end
can be found in the section entitled
.bd "&sectttl$."
in the
.us "&bookttl$.".
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
.dm refalso begin
.if &e'&dohelp eq 0 .do begin
See :HDREF refid='&*'..
.do end
.el .do begin
See :ZHDREF refid='&*'..
.do end
.dm refalso end

.dm mi begin
:SF font=3.&*:eSF.
.dm mi end

.dm mm begin
:SF font=3.&*:eSF.
.ix '&* menu'
.dm mm end

.dm menuref begin
.mi &*1
from the
.mi &*2
menu
.dm menuref end

.dm popup begin
.ix '&* menu item'
.mi &*
from the pop-up menu
.dm popup end

.dm dot begin
.ct .li .
.dm dot end

.dm cmddef begin
.section &*
.ix 'command' '&*'
.ix '&* command'
.dm cmddef end

.dm wnddef begin
&*
.ix 'window' '&*'
.ix '&*' 'window'
.dm wnddef end

.dm optdef begin
.note &*
.ix 'options' '&*'
.ix '&* option'
.dm optdef end
.*
.* End Useful macros created by John for the debugger book.
.*
