:cmt. *******************************************************************
:cmt.
:cmt.   Help Tags File for Non-Help Processing
:cmt.   ======================================
:cmt.
:cmt.   This file defines the Help tags for non-WHP help processing.
:cmt.
:cmt. *******************************************************************

:cmt. *****************
:cmt. 'Z' override tags
:cmt. *****************

:cmt. **** :NOTE ****
.dm znote begin
:note &*
.dm znote end
.gt znote add znote cont

:cmt. **** :FIGREF ****
.dm zfigref begin
:figref &*
.dm zfigref end
.gt zfigref add zfigref cont

:cmt. **** :FIGCAP ****
.dm zfigcap begin
:figcap &*
.dm zfigcap end
.gt zfigcap add zfigcap cont

:cmt. **** :FIG ****
.dm zfig begin
:fig &*
.dm zfig end
.gt zfig add zfig cont

:cmt. **** :EFIG ****
.dm zefig begin
:efig &*
.dm zefig end
.gt zefig add zefig cont

:cmt. **** :UL ****
.dm zul begin
:ul &*
.dm zul end
.gt zul add zul cont

:cmt. **** :LI ****
.dm zli begin
:li &*
.dm zli end
.gt zli add zli cont

:cmt. **** :EUL ****
.dm zeul begin
:eul &*
.dm zeul end
.gt zeul add zeul cont

:cmt. **** :OL ****
.dm zol begin
:ol &*
.dm zol end
.gt zol add zol cont

:cmt. **** :EOL ****
.dm zeol begin
:eol &*
.dm zeol end
.gt zeol add zeol cont

:cmt. **** :SL ****
.dm zsl begin
:sl &*
.dm zsl end
.gt zsl add zsl cont

:cmt. **** :ESL ****
.dm zesl begin
:esl &*
.dm zesl end
.gt zesl add zesl cont

:cmt. **** :DL ****
.dm zdl begin
:dl &*
.dm zdl end
.gt zdl add zdl cont

:cmt. **** :EDL ****
.dm zedl begin
:edl &*
.dm zedl end
.gt zedl add zedl cont

:cmt. **** :DD ****
.dm zdd begin
:dd &*
.dm zdd end
.gt zdd add zdd cont

:cmt. **** :DT ****
.dm zdt begin
:dt &*
.dm zdt end
.gt zdt add zdt cont

:cmt. **** :DTHD ****
.dm zdthd begin
:dthd &*
.dm zdthd end
.gt zdthd add zdthd cont

:cmt. **** :DDHD ****
.dm zddhd begin
:ddhd &*
.dm zddhd end
.gt zddhd add zddhd cont

:cmt. **** :HDREF ****
.dm zhdref begin
:hdref &*
.dm zhdref end
.gt ZHDREF add ZHDREF cont

.dm fixhd begin
.se hdvl='&'pos( "ctx='", &* )'
.if &hdvl. eq 0 .do begin
.   .se hdtxt='&*'
.do end
.el .do begin
.   .se hdtxt="&'left(&*, &hdvl. - 1 )&'substr( &*, &hdvl. + 5 + &'pos( "'", &'substr( &*, &hdvl. + 5 ) ) )"
.do end
.dm fixhd end

:cmt. **** :H0 ****
.dm ZH0 begin
.fixhd &*
:H0 &hdtxt.
.dm ZH0 end
.gt ZH0 add ZH0
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H1 ****
.dm ZH1 begin
.fixhd &*
:H1 &hdtxt.
.dm ZH1 end
.gt ZH1 add ZH1

:cmt. **** :H2 ****
.dm ZH2 begin
.fixhd &*
:H2 &hdtxt.
.dm ZH2 end
.gt ZH2 add ZH2
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H3 ****
.dm ZH3 begin
.fixhd &*
:H3 &hdtxt.
.dm ZH3 end
.gt ZH3 add ZH3
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :H4 ****
.dm ZH4 begin
.fixhd &*
:H4 &hdtxt.
.dm ZH4 end
.gt ZH4 add ZH4
.ga * CTX
.ga * * VALUE '' DEFAULT
.ga * * ANY

:cmt. **** :I1 ****
.dm ZI1 begin
:I1 &*
.dm ZI1 end
.gt ZI1 add ZI1 cont

:cmt. ************************
:cmt. New Help Processing Tags
:cmt. ************************

:cmt. **** :HELPPFX ****
.dm helppfx begin
.dm helppfx end
.gt helppfx add helppfx cont attr
.ga * pfx ANY
.ga * * value '' default
.gt helppfx off

:cmt. **** :HELPLVL ****
.dm helplvl begin
.dm helplvl end
.gt helplvl add helplvl cont attr
.ga * level RANGE 0 4 4 4
.gt helplvl off

:cmt. **** :EXHELP ****
.dm exhelp begin
.dm exhelp end
.gt exhelp add exhelp cont
.gt exhelp off

:cmt. **** :HLINK ****
.dm hlink begin
&*
.dm hlink end
.gt hlink add hlink cont att
.ga * ctx ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EHLINK ****
.dm ehlink begin
.dm ehlink end
.gt ehlink add ehlink cont att
.gt ehlink off

:cmt. **** :FLINK ****
.dm flink begin
&*
.dm flink end
.gt flink add flink cont att
.ga * file ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EFLINK ****
.dm eflink begin
.dm eflink end
.gt eflink add eflink cont att
.gt eflink off

:cmt. **** :HDFN ****
.dm hdfn begin
&*
.dm hdfn end
.gt hdfn add hdfn cont att
.ga * ctx ANY
.ga * * VALUE '' DEFAULT
.ga * hd ANY
.ga * * VALUE '' DEFAULT

:cmt. **** :EHDFN ****
.dm ehdfn begin
.dm ehdfn end
.gt ehdfn add ehdfn cont
.gt ehdfn off

:cmt. **** :HBMP ****
.dm hbmp begin
.dm hbmp end
.gt hbmp add hbmp cont
.gt hbmp off

:cmt. **** :CTXKW ****
.dm ctxkw begin
.dm ctxkw end
.gt ctxkw add ctxkw cont att
.gt ctxkw off

:cmt. **** :QREF ****
.dm qref begin
&*str.&*.
.dm qref end
.gt qref add qref cont att
.ga * str REQ ANY
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :POPREF ****
.dm popref begin
&*str.&*.
.dm popref end
.gt popref add popref cont att
.ga * str REQ ANY
.ga * pfx ANY
.ga * * value '' default

:cmt. **** :SEEALSO ****
.dm seealso begin
.dm seealso end
.gt seealso add seealso cont
.gt seealso off

:cmt. **** :CONTPC ****
.dm conttpc begin
.dm conttpc end
.gt conttpc add conttpc cont
.gt conttpc off

:cmt. **** :POPHELP ****
.dm pophelp begin
:CMT. :h4.&*
.dm pophelp end
:CMT. .gt pophelp add pophelp att nocont
:CMT. .ga * cnt
:CMT. .ga * * VALUE 'yes' DEFAULT
:CMT. .ga * * VALUE 'no'
:CMT. .ga * pfx ANY
:CMT. .ga * * value '' default

:cmt. **** :BOX ****
.dm box begin
.dm box end
.gt box add box cont
.gt box off

:cmt. **** :EBOX ****
.dm ebox begin
.dm ebox end
.gt ebox add ebox cont
.gt ebox off

:cmt. **** :TABXMP ****
.dm tabxmp begin
.tb set &*1
.tb &'subword( &*, 2 )
:p.
.dm tabxmp end
.dm xtabxmp begin
.tabxmp &*
.dm xtabxmp end
.gt tabxmp add xtabxmp cont


:cmt. **** :ETABXMP ****
.dm etabxmp begin
.tb
:p.
.dm etabxmp end
.gt etabxmp add etabxmp cont

