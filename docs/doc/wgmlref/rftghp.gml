:H2.HP0, HP1, HP2, HP3
:XMP.
Format&gml. &gml.HP:HP1.n:eHP1..
        (:HP1.n:eHP1.=0,1,2,3)
:eXMP.
:I1.highlight phrase
:I2 refid='gthp0' pg=major.gthp0
:I2 refid='gthp1' pg=major.gthp1
:I2 refid='gthp2' pg=major.gthp2
:I2 refid='gthp3' pg=major.gthp3
:pc.These tags start the highlighting of phrases at one of the four
levels provided by GML.
The actual highlighting to be performed is determined by the
type of device for which the document is being formatted.
Examples of highlighting include underlining, displaying in bold
face, or using a different character shape (such as italics).
:p.Highlighting may not be used when the GML layout explicitly determines
the emphasis to be used, such as in the text of a heading.
:P.
The highlighting tags are paragraph elements.
They are used with text to create the content of a basic document
element, such as a paragraph.
A corresponding :hp2.&gml.EHPn:ehp2. tag must be specified for each
:hp2.&gml.HPn:ehp2. tag.
