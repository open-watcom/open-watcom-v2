:H2.FIGREF
:XMP.
Format&gml. &gml.FIGREF refid='id-name'
                \(page=yes
                      no\).
:eXMP.
:I1.figure reference
:I2 refid='gtfigre' pg=major.gtfigre
:P.
This tag causes a figure reference to be generated.
The text :Q.Figure:eQ. followed by the figure number
will be generated at the point where the :hp2.&gml.figref:ehp2. tag
is specified.
The figure reference tag is a paragraph element,
and is used with text to create the content of a basic document
element.
The figure being referenced must have a figure caption specified.
:p.
The :hp2.refid:ehp2.
:I2 refid='garefid'.garefid
attribute will determine
the figure for which the reference will be generated.
The specified identifier name must be the value
of the id attribute on the figure you wish to reference.
:p.
The :hp2.page:ehp2.
:I2 refid='gapage'.gapage
attribute controls the output of
the figure page number.
If the attribute value :hp1.yes:ehp1. is specified,
the text :q.on page:eq. followed by the page number
of the referenced figure is placed after the figure reference text.
If the attribute value :hp1.no:ehp1. is specified,
the page number
of the referenced figure is not generated.
If the page attribute is not specified, the figure page
number is generated when the figure and the reference
to it are not on the same output page.
