:CMT. **********************************************************************
:CMT. This file contains the default layout definitions for
:CMT. Open Watcom documentation.
:CMT. You may add to or change the layout by having an additional
:CMT. layout section in your document.  Please do so only if
:CMT. it is absolutely necessary.
:CMT.
:CMT. The margins have been set for a 7x9 format.
:CMT. Printing requires that the document page be reduced 82%.
:CMT. **********************************************************************

:set symbol='format' value='7x9'.
:LAYOUT.
:PAGE
        top_margin='.30i'
        left_margin='1.25i'
        right_margin='7.25i'
        depth='8.5i'
:DEFAULT
        binding=0
        input_esc='~'
        justify=no
:TITLEP
        spacing=1
:TITLE
        page_position=centre
        pre_top_skip=3
        left_adjust=0
        right_adjust=0
        font=11
        skip=1
:DOCNUM
        left_adjust=0
        right_adjust=0
        page_position=centre
        font=9
        pre_skip=2
        docnum_string=""
:DATE
        page_position=centre
        pre_skip=2
        left_adjust=0
        right_adjust=0
        font=2
:AUTHOR
        page_position=centre
        skip=0
        pre_skip=12
        left_adjust=0
        right_adjust=0
        font=2
:ADDRESS
        page_position=centre
        pre_skip=1
        left_adjust=0
        right_adjust=0
        font=2
:ALINE
        skip=0
:ABSTRACT
        header=no
        font=2
        page_reset=no
:PREFACE
        pre_top_skip=1
        font=11
        header=yes
        preface_string=""
        page_eject=odd
        spacing=1
:P
        pre_skip=1
        line_indent=0
        post_skip=0
:PC
        pre_skip=1
        line_indent=0
        post_skip=0
:XMP
        pre_skip=0
        post_skip=0
        font=4
        left_indent='0.3i'
:FIG
        left_adjust=7
        pre_skip=1
        post_skip=1
        font=4
        default_place=inline
        default_frame=none
:FIGCAP
        pre_lines=0
        font=1
        figcap_string="Figure "
        string_font=3
        delim='.'
:FIGDESC
        pre_lines=0
        font=1
:FN
        align='0.3i'
        skip=0
        font=1
        frame=rule
:OL
        number_style=hd
        left_indent='0.3i'
        align='0.3i'
        spacing=1
        post_skip=0
:UL
        left_indent='0.3i'
        align='0.05i'
        spacing=1
        pre_skip=1
        post_skip=0
        skip=1
:SL
        left_indent='0.3i'
        pre_skip=1
        skip=0
        spacing=1
        post_skip=0
:DL
        left_indent='0.0i'
        pre_skip=1
        post_skip=0
        align='0.76i'
:DTHD
        font=3
:DDHD
        font=3
:DT
        font=1
:DD
        font=0
:H0
        indent=0
        page_position=right
        page_eject=odd
        number_reset=no
        number_form=none
        font=12
:H1
        pre_top_skip=0
        pre_skip=0
        post_skip=2
        number_form=new
        number_font=12
        font=11
        page_eject=odd
        page_position=left
        display_heading=yes
:H2
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=prop
        number_font=10
        number_style=h
        font=10
        page_eject=no
:H3
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=none
        number_font=9
        number_style=h
        font=9
        page_eject=no
:H4
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=none
        number_font=8
        number_style=h
        font=8
        page_eject=no
:H5
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=none
        font=2
        page_eject=no
        line_break=yes
:H6
        number_form=none
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        font=10
        page_position=centre
        page_eject=no
        line_break=yes
:NOTE
        note_string="Notes: "
:TOC
        spacing=1
        toc_levels=5
:TOCH0
        pre_skip=2
        post_skip=1
        skip=2
        indent=4
:TOCH1
        pre_skip=1
        post_skip=0
        skip=1
        indent=4
:TOCH2
        pre_skip=0
        post_skip=0
        skip=0
        indent=4
:TOCH3
        pre_skip=0
        post_skip=0
        skip=0
        indent=4
:TOCH4
        pre_skip=0
        post_skip=0
        skip=0
        indent=4
:TOCH5
        pre_skip=0
        post_skip=0
        skip=0
        indent=4
:APPENDIX
        pre_top_skip=0
        pre_skip=0
        post_skip=3
        number_font=11
        font=11
        page_eject=odd
        section_eject=yes
        header=yes
        appendix_string=""
        number_style=bd
:INDEX
        columns=2
        spacing=1
        page_eject=odd
:IXHEAD
        pre_skip=1
        post_skip=1
        font=8
:I1
        pre_skip=0
        post_skip=0
        skip=0
:I2
        pre_skip=0
        post_skip=0
        skip=0
        indent='0.4cm'
:I3
        pre_skip=0
        post_skip=0
        skip=0
        indent='0.4cm'

:CMT.   B O D Y  -  T O P

:BANNER
        docsect=body
        place=topodd
        left_adjust=0
        right_adjust=0
        depth=3
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=0
        depth='0.23i'
        font=8
        refnum=1
        region_position=right
        pouring=last
        contents="&amp.headtxt1$."
:eBANREGION
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset='0.23i'
        depth=1
        font=5
        refnum=2
        region_position=left
        pouring=none
        contents=rule
:eBANREGION
:eBANNER
:BANNER
        docsect=body
        place=topeven
        refdoc=body
        refplace=topodd
:BANREGION
        refnum=1
        region_position=left
        contents="&amp.headtxt0$."
:eBANREGION
:eBANNER

:CMT.   B O D Y  -  B O T T O M

:BANNER
        docsect=body
        place=botodd
        left_adjust=0
        right_adjust=0
        depth=2
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=1
        depth=1
        font=8
        refnum=1
        region_position=right
        pouring=last
        script_format=no
        contents="&amp.headtext$".
:eBANREGION
:BANREGION
        indent=0
        hoffset=right
        width='.4i'
        voffset=1
        depth=1
        font=8
        refnum=2
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:eBANNER
:BANNER
        docsect=body
        place=boteven
        refdoc=body
        refplace=botodd
:BANREGION
        refnum=1
        width='.4i'
        region_position=left
        contents=pgnuma
        script_format=no
:eBANREGION
:BANREGION
        refnum=2
        width=extend
        region_position=left
        script_format=no
        contents="&amp.headtext$".
:eBANREGION
:eBANNER

:CMT.   H E A D 0  -  T O P

:BANNER
        docsect=head0
        place=topodd
        left_adjust=0
        right_adjust=0
        depth=3
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=0
        depth=1
        font=0
        refnum=1
        region_position=right
        pouring=last
        contents=' '
:eBANREGION
:eBANNER
:BANNER
        docsect=head0
        place=topeven
        refdoc=head0
        refplace=topodd
:eBANNER

:CMT.   H E A D 0  -  B O T T O M

:BANNER
        docsect=head0
        place=bottom
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   H E A D 1  -  T O P

:BANNER
        docsect=head1
        place=topodd
        depth=3
        left_adjust=0
        right_adjust=0
:BANREGION
        refnum=1
        voffset=0
        font=8
        region_position=left
        indent=0
        hoffset=left
        width=extend
        depth=1
        pouring=last
        contents=' '
:eBANREGION
:eBANNER

:CMT.   H E A D 1  -  B O T T O M

:BANNER
        docsect=head1
        place=botodd
        refdoc=body
        refplace=botodd
:eBANNER

:CMT.   A B S T R A C T  -  T O P

:BANNER
        docsect=abstract
        place=topodd
        refdoc=body
        refplace=topodd
:BANREGION
        refnum=1
        contents=none
:eBANREGION
:BANREGION
        refnum=2
        contents=none
:eBANREGION
:eBANNER
:BANNER
        docsect=abstract
        place=topeven
        refdoc=abstract
        refplace=topodd
:eBANNER

:CMT.   A B S T R A C T  -  B O T T O M

:BANNER
        docsect=abstract
        place=botodd
        refdoc=body
        refplace=botodd
:BANREGION
        refnum=1
        hoffset=center
        width=5
        font=0
        region_position=centre
        contents=pgnumr
        script_format=no
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        docsect=abstract
        place=boteven
        refdoc=abstract
        refplace=botodd
:eBANNER

:CMT.   P R E F A C E  -  T O P

:BANNER
        docsect=preface
        place=topodd
        refdoc=body
        refplace=topodd
:BANREGION
        refnum=1
        contents=none
:eBANREGION
:eBANNER
:BANNER
        docsect=preface
        place=topeven
        refdoc=preface
        refplace=topodd
:eBANNER

:CMT.   P R E F A C E  -  B O T T O M

:BANNER
        docsect=preface
        place=botodd
        refdoc=abstract
        refplace=botodd
:eBANNER
:BANNER
        docsect=preface
        place=boteven
        refdoc=abstract
        refplace=boteven
:eBANNER

:CMT.   T O C  -  T O P

:BANNER
        docsect=toc
        place=topodd
        refdoc=body
        refplace=topodd
:BANREGION
        hoffset=center
        width='4i'
        voffset=1
        depth=1
        font=11
        refnum=1
        region_position=centre
        pouring=last
        contents='Table of Contents'
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
:BANNER
        docsect=toc
        place=topeven
        refdoc=toc
        refplace=topodd
:eBANNER

:CMT.   T O C  -  B O T T O M

:BANNER
        docsect=toc
        place=boteven
        refdoc=abstract
        refplace=boteven
:eBANNER
:BANNER
        docsect=toc
        place=botodd
        refdoc=abstract
        refplace=botodd
:eBANNER

:CMT.   F I G L I S T  -  T O P

:BANNER
        docsect=figlist
        place=topodd
        refdoc=toc
        refplace=topodd
:BANREGION
        refnum=1
        contents='List of Figures'
:eBANREGION
:eBANNER
:BANNER
        docsect=figlist
        place=topeven
        refdoc=figlist
        refplace=topodd
:eBANNER

:CMT.   F I G L I S T  -  B O T T O M

:BANNER
        docsect=figlist
        place=boteven
        refdoc=toc
        refplace=boteven
:eBANNER
:BANNER
        docsect=figlist
        place=botodd
        refdoc=toc
        refplace=botodd
:eBANNER

:CMT.   A P P E N D I X  -  T O P

:BANNER
        docsect=appendix
        place=topodd
        refdoc=body
        refplace=topodd
:eBANNER
:BANNER
        docsect=appendix
        place=topeven
        refdoc=body
        refplace=topeven
:eBANNER

:CMT.   A P P E N D I X  -  B O T T O M

:BANNER
        docsect=appendix
        place=botodd
        refdoc=body
        refplace=botodd
:eBANNER
:BANNER
        docsect=appendix
        place=boteven
        refdoc=body
        refplace=boteven
:eBANNER

:CMT.   I N D E X  -  T O P

:BANNER
        docsect=index
        place=topodd
        refdoc=appendix
        refplace=topodd
:BANREGION
        refnum=1
        contents='Index'
:eBANREGION
:eBANNER
:BANNER
        docsect=index
        place=topeven
        refdoc=appendix
        refplace=topeven
:BANREGION
        refnum=1
        contents='Index'
:eBANREGION
:eBANNER

:CMT.   I N D E X  -  B O T T O M

:BANNER
        docsect=index
        place=botodd
        refdoc=appendix
        refplace=botodd
:BANREGION
        refnum=1
        contents=none
        script_format=no
:eBANREGION
:eBANNER
:BANNER
        docsect=index
        place=boteven
        refdoc=appendix
        refplace=boteven
:BANREGION
        refnum=2
        contents=none
:eBANREGION
:eBANNER
:eLAYOUT.
