:set symbol="isbn" value="".
.*
:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='BANTOP'.
:INCLUDE file='XDEFS'.
:set symbol="lang"       value="FORTRAN 77".
:INCLUDE file='DEFS'.
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="Programmer's Guide".&product
:TITLE.Programmer's Guide
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='COPYRRSI'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
:INCLUDE file='PGPREF'.
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
:INCLUDE file='PGINTRO'.
.*
.sepsect DOS Programming Guide
.*
.helppref DOS:
:INCLUDE file='PGDOS16'.
.helppref Phar Lap:
:INCLUDE file='PGDOS32P'.
.helppref DOS/4GW:
:INCLUDE file='PGDOS32D'.
.helppref
:INCLUDE file='PGDOS32Q'.
.*
.sepsect The DOS/4GW DOS Extender
.*
.helppref DOS/4GW:
:INCLUDE file='RATIONAL'.
.helppref
.*
.sepsect Windows 3.x Programming Guide
.*
.helppref 16-bit Windows:
:INCLUDE file='PGWIN16'.
:INCLUDE file='PGWIN16W'.
.helppref 32-bit Windows:
:INCLUDE file='PGWIN32'.
:INCLUDE file='PGWIN32W'.
.helppref Windows:
:INCLUDE file='G-GEN'.
:INCLUDE file='G-PO'.
:cmt. :INCLUDE file='PGWDLL16'.
:INCLUDE file='PGWDLL32'.
:INCLUDE file='PGVBDLL'.
:INCLUDE file='WINFUNS'.
:INCLUDE file='PGWIN32Q'.
.helppref
:INCLUDE file='WINFUN16'.
.*
.sepsect Windows NT Programming Guide
.*
.helppref NT:
:INCLUDE file='WINNT'.
:INCLUDE file='PGWINNT'.
:INCLUDE file='PGWINNTC'.
.if '&defwinnt' eq '1' .do begin
:INCLUDE file='PGWINNTW'.
.do end
:set symbol="targetos" value="Windows NT"
:INCLUDE file='MTHREAD'.
:INCLUDE file='DYNLINK'.
.helppref.
.*
.sepsect OS/2 Programming Guide
.*
.helppref 16-bit OS/2:
:INCLUDE file='PGOS216'.
.helppref 32-bit OS/2:
:INCLUDE file='PGOS232'.
.helppref OS/2:
:set symbol="targetos" value="OS/2 2.x"
:INCLUDE file='MTHREAD'.
:INCLUDE file='DYNLINK'.
:INCLUDE file='OS2PM'.
.helppref
.*
.if '&target' eq 'QNX' .do begin
.*
.sepsect QNX Programming Guide
.*
.helppref QNX:
:INCLUDE file='PGQNX'.
.helppref
.*
.do end
.*
.sepsect Novell NLM Programming Guide
.*
:INCLUDE file='PGNLM'
.*
.sepsect Mixed Language Programming
.*
:INCLUDE file='PGCMIXF'.
.*
.sepsect Common Problems
.*
:INCLUDE file='PGQA'.
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
