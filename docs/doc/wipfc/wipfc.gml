.*
.*
:set symbol="isbn" value="".
.*
:INCLUDE file='lytchg'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='wnohelp'.
.do end
.el .do begin
:INCLUDE file='whelp'.
.do end
:INCLUDE file='fmtmacro'.
:INCLUDE file='gmlmacs'.
:INCLUDE file='xdefs'.
:INCLUDE file='ipfdefs'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.&ipfname
:TITLE.User's Guide
:INCLUDE file='doctitle'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='copyrite'.
:INCLUDE file='disclaim'.
:INCLUDE file='newslett'.
:PREFACE.
:INCLUDE file='ipfpref'.
.pa odd
:TOC.
.pa odd
.do end
.*
:BODY.
.*
.if &e'&dohelp ne 0 .do begin
:exhelp
:INCLUDE file='&book..idx'
:INCLUDE file='&book..tbl'
:INCLUDE file='&book..kw'
.do end
.*
.sepsect The &ipfname
:INCLUDE file='ipfintro'.
:INCLUDE file='ipfuse'.
:INCLUDE file='ipftags'.
:INCLUDE file='ipfcmds'.
:INCLUDE file='ipftref'.
:INCLUDE file='ipferr'.
:INCLUDE file='ipfsym'.
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.*
.cntents end_of_book
:eGDOC.
