.dm stmt begin
.   .pa
.   :set symbol="headtext$" value="&*".
.dm stmt end
.*
.dm statemnt begin
.   .stmt &*
.   .im S&*1
.dm statemnt end
.*
.dm synote begin
.   .if '&*' eq '' .do begin
.   .   .begnote $setptnt 4
.   .do end
.   .el .do begin
.   .   .begnote $setptnt &*
.   .do end
.   .if &e'&dohelp eq 0 .do begin
.   :DTHD.where:
.   :DDHD.~b
.   .do end
.   .el .do begin
.   :ZDTHD.where:
.   :ZDDHD.~b
.   .do end
.dm synote end
.*
.dm mnote begin
.   .note &*
.dm mnote end
.*
.dm exam begin
.  .if '&*1' eq 'begin' .do begin
.  .  .if '&*2' ne '' .do begin
.  .  .  .cp &*2
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
.dm nextexam begin
.  .if '&*1' eq 'begin' .do begin
.  .  :XMP.
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  :eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  :eXMP.:XMP.
.  .do end
.  .el .do begin
.  .  :XMP.
.  .  &*
.  .  :eXMP.:PC.
.  .do end
.dm nextexam end
.*
.dm begexten begin
.   .cp &WDWlvl
.   :P.
~~~~~~~~~~~~~~~~~~~~~~~extension~~~~~~~~~~~~~~~~~~~~~~~
.   .br
.   :I1.extension
.dm begexten end
.*
.dm endexten begin
.   .br
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.dm endexten end
.*
.dm id begin
.   .mono &*
.dm id end
.*
.dm kw begin
.   .se $$kw="&'strip(&*,'T',',')"
.   .se $$kw="&'strip(&$$kw.,'T','.')"
.   :I1.&$$kw.
.   :SF font=6.&*:eSF.
.dm kw end
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
:cmt..   .cp &*tmplvl
.   .xtnada
.   .in 0
.   .tb set \
.   .tb &*t0 &*t1 &*t2 &*t3 &*t4 &*t5 &*t6 &*t7 &*t8
.   .bx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
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
.   .bx off
.   .in &INDlvl
.do end
.el .do begin
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
.   .bx
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.dm box end
.*
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
.   .bx on &*2 &*4 &*5 &*6 &*7 &*8 &*9 &*10 &*3
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
.   .bx off
.   .in &INDlvl
.   .xtxmp end &$$bextrb &$$bextre
.do end
.el .do begin
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeoff
.   .do end
.   .bx
.   .if &e'&dohelp eq 1 .do begin
.   .   .codeon
.   .do end
.do end
.dm bxt end
.*
.dm mbox begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &*tmplvl
.   .xtnada
.   .bx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   :XMP.:SF font=4.
.do end
.el .if '&*' eq 'off' .do begin
.   :eSF.:eXMP.
.   .bx off
.do end
.el .if '&*' eq 'begin' .do begin
.   .cp &*tmplvl
.   .se lmargin=&sysin+1
.   .xtnada
.   .bx on &lmargin &rmargin
.   .sk 1
.   :XMP.:SF font=6.
.do end
.el .if '&*' eq 'end' .do begin
.   :eSF.:eXMP.
.   .sk 1
.   .bx off
.do end
.el .do begin
.   .bx
.do end
.dm mbox end
.*
.dm mext begin
.se *tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &*tmplvl
.   .se $$mextrb=&*2
.   .se $$mextre=&*3
.   .xtxmp begin &*2 &*3
.   .bx on &*2 &*4 &*5 &*6 &*7 &*8 &*9 &*10 &*3
.   :XMP.:SF font=4.
.do end
.el .if '&*' eq 'off' .do begin
.   :eSF.:eXMP.
.   .bx off
.   .xtxmp end &$$mextrb &$$mextre
.do end
.el .if '&*' eq 'begin' .do begin
.   .cp &*tmplvl
.   .se lmargin=&sysin+1
.   .se $$mextrb=&lmargin
.   .se $$mextre=&rmargin
.   .xtxmp begin &$$mextrb &$$mextre
.   .bx on &lmargin &rmargin
.   .sk 1
.   :XMP.:SF font=6.
.do end
.el .if '&*' eq 'end' .do begin
.   :eSF.:eXMP.
.   .sk 1
.   .bx off
.   .xtxmp end &$$mextrb &$$mextre
.do end
.el .do begin
.   .bx
.do end
.dm mext end
.*
.if &e'&dohelp eq 0 .do begin
.dm xt begin
.   .if '&*' eq 'on' .do begin
.   .   .xtpara begin
.   .do end
.   .el .if '&*' eq 'off' .do begin
.   .   .xtpara end
.   .do end
.   .el .if '&*' eq 'slim' .do begin
.   .   .ty No more slim
.   .do end
.   .el .if '&*' eq 'begin' .do begin
.   .   .xtpara begin
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .   .xtpara end
.   .do end
.   .el .do begin
.   .   .xtpara begin
.   .   &*
.   .   .xtpara end
.   .do end
.dm xt end
.*
.dm xtpara begin
.se shadestart=(current)
.se shadeleft=&$pagelm.+(&sysin.*100)+200
.se shaderight=&$pagerm.+200
.se shadetop='40'
.se shadebot='40'
.if '&*' eq 'begin' .do begin
.  .se shadetype=(start)
.  .se shadels=(yes)
.do end
.el .do begin
.  .se shadetype=(end)
.  .se shadels=(no)
.do end
.se shadefont=(@fs0)
.oc &shadebot. &shadetop. &shadeleft. &shaderight. &shadestart. &shadefont. &shadetype. &shadels. set_shadevars
:SF font=13.~n:eSF.
.dm xtpara end
.*
.dm xtxmp begin
.se shadestart=&$pagelm.+(&*2.*100)+150
.se shadeleft=&$pagelm.+(&*2.*100)+150
.se shaderight=&$pagelm.+(&*3.*100)+150
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
.oc &shadebot. &shadetop. &shadeleft. &shaderight. &shadestart. &shadefont. &shadetype. &shadels. set_shadevars
:SF font=13.~n:eSF.
.dm xtxmp end
.*
.dm xtnada begin
:SF font=14.~n:eSF.
.dm xtnada end
.do end
.*
.if &e'&dohelp eq 1 .do begin
.dm xt begin
.   .if '&*' eq 'on' .do begin
.   .do end
.   .el .if '&*' eq 'off' .do begin
.   .do end
.   .el .if '&*' eq 'slim' .do begin
.   .do end
.   .el .if '&*' eq 'begin' .do begin
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .do end
.   .el .do begin
.   .   &*
.   .do end
.dm xt end
.*
.dm xtpara begin
.dm xtpara end
.*
.dm xtxmp begin
.dm xtxmp end
.*
.dm xtnada begin
.dm xtnada end
.do end
