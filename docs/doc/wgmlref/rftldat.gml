:H2.DATE
:XMP.
Format&gml. &gml.DATE \(align=left
                     right\)
              \(depth='vert-space-unit'\).<text line>
:eXMP.
:I2 refid='gtdate'.gtdate
:P.
This tag specifies the date associated with the letter,
and is specified after the :HP2.&gml.from:eHP2. tag.
The current date is used if the optional date text line is not specified.
The :HP2.&gml.date:eHP2. tag may be omitted from the letter.
:P.
The :HP2.align:eHP2.
:I2 refid='gaalign'.gaalign
attribute positions the date text on the output page.
The attribute value :HP1.left:eHP1. causes the date text to appear
at the left margin of the letter.
The attribute value :HP1.right:eHP1. causes the date text to appear
at the right margin of the letter.
The value of the align attribute will be determined by the
layout if it has not been specified.
:P.
The :HP2.depth:eHP2.
:I2 refid='gadepth'.gadepth
attribute accepts any valid vertical space unit.
The specified amount of space is placed before the date text.
