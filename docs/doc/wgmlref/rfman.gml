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
:INCLUDE file='EXTRA'.
.*
.if &e'&dohelp eq 0 .do begin
:LAYOUT
:PC
    pre_skip = 1
:TOC
    toc_levels = 4
:H4
    indent='0.8i'
:eLAYOUT
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:INCLUDE file='rftitle'.
:INCLUDE file='rfabst'.
:INCLUDE file='rfpref'.
:cmt :TITLEP.
:cmt :TITLE stitle="User's Guide".&dbgname.
:cmt :TITLE.User's Guide
:cmt :INCLUDE file='DOCTITLE'.
:cmt :eTITLEP.
:cmt :ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
.pa odd
:TOC.
:FIGLIST.
.pa odd
.do end
.*
:BODY.
.if &e'&dohelp ne 0 .do begin
:exhelp
:include file='&book..idx'
:include file='&book..tbl'
:include file='&book..kw'
.do end
:INCLUDE file='rfdindex'.
:INCLUDE file='rfdmisin'.
:H0.Tutorial
:INCLUDE file='rftut'.
:H0.GML Reference
:INCLUDE file='rfspec'.
:INCLUDE file='rftags'.
:INCLUDE file='rfltags'.
:INCLUDE file='rfscrip'.
:INCLUDE file='rflay'.
:INCLUDE file='rfsum'.
:INCLUDE file='rfrun'.
:H0.Device Reference
:INCLUDE file='rfdev'.
:INCLUDE file='rfdrun'.
:INCLUDE file='rffile'.
:INCLUDE file='rflib'.
.sepsect Appendices
:Appendix.
:INCLUDE file='rfscripa'.
:INCLUDE file='rfgerra'.
:INCLUDE file='rfderra'.
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INCLUDE file='rftrade'.
:INDEX.
.do end
.*
.cntents end_of_book
:eGDOC.
