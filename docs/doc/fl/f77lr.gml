:set symbol="isbn" value="".
.*
:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:set symbol="lang"       value="FORTRAN 77".
:INCLUDE file='DEFS'.
:INCLUDE file='FLMACS'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="Language Reference".&product
:TITLE.Language Reference
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
.im copyrite
.im disclaim
.im newslett
:PREFACE.
.im flpref
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
.sepsect Language Reference
.im fsrcfmt
.im fstats
.im ftypes
.im farrays
.im fsubstr
.im frecord
.im fexpr
.im fassmnt
.im fstruct
.im fio
.im fformats
.im fsubp
.sepsect Appendices
:APPENDIX.
.im f77ext
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
