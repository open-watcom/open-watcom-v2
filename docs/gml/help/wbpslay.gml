:CMT.************************************************************
:CMT. This file contains the default layout definitions for
:CMT. the WATCOM Student Workbench documentation.
:CMT. You may add to or change the layout by having an additional
:CMT. layout section in your document.  Please do so only if
:CMT. it is absolutely necessary.
:CMT.
:CMT. The margins have been set for a 6x9 format.
:CMT. Printing requires that the document page be centered
:CMT. on the 8.5x11 page.
:CMT.************************************************************

:LAYOUT
:PAGE
        left_margin='1.75i'
        right_margin='6.75i'
        top_margin='.5i'
        depth='8.00i'
:DEFAULT
        binding=0
        input_esc='\'
:TITLEP
        spacing=1
:TITLE
        page_position = right
        pre_top_skip = 7
        left_adjust = 0
        right_adjust = 0
        font = 11
        skip = 1
:AUTHOR
        page_position = right
        skip = 0
        pre_skip = 8
        left_adjust = 0
        right_adjust = 0
        font = 2
:ADDRESS
        page_position = right
        pre_skip = 4
        left_adjust = 0
        right_adjust = 0
        font = 2
:DATE
        page_position = right
        pre_skip = 2
        left_adjust = 0
        right_adjust = 0
        font = 2
:ALINE
        skip=0
:ABSTRACT
        header=no
        font=2
        page_reset = no
:PREFACE
        font = 4
        preface_string = "Preface"
        page_eject = odd
:P
        pre_skip=1
        line_indent=0
        post_skip=1
:PC
        pre_skip=0
        line_indent=0
        post_skip=1
:XMP
        pre_skip=1
        post_skip=1
        font=6
        left_indent='.3i'
:FIG
        default_place=inline
        default_frame=none
        pre_skip=1
        post_skip=1
        font=6
:FIGCAP
        pre_lines = 0
        font = 1
        figcap_string = "Figure "
        string_font = 3
        delim = '.'
:FIGDESC
        pre_lines=0
        font=1

:CMT.*******************************************************************
:CMT. Headings:
:CMT.      :h0         - major document sections (eg, tutorial, reference)
:CMT.                  - chapter numbers will not start over
:CMT.      :h1         - chapters
:CMT.      :h2         - numbered section
:CMT.      :h3         - numbered section
:CMT.      :h4         - numbered section
:CMT.      :cmt.:h4         - large headings - cause page break
:CMT.      :cmt.            - skip level h4 if you don't want these
:CMT.      :cmt.            - see commented out heading
:CMT.      :h5         - unnumbered heading - in TOC
:CMT.      :h6         - unnumbered heading - not in TOC
:CMT.
:CMT.  Table of contents: h0, h1, h2, h3, h4, h5
:CMT.*******************************************************************
:HEADING
        stop_eject=no
:H0
        indent=0
        page_position=right
        page_eject=odd
        number_reset=no
        number_font=11
        font=11
:H1
        indent=0
        page_position=right
        page_eject=odd
        pre_top_skip=1
        number_form=new
        display_heading=no
        number_font=4
        font=4
:H2
        number_form = prop
        number_style = h
        post_skip=1
        pre_top_skip=2
        font=2
        number_font=2
        page_eject=no
:H3
        number_form = prop
        number_style = h
        post_skip=1
        pre_top_skip=2
        font=2
        number_font=2
        page_eject = no
:H4
        number_form = prop
        number_style = h
        post_skip=1
        pre_top_skip=2
        font=2
        number_font=2
        page_eject = no
:cmt.:H4
:cmt.   page_position=centre
:cmt.   number_form = none
:cmt.   post_skip=1
:cmt.   pre_top_skip=1
:cmt.   font=11
:cmt.   page_eject = yes
:H5
        number_form = none
        post_skip=0
        pre_top_skip=1
        font=2
        page_eject = no
        line_break = yes
:H6
        number_form = none
        post_skip=0
        pre_top_skip=1
        font=2
        page_eject = no
        line_break = yes
:OL
        number_style=hd
        left_indent='0.3i'
        align='.3i'
        spacing=1
        post_skip=1
:UL
        left_indent='0.3i'
        align='0.2i'
        skip=0
        spacing=1
        pre_skip=1
        post_skip=1
:SL
        left_indent='0.3i'
        pre_skip=1
        skip=0
        spacing=1
        post_skip=1
:DL
        left_indent='0.4i'
        pre_skip=1
        post_skip=1
:DTHD
        font=3
:DDHD
        font=3
:DT
        font=1
:DD
        font=0
:TOC
        spacing = 1
        toc_levels = 5
:TOCH0
        skip = 2
        indent = 2
        pre_skip = 2
        post_skip = 1
:TOCH1
        skip = 1
        indent = 2
        pre_skip = 1
        post_skip = 0
:TOCH2
        skip = 0
        indent = 2
        pre_skip=0
        post_skip=0
:TOCH3
        skip = 0
        indent = 2
        pre_skip=0
        post_skip=0
:TOCH4
        skip = 0
        indent = 2
        pre_skip=0
        post_skip=0
:TOCH5
        skip = 0
        indent = 2
        pre_skip=0
        post_skip=0
:INDEX
        columns = 2
        spacing = 1
        page_eject = odd
:IXHEAD
        post_skip=2
:I1
        pre_skip = 0
        post_skip = 0
        skip = 0
:I2
        pre_skip = 0
        post_skip = 0
        skip = 0
        indent = '0.4cm'
:I3
        pre_skip = 0
        post_skip = 0
        skip = 0
        indent = '0.4cm'
:APPENDIX
        font = 4
        number_font = 4
        header=no
        page_eject = odd
        section_eject = yes
:BANNER
        place=boteven
        docsect=head0
:BANREGION
        refnum=1
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        place=botodd
        docsect=head0
:BANREGION
        refnum=1
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topodd
        docsect=head0
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 0
        refnum = 1
        region_position = right
        pouring = last
        contents = ' '
:EBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 5
        place = topodd
        docsect = head1
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 1
        depth = 1
        font = 4
        refnum = 1
        region_position = right
        pouring = last
        contents = 'Chapter  &amp.$hnum1.'
:EBANREGION
:BANREGION
        indent = 0
        hoffset = right
        width = extend
        voffset = 4
        depth = 1
        font = 4
        refnum = 2
        region_position = right
        pouring = last
        contents = headtext1
:EBANREGION
:EBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topodd
        docsect=body
:BANREGION
        indent=0
        hoffset=left
        width='4i'
        voffset=0
        depth=2
        font=0
        refnum=1
        region_position=left
        pouring=last
        contents=headtext1
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=2
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topeven
        docsect=body
:BANREGION
        indent=0
        hoffset=left
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=1
        region_position=left
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='4i'
        voffset=0
        depth=2
        font=0
        refnum=2
        region_position=right
        pouring=last
        contents=headtext1
:eBANREGION
:eBANNER
:BANNER
        place=boteven
        docsect=body
:BANREGION
        refnum=1
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        place=botodd
        docsect=body
:BANREGION
        refnum=1
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topodd
        docsect=appendix
:BANREGION
        indent=0
        hoffset=left
        width='4i'
        voffset=0
        depth=2
        font=0
        refnum=1
        region_position=left
        pouring=last
        contents=headtext1
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=2
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topeven
        docsect=appendix
:BANREGION
        indent=0
        hoffset=left
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=1
        region_position=left
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='4i'
        voffset=0
        depth=2
        font=0
        refnum=2
        region_position=right
        pouring=last
        contents=headtext1
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 2
        place = botodd
        docsect = abstract
:BANREGION
        indent = 0
        hoffset = center
        width = 5
        voffset = 1
        depth = 1
        font = 0
        refnum = 1
        region_position = centre
        pouring = last
        contents = pgnumr
:EBANREGION
:EBANNER
:BANNER
        place = boteven
        docsect = abstract
        refplace = botodd
        refdoc = abstract
:EBANNER
:BANNER
        place = botodd
        docsect = preface
        refplace = botodd
        refdoc = abstract
:EBANNER
:BANNER
        place = boteven
        docsect = preface
        refplace = boteven
        refdoc = abstract
:EBANNER
:BANNER
        place = boteven
        docsect = toc
        refplace = boteven
        refdoc = abstract
:EBANNER
:BANNER
        place = botodd
        docsect = toc
        refplace = botodd
        refdoc = abstract
:EBANNER
:BANNER
        place = boteven
        docsect = figlist
        refplace = boteven
        refdoc = abstract
:EBANNER
:BANNER
        place = botodd
        docsect = figlist
        refplace = botodd
        refdoc = abstract
:EBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = topodd
        docsect = toc
:BANREGION
        indent = 0
        hoffset = center
        width = '4i'
        voffset = 1
        depth = 1
        font = 4
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'Table of Contents'
:EBANREGION
:EBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = topeven
        docsect = toc
:BANREGION
        indent = 0
        hoffset = center
        width = '4i'
        voffset = 1
        depth = 1
        font = 4
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'Table of Contents'
:EBANREGION
:EBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = topodd
        docsect = figlist
:BANREGION
        indent = 0
        hoffset = center
        width = '4i'
        voffset = 1
        depth = 1
        font = 4
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'List of Figures'
:EBANREGION
:EBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = topeven
        docsect = figlist
:BANREGION
        indent = 0
        hoffset = center
        width = '4i'
        voffset = 1
        depth = 1
        font = 4
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'List of Figures'
:EBANREGION
:EBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topodd
        docsect=index
:BANREGION
        indent=0
        hoffset=left
        width='4i'
        voffset=0
        depth=2
        font=4
        refnum=1
        region_position=left
        pouring=last
        contents='Index'
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=2
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=3
        place=topeven
        docsect=index
:BANREGION
        indent=0
        hoffset=left
        width='.7i'
        voffset=0
        depth=1
        font=0
        refnum=1
        region_position=left
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='4i'
        voffset=0
        depth=2
        font=4
        refnum=2
        region_position=right
        pouring=last
        contents='Index'
:eBANREGION
:eBANNER
:eLAYOUT
:INCLUDE
        file='wbsymbol'.
