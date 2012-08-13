:INCLUDE file='FMTMACRO'.
:INCLUDE file='EXTRA'.
:INCLUDE file='BOOKMAC'.
:LAYOUT
:FIG
        left_adjust = '0.5i'
:eLAYOUT

:set symbol="imgname"	 value="WATCOM Image Editor".

:GDOC.
:FRONTM.
:TITLEP.
:TITLE stitle="User's Guide".WATCOM Image Editor Reference and User's Guide
:set symbol="isbn" value="0-00000-000-0".
:AUTHOR.Paul Fast
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
.pa odd
:TOC.
:FIGLIST.
:BODY.
.if &ishelp ne 1 .do begin
:SEPSECT.WATCOM Image Editor User's Guide
.do end
:INCLUDE file='intro'.
.if &ishelp ne 1 .do begin
:INCLUDE file='basic'.
:INCLUDE file='about'.
:INCLUDE file='file'.
:INCLUDE file='tools'.
:INCLUDE file='edit'.
:INCLUDE file='colours'.
:INCLUDE file='options'.
:BACKM.
:INDEX.
:eGDOC.
