:cmt. test :xmp :exmp and some allowed tags inside
:cmt.
:cmt.
:LAYOUT.
:P
        line_indent = 0
        pre_skip = 1
        post_skip = 0
:PC
        line_indent = 6
        pre_skip = 1
        post_skip = 0
:LP
        left_indent = 4
        right_indent = 3
        line_indent = 2
        pre_skip = 1
        post_skip = 0
        spacing = 1
:DT
        font = 2
:GT
        font = 2
:DTHD
        font = 1
:DD
        line_left = '0.5i'
        font = 0
:GD
        font = 0
:DDHD
        font = 1
:SL
        level = 1
        left_indent = 2
        right_indent = 0
        pre_skip = 1
        skip = 0
        spacing = 1
        post_skip = 0
        font = 0
:OL
        level = 1
        left_indent = 2
        right_indent = 15
        pre_skip = 1
        skip = 0
        spacing = 1
        post_skip = 0
        font = 0
        align = '0.4i'
        number_style = hd
        number_font = 0
:UL
        level = 1
        left_indent = 2
        right_indent = 0
        pre_skip = 1
        skip = 0
        spacing = 1
        post_skip = 0
        font = 0
        align = '0.4i'
        bullet = '*'
        bullet_translate = yes
        bullet_font = 0
:DL
        level = 1
        left_indent = 2
        right_indent = 0
        pre_skip = 0
        skip = 0
        spacing = 1
        post_skip = 0
        align = '1i'
        line_break = no
:GL
        level = 1
        left_indent = 2
        right_indent = 0
        pre_skip = 0
        skip = 0
        spacing = 1
        post_skip = 0
        align = 0
        delim = ':'
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 3
        place = top
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
:convert file='txmp.lay'.
:eLAYOUT.
:GDOC.
:BODY.
.ty body  in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
:p.
First paragraph. Example follows:
:cmt. :xmp depth='2cm'.
:xmp.
  Line1 xxxx
  Line2 yyyy
  :p. P. Line3 xyz xyz xyz xyz xyz
  Line44 zzzz 1111
.ty typexxxx
:p. P. new paragraph with a large line to let it overflow or perhaps truncated or something different or something quite different?
:p. P. another paragraph this is last of example.
:exmp.
:p.
&gml.SL follows.
.ty p     in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
:sl compact.
    :li.simple 1 compact
.ty li sl in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
    :li.simple 2 compact
:lp.
&gml.OL follows.
    :ol.
.*      This is text before first &gml.li which extends over more
.*         than one line to see how it looks.
        :li.ordered 1 no compact spacing 2 and some text to hit the right margin
to see how a multi line list item looks
.ty li ol in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
        :li.ordered 2 no compact
        :note. This is a note which extends over more than one line to see how it looks.
    :lp.list part
still list part still list part still list part still list part still list part
still list part still list part still list part still list part still list part
.ty lp    in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
        :li.ordered 3 no compact
    :ul
        :li.ul with eul in txl2 include file
.im txl2.inc
    :eol.
:LP.
&gml.OL end.
:LP.
&gml.UL follows.
    :ul.
        :li.unordered 1 no compact
        :li.unordered 2 no compact
    :eul.
:LP.&gml.UL end.
    :li.simple 3 compact
:ul   compact.
:lp. ul compact started with &gml.LP as first item.
:li. ul item 1
:li. ul item 2
:li. ul item 3
:li. ul item 4
:eul.
:esl.
&gml.SL end.
.ty end  in=&$in inr=&$inr ir=&$ir ll=&$ll lc=&$lc lst=&$lst pg=&$pgnuma
:EGDOC.
