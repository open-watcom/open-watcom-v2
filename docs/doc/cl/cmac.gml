.*
.* This is the same as ".illust", except concatenation (.co) is not off.
.* The default font is italic.
.* This is used for illustrations where multiple fonts are used.
.*
.dm cillust begin
.  .if '&*' eq 'begin' .do begin
.  .  .keep begin
.  .  :P.
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .do end
.  .el .if '&*' eq 'end' .do begin
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  .keep end
.  .  :P.
.  .do end
.  .el .if '&*' eq 'break' .do begin
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  .keep break
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .do end
.  .el .do begin
.  .  :P.
.  .  .keep begin
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .  &*
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  :P.
.  .  .keep end
.  .do end
.dm cillust end
.*
.* Inside a .cillust, skip to a new line and put out a bold "or",
.* then skip to another new line.
.*
.dm cor begin
.  :eSF.
.  .br
:HP2.&SYSRB.&SYSRB.or:eHP2.
.  .br
.  :SF font=1.
.dm cor end
.*
.* Inside a .cillust, break to a new line.
.* Required to get around a GML bug where font changes back to 0.
.*
.dm cbr begin
.  :eSF.
.  .br
.  :SF font=1.
.dm cbr end
.*
.* Inside a .cillust, skip a line.
.* Required to get around a GML bug where font changes back to 0.
.*
.dm csk begin
.  :eSF.
.  ..sk 1 c
.  :SF font=1.
.dm csk end
.*
.* This is the same as .illust, except underlining (.us) is turned on.
.*
.dm uillust begin
.if '&*' eq 'begin' .do begin
.  .  .keep begin
.  .  ..sk 1 c
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .  ..co off
.do end
.el .if '&*' eq 'end' .do begin
.  .  ..co on
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  ..sk 1 c
.  .  .keep end
.do end
.el .if '&*' eq 'break' .do begin
.  .  ..co on
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  .keep break
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .  ..co off
.do end
.el .do begin
.  .  ..sk 1 c
.  .  ..in +&INDillust. *
.  .  :SF font=1.
.  .  ..co off
.  .  ..li &*
.  .  ..co on
.  .  :eSF.
.  .  ..in -&INDillust. *
.  .  ..sk 1 c
.do end
.dm uillust end
.*
.* Inside a .uillust, skip to a new line and put out a bold "or",
.* then skip to another new line.
.*
.dm uor begin
.  :eSF.
.  .bd  or
.  :SF font=1.
.dm uor end
.*
.* Indent text for a while.
.*
.dm discuss begin
.  ..sr *indent=&INDillust.+&INDillust.
.  ..if '&*' = 'begin' ..th ..do begin
.  .  ..in +&*indent. *
.  .  ..br
.  ..do end
.  ..el ..if '&*' = 'end' ..th ..do begin
.  .  ..in -&*indent. *
.  .  ..br
.  ..do end
.  ..el ..do begin
.  .  ..in -&*indent. *
.  .  .br
&*
.  .  ..in +&*indent. *
.  .  .br
.  ..do end
.dm discuss end
.*
.* Show a keyword in mono, and add an index reference for it.
.*
.dm kw begin
.  .ix 'keyword' '&*'
.  .mono &*
.dm kw end
.*
.dm kwix begin
.  .ix 'keyword' '&*'
.dm kwix end
.*
.dm kwfont begin
.  .mono &*
.dm kwfont end
.*
.dm kwon begin
.  .monoon
.dm kwon end
.*
.dm kwoff begin
.  .monooff
.dm kwoff end
.*
.* Show a preprocessor keyword in mono, and add an index reference for it.
.*
.dm kwpp begin
.  .ix 'preprocessor directive' '&*'
.  .mono &*
.dm kwpp end
.*
.dm ppix begin
.  .ix 'preprocessor directive' '&*'
.dm ppix end
.*
.dm ppfont begin
.  .mono &*
.dm ppfont end
.*
.* Show a constant name in mono.
.*
.dm kwconst begin
.  .mono &*
.dm kwconst end
.*
.* Show a macro in mono, and add an index reference for it.
.*
.dm mkw begin
.  .ix '&* predefined macro'
.  .ix 'macro' 'predefined' '&*'
.  .mono &*
.dm mkw end
.*
.dm mkwfont begin
.  .mono &*
.dm mkwfont end
.*
.dm mkwix begin
.  .ix 'macro' 'predefined' '&*'
.dm mkwix end
.*
.* Show a library headername, and add an index reference for it.
.*
.dm hdr begin
.  .ix 'header' '&*'
.  .mono &*
.dm hdr end
.*
.* Show a library function, and add an index reference for it.
.*
.dm libfn begin
.  .ix 'library function' '&*'
.  .mono &*
.dm libfn end
.*
.* For shaded areas of the book.
.*
.dm shade begin
.   .se *tmplvl=&WDWlvl-3
.   .if '&*' eq 'begin' .do begin
.   .   .sr shadeing=1
.   .   .cp &*tmplvl
.   .   .se lmargin=&sysin+1
.   .   .se $$shaderb=&lmargin
.   .   .se $$shadere=&rmargin
.   .   :I1.implementation-specific behavior
.   .   .br
.   .   .keep begin
.   .   .xtxmp begin &$$shaderb &$$shadere
.   .   :LQ.
.   .   .if &e'&dohelp eq 0 .do begin
.   .   .do end
.   .   .el .do begin
.   .   .   .pp
.   .   .do end
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .   .sr shadeing=0
.   .   .br
.   .   :eLQ.
.   .   .xtxmp end &$$shaderb &$$shadere
.   .   .if &e'&dohelp eq 0 .do begin
.   .   .   .sk -1
.   .   .do end
.   .   .el .do begin
.   .   .   .bd off
.   .   .do end
.   .   .keep end
.   .do end
.dm shade end
.*
.dm shadeold begin
.   .if '&*' eq 'begin' .do begin
.   .   .sr shadeing=1
.   .   :I1.implementation-specific behavior
.   .   .br
.   .   .keep begin
.   .   :LQ.
.   .   .if &e'&dohelp eq 0 .do begin
.   .   .   ..sk 2
.   .   .   :SF font=15.~n:eSF.
.   .   .   ..sk -1
.   .   .do end
.   .   .el .do begin
.   .   .   .pp
.   .   .do end
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .   .sr shadeing=0
.   .   .br
.   .   .if &e'&dohelp eq 0 .do begin
.   .   .   ..sk -1
.   .   .   :SF font=15.~n:eSF.
.   .   .do end
.   .   .el .do begin
.   .   .   .bd off
.   .   .do end
.   .   :eLQ.
.   .   .keep end
.   .do end
.dm shadeold end
.*
.* Redefine paragraph control macros for on-line help
.*
.if &e'&dohelp eq 1 .do begin
.*
.dm pp begin
:P.
.if '&shadeing' eq '1' .bd on
.dm pp end
.*
.dm np begin
:P.
.if '&shadeing' eq '1' .bd on
.dm np end
.*
.dm pc begin
:PC.
.if '&shadeing' eq '1' .bd on
.dm pc end
.*
.do end
.*
.dm addindent begin
.  ..in +&*. *
.dm addindent end
.*
.dm noindent begin
.  ..in -&*. *
.dm noindent end
.*
.dm bigterms begin
.  .addindent 3
.dm bigterms end
.*
.dm bigterm begin
.  .noindent 3
.  :P.
.  :HP2.
.dm bigterm end
.*
.dm bigdesc begin
.  :eHP2.
.  .addindent 3
.  :P.
.dm bigdesc end
.*
.dm endbigterms begin
.  .noindent 3
.dm endbigterms end
.*
:SET symbol='SYSRB'     value='~b'.
:SET symbol='DAGGER'    value='~D'.
.*
.dm minus begin
.  .mono -
.dm minus end
.*
:SET symbol='MINUS'     value=';.ct .minus;.ct '
.*
.dm trademark begin
.  :SF font=7.~T:eSF.
.dm trademark end
.*
:SET symbol='TRADEMARK' value=';.ct;.trademark;.ct '.
.*
.dm langle begin
.  :SF font=7.~<:eSF.
.dm langle end
.*
:SET symbol='LANGLE'   value=';.ct;.langle;.ct '.
.*
.dm rangle begin
.  :SF font=7.~>:eSF.
.dm rangle end
.*
:SET symbol='RANGLE'   value=';.ct;.rangle;.ct '.
.*
.dm @monobeg begin
.  :SF font=4.&*.
.dm @monobeg end
.*
.dm @monoend begin
.  :eSF.&*.
.dm @monoend end
.*
..gt MONO  add @monobeg continue
..gt eMONO add @monoend continue
.*
.dm @minus begin
.  :SF font=4.-:eSF.&*
.dm @minus end
.*
..gt MINUS add @minus continue
.*
.dm @msemi begin
.  :SF font=4.;:eSF.&*.
.dm @msemi end
.*
..gt MSEMI add @msemi continue
.*
.dm @italbeg begin
.  :SF font=1.&*.
.dm @italbeg end
.*
.dm @italend begin
.  :eSF.&*.
.dm @italend end
.*
..gt ITAL  add @italbeg continue
..gt eITAL add @italend continue
.*
:CMT. Fix these!
.*
.dm super begin
.  :SF font=5.(&*.):eSF.
.dm super end
.*
:SET symbol='SUPER0'   value=';.ct .super 0;.ct '.
:SET symbol='SUPER1'   value=';.ct .super 1;.ct '.
:SET symbol='SUPER2'   value=';.ct .super 2;.ct '.
:SET symbol='SUPER3'   value=';.ct .super 3;.ct '.
:SET symbol='SUPER4'   value=';.ct .super 4;.ct '.
:SET symbol='SUPER5'   value=';.ct .super 5;.ct '.
:SET symbol='SUPER6'   value=';.ct .super 6;.ct '.
:SET symbol='SUPER7'   value=';.ct .super 7;.ct '.
:SET symbol='SUPER8'   value=';.ct .super 8;.ct '.
:SET symbol='SUPER9'   value=';.ct .super 9;.ct '.
:CMT.
:CMT. These are for the "examples" chapter, to ensure that
:CMT. the mono-spaced superscripts are only 1 character wide.
:CMT.
:SET symbol='MSUPER0'  value='0'.
:SET symbol='MSUPER1'  value='1'.
:SET symbol='MSUPER2'  value='2'.
:SET symbol='MSUPER3'  value='3'.
:SET symbol='MSUPER4'  value='4'.
:SET symbol='MSUPER5'  value='5'.
:SET symbol='MSUPER6'  value='6'.
:SET symbol='MSUPER7'  value='7'.
:SET symbol='MSUPER8'  value='8'.
:SET symbol='MSUPER9'  value='9'.
.*
.dm helppref begin
.if &e'&dohelp eq 1 .do begin
.   .if '&*' ne '' .do begin
.   :helppfx pfx='&* '.
.   .se pfx$='&* '
.   .do end
.   .el .do begin
.   :helppfx.
.   .se pfx$=''
.   .do end
.   .pu 1 .helppref &*
.do end
.dm helppref end
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
.dm my_ext begin
.   .se *tmplvl=&WDWlvl-3
.   .if '&*' eq 'begin' .do begin
.   .   .sr shadeing=1
.   .   .cp &*tmplvl
.   .   .se lmargin=&sysin+1
.   .   .se $$myextrb=&lmargin
.   .   .se $$myextre=&rmargin
.   .   :I1.implementation-specific behavior
.   .   .xtxmp begin &$$myextrb &$$myextre
.   .   .br
.   .   .keep begin
.   .   :LQ.
.   .   ~n
.   .do end
.   .el .if '&*' eq 'end' .do begin
.   .   .sr shadeing=0
.   .   .br
.   .   :eLQ.
.   .   .xtxmp end &$$myextrb &$$myextre
.   .   .keep end
.   .do end
.dm my_ext end
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
