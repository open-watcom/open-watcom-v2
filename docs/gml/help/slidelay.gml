:LAYOUT.
:PAGE
        top_margin='1.3i'
        left_margin = '1i'
        right_margin = '7.5i'
        depth = '9.0i'
:DEFAULT
        font = 0
        input_esc='^'
	justify = no
:TITLE  right_adjust=0
        page_position=center
        font = 6
        pre_top_skip = 4
:DOCNUM font = 8
:DATE   font = 8
:AUTHOR font = 8 pre_skip = 20
:ADDRESS font = 8
:H0
        font=6
        page_position=left
        indent=0
        pre_top_skip = 0
        post_skip = 2

:H1
        font=4
        number_form=none
        page_position=left
        page_eject=yes
        pre_top_skip = 1
        post_skip = 2
:H2
        font=4
        number_form=none
        page_position=left
        page_eject=no
        pre_top_skip = 1
        post_skip = 2
:H3
        font=4
        number_form=none
        page_position=left
        page_eject=no
        pre_top_skip = 0
        post_skip = 1
:XMP
        font=5
        pre_skip = 1
:FIG
        pre_skip = 1
        post_skip = 1
        default_frame=box
        default_place=inline
        font=5
:UL
        bullet_font = 6
        font = 0
:SL
        font = 0
:OL
        font = 0
        number_font = 0
:DT
        font = 7
:DD
        font = 0
:PREFACE
        header=no
:TOC
        left_adjust = 0
        right_adjust = 0
        spacing = 1
        columns = 1
        toc_levels = 4
:TOCH0
        indent = 0
        skip = 0
        pre_skip = 0
        post_skip = 0
:BANNER
    place = botodd
    docsect = preface
:BANREGION
    refnum = 1
:eBANREGION
:eBANNER
:BANNER
    place = boteven
    docsect = preface
:BANREGION
    refnum = 1
:eBANREGION
:eBANNER
:BANNER
    place = botodd
    docsect = head0
:BANREGION
    refnum = 1
    contents = sec
    font = 0
:eBANREGION
:BANREGION
    refnum = 2
    contents = pgnuma
    font = 0
:eBANREGION
:eBANNER
:BANNER
    place = boteven
    docsect = head0
:BANREGION
    refnum = 1
    contents = sec
    font = 0
:eBANREGION
:BANREGION
    refnum = 2
    contents = pgnuma
    font = 0
:eBANREGION
:eBANNER
:BANNER
    place = botodd
    docsect = body
:BANREGION
    refnum = 1
    contents = sec
    font = 0
:eBANREGION
:BANREGION
    refnum = 2
    contents = pgnuma
    font = 0
:eBANREGION
:eBANNER
:BANNER
    place = boteven
    docsect = body
:BANREGION
    refnum = 1
    contents = sec
    font = 0
:eBANREGION
:BANREGION
    refnum = 2
    contents = pgnuma
    font = 0
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 1
        place = topodd
        docsect = toc
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 6
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'Table of Contents'
:EBANREGION
:EBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 2
        place = topeven
        docsect = toc
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 6
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'Table of Contents'
:EBANREGION
:EBANNER
:eLAYOUT.
:include file='slidesym.gml'

