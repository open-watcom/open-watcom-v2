.se trp_majver=1
.se trp_minver=3
.se ovl_majver=3
.se ovl_minver=0
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
:TITLE.&company Debugger Interface
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
.sepsect WATCOM Debugging Information Format VERSION 4.0
.*
:include file='dbg'
.*
.sepsect Debugger Trap File Interface VERSION &trp_majver..&trp_minver.
.*
:include file='trap'
.*
.sepsect Overlay Manager Interface VERSION &ovl_majver..&ovl_minver.
.*
:include file='ovl'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
