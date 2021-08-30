:CMT. *******************************************************************
:CMT.
:CMT.   WHP Help Tags File
:CMT.   ==================
:CMT.
:CMT.   This file redefines base GML tags to perform the necessary
:CMT.   operations for WHP Help processing. It also defines Help specific
:CMT.   tags used for help processing.
:CMT.
:CMT. *******************************************************************


:CMT. **** This list of characters is supported by WHPCVT. See its
:CMT. **** project for details.
:CMT. WGML symbol        value               WHPCVT symbol
:set symbol='rsxon'     value='&'d2c(195)'. :CMT. WHP_EXCLUDE_ON
:set symbol='rsxoff'    value='&'d2c(180)'. :CMT. WHP_EXCLUDE_OFF
:set symbol='rsctx'     value='&'d2c(247)'. :CMT. WHP_CTX_DEF
:set symbol='rstpc'     value='&'d2c(238)'. :CMT. WHP_TOPIC
:set symbol='rslnk'     value='&'d2c(127)'. :CMT. WHP_HLINK
:set symbol='rsdfn'     value='&'d2c(234)'. :CMT. WHP_DFN
:set symbol='rslsti'    value='&'d2c(211)'. :CMT. WHP_LIST_ITEM
:set symbol='rslstus'   value='&'d2c(210)'. :CMT. WHP_LIST_START
:set symbol='rslstue'   value='&'d2c(212)'. :CMT. WHP_LIST_END
:set symbol='rslstss'   value='&'d2c(213)'. :CMT. WHP_SLIST_START
:set symbol='rslstse'   value='&'d2c(214)'. :CMT. WHP_SLIST_END
:set symbol='rslstos'   value='&'d2c(200)'. :CMT. WHP_OLIST_START
:set symbol='rslstoe'   value='&'d2c(201)'. :CMT. WHP_OLIST_END
:set symbol='rslstds'   value='&'d2c(202)'. :CMT. WHP_DLIST_START
:set symbol='rslstde'   value='&'d2c(203)'. :CMT. WHP_DLIST_END
:set symbol='rslstdt'   value='&'d2c(204)'. :CMT. WHP_DLIST_TERM
:set symbol='rskw'      value='&'d2c(236)'. :CMT. WHP_CTX_KW
:set symbol='rshln'     value='&'d2c(167)'. :CMT. WHP_TOPIC_LN
:set symbol='rshnln'    value='&'d2c(248)'. :CMT. WHP_TOPIC_NOLN
:set symbol='rspar'     value='&'d2c(158)'. :CMT. WHP_PAR_RESET
:set symbol='rsxoffb'   value='&'d2c(159)'. :CMT. WHP_EXCLUDE_OFF_BLANK
:set symbol='rsup'      value='&'d2c(160)'. :CMT. WHP_UP_TOPIC
:set symbol='rsbmp'     value='&'d2c(176)'. :CMT. WHP_BMP
:set symbol='rsbxon'    value='&'d2c(219)'. :CMT. WHP_BOX_ON
:set symbol='rsbxof'    value='&'d2c(254)'. :CMT. WHP_BOX_OFF
:set symbol='rsddesc'   value='&'d2c(185)'. :CMT. WHP_DLIST_DESC
:set symbol='rsspace'   value='&'d2c(255)'. :CMT. WHP_SPACE_NOBREAK
:set symbol='rstbxmp'   value='&'d2c(170)'. :CMT. WHP_TABXMP
:set symbol='rsflnk'    value='&'d2c(232)'. :CMT. WHP_FLINK
:CMT. **** list type character (second byte) supported by WHPCVT ***/
:set symbol='rslstc'    value='c'.          :CMT. WHP_LIST_COMPACT
:CMT. **** This list of characters used by GML WHELPDRV driver.
:CMT. WGML symbol        value                WHPCVT symbol
:CMT.                   value='&'d2c(157)'.  WHP_FONTTYPE
:CMT.                   value='&'d2c(239)'.  WHP_FONTSTYLE_START
:CMT.                   value='&'d2c(240)'.  WHP_FONTSTYLE_END

:set symbol='hlptype'   value=''.
:set symbol='hlvl'      value='0'.
:set symbol='stlst'     value=''.
:set symbol='headlvl'   value=4
:set symbol='hdpfx'     value=''.

:cmt. **** the "A" in the next string is really supposed to be an "A"
:set symbol='trto1'     value='_MPTDB_XYUVLR__E__A_________'.
:set symbol='trfrom1'   value=" -+*/\%<>[](){}=#~@$,.&|':?!".
:set symbol='trto2'     value='_'.
:set symbol='trfrom2'   value='"'.
:cmt. **** these two are done with '.sr' to get around a wierd GML problem
.cw `
.sr  trto  ='&trto1.&trto2.S'
.sr  trfrom='&trfrom1.&trfrom2.;'
.cw ;

:CMT.*******************************************************************
:CMT.**** ctxstr macro for those who want the translated ctx string ****
:CMT.*******************************************************************

.dm ctxstr begin
.se ctx_str=&'translate(`&*`,`&trto.`,`&trfrom.`)
.dm ctxstr end

:CMT.*******************************************************************
:CMT.******** Redefinitions of base GML Tags for help stuff ************
:CMT.*******************************************************************

:cmt. **** :NOTE ****
.dm znote begin
:p.
.bd NOTE:
.dm znote end
.gt note add znote att nocont
.gt znote add znote att nocont

:cmt. **** :FIGREF ****
.dm zfigref begin
:figref refid=&*refid page=no.&*
.dm zfigref end
.gt figref add zfigref att nocont
.ga * refid
.ga * * any
.ga * page
.ga * * value '' default
.ga * * any
.gt zfigref add zfigref att nocont
.ga * refid
.ga * * any
.ga * page
.ga * * value '' default
.ga * * any

:cmt. **** :FIGCAP ****
.dm zfigcap begin
:figcap.&*
.dm zfigcap end
.gt figcap add zfigcap cont
.gt zfigcap add zfigcap cont

:cmt. **** :FIG ****
.dm zfig begin
.if &'length(&*id.) eq 0 .do begin
.   :fig.&*
.do end
.el .do begin
.   :fig id='&*id'.&*
.do end
.dm zfig end
.gt fig add zfig att nocont
.ga * depth range 0 32000 0
.ga * frame value none default
.ga * * value box
.ga * * value rule
.ga * id length 8
.ga * place value inline default
.ga * * value top
.ga * * value bottom
.gt zfig add zfig att nocont
.ga * depth range 0 32000 0
.ga * frame value none default
.ga * * value box
.ga * * value rule
.ga * id length 8
.ga * place value inline default
.ga * * value top
.ga * * value bottom

:cmt. **** :EFIG ****
.dm zefig begin
:efig.&*
.dm zefig end
.gt efig add zefig att nocont
.gt zefig add zefig att nocont

:cmt. **** :UL ****
.dm zul begin
:sl
.se stlst=`&rslstus.&rslstc.`
.dm zul end
.gt ul add zul
.gt zul add zul

:cmt. **** :LI ****
:cmt. The first ':li.' will put out the list start character before the
:cmt. list item character, resetting '&stlst.' to an empty string.
:cmt. There is a problem with lists, which cannot be fixed, so
:cmt. beware: a ':li:p' combo will generate an extra blank line.
:cmt. All attempts to fix this have been tried, but they result
:cmt. in multi-paragraph list items being screwed. So, don't
:cmt. bother trying to fix this for that bug because its been
:cmt. tried extensively! (djp).
.dm zli begin
.if &'length(&stlst.) eq 0 .do begin
.   :li.
.do end
.el .do begin
.   :li.&stlst.
.   :pb.
.   .se stlst=''
.do end
:p.&rslsti.&*
.dm zli end
.gt li add zli
.gt zli add zli

:cmt. **** :EUL ****
.dm zeul begin
:pb.&rslstue.
:esl
.dm zeul end
.gt eul add zeul
.gt zeul add zeul

:cmt. **** :OL ****
.dm zol begin
:sl
.se stlst=`&rslstos`
.dm zol end
.gt ol add zol
.gt zol add zol

:cmt. **** :EOL ****
.dm zeol begin
:pb.&rslstoe
:esl
.dm zeol end
.gt eol add zeol
.gt zeol add zeol

:cmt. **** :SL ****
.dm zsl begin
:sl
.se stlst=`&rslstss.&rslstc.`
.dm zsl end
.gt sl add zsl
.gt zsl add zsl

:cmt. **** :ESL ****
.dm zesl begin
:pb.&rslstse
:esl
.dm zesl end
.gt esl add zesl
.gt zesl add zesl

:cmt. **** :DL ****
:cmt. *** DL cannot be done with real DL, since tags are used in the DT
:cmt. *** for indexes and elsewhere (namely, hyperlinks).
.dm zdl begin
.se dfnst='first'
.dm zdl end
.gt dl add zdl
.gt zdl add zdl

:cmt. *** DL init with the first DT ***
.dm zdlinit begin
.sk 1
.if '&dfnst.' eq 'first' .do begin
&rslstds.
.   :pb.
.   .se dfnst=''
.do end
.dm zdlinit end

:cmt. **** :EDL ****
.dm zedl begin
:pb.&rslstde.
.sk 1
.dm zedl end
.gt edl add zedl
.gt zedl add zedl

:cmt. **** :DD ****
.dm zdd begin
:p.&rsddesc.&*
.dm zdd end
.gt dd add zdd
.gt zdd add zdd

:cmt. **** :ZDD2 ****
.dm zdd2 begin
:p.&rsddesc.:SF font=2.&*.:eSF.
.dm zdd2 end
.gt zdd2 add zdd2

:cmt. **** :ZDD3 ****
.dm zdd3 begin
:p.&rsddesc.:SF font=3.&*.:eSF.
.dm zdd3 end
.gt zdd3 add zdd3

:cmt. **** :DT ****
.dm zdt begin
.zdlinit
&rslstdt.&*.
.dm zdt end
.gt dt add zdt
.gt zdt add zdt

:cmt. **** :ZDT2 ****
.dm zdt2 begin
.zdlinit
&rslstdt.:SF font=2.&*.:eSF.
.dm zdt2 end
.gt zdt2 add zdt2

:cmt. **** :ZDT3 ****
.dm zdt3 begin
.zdlinit
&rslstdt.:SF font=3.&*.:eSF.
.dm zdt3 end
.gt zdt3 add zdt3

:cmt. **** :DTHD ****
.gt dthd add zdt3 cont
.gt zdthd add zdt3 cont
.gt zdthd2 add zdt2 cont
.gt zdthd3 add zdt3 cont

:cmt. **** :DDHD ****
.gt ddhd add zdd3 cont
.gt zddhd add zdd3 cont
.gt zddhd2 add zdd2 cont
.gt zddhd3 add zdd3 cont

:cmt. **** :HDREF ****
.dm zhdref begin
:hlink ctx='&'translate(&&*refid..,`&trto.`,`&trfrom.`)'.&&*refid..:ehlink.&*.
.dm zhdref end
.gt HDREF add ZHDREF att nocont
.ga * REFID
.ga * * ANY
.ga * PAGE
.ga * * VALUE '' DEFAULT
.ga * * ANY
.gt ZHDREF add ZHDREF att nocont
.ga * REFID
.ga * * ANY
.ga * PAGE
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** .HELPHD ****
.dm helphd begin
.if &'length(&*ctx.) eq 0 .do begin
.   .defhelp &'translate(`&*`,`&trto.`,`&trfrom.`) `&*` `&hdtxt`
.do end
.el .do begin
.   .defhelp &*ctx `&*` `&hdtxt`
.do end
.dm helphd end
.gt HELPHD add HELPHD att cont
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY


:cmt. **** .HINLINE ****
.dm hinline begin
.br
&rsspace
:pb
:SF font=15.&*.:eSF.
:p.
.dm hinline end

:cmt. **** :H0 ****
.dm ZH0 begin
.se title=`&hdpfx.&*.`
.if &'length(&*id.) eq 0 .do begin
.   :H0.&title.
.do end
.el .do begin
.   .se &*id.=`&title.`
.   :H0 id='&*id.'.&title.
.do end
.se hlvl='0'
:helphd.&title.
.dm ZH0 end
.gt H0 add ZH0 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.gt ZH0 add ZH0 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any

:cmt. **** :H1 ****
.dm ZH1 begin
.se hdtxt=`&hdpfx.&*.`
.if &'length(&*id.) eq 0 .do begin
.   :H1.&hdtxt.
.do end
.el .do begin
.   .se &*id.=`&hdtxt.`
.   :H1 id=`&*id.`.&hdtxt.
.do end
.if &headlvl. >= 1 .do begin
.   .se hlvl='1'
.   :helphd ctx='&*ctx.'.&hdtxt.
.do end
.el .do begin
.   .hinline &hdtxt.
.do end
.dm ZH1 end
.gt H1 add ZH1 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * BRIEF
.ga * * VALUE 'no' DEFAULT
.ga * * ANY
.ga * STITLE any
.gt ZH1 add ZH1 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * BRIEF
.ga * * VALUE 'no' DEFAULT
.ga * * ANY
.ga * STITLE any
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H2 ****
.dm ZH2 begin
.se title=`&hdpfx.&*.`
.if &'length(&*id.) eq 0 .do begin
.   :H2.&title.
.do end
.el .do begin
.   .se &*id.=`&title.`
.   :H2 id=`&*id.`.&title.
.do end
.if &headlvl. >= 2 .do begin
.   .se hlvl='2'
.   :helphd ctx='&*ctx.'.&title.
.do end
.el .do begin
.   .hinline &title.
.do end
.dm ZH2 end
.gt H2 add ZH2 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.gt ZH2 add ZH2 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H3 ****
.dm ZH3 begin
.se title=`&hdpfx.&*.`
.if &'length(&*id.) eq 0 .do begin
.   :H3.&title.
.do end
.el .do begin
.   .se &*id.=`&title.`
.   :H3 id=`&*id.`.&title.
.do end
.if &headlvl. >= 3 .do begin
.   .se hlvl='3'
.   :helphd ctx='&*ctx.'.&title.
.do end
.el .do begin
.   .hinline &title.
.do end
.dm ZH3 end
.gt H3 add ZH3 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.gt ZH3 add ZH3 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H4 ****
.dm ZH4 begin
.se title=`&hdpfx.&*.`
.if &'length(&*id.) eq 0 .do begin
.   :H4.&title.
.do end
.el .do begin
.   .se &*id.=`&title.`
.   :H4 id=`&*id.`.&title.
.do end
.if &headlvl. >= 4 .do begin
.   .se hlvl='4'
.   :helphd ctx='&*ctx'.&title.
.do end
.el .do begin
.   .hinline &title.
.do end
.dm ZH4 end
.gt H4 add ZH4 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.gt ZH4 add ZH4 att nocont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * STITLE any
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :I1 ****
.dm ZI1 begin
.if &'length(&*id.) eq 0 .do begin
:I1.&*.
.do end
.el .do begin
:I1 id='&*id.'.&*.
.do end
.ctxkw1 &*.
.dm ZI1 end
.gt I1 add ZI1 att cont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * PG ANY
.gt ZI1 add ZI1 att cont
.ga * ID
.ga * * VALUE '' DEFAULT
.ga * * ANY
.ga * PG ANY

:cmt. ************************
:cmt. New Help Processing Tags
:cmt. ************************

:cmt. **** :HELPPFX ****
.dm helppfx begin
.se hdpfx=`&*pfx.`
.dm helppfx end
.gt helppfx add helppfx cont attr
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :HELPLVL ****
.dm helplvl begin
.se headlvl=&*level.
.dm helplvl end
.gt helplvl add helplvl cont attr
.ga * level RANGE 0 4 4 4

:cmt. **** .DEFHELP ****
.dm defhelp begin
:p.&rsxoff.
:pb.&rsctx.&hlvl.&rsctx.&hlptype.&*1.&rsctx.&*2
.if &*0 ne 2 .ct &rsctx.&*3
.if &*0 ne 3 .ct &rsctx.&*4
:pb.&rstpc.&*1
:pb.
.dm defhelp end
.dm xdefhelp begin
.defhelp &*
.dm xdefhelp end
.gt defhelp add xdefhelp cont

:cmt. **** .ADDHELP ****
.dm addhelp begin
:p.&rsxoff.
:pb.&rstpc.&*1
.if &*0 eq 2 .ct &rstpc.&*2
:pb.
.dm addhelp end
.dm xaddhelp begin
.addhelp &*
.dm xaddhelp end
.gt addhelp add xaddhelp cont

:cmt. **** .EHELP ****
.dm ehelp begin
:p.&rsxon
:pb.
.dm ehelp end
.gt ehelp add ehelp cont

:cmt. **** :EXHELP ****
.dm exhelp begin
.ehelp
.dm exhelp end
.gt exhelp add exhelp cont

:cmt. **** :HLINK ****
.dm hlink begin
.if &'length(&*hd.) ne 0 .do begin
.se ctx=&'translate(`&*hd.`,`&trto.`,`&trfrom.`)
.do end
.el .do begin
.se ctx=`&*ctx.`
.do end
&rslnk.&ctx.&rslnk.&*.
.dm hlink end
.gt hlink add hlink cont att
.ga * ctx ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EHLINK ****
.dm ehlink begin
&rslnk.&*.
.dm ehlink end
.gt ehlink add ehlink cont att

:cmt. **** :FLINK ****
.dm flink begin
&rsflnk.&*file.&rsflnk.&*hd.&rsflnk.&*.
.dm flink end
.gt flink add flink cont att
.ga * file ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EfLINK ****
.dm eflink begin
&rsflnk.&*.
.dm eflink end
.gt eflink add eflink cont att

:cmt. **** :HDFN ****
.dm hdfn begin
.if &'length(&*hd.) ne 0 .do begin
.   .se ctx=&'translate(`&*hd.`,`&trto.`,`&trfrom.`)
.do end
.el .do begin
.   .se ctx=`&*ctx.`
.do end
&rsdfn.&ctx.&rsdfn.&*.
.dm hdfn end
.gt hdfn add hdfn cont att
.ga * ctx ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EHDFN ****
.dm ehdfn begin
.ct &rsdfn.&*.
.dm ehdfn end
.gt ehdfn add ehdfn cont

:cmt. **** :HBMP ****
.dm hbmp begin
&rsbmp.&*2.&rsbmp.&*1.&rsbmp
.dm hbmp end
.dm zhbmp begin
.hbmp &*
.dm zhbmp end
.gt hbmp add zhbmp cont

:cmt. **** :CTXKW ****
.dm ctxkw1 begin
&rskw.&*.&rskw.
.dm ctxkw1 end
.dm ctxkw begin
.if &*0 >= 1 .do begin
.   .ctxkw1 &*1
.do end
.if &*0 >= 2 .do begin
.   .ctxkw1 &*2
.do end
.if &*0 >= 3 .do begin
.   .ctxkw1 &*3
.do end
.if &*0 >= 4 .do begin
.   .ctxkw1 &*4
.do end
.if &*0 >= 5 .do begin
.   .ctxkw1 &*5
.do end
.if &*0 >= 6 .do begin
.   .ctxkw1 &*6
.do end
.if &*0 >= 7 .do begin
.   .ctxkw1 &*7
.do end
.if &*0 >= 8 .do begin
.   .ctxkw1 &*8
.do end
.if &*0 >= 9 .do begin
.   .ctxkw1 &*9
.do end
.dm ctxkw end
.dm xctxkw begin
.ctxkw &*
.dm xctxkw end
.gt ctxkw add xctxkw cont

:cmt. **** :QREF ****
.dm qref begin
:hlink ctx='&'translate(&*pfx.&*top.,`&trto.`,`&trfrom.`)'.&*pfx.&*str.:ehlink.&*.
.dm qref end
.gt qref add qref cont att
.ga * top REQ ANY
.ga * str REQ ANY
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :POPREF ****
.dm popref begin
:hdfn ctx='&'translate(&*pfx.&*str.,`&trto.`,`&trfrom.`)'.&*pfx.&*str.:ehdfn.&*.
.dm popref end
.gt popref add popref cont att
.ga * str REQ ANY
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :SEEALSO ****
.dm seealso begin
:H6.See Also
:zsl.
.if &*0 >= 1 .do begin
.   :zli. :qref top='&*1' str='&*1'.
.do end
.if &*0 >= 2 .do begin
.   :zli. :qref top='&*2' str='&*2'.
.do end
.if &*0 >= 3 .do begin
.   :zli. :qref top='&*3' str='&*3'.
.do end
.if &*0 >= 4 .do begin
.   :zli. :qref top='&*4' str='&*4'.
.do end
.if &*0 >= 5 .do begin
.   :zli. :qref top='&*5' str='&*5'.
.do end
.if &*0 >= 6 .do begin
.   :zli. :qref top='&*6' str='&*6'.
.do end
.if &*0 >= 7 .do begin
.   :zli. :qref top='&*7' str='&*7'.
.do end
.if &*0 >= 8 .do begin
.   :zli. :qref top='&*8' str='&*8'.
.do end
.if &*0 >= 9 .do begin
.   :zli. :qref top='&*9' str='&*9'.
.do end
:zesl.
.dm seealso end
.dm xseealso begin
.seealso &*
.dm xseealso end
.gt seealso add xseealso cont

:cmt. **** :CONTPC ****
.dm conttpc begin
.if &*0 eq 1 .do begin
.   :addhelp &'translate(`&*1.`,`&trto.`,`&trfrom.`)
.do end
.if &*0 eq 2 .do begin
.   :addhelp &'translate(`&*1.`,`&trto.`,`&trfrom.`) &*2
.do end
.dm conttpc end
.dm xconttpc begin
.conttpc &*
.dm xconttpc end
.gt conttpc add xconttpc cont

:cmt. **** :POPHELP ****
.dm pophelp begin
.if &*cnt eq 'yes' :h4.&*pfx.&*
.se hlptype=`&rshnln`
.se hlvl='4'
.helphd &*pfx.&*
.se hlptype=''
.dm pophelp end
.gt pophelp add pophelp att nocont
.ga * cnt
.ga * * VALUE 'yes' DEFAULT
.ga * * VALUE 'no'
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :BOX ****
.dm box begin
:pb.&rsbxon
:pb.
.dm box end
.gt box add box cont

:cmt. **** :EBOX ****
.dm ebox begin
:pb.&rsbxof
.dm ebox end
.gt ebox add ebox cont

:cmt. **** :TABXMP ****
.dm tabxmp begin
.co off
:p.&rstbxmp.&*1 &'subword( &*, 2 )
:pb.
.dm tabxmp end
.dm xtabxmp begin
.tabxmp &*
.dm xtabxmp end
.gt tabxmp add xtabxmp cont

:cmt. **** :ETABXMP ****
.dm etabxmp begin
:pb.&rstbxmp.
:p.
.co on
.dm etabxmp end
.gt etabxmp add etabxmp cont

.dc STOP OFF
