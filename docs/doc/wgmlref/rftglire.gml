:H2.LIREF
:XMP.
Format&gml. &gml.LIREF refid='id-name'
               \(page=yes
                     no\).
:eXMP.
:I2 refid='gtliref' pg=major.gtliref
:P.
This tag generates a reference to an item in an ordered list.
The list item reference tag is a paragraph element,
and is used with text to create the content of a basic document
element.
The number text from the referenced list item
is inserted into the output.
:p.
The :hp2.refid:ehp2.
:I2 refid='garefid'.garefid
attribute will determine
the list item for which the reference will be generated.
The specified identifier name must be the value
of the id attribute on the list item tag you wish to reference.
:p.
The :hp2.page:ehp2.
:I2 refid='gapage'.gapage
attribute controls the output of
the list item page number.
If the attribute value :hp1.yes:ehp1. is specified,
the text :q.on page:eq. followed by the page number
of the referenced list item is placed after the annotation text.
If the attribute value :hp1.no:ehp1. is specified,
the page number
of the referenced list item is not generated.
If the page attribute is not specified, the list item page
number is generated only if the list item reference is not on
the same page as the list item.
