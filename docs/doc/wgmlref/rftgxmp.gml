:H2.XMP
:xmp
Format&gml. &gml.XMP \(depth='vert-space-unit'\).
             <paragraph elements>
             <basic document elements>
:exmp
:I2 refid='gtxmp' pg=major.gtxmp
:P.
This tag signals the start of an example.
Each line of source text following the example tag is
placed in the output document without normal text processing.
Spacing between words is preserved, and the input text is
not right justified.
Input source lines which do not fit on a line in the output
document are split into two lines on a character, rather than
a word basis.
An example may be used where a basic document element is
permitted to appear, except
within a figure, footnote, or example.
A corresponding :hp2.&gml.exmp:ehp2. tag must be specified for each
:hp2.&gml.xmp:ehp2. tag.
:P.
If the example does not fit on the current page or column, it is
forced to the next one.
If the current column is empty, the example will be split into
two parts.
:INCLUDE file='rftgadep'.
