:H2 id='gtfig'.FIG
:XMP.
Format&gml. &gml.FIG \(depth='vert-space-unit'\)
             \(frame=box
                    rule
                    none
                    'character string'\)
             \(id='id-name'\)
             \(place=top
                    bottom
                    inline\)
             \(width=page
                    column
                    'hor-space-unit'\).
             <paragraph elements>
             <basic document elements>
:eXMP.
:I1.figures
:I2 refid='gtfig' pg=major.gtfig
:P.
This tag signals the start of a figure.
Each line of source text following the figure tag is
placed in the output document without normal text processing.
Spacing between words is preserved, and the input text is
not right justified.
Input source lines which do not fit on a line in the output
document are split into two lines on a character, rather than
a word basis.
A figure may be used where a basic document element is
permitted, except
within a figure, footnote, or example.
:P.
If the figure does not fit on the current page or column, it is
forced to the next one.
If the current column is empty, the figure will be split into
two parts.
:INCLUDE file='rftgadep'.
:p.
The :hp2.frame:ehp2. attribute will determine
:I2 refid='gaframe'.gaframe
the framing value for the figure.
The layout for the document specifies a default frame value
if the frame attribute is not specified.
The frame is
created with the appropriate characters for
the output device selected.
The attribute value :hp1.box:ehp1. will cause
the entire figure to be enclosed by a box.
The attribute value :hp1.rule:ehp1. will cause
a line to be created before the top and after the
bottom of the figure.
The sides of the figure will not be enclosed.
The rule line at the top of the figure is not produced
if the place of the figure is :HP1.top:eHP1..
The bottom rule is not produced if the place of the figure is
:HP1.bottom:eHP1..
The attribute value :hp1.none:ehp1. will cause
no framing to occur.
If a character string is used as the framing value,
a framing value of rule will be in effect, using
the specified character string to create the rule lines.
:P.
The :hp2.id:ehp2. attribute will associate an
:I2 refid='gaid'.gaid
identifier name to the figure.
If an identifier name is specified, the figure caption tag
(:HP2.&gml.figcap:eHP2.)
must also be specified in the figure.
The quoted name is used by the :hp2.&gml.figref:ehp2. tag to
generate a figure reference to the figure.
:p.
The :hp2.place:ehp2. attribute determines the
:I2 refid='gaplace'.gaplace
page position of the figure.
The layout for the document specifies a default place value
if the place attribute is not specified.
A place value of :hp1.top:ehp1. causes the
figure to be placed at the top of the next available page or column.
Any text which follows the figure in the input
may 'float' before the figure to fill up the previous page.
A place value of :hp1.bottom:ehp1. causes the
figure to be placed at the bottom of the next available page or column.
Any text which follows the figure in the input
may float before the figure to fill up the previous page and
before the figure on the current page.
A place value of :hp1.inline:ehp1. causes the
figure to be output within the context of the input text
which surrounds it.
Text which follows the figure in the input will not
float before the figure.
:p.
The :hp2.width:ehp2. attribute allows you to specify
:I2 refid='gawidth'.gawidth
the width of the figure.
The attribute value :hp1.page:ehp1. specifies that the
figure will be as wide as the page, even if the document
is formatted for more than one column.
The attribute value :hp1.column:ehp1. specifies that the
figure shall be one column wide.
If a horizontal space unit is used as the attribute value,
the figure will have the width specified by the attribute value.
The width attribute value is linked to the font
of the figure (see :HDREF refid='fnlink'.).
