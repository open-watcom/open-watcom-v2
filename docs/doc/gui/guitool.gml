:set symbol="isbn" value="".
.if &e'&book eq 0 .do begin
:set symbol="allinone" value=1.
.do end
.*
:INCLUDE file='LYTCHG'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='SYMBOLS'
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
.*
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&company Graphical Tools
:TITLE.User's Guide
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'
:PREFACE.
:INCLUDE file='WGUIPREF'.
.pa odd
:TOC.
:FIGLIST.
.pa odd
.do end
.*
:BODY.
.if &e'&dohelp eq 1 .do begin
:exhelp
:include file='&book..idx'
:include file='&book..tbl'
:include file='&book..kw'
.do end
.*
.if '&'lower(&book)' eq 'ide' or &e'&allinone eq 1 .do begin
.sepsect &wide
:INCLUDE file='wguiwide.gml'
.do end
.if '&'lower(&book)' eq 'wbrw' or &e'&allinone eq 1 .do begin
.sepsect Browser
:INCLUDE file='wguibrow.gml'
.do end
.if '&'lower(&book)' eq 'wprof' or &e'&allinone eq 1 .do begin
.sepsect Profile Analysis Tools
:INCLUDE file='wprof.gml'
.do end
.if '&'lower(&book)' eq 'editor' or &e'&allinone eq 1 .do begin
.sepsect Editor
:INCLUDE file='wedref.gml'
:INCLUDE file='wedregx.gml'
.do end
.sepsect Resource Editors
.if '&'lower(&book)' eq 'resedt' or &e'&allinone eq 1 .do begin
:INCLUDE file='wre.gml'
.do end
.if '&'lower(&book)' eq 'resacc' or &e'&allinone eq 1 .do begin
:INCLUDE file='wguiaccl.gml'
.do end
.if '&'lower(&book)' eq 'resdlg' or &e'&allinone eq 1 .do begin
:INCLUDE file='wguidlog.gml'
.do end
.if '&'lower(&book)' eq 'resimg' or &e'&allinone eq 1 .do begin
:INCLUDE file='wguimage.gml'
.do end
.if '&'lower(&book)' eq 'resmnu' or &e'&allinone eq 1 .do begin
:INCLUDE file='wguimenu.gml'
.do end
.if '&'lower(&book)' eq 'resstr' or &e'&allinone eq 1 .do begin
:INCLUDE file='wguistrg.gml'
.do end

.if '&'lower(&book)' eq 'rescomp' or &e'&allinone eq 1 .do begin
.sepsect Resource Compiler
:INCLUDE file='wrc.gml'
.do end
.if '&'lower(&book)' eq 'zoom' or &e'&allinone eq 1 .do begin
.sepsect Zoom
:INCLUDE file='wguizoom.gml'
.do end
.if '&'lower(&book)' eq 'heapwalk' or &e'&allinone eq 1 .do begin
.sepsect Heap Walker
:INCLUDE file='wguiheap.gml'.
.do end
.if '&'lower(&book)' eq 'spy' or &e'&allinone eq 1 .do begin
.sepsect Spy
:INCLUDE file='wguispy.gml'
.do end
.if '&'lower(&book)' eq 'ddespy' or &e'&allinone eq 1 .do begin
.sepsect DDE Spy
:INCLUDE file='wguidspy.gml'
.do end
.if '&'lower(&book)' eq 'drwatcom' or &e'&allinone eq 1 .do begin
.sepsect &drwc for Win16
.helppref Win16:
:INCLUDE file='wguidrwc.gml'
.helppref
.do end
.if '&'lower(&book)' eq 'drnt' or &e'&allinone eq 1 .do begin
.sepsect &drnt for Win32
.helppref Win32:
:INCLUDE file='wguidrnt.gml'
.helppref
.do end
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
