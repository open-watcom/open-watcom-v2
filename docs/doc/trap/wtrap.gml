.se trp_majver=17
.se trp_minver=1
:INCLUDE file='LYTCHG'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='DEFS'.
:INCLUDE file='extra'.
.*
.if &e'&dohelp eq 0 .do begin
.*
.* Layout changes specific to this document
.*
.* Reuse numbering from 1 to each separate section
.*
:LAYOUT.
:H0
    number_reset=yes
:eLAYOUT.
:INCLUDE file='WNOHELP'.
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.&company Debugger Trap File Interface
:TITLE.VERSION &trp_majver..&trp_minver.
:AUTHOR.Originally written by WATCOM International Corp.
:AUTHOR.Revised by Open Watcom contributors
:eTITLEP.
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
:include file='trap'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
