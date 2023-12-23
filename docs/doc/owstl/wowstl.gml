.*
.*
:set symbol="isbn" value="".
.*
:INCLUDE file='lytchg'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='wnohelp'.
.do end
.el .do begin
:INCLUDE file='whelp'.
.do end
:INCLUDE file='fmtmacro'.
:INCLUDE file='gmlmacs'.
:INCLUDE file='xdefs'.
:INCLUDE file='owgml'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE.&owstlname.
:TITLE.Developer's Guide
:INCLUDE file='doctitle'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='copyriow'.
:INCLUDE file='disclaim'.
:INCLUDE file='newslett'.
:PREFACE.
:CMT :INCLUDE file='ipfpref'.
.pa odd
:TOC.
.pa odd
.do end
.*
:BODY.
.*
.if &e'&dohelp ne 0 .do begin
:exhelp
:INCLUDE file='&book..idx'
:INCLUDE file='&book..tbl'
:INCLUDE file='&book..kw'
.do end
.*
.sepsect The &owstlname
:INCLUDE file='Intro'.
:INCLUDE file='algorith'
:INCLUDE file='deque'.
:INCLUDE file='list'.
:INCLUDE file='rbtree'.
:INCLUDE file='stack'
:INCLUDE file='string'.
:INCLUDE file='type_tra'
:INCLUDE file='vector'
.*
.if &e'&dohelp eq 0 .do begin
:BACKM.
:INDEX.
.do end
.*
.cntents end_of_book
:eGDOC.
