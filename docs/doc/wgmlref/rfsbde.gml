:H2.Examples and Figures
:H3.Example
:I1.examples
:I1.xmp
:I1.depth
:FIG place=inline frame=box.
&gml.XMP depth = 'vert-space-unit'.
     (depth is optional)
   paragraph elements
   basic document elements
&gml.eXMP.
:eFIG.
:H3.Figure
:I1.figure
:I1.fig
:I1.figcap
:I1.figdesc
:FIG place=inline frame=box.
&gml.FIG attribute.
   paragraph elements
   basic document elements
&gml.FIGCAP.caption text
   (figcap is optional)
&gml.FIGDESC.
   (figdesc is optional)
     paragraph elements
     basic document elements
&gml.eFIG.
:eFIG.
:PC.
The attribute, if specified, is one or more of
:SL compact.
:LI.depth = 'vert-space-unit'
:I1.depth
:LI.frame = box | rule | none | 'char-string'
:I1.frame
:LI.id = 'id-name'
:I1.id
:LI.place = top | bottom | inline
:I1.place
:LI.width = page | column | 'hor-space-unit'
:I1.width
:eSL.
:H3.Figure Reference
:I1.figure reference
:I1.figref
:I1.refid
:I1.page
:FIG place=inline frame=box.
&gml.FIGREF refid = 'id-name' page = yes | no .
                          (page is optional)
:eFIG.
:H2.Headings
:H3.Heading
:I1.headings
:I1.h0
:I1.h1
:I1.h2
:I1.h3
:I1.h4
:I1.h5
:I1.h6
:FIG place=inline frame=box.
&gml.H:HP1.n:eHP1. attribute.text line
:eFIG.
:P.
The value of :HP1.n:eHP1. must be one of 0, 1, 2, 3, 4, 5, or 6.
The attribute, if specified, is one or more of
:SL compact.
:LI.id = 'id-name'
:I1.id
:LI.stitle = 'char-string'
:I1.stitle
:eSL.
:PC.
The :HP1.stitle:eHP1. attribute is only permitted if :HP1.n:eHP1.
is 0 or 1.
:H3.Heading Reference
:I1.heading reference
:I1.hdref
:I1.refid
:I1.page
:FIG place=inline frame=box.
&gml.HDREF refid = 'id-name' page = yes | no .
                         (page is optional)
:eFIG.
:H2.Lists
:IH1.lists
:I2.address
:I2.definition
:I2.glossary
:I2.unordered
:I2.ordered
:I2.simple
:I2.list reference
:H3.Address
:I1.address
:I1.aline
:FIG place=inline frame=box.
&gml.ADDRESS.
&gml.ALINE.address line
   (may occur several times)
&gml.eADDRESS.
:eFIG.
:H3.Definition List
:I1.dl
:I1.dt
:I1.dd
:I1.ddhd
:I1.ddhd
:I1.lp
:FIG place=inline frame=box.
&gml.DL attribute.
   &gml.DTHD.text line
      (may occur several times)
   &gml.DDHD.text line
      (for every DTHD there must be a DDHD)
   &gml.DT.text line
      (may occur several times)
   &gml.DD.
       paragraph elements
       basic document elements
      (for every DT there must be a DD)
   &gml.LP.
       paragraph elements
      (LP is optional)
&gml.eDL.
:eFIG.
:P.
The attribute
if specified, may be one or more of
:SL compact.
:LI.break
:I1.break
:LI.compact
:I1.compact
:LI.headhi = 'integer number'
:I1.headhi
:LI.termhi = 'integer number'
:I1.termhi
:LI.tsize = 'hor-space-unit'
:I1.tsize
:eSL.
:H3.Glossary List
:I1.gl
:I1.gt
:I1.gd
:I1.lp
:FIG place=inline frame=box.
&gml.GL attribute.
   &gml.GT.text line
      (may occur several times)
   &gml.GD.
       paragraph elements
       basic document elements
      (for every GT there must be a GD)
   &gml.LP.
       paragraph elements
      (LP is optional)
&gml.eGL.
:eFIG.
:P.
The attribute
if specified, may be one or more of
:SL compact.
:LI.compact
:I1.compact
:LI.termhi = 0 | 1 | 2 | 3
:I1.termhi
:eSL.
:H3.Ordered List
:I1.ol
:I1.li
:I1.lp
:I1.id
:FIG place=inline frame=box.
&gml.OL compact.
    (compact is optional)
   &gml.LI id = 'id-name'.
      (id is optional)
      (LI may occur several times)
       paragraph elements
       basic document elements
   &gml.LP.
       paragraph elements
      (LP is optional)
&gml.eOL.
:eFIG.
:H3.Simple List
:I1.sl
:I1.li
:I1.lp
:I1.id
:FIG place=inline frame=box.
&gml.SL compact.
    (compact is optional)
   &gml.LI id = 'id-name'.
      (id is optional)
      (LI may occur several times)
       paragraph elements
       basic document elements
   &gml.LP.
       paragraph elements
      (LP is optional)
&gml.eSL.
:eFIG.
:H3.Unordered List
:I1.ul
:I1.li
:I1.lp
:I1.id
:FIG place=inline frame=box.
&gml.UL compact.
    (compact is optional)
   &gml.LI id = 'id-name'.
      (id is optional)
      (LI may occur several times)
       paragraph elements
       basic document elements
   &gml.LP.
       paragraph elements
      (LP is optional)
&gml.eUL.
:eFIG.
:H3.List Reference
:I1.list reference
:I1.liref
:I1.id
:I1.page
:FIG place=inline frame=box.
&gml.LIREF refid = 'id-name' page = yes | no .
                         (page is optional)
:eFIG.
:H2.Notes
:I1.notes
:I2.footnote
:I2.footnote reference
:H3.Footnote
:I1.fn
:I1.id
:FIG place=inline frame=box.
&gml.FN id = 'id-name'.
   (id is optional)
   paragraph elements
   basic document elements
&gml.eFN.
:eFIG.
:H3.Footnote Reference
:I1.fn
:I1.refid
:FIG place=inline frame=box.
&gml.FNREF refid = 'id-name'.
:eFIG.
:H3.Note
:I1.note
:FIG place=inline frame=box.
&gml.NOTE.
   paragraph elements
:eFIG.
:H2.Paragraphs
:H3.Paragraph
:I1.p
:FIG place=inline frame=box.
&gml.P.
   paragraph elements
:eFIG.
:H3.Paragraph Continuation
:I1.pc
:FIG place=inline frame=box.
&gml.PC.
   paragraph elements
:eFIG.
:H2.Quotes and Highlighted Phrases
:I1.cit
:H3.Citation
:FIG place=inline frame=box.
&gml.CIT.
    paragraph elements
&gml.eCIT.
:eFIG.
:H3.Highlighted Phrase
:I1.hp0
:I1.hp1
:I1.hp2
:I1.hp3
:FIG place=inline frame=box.
&gml.HP:HP1.n:eHP1..
   paragraph elements
&gml.eHP:HP1.n:eHP1..
:eFIG.
:P.
The value of :HP1.n:eHP1. must be one of 0, 1, 2, or 3
:H3.Long Quotation
:I1.lq
:FIG place=inline frame=box.
&gml.LQ.
   basic document elements
&gml.eLQ.
:eFIG.
:H3.Quote
:I1.q
:FIG place=inline frame=box.
&gml.Q.
   paragraph elements
&gml.eQ.
:eFIG.
:H3.Set Font
:I1.sf
:FIG place=inline frame=box.
&gml.SF font=:HP1.n:eHP1..
   paragraph elements
&gml.eSF.
:eFIG.
:H2.Graphics
:I1.graphic
:FIG place=inline frame=box.
&gml.GRAPHIC file = 'char-string'
         depth = 'vert-space-unit'
         width = 'hor-space-unit'
         scale = integer number
         xoff = 'hor-space-unit'
         yoff = 'vert-space-unit'.
:eFIG.
:P.
The :HP1.file:eHP1. attribute must always be specified.
The :HP1.depth:eHP1. attribute is required if the graphic
file is in the PostScript format.
