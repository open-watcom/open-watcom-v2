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
:CMT.
:CMT. Audit Trail:
:CMT.   Date       Name                 Reason
:CMT.   ****       ****                 ******
:CMT.   88/5/25    Dave Neudoerffer     Initial implementation
:CMT.   88/5/27    Dave Neudoerffer     spacing around lists
:CMT.   88/5/27    Dave Neudoerffer     centered heading 4's
:CMT.   88/5/30    Dave Neudoerffer     Fixed figure headings
:CMT.   88/6/10    Dave Yach            Fixed bannering for new GML
:CMT.
:CMT.************************************************************

:set symbol='indent'      value='0i'.
:cmt. :set symbol='bxindent'    value='6'.
:cmt. :set symbol='tbindent'    value='8'.
:set symbol='bxindent'    value='16'.
:set symbol='tbindent'    value='18'.
:set symbol='banodd'      value=''.
:set symbol='baneven'     value=''.
:LAYOUT
:PAGE
        left_margin=0
        right_margin=70
        top_margin=0
        depth=80
:DEFAULT
        binding=0
        input_esc='\'
        justify=no
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
        pre_skip = 10
        left_adjust = 0
        right_adjust = 0
        font = 2
:ADDRESS
        page_position = right
        pre_skip = 2
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
:PREFACE
        spacing=1
        font=2
:P
        pre_skip=0
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
        pre_skip=0
        post_skip=0
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
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        display_heading=no
        number_form=none
        number_reset=no
        number_font=0
        font=0
:H1
        indent=0
        page_position=left
        page_eject=yes
        post_skip=0
        pre_top_skip=0
        number_form=new
        display_heading=no
        number_font=0
        font=0
:H2
        display_heading=no
        number_form=prop
        number_style = h
        post_skip=0
        pre_top_skip=0
        font=0
        number_font=0
        page_eject=yes
:H3
        display_heading=no
        number_form=prop
        number_style = h
        post_skip=0
        pre_top_skip=0
        font=0
        number_font=0
        page_eject = yes
:H4
        display_heading=no
        number_form = none
        number_style = h
        post_skip=0
        pre_top_skip=0
        font=0
        number_font=0
        page_eject = yes
:cmt.:H4
:cmt.   page_position=centre
:cmt.   number_form = none
:cmt.   post_skip=1
:cmt.   pre_top_skip=1
:cmt.   font=11
:cmt.   page_eject = yes
:H5
        display_heading=yes
        number_form=none
        post_skip=0
        pre_top_skip=1
        font=3
        page_eject = no
        line_break = yes
:H6
        display_heading=yes
        number_form = none
        post_skip=0
        pre_top_skip=1
        font=3
        page_eject = no
        line_break = yes
:OL
        number_style=hd
        left_indent=3
        align=3
        spacing=1
        post_skip=1
:UL
        left_indent=2
        align=2
        skip=0
        spacing=1
        pre_skip=1
        post_skip=1
:SL
        left_indent=0
        pre_skip=0
        skip=0
        spacing=1
        post_skip=0
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
        display_heading = no
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
        left_adjust = 0
        right_adjust = 0
        depth = 3
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
        contents = ''
:EBANREGION
:EBANNER
:eLAYOUT
:INCLUDE file='wbsymbol'.
:cmt :INCLUDE file='symbol'.
:INCLUDE file='help'.
