.sr docnum='BA0614'
.sr readme = 1
.*
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
:set symbol="lang"       value="C/C++".
:INCLUDE file='DEFS'.
.*
.* Layout changes specific to this document
.*
.* Switch off numbering for low-level heading
.*
.if &e'&dohelp eq 0 .do begin
:LAYOUT.
  :H3
    number_form=none
  :H4
    number_form=none
  :H5
    number_form=none
:eLAYOUT.
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
.*
:FRONTM.
:TITLEP.
:TITLE stitle="Getting Started".&product
:TITLE.Getting Started
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
.pa odd
:TOC.
:cmt.:FIGLIST.
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
:include file='intro'
:include file='install'
:include file='tutorial'
:include file='docson'
:include file='bench'
:cmt.:include file='redist'
:include file='diffs'
:include file='lic'
:include file='tshoot'
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.cntents end_of_book
:eGDOC.
