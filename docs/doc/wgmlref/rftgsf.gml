:H2.SF
:XMP.
Format&gml. &gml.SF font=number.
:eXMP.
:I1.highlight phrase
:I2 refid='gtsf' pg=major.gtsf
:P.
The set font tag starts the highlighting of phrases at the level specified
by the required attribute :HP1.font:eHP1..
The actual highlighting to be performed is determined by the
type of device for which the document is being formatted.
Examples of highlighting include underlining, displaying in bold
face, or using a different character shape (such as italics).
:P.
The value of the :HP1.font:eHP1. attribute is a non-negative integer number.
If the specified number is larger than the last defined font for
the document, font for zero is used.
:P.
Highlighting may not be used when the GML layout explicitly determines
the emphasis to be used, such as in the text of a heading.
:P.
The set font tag is a paragraph element.
It is used with text to create the content of a basic document
element, such as a paragraph.
A corresponding :hp2.&gml.ESF:ehp2. tag must be specified for each
:hp2.&gml.SF:ehp2. tag.
