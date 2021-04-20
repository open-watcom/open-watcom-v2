.* symbol "isbn" defined in grsyms
.*
.* Open Watcom C Graphics Library Reference
.* Open Watcom FORTRAN 77 Graphics Library Reference
.*
:CMT.   :set symbol='$draft' value='yes'.
:set symbol='which' value='F77'.
:set symbol='machsys' value='DOS'.
.*
:INCLUDE file='lytchg'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='wnohelp'.
.do end
.el .do begin
:INCLUDE file='whelp'.
.do end
:INCLUDE file='bantop'.
:INCLUDE file='fmtmacro'.
:INCLUDE file='xdefs'.
:INCLUDE file='symbols'.
:INCLUDE file='grsyms'.
:INCLUDE file='grmacs'.
:INCLUDE file='cmanmac'.
:INCLUDE file='libmacs'.
:INCLUDE file='libfuns'.
:INCLUDE file='liblist7'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE &company &lang
:TITLE Graphics Library Reference
.if '&machsys' eq 'QNX' .do begin
:TITLE.for QNX
.do end
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='copyrite'.
:INCLUDE file='disclaim'.
.if '&machsys' ne 'QNX' .do begin
:INCLUDE file='newslett'.
.do end
:PREFACE.
:INCLUDE file='grpref'.
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
.chap Graphics Library
.*
.im gfunmacs
:INCLUDE file='graphov'.
.im gfunrems
.*
.chap Graphics Library &rroutines
.*
:INCLUDE file='gr_fnbeg'.
:INCLUDE file='gr_index'.
:CMT.   .chap Graphics Examples
:CMT.   :INCLUDE file='gr_samp'.
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.*
.cntents end_of_book
:eGDOC.
