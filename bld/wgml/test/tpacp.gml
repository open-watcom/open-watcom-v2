.* test for .pa and .cp
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
:layout
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
:cmt. :h1 id='meinlangernamealsId'.Is was?
:p.
:hp1.HP1 text in input line one.:ehp1.
:hp2.HP2 text:ehp2.
:hp3.HP3 text:ehp3.
.br
.us us text us
no more us text
.br
More text in another line three
More text in another line four
.pa nostart
More text in another line five
.pa odd
More text in another line six
.pa odd
:hp1.HP1 text in input line one.:ehp1.
More text in another line two
More text in another line three
More text in another line four
More text in another line five
.pe 100
     Performed text with a count of 100.
.cp 18
.br
After cp
:egdoc
