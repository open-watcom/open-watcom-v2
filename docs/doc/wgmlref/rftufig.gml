:H2.Figures
:pc.
Figures are used to create space for illustrated material
:I1.figures
to be placed in the document.
If you enter text within the figure, it is processed in the
same manner as the input text in an example.
:p.
A figure is illustrated in the following example&gml.
:fig id=rfgfig1 place=inline frame=box.
  :INCLUDE file='rfgfig1'.
  :figcap.Very Simple Figure
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
  :INCLUDE file='rflfig1'.
  :figcap.
 :figdesc.Output of :figref refid=rfgfig1 page=no.
:efig.
:pc.The figure begins with the
:hp2.&gml.fig:ehp2.
:I2 refid='ttfig'.ttfig
tag and is completed by the :hp2.&gml.efig:ehp2.
:I2 refid='ttefig'.ttefig
tag.
The lines between these tags are not processed together.
:p.Most figures will have captions.
The caption is supplied by the :hp2.&gml.figcap:ehp2.
:I2 refid='ttfigca'.ttfigca
tag as illustrated
in the following example&gml.
:fig id=rfgfig2 place=inline frame=box.
  :INCLUDE file='rfgfig2'.
  :figcap.Figure with Caption
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
 :INCLUDE file='rflfig2'.
 :figcap.
 :figdesc.Output of :figref refid=rfgfig2 page=no.
:efig.
:PC.Note that the caption is included in the figure
and that &WGML. automatically supplied a number for the figure.
When a figure list is created in the front material, the
figure caption is used to create a figure list entry.
:pc.A description of the figure may be included following the
caption.
This is illustrated as follows&gml.
:fig id=rfgfig3 place=inline frame=box.
  :INCLUDE file='rfgfig3'.
  :figcap.Illustration of a Description with Figure
:efig.
:pc.The description, specified with the :hp2.&gml.figdesc:ehp2.
:I2 refid='ttfigde'.ttfigde
tag, follows the caption.
:fig place=inline frame=box.
  :INCLUDE file='rflfig3'.
  :figcap.
  :figdesc.Output of :figref refid=rfgfig3 page=no.
:efig.
:pc.The text associated with the description
is processed together following the caption.
&WGML. automatically supplied a colon after the caption
to separate it from the description.
