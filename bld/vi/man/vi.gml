:INCLUDE file='FMTMACRO'.
:INCLUDE file='EXTRA'.
:LAYOUT
:FIG
        left_adjust = '0.5i'
:eLAYOUT

:set symbol="edname"	 value="WATCOM Editor".

:GDOC.
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".&edname Reference and User's Guide
:set symbol="isbn" value="6-66666-666-6".
:AUTHOR.Craig Eisler
:AUTHOR.
:AUTHOR.
:ADDRESS.
:ALINE.WATCOM International Corporation
:ALINE.Waterloo, Ontario, Canada
:eADDRESS.
:eTITLEP.
:ABSTRACT.
:INCLUDE file='COPYRITE'.
:INCLUDE file='DISCLAIM'.
:INCLUDE file='NEWSLETT'.
:PREFACE.
.dc cw 
:INCLUDE file='VIPREF'.
.pa odd
:TOC.
:FIGLIST.
:BODY.
.if &ishelp ne 1 .do begin
:SEPSECT.Editor User's Guide
.do end
:cmt :INCLUDE file='intro'.
.if &ishelp ne 1 .do begin
:cmt :INCLUDE file='basic'.
:cmt :INCLUDE file='inter'.
:cmt :INCLUDE file='advan'.
.if &ishelp ne 1 .do begin
:SEPSECT.Windows Editor User's Guide
.do end
:SEPSECT.Editor Reference
.do end
:cmt :INCLUDE file='env'.
:cmt :INCLUDE file='modes'.
:cmt :INCLUDE file='cmds'.
:cmt :INCLUDE file='win_menu'.
.* :INCLUDE file='settings'.
:cmt :INCLUDE file='regexp'.
:cmt :INCLUDE file='scripts'.
.if &ishelp ne 1 .do begin
:SEPSECT.Appendices
.do end
:APPENDIX
:cmt :INCLUDE file='a_cmdmode'.
:cmt :INCLUDE file='a_errmsg'.
:cmt :INCLUDE file='a_ctags'.
:cmt :INCLUDE file='a_symkeys'.
:cmt :INCLUDE file='a_errcode'.
:BACKM.
:INDEX.
:eGDOC.
