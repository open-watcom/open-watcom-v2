:INCLUDE file='mspslay.gml'
:LAYOUT.
:PC
    pre_skip = 1
:TOC
    toc_levels = 4
:APPENDIX
    header = yes
:BANNER
        left_adjust=0
        right_adjust=0
        depth=2
        place=botodd
        docsect=body
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=1
        depth=1
        font=17
        refnum=1
        region_position=right
        pouring=last
        script_format=no
        contents="&amp.$htext2".
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='.4i'
        voffset=1
        depth=1
        font=17
        refnum=2
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=2
        place=boteven
        docsect=body
:BANREGION
        indent=0
        hoffset=left
        width='.4i'
        voffset=1
        depth=1
        font=17
        refnum=1
        region_position=left
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width=extend
        voffset=1
        depth=1
        font=17
        refnum=2
        region_position=left
        pouring=last
        script_format=no
        contents="&amp.$htext2".
:eBANREGION
:eBANNER
:BANNER
        place=botodd
        docsect=head1
        refplace=botodd
        refdoc=body
:eBANNER
:BANNER
        place=boteven
        docsect=head1
        refplace=boteven
        refdoc=body
:eBANNER
:eLAYOUT.

.dm hp2 begin
:HP2.
.dm hp2 end

.dm ehp2 begin
:eHP2.
.dm ehp2 end

.dm choose begin
.in +3
.dm choose end

.dm echoose begin
.sk 2
.in -3
.dm echoose end

.dm mouse begin
.sk 2
.cp 6
.in -3
:HP2.Mouse::eHP2.
.sk
.in +3
.dm mouse end

.dm keyboard begin
.sk 2
.cp 6
.in -3
:HP2.Keyboard::eHP2.
.sk
.in +3
.dm keyboard end

.dm screen begin
:FIG frame=none.
:GRAPHIC file='&*1..ps' depth='2.5i'.
:FIGCAP.&*2.
:eFIG.
.dm screen end


:SET symbol='hp2' value=';.hp2;.ct '
:SET symbol='ehp2' value=';.ehp2;.ct '
:SET symbol='menu' value=';.sf1;.ct '
:SET symbol='emenu' value=';.esf;.ct '
:SET symbol='mono' value=';.sf8;.ct '
:SET symbol='emono' value=';.esf;.ct '

:SET symbol='wgml' value='WATCOM Script/GML'
