:INCLUDE file='FMTMACRO'.
:INCLUDE file='EXTRA'.
:LAYOUT
:FIG
        left_adjust = '0.5i'
:eLAYOUT

:set symbol="edname"     value="Open Watcom Editor".

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
:INCLUDE file='intro'.
.if &ishelp ne 1 .do begin
:INCLUDE file='basic'.
:INCLUDE file='inter'.
:INCLUDE file='advan'.
.if &ishelp ne 1 .do begin
:SEPSECT.Windows Editor User's Guide
.do end
:SEPSECT.Editor Reference
.do end
:INCLUDE file='env'.
:INCLUDE file='modes'.
:INCLUDE file='cmds'.
:INCLUDE file='win_menu'.
:INCLUDE file='settings'.
:INCLUDE file='regexp'.
:INCLUDE file='scripts'.
.if &ishelp ne 1 .do begin
:SEPSECT.Appendices
.do end
:APPENDIX
:INCLUDE file='a_cmdmode'.
:INCLUDE file='a_errmsg'.
:INCLUDE file='a_ctags'.
:INCLUDE file='a_symkeys'.
:INCLUDE file='a_errcode'.
:BACKM.
:INDEX.
:eGDOC.
