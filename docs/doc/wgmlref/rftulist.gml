:H2.Lists
:pc.Documents often contain lists.
With GML, there are a number of entities that may
be used to specify these lists.
The entity used depends upon
the general type of list.
Four of the list types which can be specified in GML
are&gml.
:dl.
:dt.Simple
:IH1.lists
:I2 id='simpl'.simple list
:dd.Items in the list are not annotated.
A simple list, such as a list of recipe ingredients, do not
usually have a particular order.
:dt.Unordered
:I2 id='unorl'.unordered list
:DD.List order is not important.
Each item is emphasized by an annotation symbol
such as a bullet or asterisk.
:dt.Ordered
:I2 id='ordrl'.ordered list
:dd.The list items are annotated by a sequence of numbers.
The style of the numbers, such as Roman numerals or Arabic,
is determined by the layout.
:dt.Definition
:I2 id='defil'.definition list
:dd.Each item in the list is annotated by a term that
is specified in the GML source.
The definitions that you are reading were entered using
a definition list.
:edl.
:pc.The content of a list item may be started with
a paragraph element.
If you do not specify a tag at the beginning of a list item,
the text for the list item will be processed
together in the same way as with paragraphs.
The following subsections illustrate each of these list types.
:H3.Simple Lists
:pc.Simple lists consist of list items which are displayed
:IREF refid='simpl'.
in the order entered in the GML source file.
The list item is not annotated.
:fig id=rfgsl place=inline frame=box.
 :INCLUDE file='rfgsl'.
 :figcap.Simple List
:efig
:INCLUDE file='rffhead'.
:fig id=rflsl place=inline frame=box.
 :INCLUDE file='rflsl'.
 :figcap.
 :figdesc.Output of :figref refid=rfgsl page=no.
:efig.
:pc.The three list items are presented
in blocks separated by a blank line.
The blocks are indented from the document text, before and after
the list,
in a layout-dependent fashion
to emphasize that they are list items.
:p.The following tags have been introduced&gml.
:dl.
:dt.&gml.sl
:I2 refid='ttsl'.ttsl
:dd.This tag signifies the start of a simple list.
:dt.&gml.li
:I2 refid='ttli'.ttli
:dd.This tag identifies a list item.
The text for the list item may be given as a paragraph element.
:dt.&gml.esl
:I2 refid='ttesl'.ttesl
:dd.This tag signifies the end of a simple list.
:edl.
:H3.Unordered Lists
:pc.Unordered lists consist of list items which are displayed
:IREF refid='unorl'.
in the order given in the GML source.
Each item is annotated with some layout-dependent text such as a bullet
or asterisk.
The presence of the annotation text in the resulting
document provides an extra emphasis to each list item.
:fig id=rfgul place=inline frame=box.
 :INCLUDE file='rfgul'.
 :figcap.Unordered List
:efig
:INCLUDE file='rffhead'.
:fig id=rflul place=inline frame=box.
 :INCLUDE file='rflul'.
 :figcap.
 :figdesc.Output of :figref refid=rfgul page=no.
:efig.
:pc.Each list item is preceded by the annotation symbol.
The second item consists of two paragraphs.
The text of the list items
has been indented in a layout-dependent fashion.
:p.The following tags have been discussed&gml.
:dl.
:dt.&gml.ul
:I2 refid='ttul'.ttul
:dd.This tag signifies the start of an unordered list.
:dt.&gml.li
:I2 refid='ttli'.ttli
:dd.This tag identifies a list item.
The text for the list item may be given as a paragraph element.
:dt.&gml.eul
:I2 refid='tteul'.tteul
:dd.This tag signifies the end of an unordered list.
:edl.
:H3.Ordered Lists
:pc.Ordered lists consist of list items which are displayed
:IREF refid='ordrl'.
in the order given in the GML source.
Each item is annotated in a sequence.
The annotation distinguishes each list item and is often
used to itemize steps in a procedure.
:fig id=rfgol place=inline frame=box.
 :INCLUDE file='rfgol'.
 :figcap.Ordered List
:efig
:INCLUDE file='rffhead'.
:fig id=rflol place=inline frame=box.
 :INCLUDE file='rflol'.
 :figcap.
 :figdesc.Output of :figref refid=rfgol page=no.
:efig.
:pc.The three list items are
each preceded by a list item number.
The second item consists of two paragraphs.
The text of the list items
has been indented in a layout-dependent fashion.
:p.The following tags have been discussed&gml.
:dl.
:dt.&gml.ol
:I2 refid='ttol'.ttol
:dd.This tag signifies the start of an ordered list.
:dt.&gml.li
:I2 refid='ttli'.ttli
:dd.This tag identifies a list item.
The text for the list item may be given as a paragraph element.
:dt.&gml.eol
:I2 refid='tteol'.tteol
:dd.This tag signifies the end of an ordered list.
:edl.
:H3.Definition Lists
:pc.Definition lists are similar to the other lists, except that the
:IREF refid='defil'.
annotation text which is to precede a list item is supplied by a
:hp2.&gml.dt:ehp2.
:I2 refid='ttdt'.ttdt
(:hp2.D:ehp2.efinition :hp2.T:ehp2.erm) tag.
The contents of the list item follows a :hp2.&gml.dd:ehp2.
:I2 refid='ttdd'.ttdd
(:hp2.D:ehp2.efinition :hp2.D:ehp2.escription) tag.
List items may contain a number of paragraphs.
:fig id=rfgdl place=inline frame=box.
 :INCLUDE file='rfgdl'.
 :figcap.Definition List
:efig
:INCLUDE file='rffhead'.
:fig id=rfldl place=inline frame=box.
 :INCLUDE file='rfldl'.
 :figcap.
 :figdesc.Output of :figref refid=rfgdl page=no.
:efig.
:pc.The three list items are
each preceded by the term given in the :hp2.&gml.dt:ehp2 tag.
The second item consists of two paragraphs.
The text of the list items
has been indented in a layout-dependent fashion.
:p.The following tags have been discussed&gml.
:dl.
:dt.&gml.dl
:I2 refid='ttdl'.ttdl
:dd.This tag signifies the start of a definition list.
:dt.&gml.dt
:I2 refid='ttdt'.ttdt
:dd.This tag specifies the term to precede the text for the list item.
The text of the definition term is given as a :HP2.text line:eHP2..
:dt.&gml.dd
:I2 refid='ttdd'.ttdd
:dd.This tag identifies the start of the text for a list item.
The text for the list item may be given as a paragraph element.
:dt.&gml.edl
:I2 refid='ttedl'.ttedl
:dd.This tag signifies the end of a definition list.
:edl.
:H3.Nesting lists
:pc.A list can appear as part of a list item.
The "inner" list is said to be :hp2.nested:ehp2. inside
:IH1.lists
:I2.nesting
the outer list. This is illustrated by the following&gml.
:fig id=rfgnest place=inline frame=box.
 :INCLUDE file='rfgnest'.
 :figcap.Illustration of Nested List
:efig.
:INCLUDE file='rffhead'.
:fig id=rflnest place=inline frame=box.
 :INCLUDE file='rflnest'.
 :figcap.
 :figdesc.Output of :figref refid=rfgnest page=no.
:efig.
:PC.Note that an ordered list is nested inside the second item of the
outside ordered list.
All list types
may be nested as part of another list item.
The nesting can take place to arbitrary depth, although excessive
nesting will tend to make the document hard to read.
:H3.List Parts
:I2 refid='ttlp'.ttlp
:pc.A list part is a special entity which may be included to
temporarily suspend a list.
This entity is used to provide an explanation for the
list items which follow.
The list part entity causes the indentation
to be reset to the value at the start of the current list.
A paragraph element is then formatted at this indentation
until the next list item is encountered with a
:hp2.&gml.li:ehp2. or :hp2.&gml.dt:ehp2. tag.
Consider the following example&gml.
:fig id=rfglp place=inline frame=box.
 :INCLUDE file='rfglp'.
 :figcap.Illustration List Part
:efig.
:INCLUDE file='rffhead'.
:fig id=rfllp place=inline frame=box.
 :INCLUDE file='rfllp'.
 :figcap.
 :figdesc.Output of :figref refid=rfglp page=no.
:efig.
:pc.The list part in the example consists of
two paragraphs.
The annotation of the list items is continued following the
list part.
List parts may be used with any of the list types.
