:H2.Examples
:p.
Text is often included in situations where it is desired that the
separate lines in the input are not processed together
as they are in a paragraph.
One entity for which text is processed like this is called
an :hp2.example:ehp2.
:I1.examples
in the GML language.
Consider the following&gml.
:FIG id=rfgxmp place=inline frame=box.
 :INCLUDE file='rfgxmp'.
 :FIGCAP.Simple XMP example
:eFIG.
:PC.The example text which is to be placed in the output
is found between the
:HP2.&gml.xmp:eHP2.
:I2 refid='ttxmp'.ttxmp
and :hp2.&gml.exmp:ehp2.
:I2 refid='ttexmp'.ttexmp
tags.
:INCLUDE file='rffhead'.
:FIG place=inline frame=box.
 :INCLUDE file='rflxmp'.
 :FIGCAP.
 :FIGDESC.Output of :figref refid=rfgxmp page=no.
:eFIG.
:pc.The lines in the example entity have not been
processed in the same way as in a paragraph.
The layout with which the document is formatted determines
aspects such as the indentation and
the number of lines skipped before and
after the example.
:p.The following tags have been introduced in this section&gml.
:dl.
:dt.&gml.xmp
:I2 refid='ttxmp'.ttxmp
:dd.This tags marks the start of an example.
Text in an example is not processed in the same way as in a
paragraph.
With the layout used for this document, the document source
lines of a paragraph are processed together and justified.
The source lines of an example are not processed together and
are not justified.
:dt.&gml.exmp
:I2 refid='ttexmp'.ttexmp
:dd.This tag marks the end of an example.
:edl.
:pc.Examples always appear in a single column on a page.
When there is insufficient room on a page
to contain an example, it is
placed at the start of the next page.
:p.Once the material in this section is understood, &WGML.
may be used to format most documents.  The remaining
features of the language may be viewed as ease-of-use
features&gml. the other entities are used to identify specific
types of document elements more precisely.
