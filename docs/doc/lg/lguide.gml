.*
.*
:set symbol="isbn" value="".
.*
:INCLUDE file='lytchg'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='wnohelp'.
.do end
:INCLUDE file='fmtmacro'.
:INCLUDE file='gmlmacs'.
:INCLUDE file='xdefs'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&lnkname
:TITLE.User's Guide
:INCLUDE file='doctitle'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='copyrite'.
:INCLUDE file='disclaim'.
:INCLUDE file='newslett'.
:PREFACE.
:INCLUDE file='lpref'.
.pa odd
:TOC.
.pa odd
.do end
.*
:BODY.
.*
.if &e'&dohelp eq 1 .do begin
:exhelp
:include file='&book..idx'
:include file='&book..tbl'
:include file='&book..kw'
.do end
.*
.sepsect The &company Linker
:INCLUDE file='wlusage'.
:INCLUDE file='tutmain'.
:INCLUDE file='lnkmain'.
:INCLUDE file='spdos'.
:INCLUDE file='spzdos'.
:INCLUDE file='spraw'.
:INCLUDE file='spelf'.
:INCLUDE file='spnov'.
:INCLUDE file='spos2'.
:INCLUDE file='spphar'.
:INCLUDE file='spqnx'.
:INCLUDE file='spwin16'.
:INCLUDE file='spwivxd'.
:INCLUDE file='spwin32'.
:INCLUDE file='wlerror'.
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.*
.cd set 1
.cntents end_of_book
:eGDOC.
