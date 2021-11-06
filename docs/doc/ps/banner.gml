:LAYOUT.
:TOC
        toc_levels=6
:TOCH5
        indent=0
:H5
        pre_top_skip=0
        pre_skip=2
        post_skip=0
        page_eject=no
        number_form=none
        display_heading=yes
:eLAYOUT.
.*
.dm topsect begin
.sr *ban=&headtext$.
.if &syslc. gt 3 .do begin
.   .in
.   .tb 1 _/&syscl.
.   .tb set $
$$
.   .tb set
.   .tb
.do end
.sr headtext$=&*ban.
.cp 10
:H5.&*
.se headtxt0$=&*
.se headtxt1$=&*
.se SCTlvl=1
.cntents &*
.dm topsect end
