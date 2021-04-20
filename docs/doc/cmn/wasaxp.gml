.if &e'&lang eq 0 .se lang="C/C++"
.if &e'&isbn eq 0 .se isbn="1-55094-???-?"
:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
.el .do begin
:INCLUDE file='WHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='DEFS'.
.*
.if &e'&dohelp ne 0 .do begin
.dm sepsect begin
.dm sepsect end
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&wasname User's Guide
:AUTHOR.WATCOM International Corporation
:ADDRESS.
:ALINE.Waterloo, Ontario, Canada
:eADDRESS.
:cmt. :graphic depth='0.7i' xoff='-2.55i' yoff='0.0i' scale=50 file='watcom.eps'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
.preface Preface
.np
The
.us &wasname User's Guide
describes the AXP assembler.
:TOC.
:FIGLIST.
.pa odd
.do end
.*
:BODY.
.*
.if &e'&dohelp ne 0 .do begin
:exhelp
:include file='&book..idx'
:include file='&book..tbl'
:include file='&book..kw'
.do end
.sepsect The &wasname
.*
:INCLUDE file='WAS'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.cntents end_of_book
:eGDOC.
