:INCLUDE file='LYTCHG'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:set symbol="lang"       value="FORTRAN 77".
:INCLUDE file='DEFS'.
.*
.if &e'&dohelp eq 1 .do begin
.dm sepsect begin
.dm sepsect end
.do end
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
.ixbook &product User's Guide
.ixhelp fuguide
.ixbook &product Programmer's Guide
.ixhelp fpguide
.ixbook &product Language Reference
.ixhelp f77lr
.ixbook &product Tools User's Guide
.ixhelp ftools
.if '&dotarget' ne 'os2' .do begin
. .ixbook &company Graphical Tools User's Guide
. .if '&dotarget' ne 'dos' .do begin
.   .ixhelp ide
.   .ixhelp wbrw
. .do end
. .ixhelp wprof
. .if '&dotarget' ne 'dos' .do begin
.   .ixhelp editor
.   .ixhelp resedt
.   .ixhelp resacc
.   .ixhelp resdlg
.   .ixhelp resimg
.   .ixhelp resmnu
.   .ixhelp resstr
. .do end
. .ixhelp rescomp
. .if '&dotarget' ne 'dos' .do begin
.   .ixhelp zoom
.   .ixhelp heapwalk
.   .ixhelp spy
.   .ixhelp ddespy
.   .ixhelp drwatcom
.   .ixhelp drnt
. .do end
.do end
.ixbook &lnkname User's Guide
.ixhelp lguide
.ixbook &dbgname User's Guide
.ixhelp wd
.ixbook &product Graphics Library Reference
.ixhelp f77graph
.* .ixbook &product Diagnostic Messages Online Help
.* .ixhelp wfcerrs
.*
:eGDOC.
