.* test document with some sections changes
.* and &amp.h0 - &amp.h6 headers
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
:cmt. construct extensions for wgml4 and new wgml layout files
.if &'left('&sysversion',1) = 'O' .se lext=loy
.el .se lext=lo4
.if &'left('&sysversion',1) = 'O' .se lext2=lay
.el .se lext2=la4
:layout
:convert file="&fil..&lext.".:cmt. save layout 'before'
:default
        columns = 2
:abstract
        header = yes
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 4
        place = topodd
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
        script_format = yes
        contents = '//Abstract ODD &$amp.$pgnumr.//'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 4
        place = topeven
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
        script_format = yes
        contents = '//Abstract EVEN &$amp.$pgnumr.//'
:eBANREGION
:eBANNER
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
        contents = '/&$amp.date./-top body &$amp.$htext1.-&$amp.$htext2.-/ Page &$amp.$pgnuma./'
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
        contents = '/>&$amp.$hnum2.</bottom body $HEAD1 >&amp.$head1.</ &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:BANNER
        left_adjust = 0
        right_adjust = 0
        depth = 1
        place = bottom
        docsect = head2
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
        contents = '/>&$amp.$hnum2.</bottom head2 $HEAD1 >&amp.$head1.</ &$amp.$pgnuma./'
:eBANREGION
:eBANNER
:convert file="&fil..&lext2.".:cmt. save layout 'after'
:elayout
:gdoc
:frontm
:titlep
:cmt. H0.text not allowed
..if 1 eq 1 ..th ..do begin
    :title stitle='sect doc'.Section Test Document
    :title stitle='sect doc2'.Title line 2
..do end
.ty title sysin=&$in  sysinr=&$inr sysir=&$ir sysll=&$ll lm=&$pagelm rm=&$pagerm
.ty title sysin=&$in  sysinr=&$inr sysir=&$ir sysll=&$ll  syslc=&$lc syslst=&$lst
:docnum.1234567
:date.Date was 25.08.2010
:author.First Author
:author.My Second Personality
:address.
:aline.address 1 1
:aline.address 1 2
:cmt. Text not allowed here
:eaddress.
:cmt. Text not allowed here
:address.
:aline.address 2 1
:aline.address 2 2
:eaddress.
:address.
:eaddress.
:etitlep
:cmt. Text not allowed here
:abstract
:p.Text in abstract.
:h2 id='abstr2'.H2 abstract H2
.pe 100
        Text in Abstract to create a page overflow to see wgml4.0 page overflow.
:body
:H0 id='body0'.H0 body h0
:H1 id='body1'.H1 in Body and some looooooooooooooooooong text
:H2 id='body21'.H2 1 in Body
:cmt. :h1 id='meinlangernamealsId'.Is was?
:p.
.li on
:hp1.HP1 text in input line one.:ehp1.
    :hp2.HP2 text:ehp2.
:hp3.HP3 text:ehp3.
.br
.us us text us
no more us text
.br
.li off
:H2 id='body22'.H2 2 in Body
More text in another line two
More text in another line three
More text in another line four
.pa nostart
More text in another line five after pa nostart
:cmt. .pa odd
:H2 id='body23'.H2 3 in Body.
More text in another line six
.pa odd
:H2 id='body24' xx. H2 4 in BODY with a looooooooooooooot of text.
:H3 id='body31'. H3 1 in BODY
:hp1.HP1 text in input line.:ehp1.
More text in another line six
More text in another line seven
More text in another line eight
More text in another line nine
.br
.se pe=0
.pe 50
.se pe=&pe.+1;   (&pe.). Performed line with a count of 100.
:fn
This is a footnote to watch what happens to a long footnote in Multicolumn mode
:efn
.pe 50
.se pe=&pe.+1;   (&pe.). Performed line with a count of 100.
.im ts.inc
.cp 18
.br
After cp
:backm
:egdoc
