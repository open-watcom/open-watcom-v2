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
:INCLUDE file='EXTRA'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.SPECIFICATION FOR THE DWARF WRITING LIBRARY
:TITLE.Draft #6
:AUTHOR.Originally written by WATCOM International Corp.
:AUTHOR.Revised by Open Watcom contributors
:DATE.
:eTITLEP.
:TOC.
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
.*
:include file='dwarfw'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
