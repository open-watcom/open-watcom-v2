:H2.Quotations
:pc.GML provides two types of quotations&gml.
short quotations (inline quotations) and
:I1.quotations
:I2.short
long quotations
:I2.long
(or excerpts).
Short quotations are used to quote a few phrases or sentences
within a block of text;
long quotations are used to identify a block of text which
is a quote.
:p.The following example illustrates a short quotation&gml.
:fig id=rfgshqu place=inline frame=box.
 :INCLUDE file='rfgshqut'.
 :figcap.Illustration of a Short Quotation
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
 :INCLUDE file='rflshqut'.
 :figcap.
 :figdesc.Output of :figref refid=rfgshqu page=no.
:efig
:pc.Note that the first quoted phrase, enclosed by the
:hp2.&gml.q:ehp2.
:I2 refid='ttq'.ttq
and :hp2.&gml.eq:ehp2.
:I2 refid='tteq'.tteq
tags, has been placed
within quotation characters.
The second quoted phrase has another quoted phrase nested inside it.
Note the nested phrase is emphasized with apostrophe (') characters.
The use of short quotations is encouraged, where applicable, for a
number of reasons&gml.
:ol.
:li.It encourages uniformity.
:li.Different quotation styles may be used depending upon the
layout used and the device which is used to print the document.
:eol.
:pc.A short quotation may be used whenever text is being
processed.
:p.The following example illustrates a long quotation&gml.
:fig id=rfglgqu place=inline frame=box.
 :INCLUDE file='rfglgqut'.
 :figcap.Illustration of a Long Quotation
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
 :INCLUDE file='rfllgqut'.
 :figcap.
 :figdesc.Output of :figref refid=rfglgqu page=no.
:efig
:pc.It may be noted that the text of the preceding example,
enclosed by the
:hp2.&gml.lq:ehp2.
:I2 refid='ttlq'.ttlq
and :hp2.&gml.elq:ehp2.
:I2 refid='ttelq'.ttelq
tags, has been indented
according to the style of the layout used.
A long quotation will cause the implicit end of a paragraph entity.
