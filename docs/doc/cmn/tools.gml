.if &e'&lang eq 0 .se lang="C/C++"
.if &e'&isbn eq 0 .se isbn="1-55094-???-?"
:INCLUDE file='LYTCHG'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='DEFS'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&product Tools
:TITLE.User's Guide
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
:INCLUDE file='TOOLPREF'.
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
.sepsect The &wclname Utility
.*
:INCLUDE file='COMPLINK'.
.*
.sepsect The &asmname
.*
:INCLUDE file='WASM'
.*
:CMT. .sepsect The &wasname
:CMT. .*
:CMT. :INCLUDE file='WAS'
.*
.sepsect Object File Utilities
.*
:INCLUDE file='WLIB'.
.se arch='32-bit'
:INCLUDE file='WDIS'.
:INCLUDE file='WLFCOPT'.
:INCLUDE file='EXE2BIN'.
.*
.sepsect Executable Image Utilities
.*
:INCLUDE file='BPATCH'.
:INCLUDE file='WSTRIP'.
.*
.sepsect The Make/Touch Utilities
.*
:INCLUDE file='WMAKE'.
:INCLUDE file='WTOUCH'.
.*
.sepsect The IDE2MAKE Batch Utility
.*
:INCLUDE file='IDE2MAKE'.
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
