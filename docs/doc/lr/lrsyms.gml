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
:set symbol='us'        value=';.sf1 '.
:set symbol='eus'       value=';.esf '.
:set symbol='bd'        value=';.sf2 '.
:set symbol='ebd'       value=';.esf '.
:set symbol='mn'        value=';.sf4 '.
:set symbol='emn'       value=';.esf '.
.*
:set symbol="lang"      value="C".
.*
.* O/S specific definitions (DOS, OS/2, QNX, etc.)
.*
.* define default values
.*
:set symbol="version"   value="110".
:set symbol="company"   value="Watcom".
:set symbol="farfnc"    value="1".
:set symbol="sw"        value="/".
:set symbol="pc"        value="\".
:set symbol="ps"        value=";".
:set symbol="psword"    value="semicolon".
:set symbol="hdrdir"    value="\watcom\h".
:set symbol="hdrdirup"  value="\WATCOM\H".
:set symbol="hdrdir"    value="/watcom/h".
:set symbol="hdrsys"    value="/watcom/h/sys".
:set symbol="iohdr"     value="io.h".
:set symbol="doshdr"    value="dos.h".
:set symbol="dirhdr"    value="direct.h".
:set symbol="handle"    value="handle".
:set symbol="fd"        value="handle".
:set symbol="off_t"     value="off_t".
.*
.if '&machsys' eq 'QNX' .do begin
:set symbol="target"    value="QNX".
:set symbol="sw"        value="-".
:set symbol="pc"        value="/".
:set symbol="ps"        value=":".
:set symbol="psword"    value="colon".
:set symbol="hdrdir"    value="/usr/include".
:set symbol="hdrdirup"  value="/usr/include".
:set symbol="hdrsys"    value="/usr/include/sys".
:set symbol="iohdr"     value="unistd.h".
:set symbol="doshdr"    value="i86.h".
:set symbol="dirhdr"    value="dirent.h".
:set symbol="handle"    value="descriptor".
:set symbol="fd"        value="fildes".
:set symbol="off_t"     value="off_t".
.do end
.*
.if '&machsys' eq 'DOS' .do begin
:set symbol="target"    value="DOS".
.do end
.*
.if '&machsys' eq 'NEC' .do begin
:set symbol="target"    value="NEC".
.do end
.*
.if '&machsys' eq 'TEST' .do begin
:set symbol="target"    value="DOS".
.do end
.*
:set symbol="pi"        value=";.ct .sf7 ~P;.esf ".
:set symbol="minus"     value="-".
:set symbol="lbrkt"     value="[".
:set symbol="rbrkt"     value="]".
:cmt. :set symbol="period"    value=".".
:cmt. :set symbol="semico"    value=";".
:cmt. :set symbol="slash"     value="\".
:cmt. :set symbol="caret"     value="^".
