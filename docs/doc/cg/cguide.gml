:set symbol="isbn" value="".
.*
:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:set symbol="lang"      value="C/C++".
:INCLUDE file='DEFS'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
.*
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&product
:TITLE.User's Guide
.if '&target' eq 'AXPNT' .do begin
:TITLE.for DEC Alpha AXP
.do end
.el .do begin
.do end
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
:INCLUDE file='CPREF'.
.pa odd
:TOC.
:FIGLIST.
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
.sepsect &product User's Guide
.*
:INCLUDE file='ABOUT'.
:INCLUDE file='CPOPTS'.
:INCLUDE file='CPWCC'.
:INCLUDE file='CPHEADER'.
:INCLUDE file='CPLIBR'.
.*
.sepsect 16-bit Topics
:set symbol="machine"    value="8086".
:INCLUDE file='WMODELS'.
:INCLUDE file='CCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect 32-bit Topics
:set symbol="machine"    value="80386".
:INCLUDE file='WMODL386'.
:INCLUDE file='CCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect In-line Assembly Language
:INCLUDE file='CPASM'.
.*
.sepsect Structured Exception Handling in C
:INCLUDE file='SEH'.
.*
.sepsect Embedded Systems
:INCLUDE file='CROMABLE'.
.*
.sepsect Appendices
:APPENDIX.
:INCLUDE file='ENVIRON'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='MSGMACS'.
:INCLUDE file='CMSGS'.
:INCLUDE file='CPPMSGS'.
.do end
:INCLUDE file='CRTMSGS'.
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
