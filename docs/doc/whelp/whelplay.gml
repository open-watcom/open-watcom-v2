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

:set symbol='indent'      value='0i'.
:set symbol='left_in'     value='.3i'.
:set symbol='left_in2'    value='.4i'.
:set symbol='bxindent'    value='4'.
:set symbol='tbindent'    value='6'.
:set symbol='figbxind'    value='1'.
:set symbol='figtbind'    value='3'.
:set symbol='boxtbind'    value='2'.
:set symbol='cmdtbind'    value='13'.
:set symbol='banodd'      value=''.
:set symbol='baneven'     value=''.
:set symbol='inbanod'     value=''.
:set symbol='inbanev'     value=''.
:set symbol='svbanod'     value=''.
:set symbol='svbanev'     value=''.


:set symbol='format' value='help'.
:LAYOUT
:PAGE
        top_margin=0
        left_margin=0
        right_margin=2000
        depth=200
:DEFAULT
        binding=0
        input_esc='\'
        justify=no
:P
        pre_skip=0
        line_indent=0
        post_skip=1
:PC
        pre_skip=0
        line_indent=0
        post_skip=1
:XMP
        pre_skip=0
        post_skip=1
        font=4
        left_indent='0.5i'
:FIG
        default_place=inline
        default_frame=none
        pre_skip=0
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
        align='0.3i'
        skip=0
        font=1
        frame=rule
:OL
        left_indent=0
        align=0
        spacing=1
        pre_skip=1
        post_skip=1
        skip=0
        number_style=hd
:UL
        left_indent=0
        align=0
        spacing=1
        pre_skip=1
        post_skip=1
        skip=0
        bullet=''
:SL
        left_indent=0
        pre_skip=0
        skip=0
        spacing=1
        post_skip=1
:DL
        level=1
        left_indent=0
        pre_skip=1
        post_skip=1
        align=0
:DL
        level=2
        left_indent=0
        pre_skip=0
        post_skip=1
        align=0
:DTHD
        font=3
:DDHD
        font=3
:DT
        font=4
:DD
        font=0
:H0
        indent=0
        page_position=left
        page_eject=no
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        line_break=yes
:H1
        indent=0
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        line_break=yes
:H2
        indent=0
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        line_break=yes
:H3
        indent=0
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        line_break=yes
:H4
        indent=0
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        line_break=yes
:H5
        indent=0
        page_position=left
        page_eject=no
        post_skip=0
        pre_top_skip=1
        pre_skip=1
        display_heading=yes
        number_form=none
        number_reset=no
        number_font=0
        font=15
        line_break=yes
:H6
        indent=0
        page_position=left
        page_eject=no
        post_skip=0
        pre_top_skip=0
        pre_skip=0
        display_heading=yes
        number_form=none
        number_reset=no
        number_font=0
        font=16
        line_break=yes
:TOC
        spacing = 1
        toc_levels = 4
:TOCH0
        display_in_toc=no
        font = 14
:TOCH1
        skip = 1
        indent = 2
        pre_skip = 1
        post_skip = 0
        font = 15
:TOCH2
        skip = 0
        indent = 8
        pre_skip=0
        post_skip=0
:TOCH3
        skip = 0
        indent = 8
        pre_skip=0
        post_skip=0
:TOCH4
        skip = 0
        indent = 8
        pre_skip=0
        post_skip=0
:TOCH5
        skip = 0
        indent = 8
        pre_skip=0
        post_skip=0
:INDEX
        columns = 2
        spacing = 1
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
        indent = 0
        pre_top_skip = 0
        post_skip = 0
        spacing = 1
        number_form = new
        page_position = left
        number_style = b
        page_eject = yes
        line_break = yes
        display_heading=no
        number_reset = yes
        appendix_string = "APPENDIX "
:BANNER
        place=bottom
        docsect=head0
:BANREGION
        refnum=1
:eBANREGION
:eBANNER

:BANNER
        place=bottom
        docsect=body
:BANREGION
        refnum=1
:eBANREGION
:eBANNER
:BANNER
        place = bottom
        docsect = abstract
:BANREGION
        refnum=1
:eBANREGION
:EBANNER
:BANNER
        place = bottom
        docsect = preface
:BANREGION
        refnum=1
:eBANREGION
:EBANNER
:BANNER
        place = top
        docsect = toc
:BANREGION
        refnum=1
:EBANREGION
:EBANNER
:CONVERT.WHELPLAY.LAY
:eLAYOUT

:INCLUDE file='whelp'.

