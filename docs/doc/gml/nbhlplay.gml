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

:set symbol='format' value='help'.
:LAYOUT.
:PAGE
        top_margin='.30i'
        left_margin='0.00i'
        right_margin='7.65i'
        depth='15.0i'
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
        font=0
        pre_skip=2
        docnum_string="Document Number "
:DATE
        page_position=centre
        pre_skip=2
        left_adjust=0
        right_adjust=0
        font=2
:AUTHOR
        page_position=centre
        skip=0
        pre_skip=8
        left_adjust=0
        right_adjust=0
        font=2
:ADDRESS
        page_position=centre
        pre_skip=4
        left_adjust=0
        right_adjust=0
        font=2
:ALINE
        skip=0
:ABSTRACT
        header=no
        font=2
        page_eject=no
        page_reset=no
        abstract_string = none
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
        left_indent='.28i'
:FIG
        default_place=inline
        default_frame=none
        pre_skip=1
        post_skip=1
        font=4
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
        align='0.28i'
        skip=0
        font=1
        frame=rule
:OL
        number_style=hd
        left_indent='0.28i'
        align='0.28i'
        spacing=1
        post_skip=0
:UL
        left_indent='0.28i'
        align=1
        spacing=1
        pre_skip=1
        post_skip=0
        skip=1
:SL
        left_indent='0.28i'
        pre_skip=1
        skip=0
        spacing=1
        post_skip=0
:DL
        left_indent='0.0i'
        pre_skip=1
        post_skip=0
        align='11'
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
        number_form=prop
        number_font=9
        number_style=h
        font=9
        page_eject=no
:H4
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=prop
        number_font=8
        number_style=h
        font=8
        page_eject=no
:H5
        pre_top_skip=0
        pre_skip=1
        post_skip=0
        number_form=none
        font=8
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
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   B O D Y  -  B O T T O M

:BANNER
        docsect=body
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   H E A D 0  -  T O P

:BANNER
        docsect=head0
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   H E A D 0  -  B O T T O M

:BANNER
        docsect=head0
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   H E A D 1  -  T O P

:BANNER
        docsect=head1
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   H E A D 1  -  B O T T O M

:BANNER
        docsect=head1
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   A B S T R A C T  -  T O P

:BANNER
        docsect=abstract
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   A B S T R A C T  -  B O T T O M

:BANNER
        docsect=abstract
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER
:BANNER
        docsect=abstract
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   P R E F A C E  -  T O P

:BANNER
        docsect=preface
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   P R E F A C E  -  B O T T O M

:BANNER
        docsect=preface
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER
:BANNER
        docsect=preface
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   T O C  -  T O P

:BANNER
        docsect=toc
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   T O C  -  B O T T O M

:BANNER
        docsect=toc
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   F I G L I S T  -  T O P

:BANNER
        docsect=figlist
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   F I G L I S T  -  B O T T O M

:BANNER
        docsect=figlist
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   A P P E N D I X  -  T O P

:BANNER
        docsect=appendix
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   A P P E N D I X  -  B O T T O M

:BANNER
        docsect=appendix
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CMT.   I N D E X  -  T O P

:BANNER
        docsect=index
        place=top
        left_adjust=0
        right_adjust=0
        depth=0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:CMT.   I N D E X  -  B O T T O M

:BANNER
        docsect=index
        place=bottom
        left_adjust=0
        right_adjust=0
        depth=0
:eBANNER

:CONVERT.NBHLPLAY.LAY
:eLAYOUT.
