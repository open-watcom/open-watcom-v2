:set symbol='help' value=''.
:set symbol='ehelp' value=''.
:set symbol='hbrace' value='{'.
:set symbol='ehbrace' value='}'.
:set symbol='b' value='ÿ'
:set symbol='hs' value='\b\b '
:set symbol='colon' value=':'
:set symbol='appsp' value=''.

:set symbol='crsup'     value=';.sf20;.ct ;.esf;.ct '.
:set symbol='crsdn'     value=';.sf20;.ct ;.esf;.ct '.
:set symbol='crslt'     value=';.sf20;.ct ®;.esf;.ct '.
:set symbol='crsrt'     value=';.sf20;.ct ¯;.esf;.ct '.
:set symbol='bksp'      value=';.sf20;.ct ;.esf;.ct '.
:set symbol=bup         value=';.sf21;.ct ;.esf;.ct '.
:set symbol=bdown       value=';.sf21;.ct ;.esf;.ct '.

.dm tail begin
:fig frame=none.
&colon.t
.dm tail end

.dm etail begin
&colon.et
:efig.
.dm etail end

.dm tline begin
&*.
.dm tline end

.dm helphd begin
:sl.
:li.::::&'translate(&*1,'ÿ',' ')
:li.&colon.h
:li.&helphead.
:li.&colon.eh
:esl.
.dm helphd end

.dm snap begin
:cmt.:fig id='&*1.' place=inline frame=none.
:cmt.:graphic file='&*1..ps' depth= '2.63i'.
:cmt.:figcap.&*2.
:cmt.:efig.
.dm snap end

.dm fig begin
.hid5 *id='&*id.' *head='&*hhead.'
:fig id='&*id.' frame=&*frame..
.dm fig end

.dm figref begin
the figure in the Users Guide
.dm figref end

.dm hinclude begin
.dm hinclude end

.dm comref begin
&help.&refid.&ehelp.
.dm comref end

.dm hdref begin
&help.&&*refid..&ehelp.&*.
.dm hdref end

.dm H_0 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H0.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H0 id='&*id.'.::::&hlptxt.
.do end
.   .br
.dm H_0 end

.dm H_1 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H1.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H1 id='&*id.'.::::&hlptxt.
.do end
.   .br
.   .helphd &hlptxt
.dm H_1 end

.dm H_2 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H2.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H2 id='&*id.'.::::&hlptxt.
.do end
.   .br
.   .helphd &hlptxt
.dm H_2 end

.dm H_3 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H3.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H3 id='&*id.'.::::&hlptxt.
.do end
.   .br
.   .helphd &hlptxt
.dm H_3 end

.dm H_4 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H4.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H4 id='&*id.'.::::&hlptxt.
.do end
.   .br
.   .helphd &hlptxt
.dm H_4 end

.dm H_5 begin
:set symbol='hlptxt' value='&'translate(&*,'ÿ',' ')'
.if '&*id.' eq '' .do begin
    :H5.::::&hlptxt.
.do end
.el .do begin
    :set symbol='&*id.' value='&hlptxt.'.
    :H5 id='&*id.'.::::&hlptxt.
.do end
.   .br
.   .helphd &hlptxt
.dm H_5 end

.gt HDREF add hdref att nocont
.ga * PAGE value yes
.ga * PAGE value no
.ga * REFID 
.ga * * ANY 

.gt FIGREF add FIGREF att nocont
.ga * REFID 
.ga * * ANY 
.ga * PAGE
.ga * * VALUE '' DEFAULT
.ga * * ANY 
