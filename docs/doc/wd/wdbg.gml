:set symbol="isbn" value="".
.*
:INCLUDE file='LYTCHG'.
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
.if &e'&dohelp eq 0 .do begin
:INCLUDE file='WNOHELP'.
.do end
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&dbgname.
:TITLE.User's Guide
:INCLUDE file='DOCTITLE'.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
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
.sepsect Introduction
:INCLUDE file='wdbgintr.gml'.
.sepsect Preparing a Program to be Debugged
:INCLUDE file='wdbgprep.gml'.
.sepsect Starting the Debugger
:INCLUDE file='wdbgstrt.gml'.
.sepsect &dbgname. Environment
:INCLUDE file='wdbgenvr.gml'.
.sepsect Navigating Through a Program
:INCLUDE file='wdbgnav.gml'.
.sepsect Controlling Program Execution
:INCLUDE file='wdbgexec.gml'.
.sepsect Examining and Modifying the Program State
:INCLUDE file='wdbgexam.gml'.
.sepsect Breakpoints
:INCLUDE file='wdbgbrkp.gml'.
.sepsect Assembly Level Debugging
:INCLUDE file='wdbgasm.gml'.
.sepsect Remote Debugging
:INCLUDE file='wdbgrem.gml'.
:cmt..sepsect Debugging Techniques
.sepsect Interrupting A Running Program
:INCLUDE file='wdbgintp.gml'.
.sepsect Operating System specifics
:INCLUDE file='wdbgos.gml'.
.sepsect Expressions
:INCLUDE file='wdbgexpr.gml'.
.sepsect Appendices
:Appendix.
:INCLUDE file='wdbgcmd.gml'.
:INCLUDE file='wdbgsymb.gml'.
:INCLUDE file='wdbgwire.gml'.
:INCLUDE file='wdbgrfx.gml'.
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
