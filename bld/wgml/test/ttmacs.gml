.dm millust begin
.if '&*' eq 'begin' .do begin
.co off
~b
.do end
.el .if '&*' eq 'end' .do begin
.co on
.do end
.el .if '&*' eq 'break' .do begin
.co on
.co off
.do end
.el .do begin
.co off
~b
&*
.co on
:P.
.do end
.dm millust end

.dm monoon begin
:SF font=4.
.dm monoon end

.dm monooff begin
:eSF.
.dm monooff end

.dm kw begin
.  .ix 'keyword' '&*'
.  .mono &*
.dm kw end

.dm kwon begin
.  .monoon
.dm kwon end

.dm kwoff begin
.  .monooff
.dm kwoff end

.dm beglevel begin
.if '&SCTlvl' eq '5' .ty too many section levels
.el .if '&SCTlvl' eq '4' :set symbol="SCTlvl" value = "5".
.el .if '&SCTlvl' eq '3' :set symbol="SCTlvl" value = "4".
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "3".
.el .if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "2".
.el .if '&SCTlvl' eq '0' :set symbol="SCTlvl" value = "1".
.pu 1 .beglevel
.dm beglevel end

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

.dm mono begin
:SF font=4.&*:eSF.
.dm mono end
.*
.dm @monobeg begin
.  :SF font=4.&*.
.dm @monobeg end
.*
.dm @monoend begin
.  :eSF.&*.
.dm @monoend end
.*
:cmt..gt MONO  add @monobeg continue
:cmt..gt eMONO add @monoend continue
.*
.dm @italbeg begin
.  :SF font=1.&*.
.dm @italbeg end
.*
.dm @italend begin
.  :eSF.&*.
.dm @italend end
.*
:CMT..gt ITAL  add @italbeg continue
:CMT..gt eITAL add @italend continue
.*
:CMT. Fix these!
.*
.dm super begin
.  :SF font=5.(&*.):eSF.
.dm super end
.*
:cmt.:SET symbol='SUPER0'   value=';.ct .super 0;.ct '.
:SET symbol='SUPER0'   value='0'.
:cmt.:SET symbol='SUPER1'   value=';.ct .super 1;.ct '.
:SET symbol='SUPER1'   value='1'.
.dm langle begin
.  :SF font=7.~<:eSF.
.dm langle end
.*
:cmt.:SET symbol='LANGLE'   value=';.ct;.langle;.ct '.
:SET symbol='LANGLE'   value=''.
.*
.dm rangle begin
.  :SF font=7.~>:eSF.
.dm rangle end
.*
:cmt.:SET symbol='RANGLE'   value=';.ct;.rangle;.ct '.
:SET symbol='RANGLE'   value=''.
.*
.dm @msemi begin
.  :SF font=4.;:eSF.&*.
.dm @msemi end
.*
.gt MSEMI add @msemi continue
.*

.dm endlevel begin
.if '&SCTlvl' eq '1' :set symbol="SCTlvl" value = "0".
.el .if '&SCTlvl' eq '2' :set symbol="SCTlvl" value = "1".
.el .if '&SCTlvl' eq '3' :set symbol="SCTlvl" value = "2".
.el .if '&SCTlvl' eq '4' :set symbol="SCTlvl" value = "3".
.el .if '&SCTlvl' eq '5' :set symbol="SCTlvl" value = "4".
.pu 1 .endlevel
.dm endlevel end

.dm begbull begin
.sr tmplvl=&WDWlvl-5
.cp &tmplvl
.if &e'&dohelp eq 0 .do begin
.   .if '&*1' eq '$compact' .do begin
:CMT..   :UL compact
.   .do end
.   .el .do begin
:CMT..   :UL
.   .do end
.do end
.el .do begin
.   .if '&*1' eq '$compact' .do begin
:CMT..   :ZUL compact
.   .do end
.   .el .do begin
:CMT..   :ZUL
.   .do end
.do end
.dm begbull end

.dm bull begin
.if &e'&dohelp eq 0 .do begin
:CMT.:LI
.do end
.el .do begin
:CMT.:ZLI
.do end
.dm bull end

.dm endbull begin
.if &e'&dohelp eq 0 .do begin
:CMT.:eUL.
.do end
.el .do begin
:CMT.:ZeUL.
.do end
.dm endbull end

.gt italics add i
.dm i begin
:HP1.&*.:eHP1.
.dm i end

.dm bxt begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .if '&*2' ne '' .sr *t0=1+&*2
.   .if '&*3' ne '' .sr *t1=1+&*3
.   .if '&*4' ne '' .sr *t2=1+&*4
.   .if '&*5' ne '' .sr *t3=1+&*5
.   .if '&*6' ne '' .sr *t4=1+&*6
.   .if '&*7' ne '' .sr *t5=1+&*7
.   .if '&*8' ne '' .sr *t6=1+&*8
.   .if '&*9' ne '' .sr *t7=1+&*9
.   .if '&*10' ne '' .sr *t8=1+&*10
.   .cp &*tmplvl
.   .se $$bextrb=&*2
.   .se $$bextre=&*3
.   .xtxmp begin &*2 &*3
.   .in 0
.   .tb set \
.   .tb &*t0 &*t2 &*t3 &*t4 &*t5 &*t6 &*t7 &*t8 &*t1
:cmt..   .bx on &*2 &*4 &*5 &*6 &*7 &*8 &*9 &*10 &*3
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.el .if '&*' eq 'off' .do begin
.   .tb set
.   .tb
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
:cmt..   .bx off
.   .in &INDlvl
.   .xtxmp end &$$bextrb &$$bextre
.do end
.el .do begin
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
:cmt..   .bx
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.dm bxt end
.*
.dm box begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .if '&*2' ne '' .sr *t0=1+&*2
.   .if '&*3' ne '' .sr *t1=1+&*3
.   .if '&*4' ne '' .sr *t2=1+&*4
.   .if '&*5' ne '' .sr *t3=1+&*5
.   .if '&*6' ne '' .sr *t4=1+&*6
.   .if '&*7' ne '' .sr *t5=1+&*7
.   .if '&*8' ne '' .sr *t6=1+&*8
.   .if '&*9' ne '' .sr *t7=1+&*9
.   .if '&*10' ne '' .sr *t8=1+&*10
.   .xtnada
.   .in 0
.   .tb set \
.   .tb &*t0 &*t1 &*t2 &*t3 &*t4 &*t5 &*t6 &*t7 &*t8
:cmt..   .bx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.el .if '&*' eq 'off' .do begin
.   .tb set
.   .tb
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
:cmt..   .bx off
.   .in &INDlvl
.do end
.el .do begin
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
:cmt..   .bx
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.dm box end
.*
.dm xtnada begin
:SF font=14.~n:eSF.
.dm xtnada end
.*
.dm sf1 begin
:SF font=1.&*.
.dm sf1 end
.*
.dm sf2 begin
:SF font=2.&*.
.dm sf2 end
.*
.dm sf4 begin
:SF font=4.&*.
.dm sf4 end
.*
.dm sf7 begin
:SF font=7.&*.
.dm sf7 end
.*
.dm esf begin
:eSF.&*.
.dm esf end
.*
.dm xtxmp begin
.se shadestart=&$pagelm.+(&*2.*100)+150
.se shadeleft=&$pagelm.+(&*2.*100)+150
.se shaderight=&$pagelm.+(&*3.*100)+50
.if '&*1' eq 'begin' .do begin
.  .se shadetype=(start)
.  .se shadetop='-270'
.  .se shadebot='270'
.  .se shadels=(yes)
.do end
.el .do begin
.  .se shadetype=(end)
.  .se shadetop='-20'
.  .se shadebot='20'
.  .se shadels=(yes)
.do end
.se shadefont=(@fs0)
:cmt..oc &shadebot. &shadetop. &shadeleft. &shaderight. &shadestart. &shadefont. &shadetype. &shadels. set_shadevars
:SF font=13.~n:eSF.
.dm xtxmp end
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
..dm boxdef begin
.  .keep begin
.  ..sr rdist  = &SYSIN.+1
.  ..sr boxcmd = &rdist.
.  ..sr tabcmd = ''
..dm boxdef end
.*
..dm boxcol begin
.  ..sr *tab   = ''
.  ..sr *width = &*1
.  ..if &*0 = 1 ..th ..do begin
.  .  ..if &rdist. ne 0 ..th ..do begin
.  .  .  ..sr *tab = &rdist.+1
.  .  ..do end
.  .  ..sr rdist = &*width.-1
.  ..do end
.  ..el ..if &*0 = 2 ..th ..do begin
.  .  ..if '&*2' = 'c' ..th ..do begin
.  .  .  ..sr *centre = (&*width.+1)/2
.  .  .  ..sr *tab    = &rdist.+&*centre.
.  .  .  ..sr *tab    = '&*tab.c'
.  .  .  ..sr rdist   = &*width.-&*centre.
.  .  ..do end
.  .  ..el ..do begin
.  .  .  ..ty Boxcol 2nd parameter error in "&*"
.  .  ..do end
.  ..do end
.  ..el ..do begin
.  .  ..ty Boxcol parameter error in "&*"
.  ..do end
.  ..sr boxcmd = '&boxcmd. +&*width.'
.  ..if '&*tab.' ne '' ..th ..do begin
.  .  ..if '&tabcmd.' eq '' ..th ..do begin
.  .  .  ..sr tabcmd = '&*tab.'
.  .  ..do end
.  .  ..el ..do begin
.  .  .  ..sr tabcmd = '&tabcmd. +&*tab.'
.  .  ..do end
.  ..do end
..dm boxcol end
.*
..dm boxbeg begin
:CMT. .  ..br
:CMT. .  ..li Final settings
:CMT. .  ..br
:CMT. .  ..li ..tb &tabcmd
:CMT. .  ..br
:CMT. .  ..li ..bx &boxcmd
:CMT. .  ..br
.  ..tb     &tabcmd
.  ..tb set &tabchar
:cmt..  ..bx on  &boxcmd
..dm boxbeg end
.*
..dm boxline begin
:cmt..  ..bx
..dm boxline end
.*
..dm boxend begin
:cmt..  ..bx off
.  ..tb
.  ..tb set
.  ..sr boxcmd off
.  ..sr tabcmd off
.  ..sr rdist  off
.  .keep end
.dm boxend end
.*
.dm smonoon begin
:SF font=5.
.dm smonoon end
.*
.dm smonooff begin
:eSF.
.dm smonooff end
.*
.dm numboxbeg begin
.  ..br
.  .if &e'&nobox eq 0 .do begin
.  .boxdef
.  .boxcol &numboxcol1.
.  .boxcol &numboxcol2.
.  .boxbeg
$Macro: :SF font=4.&*:eSF.$Value
.  .do end
.  .el .do begin
.  .millust begin
Macro: &*       Value
.  .do end
.  .boxline
.dm numboxbeg end
.*
.dm numterm begin
.  .if &e'&nobox eq 0 .do begin
$&*1.$:SF font=4.&*2:eSF.
.  .do end
.  .el .do begin
&*1             &*2
.  .do end
.dm numterm end
.*
.dm ansi begin
.  .numterm 'ISO' '&*'
.dm ansi end
.*
.dm noansi begin
.  .if &e'&nobox eq 0 .do begin
$ISO$no value specified
.  .do end
.  .el .do begin
ISO             no value specified
.  .do end
.dm noansi end
.*
.dm watcomc begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wc286.' &*1
.  ..do end
.dm watcomc end
.*
.dm c386 begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wc386.' &*1
.  ..do end
.dm c386 end
.*
.dm bothc begin
.  ..if '&target' eq 'PC'  or  '&target' eq 'PC 370' ..th ..do begin
.  .  .numterm '&wcboth.' &*1
.  ..do end
.dm bothc end
.*
.dm wlooc begin
.  ..if '&target' eq 'PC 370'  or  '&target' eq '370' ..th ..do begin
.  .  .numterm '&wlooc.' &*1
.  ..do end
.dm wlooc end
.*
.dm numboxend begin
.  .if &e'&nobox eq 0 .do begin
.  .boxend
.  .do end
.  .el .do begin
.  .millust end
.  .do end
.dm numboxend end
.*


