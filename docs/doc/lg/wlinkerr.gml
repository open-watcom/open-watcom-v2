:INCLUDE file='lytchg'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='wnohelp'.
.do end
:INCLUDE file='fmtmacro'.
:INCLUDE file='gmlmacs'.
:INCLUDE file='xdefs'.
:set symbol="lang"      value="C/C++".
:set symbol="optdoc"    value="Power++ On-line Help".
:set symbol="lnkname"   value="Power++ Linker".
:INCLUDE file='defs'.
.*
:GDOC.
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
:INCLUDE file='wlerror'.
.*
.cntents end_of_book
:eGDOC.
