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

:set symbol='indent'      value='1i'.
:set symbol='left_in' value='.3i'.
:set symbol='bxindent'    value='16'.
:set symbol='tbindent'    value='18'.
:set symbol='banodd'      value=''.
:set symbol='baneven'     value=''.

.dm h0 begin
.pa odd
:set symbol='baneven'      value=''.
.in 0
.if '&*id' eq '' .do begin
:h0.&*
.do end
.el .do begin
:h0 id='&*id'.&*
.do end
.in &indent
:set symbol='baneven'      value='&*'.
.dm h0 end

.gt h0 add h0 attribute nocont
.ga * id any

.dm h1 begin
.pa odd
:set symbol='banodd'      value=''.
.in 0
.if '&*id' eq '' .do begin
:h1.&*
.do end
.el .do begin
:h1 id='&*id'.&*
.do end
.in &indent
:set symbol='banodd'      value='&*'.
:BINCLUDE file='(t:80)rule.eps' reposition=start depth='0'.
.dm h1 end

.gt h1 add h1 attribute nocont
.ga * id any

:LAYOUT
:PAGE
        left_margin='1.25i'
        right_margin='7.25i'
        top_margin='.5i'
        depth='8.00i'
:DEFAULT
        justify=no
        binding=0
        input_esc='\'
:TITLEP
        spacing=1
:TITLE
        page_position = right
        pre_top_skip = 7
        left_adjust = 0
        right_adjust = 0
        font = 12
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
        font=13
        page_reset = no
:PREFACE
        font = 13
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
        left_indent='&left_in'
:FIG
        left_adjust='&indent.'
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
        threshold=2
:H0
        indent=0
        page_position=left
        page_eject=odd
        number_reset=no
        number_font=10
        font=10
:H1
        indent=0
        page_position=left
        page_eject=odd
        post_skip='0.2i'
        pre_top_skip=0
        number_form=new
        display_heading=yes
        number_font=18
        font=11
:H2
        number_form = prop
        number_style = h
        post_skip=0
        pre_top_skip=1
        font=12
        number_font=12
        page_eject=no
:H3
        number_form = prop
        number_style = h
        post_skip=0
        pre_top_skip=1
        font=13
        number_font=13
        page_eject = no
:H4
        indent='&indent'
        number_form = prop
        number_style = h
        post_skip=0
        pre_top_skip=1
        font=14
        number_font=14
        page_eject = no
:H5
        indent='&indent'
        number_form = none
        post_skip=0
        pre_top_skip=1
        font=15
        page_eject = no
        line_break = yes
:H6
        indent='&indent'
        number_form = none
        post_skip=0
        pre_top_skip=1
        font=16
        page_eject = no
        line_break = yes
:OL
        number_style=hd
        left_indent='&left_in'
        align='.3i'
        spacing=1
        post_skip=1
:UL
        left_indent='&left_in'
        align='.12i'
        skip=0
        spacing=1
        pre_skip=1
        post_skip=1
:SL
        left_indent='&left_in'
        pre_skip=1
        skip=0
        spacing=1
        post_skip=1
:DL
        left_indent='&left_in'
        pre_skip=1
        post_skip=1
        align='0.8i'
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
        font = 11
        number_font=18
        header=no
        page_eject = odd
        section_eject = yes
        indent=0
        page_position=left
        post_skip='0.2i'
        pre_top_skip=0
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
        pouring = none
        contents = ' '
:EBANREGION
:eBANNER
:BANNER
        docsect = head1
        place = topodd
        refdoc = head0
        refplace = topodd
:EBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=2
        place=topodd
        docsect=body
:BANREGION
        indent=0
        hoffset=right
        width='4i'
        voffset=0
        depth=1
        font=17
        refnum=1
        region_position=right
        pouring=last
        contents=&amp.banodd
:eBANREGION
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=1
        depth=1
        font=5
        refnum=2
        region_position=left
        pouring=none
        contents=rule
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=2
        place=topeven
        docsect=body
:BANREGION
        indent=0
        hoffset=left
        width='4i'
        voffset=0
        depth=1
        font=17
        refnum=1
        region_position=left
        pouring=last
        contents=&amp.baneven
:eBANREGION
:BANREGION
        indent=0
        hoffset=left
        width=extend
        voffset=1
        depth=1
        font=5
        refnum=2
        region_position=left
        pouring=none
        contents=rule
:eBANREGION
:eBANNER
:BANNER
        left_adjust=0
        right_adjust=0
        depth=2
        place=botodd
        docsect=body
:BANREGION
        indent=0
        hoffset=right
        width='.7i'
        voffset=1
        depth=1
        font=16
        refnum=1
        region_position=right
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        refnum=2
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
        width='.7i'
        voffset=1
        depth=1
        font=16
        refnum=1
        region_position=left
        pouring=last
        contents=pgnuma
:eBANREGION
:BANREGION
        refnum=2
:eBANREGION
:eBANNER
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
:BANREGION
        refnum=1
        contents = 'Appendix &amp.$hnum1.'
:eBANREGION
:eBANNER
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
        font = 16
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
        font = 17
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'Table of Contents'
:EBANREGION
:EBANNER
:BANNER
        docsect = toc
        place = topeven
        refdoc = toc
        refplace = topodd
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
        font = 17
        refnum = 1
        region_position = centre
        pouring = last
        contents = 'List of Figures'
:EBANREGION
:EBANNER
:BANNER
        docsect = figlist
        place = topeven
        refdoc = figlist
        refplace = topodd
:EBANNER
:BANNER
        docsect=index
        place=topodd
        refdoc=body
        refplace=topodd
:BANREGION
        refnum=1
        contents='Index'
:eBANREGION
:eBANNER
:BANNER
        docsect=index
        place=topeven
        refdoc=body
        refplace=topeven
:BANREGION
        refnum=1
        contents='Index'
:eBANREGION
:eBANNER
:BANNER
        docsect=index
        place=botodd
        refdoc=body
        refplace=botodd
:eBANNER
:BANNER
        docsect=index
        place=boteven
        refdoc=body
        refplace=boteven
:eBANNER
:eLAYOUT
:INCLUDE
        file='wbsymbol'.
