:H2.Referencing
:P.
A reference is used to direct the reader to a specific place in
the document for more information.
During the creation and revision of a document, the location of
the referenced entity may shift,
making it difficult to correctly maintain such
reference information as the page number.
The GML reference tags automate this function by creating the reference
for you.
This section will illustrate the use of simple referencing
with headings and figures.
:P.
When a GML entity is referenced, there must be a way to
uniquely identify the entity being referenced.
This is done with the use of an :HP2.attribute:eHP2..
:I1.attributes
Attributes modify the action of the tag or supply additional
information, such as an identifier name.
All attributes are entered before the period which ends the tag.
:P.
The :hp2.id:ehp2. attribute allows you to assign a unique
identifier to an entity.
The following example illustrates the use of the :hp2.id:ehp2.
attribute&gml.
:fig place=inline frame=box.
 :INCLUDE file='rfgshid'.
 :figcap.Illustration of the ID Attribute
:efig.
:PC.
The text :hp1.firsth0:ehp1. is the value of the heading
:hp2.id:ehp2. attribute.
This value is assigned to the heading, and must be unique within
the document.
The text :hp1.myfig:ehp1. is the identifier name for the figure.
When a figure is assigned an identifier, a figure caption must
also be specified.
:P.
A reference to a heading or figure can be made with
the :hp2.&gml.hdref:ehp2.
:I2 refid='tthdref'.tthdref
and :hp2.&gml.figref:ehp2.
:I2 refid='ttfigre'.ttfigre
tags respectively.
Both of these tags may appear anywhere in your input text, and
require the presence of the :hp2.refid:ehp2. attribute.
This attribute is used to reference a particular entity.
The following example illustrates the use of the two referencing
tags&gml.
:fig id=rfgfhre place=inline frame=box.
 :INCLUDE file='rfgfhref'.
 :figcap.Illustration of Referencing
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
 :INCLUDE file='rflfhref'.
 :figcap.
 :figdesc.Output of :figref refid=rfgfhre page=no.
:efig
:PC.
The attribute value for the :hp2.refid:ehp2. attribute
is the identifier name of the entity we wish to reference.
Note that there are two periods after the figure reference.
The first period ends the tag, while the second period is
text to end the sentence.
:P.
The text :hp1."Illustrate ID with a Heading":ehp1.
is inserted where the heading reference tag was specified.
The text :hp1.Figure 1:ehp1.
is inserted where the figure reference tag was specified.
If the reference was on a different page than the referenced
entity, the page number of the referenced entity is also
inserted.
For example, if the figure reference was on page 12 and
the referenced figure was on page 9,
the text to be inserted would be :hp1.Figure 1 on page 9:ehp1..
:P.
Referencing of headings allows you to change the wording of
a heading at any time and still have the correct heading text
used when you reference the heading in other parts of the document.
Figure referencing removes any dependence upon the ordering of
the figures in the document.
