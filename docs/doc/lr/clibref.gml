.if '&machsys' eq 'DOS' .do begin
.   .sr isbn=""
.do end
.el .if '&machsys' eq 'WIN32' .do begin
.   .sr isbn=""
.do end
.el .if '&machsys' eq 'QNX' .do begin
.   .sr isbn="1-55094-xxx-y"
.   .sr forw="for QNX"
.do end
.el .if '&machsys' eq 'TEST' .do begin
.   .sr isbn="11.0-Alpha"
.do end
.*
:INCLUDE file='LYTCHG'.
:INCLUDE file='BANTOP'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='LRSYMS'.
:set symbol="which"     value="C".
.if '&machsys' ne 'WIN32' .do begin
:INCLUDE file='GRSYMS'.
:INCLUDE file='GRMACS'.
.do end
:INCLUDE file='CMANMAC'.
:INCLUDE file='LIBMACS'.
:INCLUDE file='LIBFUNS'.
:INCLUDE file='LIBLIST7'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
.*
:FRONTM.
:TITLEP.
:TITLE stitle="Library Reference".&company C Library Reference
.if &e'&forw eq 1 .do begin
:TITLE.&forw.
.do end
.if '&machsys' eq 'NEC' .do begin
:TITLE.Supplement
:TITLE.Japanese DBCS Support
.do end
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:PREFACE.
:INCLUDE file='LIBPREF'.
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
.sepsect &company C Library Reference
:INCLUDE file='CLIBINDX'.
.if '&machsys' ne 'TEST' .do begin
:INCLUDE file='CLIBOVER'.
.if '&machsys' eq 'DOS' or '&machsys' eq 'QNX' .do begin
.chap Graphics Library
.*
.im gfunmacs
:INCLUDE file='GRAPHOV'
.im gfunrems
.do end
.do end
.if '&machsys' eq 'DOS' .do begin
:INCLUDE file='CLIBDOS'.
.do end
:INCLUDE file='CLIBFUNS'.
.pa
:set symbol="headtxt0$" value=''.
:set symbol="headtxt1$" value=''.
:CMT.    .if '&machsys' eq 'DOS' or '&machsys' eq 'QNX' .do begin
:CMT.    .chap Graphics Library
:CMT.    .*
:CMT.    .im gfunmacs
:CMT.    :INCLUDE file='GRAPHOV'
:CMT.    .im gfunrems
:CMT.    .*
:CMT.    .chap Graphics Library &rroutines
:CMT.    .*
:CMT.    :INCLUDE gr_fnbeg
:CMT.    :INCLUDE gr_index
:CMT.    .do end
.*
.if '&machsys' ne 'PP' .do begin
:INCLUDE file='CREFUNS'.
.do end
:cmt. .if '&machsys' eq 'DOS' .do begin
:cmt. :INCLUDE file='CSUMFUNS'.
:cmt. .do end
.sepsect Appendices
:APPENDIX.
:INCLUDE file='CLIBFUNC'.
.*
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
