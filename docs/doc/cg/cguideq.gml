:set symbol="isbn" value="1-55094-xxx-y"
:set symbol="prtd" value="Canada"
.*
:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:set symbol="target"    value="QNX".
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
:TITLE.Compiler and Tools
:TITLE.User's Guide for QNX
:TITLE.3rd Edition
:graphic depth='5.0i' xoff='-1.5i' yoff='0.0i' scale=100 file='pwrs.eps'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
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
.*
:set symbol="machine"    value="8086".
:INCLUDE file='WMODELS'.
:INCLUDE file='CCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect 32-bit Topics
.*
:set symbol="machine"    value="80386".
:INCLUDE file='WMODL386'.
:INCLUDE file='CCALL'.
:INCLUDE file='PRAGMA'.
.*
.sepsect In-line Assembly Language
.*
:INCLUDE file='CPASM'.
.*
.sepsect &company Tools
.*
.sr oobj=&obj.
.sepsect The &lnkname
.*
:INCLUDE file='WLUSAGE'.
:INCLUDE file='LNKMAIN'.
:INCLUDE file='SPQNX'.
:INCLUDE file='WLERROR'.
.*
.sepsect The &libname
.*
:INCLUDE file='WLIB'.
.sr obj=&oobj.
.*
.sepsect The &asmname
.*
:INCLUDE file='WASM'
.*
.sepsect The &disname
.*
.se arch='32-bit'
:INCLUDE file='WDIS'.
:INCLUDE file='WLFCOPT'.
.*
.sepsect The &stripname
.*
:INCLUDE file='WSTRIP'.
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
