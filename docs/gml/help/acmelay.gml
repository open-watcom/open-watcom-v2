:layout
:SL
        left_indent=0
        pre_skip=0
:DL
        align='1.2i'
:elayout

:cmt.    ACME symbols

:set symbol='cci' value = 'current cell indicator'.
:set symbol='amode'  value=';.sf2;.ct Ctrl+A;.esf;.ct '
:set symbol='trace'  value=';.sf2;.ct Ctrl+T;.esf;.ct '
:set symbol='mouse'  value='mouse-pointer'

:set symbol='mark'  value='mark'
:set symbol='umark'  value='Mark'
:set symbol='udelete'  value='Delete'

:set symbol='quick'    value='(see "Quick Reference")'
:psc proc='acme'.
:set symbol='help' value= '0'
:set symbol='qref' value= ''
:set symbol='eqref' value= ''.
:set symbol='nqref' value= ';.sf2;.ct '
:set symbol='enqref' value= ';.esf;.ct '
:set symbol='langle' value= '<'
:set symbol='rangle' value= '>'
:set symbol='lcurl' value= '{'
:set symbol='rcurl' value= '}'
:epsc.
:psc proc='help'.
:set symbol='help' value= '1'
:set symbol='qref' value= '<'
:set symbol='eqref' value= '>'
:set symbol='nqref' value= '<'
:set symbol='enqref' value= '>'
:set symbol='langle' value= '<<'
:set symbol='rangle' value= '>>'
:set symbol='lcurl' value= '}}'
:set symbol='rcurl' value= '}}'
:epsc.

:set symbol='helpkey' value = ';. the ;.sf2;.ct Help;.esf;.ct  key (;.sf2;.ct F1;.esf;.ct )'
:set symbol='cutkey' value = ';. the ;.sf2;.ct Cut;.esf;.ct  key (;.sf2;.ct Shift+Del;.esf;.ct )'
:set symbol='pastekey' value = ';. the ;.sf2;.ct Paste;.esf;.ct  key (;.sf2;.ct Shift+Ins;.esf;.ct )'
:set symbol='dragkey' value = ';. the ;.sf2;.ct Drag;.esf;.ct  key (;.sf2;.ct F4;.esf;.ct )'
:set symbol='copykey' value = ';. the ;.sf2;.ct Copy;.esf;.ct  key (;.sf2;.ct Ctrl+Ins;.esf;.ct )'
:set symbol='repkey' value = ';. the ;.sf2;.ct Repeat;.esf;.ct  key (;.sf2;.ct F2;.esf;.ct )'
:set symbol='delkey' value = ';. the ;.sf2;.ct Delete;.esf;.ct  key (;.sf2;.ct Del;.esf;.ct )'
:set symbol='inkey' value = ';. the ;.sf2;.ct IN;.esf;.ct  key (;.sf2;.ct F7;.esf;.ct )'
:set symbol='outkey' value = ';. the ;.sf2;.ct OUT;.esf;.ct  key (;.sf2;.ct F8;.esf;.ct )'
:set symbol='menukey' value = ';. the ;.sf2;.ct Menu;.esf;.ct  key (;.sf2;.ct F10;.esf;.ct )'

:set symbol='done' value = ';.sf2;.ct F2=Done;.esf;.ct '
:set symbol='detail' value = ';.sf2;.ct F3=Detail;.esf;.ct '
:set symbol='add' value = ';.sf2;.ct F5=Add;.esf;.ct '

:set symbol='helphead' value='<INDEX>&hs.\ <Table_of_Contents>&hs.\ <Alphabetical_Quick_Reference>'

.dm ref begin
.pa
.if &acme .do begin
:h4.&*1.
.do end
.el .do begin
:hh4.&*1.
.do end
:set symbol='baneven'      value='&*'.
:set symbol='banodd'      value='&*'.
:i1 pg=major.&*1
:i2 refid='ref'.&*1
:fig frame=box.

.dm ref end

.dm eref begin

:efig.
.dm eref end

.dm fldattr begin
.if &acme .do begin
:h5.&*
.do end
.el .do begin
.helphd &'translate(&*,'_',' ')
.do end
.dm fldattr end

.gt fldattr add fldattr att nocont
