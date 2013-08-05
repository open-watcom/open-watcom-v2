.* test for .ix and &gml. index tags
.* requires INDEX option on cmdline
.se fil=&'left(&sysfnam,&'lastpos('.',&sysfnam)-1)
.if &'left('&sysversion',1) = 'O' .se lext=lay;.el .se lext=la4
:Layout
:H1
        display_heading = yes
        page_position = right
:IXPGNUM
        font = 2
:convert file='&fil..&lext'.
:elayout
:set symbol='willi' value='emil'
:gdoc
:body
:H1 id="H00".Header 1 title --------------- title
:IH1 print="printih1".Index1Printih1
:IH2.Index2
:i1 id="h01".Index1Line
:i2 id="h02".Index22222p1&syslnum.
:i2 id="h020".Index220p1&syslnum.
:IH2 print="printih2".Index2
:I2 pg='Index2page'.Index2
:cmt.i1 id="h01".Index1p1dup
:i2 id="h011".Index1p1&syslnum.
.ix "i1" "i2" "i3p1" "extraparm1"
:i2 id="h022" refid="h02" pg='1pgstring'.Index22p1
:cmt. i2 id="h022"  pg='1pgstring'.Index22p1
:i3 pg=start.Index22p1
Text on page 1.
.pa
Text on next page 2
:i3 pg=end.Index22p1
.ix "i1" "I2" "i3P2";.cm extraparameter
.ix "b1" "b2p2"
.ix "a10" "a22p2"
.ix "a10" "a21p2"
.ix "a1" "a22p2"
.ix "cccp2"
.ix "a1" "a2p2"
.ix "a10" "A22p2"
.ix "a10" "a22p2"
.ix "a10" "A22p2"
.ix "Index1" "index2"
.pa
:IREF refid=h01.
Text on page 3
.ix "a10" "a19p3"
:IREF refid="h022".
.br
:HDref refid=h00.
.ju off
.pa
Text on page 4 and .ix . dump follows
:i3 pg=major.Index22p1
:cmt. IH2 see='Index2'.IH2see
:I2.IH1seeLxx
:IH2 see='Index1'.IH1seeLxx
:cmt.  ix . dump
:backm.
:index
:cmt. a duplicate :Index tag gives a page fault error with WGML4 on O/2
:cmt. index
:egdoc
