:cmt. For testing whether values are processed        FOR NEW WGML ONLY!
:cmt. file tlayfull.lll should look nearly the same as this one
:cmt. when all layout tags are implemented
:LAYOUT
:PAGE
        top_margin = 1
        left_margin = 2
        right_margin = 3
        depth = 4
:DEFAULT
        spacing = 5
        columns = 6
        font = 7
        justify = yes
        input_esc = '#'
        gutter = 8
        binding = 9
:WIDOW
        threshold = 10
:FN
        line_indent = 11
        align = 12
        pre_lines = 13
        skip = 14
        spacing = 15
        font = 16
        number_font = 17
        number_style = rpa
        frame = rule
:FNREF
        font = 18
        number_style = ap
:P
        line_indent = 19
        pre_skip = 20
        post_skip = 21
:PC
        line_indent = 22
        pre_skip = 23
        post_skip = 24
:FIG
        left_adjust = 25
        right_adjust = 26
        pre_skip = 27
        post_skip = 28
        spacing = 29
        font = 30
        default_place = bottom
        default_frame = none
:XMP
        left_indent = 31
        right_indent = 32
        pre_skip = 33
        post_skip = 34
        spacing = 35
        font = 36
:NOTE
        left_indent = 37
        right_indent = 38
        pre_skip = 39
        post_skip = 40
        font = 41
        spacing = 42
        note_string = "Achtung: "
:H0
        group = 43
        indent = 44
        pre_top_skip = 45
        pre_skip = 46
        post_skip = 47
        spacing = 48
        font = 49
        number_font = 50
        number_form = none
        page_position = centre
        number_style = hd
        page_eject = no
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = upper
        align = 51
:H1
        group = 0
        indent = 0
        pre_top_skip = 3
        pre_skip = 0
        post_skip = 3
        spacing = 1
        font = 3
        number_font = 3
        number_form = new
        page_position = left
        number_style = h
        page_eject = yes
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
:H2
        group = 0
        indent = 0
        pre_top_skip = 2
        pre_skip = 0
        post_skip = 2
        spacing = 1
        font = 3
        number_font = 3
        number_form = prop
        page_position = left
        number_style = h
        page_eject = no
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
:H3
        group = 0
        indent = 0
        pre_top_skip = 2
        pre_skip = 0
        post_skip = 2
        spacing = 1
        font = 3
        number_font = 3
        number_form = prop
        page_position = left
        number_style = h
        page_eject = no
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
:H4
        group = 0
        indent = 0
        pre_top_skip = 2
        pre_skip = 0
        post_skip = 2
        spacing = 1
        font = 2
        number_font = 3
        number_form = prop
        page_position = left
        number_style = h
        page_eject = no
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
:H5
        group = 0
        indent = 0
        pre_top_skip = 2
        pre_skip = 0
        post_skip = 2
        spacing = 1
        font = 2
        number_font = 3
        number_form = prop
        page_position = left
        number_style = h
        page_eject = no
        line_break = no
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
:H6
        group = 0
        indent = 0
        pre_top_skip = 2
        pre_skip = 0
        post_skip = 2
        spacing = 1
        font = 1
        number_font = 3
        number_form = prop
        page_position = left
        number_style = h
        page_eject = no
        line_break = no
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 52
:HEADING
        delim = '.'
        stop_eject = no
        para_indent = no
        threshold = 53
        max_group = 54
:LQ
        left_indent = 55
        right_indent = 56
        pre_skip = 57
        post_skip = 58
        spacing = 59
        font = 60
:DT
        font = 61
:GT
        font = 62
:DTHD
        font = 63
:CIT
        font = 64
:FIGCAP
        pre_lines = 65
        font = 66
        figcap_string = "Abbildung "
        string_font = 67
        delim = '.'
:FIGDESC
        pre_lines = 68
        font = 69
:DD
        line_left = 70
        font = 71
:GD
        font = 72
:DDHD
        font = 73
:ABSTRACT
        post_skip = 74
        pre_top_skip = 75
        font = 76
        spacing = 77
        header = yes
        abstract_string = "Abstrakt"
        page_eject = yes
        page_reset = yes
        columns = 2
:PREFACE
        post_skip = 78
        pre_top_skip = 79
        font = 80
        spacing = 81
        header = yes
        preface_string = "Vorwort"
        page_eject = yes
        page_reset = no
        columns = 2
:BODY
        post_skip = 82
        pre_top_skip = 83
        header = no
        body_string = "abc"
        page_eject = yes
        page_reset = yes
        font = 84
:BACKM
        post_skip = 85
        pre_top_skip = 86
        header = no
        backm_string = ""
        page_eject = yes
        page_reset = no
        columns = 3
        font = 3
:LP
        left_indent = 87
        right_indent = 88
        line_indent = 89
        pre_skip = 90
        post_skip = 91
        spacing = 92
:INDEX
        post_skip = 93
        pre_top_skip = 94
        left_adjust = 95
        right_adjust = 96
        spacing = 97
        columns = 98
        see_string = "See "
        see_also_string = "See also "
        header = no
        index_string = "Index"
        page_eject = yes
        page_reset = no
        font = 99
:IXPGNUM
        font = 100
:IXMAJOR
        font = 101
:IXHEAD
        pre_skip = 2
        post_skip = 3
        font = 4
        indent = 5
        frame = box
        header = yes
:I1
        pre_skip = 6
        post_skip = 7
        skip = 8
        font = 9
        indent = 10
        wrap_indent = 11
        index_delim = ".,"
        string_font = 12
:I2
        pre_skip = 13
        post_skip = 14
        skip = 15
        font = 16
        indent = 17
        wrap_indent = 18
        index_delim = ".."
        string_font = 19
:I3
        pre_skip = 20
        post_skip = 21
        skip = 22
        font = 23
        indent = 24
        wrap_indent = 25
        index_delim = "  "
:TOC
        left_adjust = 26
        right_adjust = 27
        spacing = 28
        columns = 29
        toc_levels = 3
        fill_string = "_"
:TOCPGNUM
        size = '0.5i'
        font = 30
:TOCH0
        group = 0
        indent = 31
        skip = 32
        pre_skip = 33
        post_skip = 34
        font = 35
        align = 36
        display_in_toc = yes
:TOCH1
        group = 0
        indent = 37
        skip = 38
        pre_skip = 39
        post_skip = 40
        font = 1
        align = 41
        display_in_toc = yes
:TOCH2
        group = 0
        indent = '0.22i'
        skip = 42
        pre_skip = 43
        post_skip = 44
        font = 2
        align = 45
        display_in_toc = yes
:TOCH3
        group = 0
        indent = '0.23i'
        skip = 46
        pre_skip = 47
        post_skip = 48
        font = 3
        align = 49
        display_in_toc = yes
:TOCH4
        group = 0
        indent = '0.24i'
        skip = 50
        pre_skip = 51
        post_skip = 52
        font = 4
        align = 53
        display_in_toc = no
:TOCH5
        group = 0
        indent = '0.25i'
        skip = 0
        pre_skip = 0
        post_skip = 0
        font = 5
        align = 0
        display_in_toc = no
:TOCH6
        group = 0
        indent = '0.26i'
        skip = 0
        pre_skip = 0
        post_skip = 0
        font = 6
        align = 0
        display_in_toc = no
:FIGLIST
        left_adjust = 37
        right_adjust = 38
        skip = 39
        spacing = 40
        columns = 41
        fill_string = "."
:FLPGNUM
        size = '0.55i'
        font = 42
:TITLEP
        spacing = 43
        columns = 44
:TITLE
        left_adjust = 45
        right_adjust = '6i'
        page_position = right
        font = 3
        pre_top_skip = 46
        skip = 48
:DOCNUM
        left_adjust = 49
        right_adjust = '1i'
        page_position = right
        font = 8
        pre_skip = 50
        docnum_string = "Dokument Nummer "
:DATE
        date_form = "$ml $dsn, $yl"
        left_adjust = 51
        right_adjust = '1.52i'
        page_position = right
        font = 5
        pre_skip = 53
:AUTHOR
        left_adjust = 0
        right_adjust = '1i'
        page_position = right
        font = 0
        pre_skip = 25
        skip = 1
:ADDRESS
        left_adjust = 0
        right_adjust = '1i'
        page_position = right
        font = 0
        pre_skip = 2
:ALINE
        skip = 1
:FROM
        left_adjust = 0
        page_position = right
        pre_top_skip = 6
        font = 0
:TO
        left_adjust = 0
        page_position = left
        pre_top_skip = 1
        font = 0
:ATTN
        left_adjust = 0
        page_position = left
        pre_top_skip = 1
        font = 1
        attn_string = "Attention: "
        string_font = 1
:SUBJECT
        left_adjust = 0
        page_position = centre
        pre_top_skip = 2
        font = 1
:LETDATE
        date_form = "$ml $dsn, $yl"
        depth = 15
        font = 0
        page_position = right
:OPEN
        pre_top_skip = 2
        font = 0
        delim = ':'
:CLOSE
        pre_skip = 2
        depth = 6
        font = 0
        page_position = centre
        delim = ','
        extract_threshold = 2
:ECLOSE
        pre_skip = 1
        font = 0
:DISTRIB
        pre_top_skip = 3
        skip = 1
        font = 0
        indent = '0.5i'
        page_eject = no
:APPENDIX
        indent = 0
        pre_top_skip = 0
        pre_skip = 0
        post_skip = 3
        spacing = 1
        font = 3
        number_font = 3
        number_form = new
        page_position = left
        number_style = b
        page_eject = yes
        line_break = yes
        display_heading = yes
        number_reset = yes
        case = mixed
        align = 0
        header = yes
        appendix_string = "APPENDIX "
        page_reset = no
        section_eject = yes
        columns = 1
:SL
        level = 1
        left_indent = 0
        right_indent = 0
        pre_skip = 1
        skip = 1
        spacing = 1
        post_skip = 1
        font = 0
:OL
        level = 1
        left_indent = 0
        right_indent = 0
        pre_skip = 1
        skip = 1
        spacing = 1
        post_skip = 1
        font = 0
        align = '0.4i'
        number_style = hd
        number_font = 0
:UL
        level = 1
        left_indent = 0
        right_indent = 0
        pre_skip = 1
        skip = 1
        spacing = 1
        post_skip = 1
        font = 0
        align = '0.4i'
        bullet = '*'
        bullet_translate = yes
        bullet_font = 0
:DL
        level = 1
        left_indent = 0
        right_indent = 0
        pre_skip = 1
        skip = 1
        spacing = 1
        post_skip = 1
        align = '1i'
        line_break = no
:GL
        level = 1
        left_indent = 0
        right_indent = 0
        pre_skip = 1
        skip = 1
        spacing = 1
        post_skip = 1
        align = 0
        delim = ':'
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = bottom
        docsect = head0
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 2
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = last
        script_format = yes
        contents = '/&$amp.$htext0.// &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = bottom
        docsect = body
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 2
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = last
        script_format = yes
        contents = '/&$amp.$htext1.// &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 4
        place = bottom
        docsect = abstract
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 3
        depth = 1
        font = 0
        refnum = 1
        region_position = centre
        pouring = last
        script_format = no
        contents = pgnumr
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 4
        place = bottom
        docsect = preface
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 3
        depth = 1
        font = 0
        refnum = 1
        region_position = centre
        pouring = last
        script_format = no
        contents = pgnumr
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = top
        docsect = toc
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 1
        depth = 1
        font = 3
        refnum = 1
        region_position = centre
        pouring = last
        script_format = no
        contents = 'Table of Contents'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = top
        docsect = figlist
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 1
        depth = 1
        font = 3
        refnum = 1
        region_position = centre
        pouring = last
        script_format = no
        contents = 'List of Figures'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = top
        docsect = index
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 1
        depth = 1
        font = 3
        refnum = 1
        region_position = centre
        pouring = last
        script_format = no
        contents = 'Index'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = top
        docsect = letter
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = none
        script_format = yes
        contents = '/&$amp.date.// Page &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = topodd
        docsect = letlast
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = none
        script_format = yes
        contents = '/&$amp.date.// Page &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:convert.tlayfull.lll
:eLAYOUT
