:INCLUDE file='GMLPAPER'.
:LAYOUT.
:default input_esc='\'.
:title font=10.
:BODY page_eject=no.
:H0 font=11 page_eject=no pre_top_skip='.5i'.
:h1 font=2 page_position=left.
:h2 font=2 page_position=left.
:ul skip=0.
:TOCH1
    indent = '.2i'
:P
    line_indent = 0
:BANNER
    docsect = head0
    place = topodd
    refdoc = body
    refplace = topodd
:eBANNER
:BANNER
    docsect = head0
    place = topeven
    refdoc = body
    refplace = topeven
:eBANNER
:eLAYOUT.
:include file='wbsymbol.gml'
:include file='symbol.gml'
