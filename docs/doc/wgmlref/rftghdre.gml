:H2.HDREF
:xmp
Format&gml. &gml.HDREF refid='id-name'
               \(page=yes
                     no\).
:exmp
:I1.heading reference
:I2 refid='gthdref' pg=major.gthdref
:p.This tag causes a heading reference to be generated.
The heading reference tag is a paragraph element,
and is used with text to create the content of a basic document
element.
The heading text from the referenced heading
is enclosed in
double quotation marks and inserted into the formatted document.
:p.
The :hp2.refid:ehp2.
:I2 refid='garefid'.garefid
attribute will determine
the heading for which the reference will be generated.
The specified identifier name must be the value
of the id attribute on the heading tag you wish to reference.
:p.
The :hp2.page:ehp2.
:I2 refid='gapage'.gapage
attribute controls the output of the heading page number.
If the attribute value :hp1.yes:ehp1. is specified,
the text :q.on page:eq. followed by the page number
of the referenced heading is placed after the heading text.
If the attribute value :hp1.no:ehp1. is specified,
the page number
of the referenced heading is not generated.
If the page attribute is not specified, the heading page
number is generated when the heading and the reference
to it are not on the same output page.
