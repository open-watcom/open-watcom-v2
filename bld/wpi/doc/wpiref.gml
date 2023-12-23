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
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.&company. Windows Programming Interface (WPI)
:AUTHOR.Originally written by WATCOM International Corp.
:AUTHOR.Revised by &company. contributors
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
.sepsect &company. Windows Programming Interface (WPI)
.*
:include file='wpi'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
.cd set 2
:INDEX.
.do end
.cd set 1
.cntents end_of_book
:eGDOC.
