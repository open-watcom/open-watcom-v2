.* test for :Hx :HDREF tags
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
:layout
:H1
        number_font = 3
        number_form = new
        number_style = cpb
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 1
        place = top
        docsect = body
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 2
        refnum = 1
        region_position = left
        pouring = last
        script_format = yes
        contents = '/&$amp.date./-b &$amp.$htext1. b-/ Page &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 1
        place = bottom
        docsect = body
:BANREGION
        indent = 0
        hoffset = left
        width = extend
        voffset = 0
        depth = 1
        font = 0
        refnum = 1
        region_position = left
        pouring = last
        script_format = yes
        contents = '/&$amp.$htext1.// &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:convert file="&fil..lay".
:elayout
:gdoc
:body
:h1 id='myLongNameAsIdmyLongNameAsIdmyLongNameAsIdmyLongNameAsIdmyLongNameAsId'.Very long ID
:p.
:hp1.HP1 text in input line one.:ehp1.
:hp2.HP2 text:ehp2.
:h1 id='myLongName'.Long ID
Some text with hdref on same page:
:hdref refid='myLongName'..
:h1 id='myLongN'.ID is 7 chars
:cmt. :h1 id='myLongN'.dupl ID
:hp3.HP3 text:ehp3.
:hdref refid=unknown page=yes..
.br
.us us text us
no more us text
.br
More text in another line three
More text in another line four
.pa nostart
:hp1.HP1 text in input line one.:ehp1.
More text in another line two
More text in another line three
More text in another line four
More text in another line five
.pe 100
     Performed text with a count of 100.
.cp 18
.br
.ju off
After cp see
:hdref refid='myLongNameAsIdmyLongNameAsIdmyLongNameAsIdmyLongNameAsIdmyLongNameAsId'.
.co off
Another try see :hdref refid='myLongName'.
Next try see :hdref refid='MYLONGN'..
See invalid hdref :hdref refid="notfnd" page=no.
:egdoc
