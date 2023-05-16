:set symbol="isbn" value="".
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
:set symbol="lang"       value="FORTRAN 77".
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
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
:INCLUDE file='FPREF'.
.pa odd
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
.*
.sepsect &product User's Guide
.*
:INCLUDE file='ABOUT'.
:INCLUDE file='FPOPTS'.
:INCLUDE file='FPWFC'.
:INCLUDE file='FPLIBR'.
:INCLUDE file='FCOND'.
:INCLUDE file='FDOSFILE'.
:INCLUDE file='FLIB'.
:INCLUDE file='FDATA'.
.*
.sepsect 16-bit Topics
:set symbol="machine"    value="8086".
:INCLUDE file='WMODELS'.
:INCLUDE file='FCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect 32-bit Topics
:set symbol="machine"    value="80386".
:INCLUDE file='WMODL386'.
:INCLUDE file='FCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect Appendices
:APPENDIX.
:INCLUDE file='ENVIRON'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='FERRMSG'.
:INCLUDE file='MSGMACS'.
:INCLUDE file='WFCMSGS'.
.do end
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.*
.cntents end_of_book
:eGDOC.
