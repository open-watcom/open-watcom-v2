:H2.Specifying a Layout
:P.
Initial values for all of the layout items are defined in &WGML.,
:IH1.layout
:I2.specifying
and is called the
:HP2.default layout:eHP2..
:IH1.layout
:I2.default values
:I1.default layout
A new layout is created by modifying the initial values of the default
layout.
Only those values that you wish to change need to be specified.
:P.
The layout section starts with the
:HP2.&gml.layout:eHP2.
:I2 refid='ttlayou'.ttlayou
tag and ends with the
:HP2.&gml.elayout:eHP2.
:I2 refid='ttelayo'.ttelayo
tag.
The layout values are grouped into sections, and are specified in
the same way as the GML tags.
The sections are identified by layout tags, with the individual layout
values specified by tag attributes.
Most of the GML document elements have corresponding layout tags.
:P.
The layout section is specified before the :HP2.&gml.gdoc:eHP2. tag.
The best way to do this is with the
:HP2.layout:eHP2.
:I2 refid='gropt'.layout
option on the &WGML. command line (see :HDREF refid='rfrlayo'.).
You may also choose to include the layout specification directly into
your source document.
Specifying the layout on the command line makes it easier to switch
layouts.
:P.
The layout section may be specified more than once.
Authors can share a common layout
and still specify layout changes for their own document.
Each layout section modifies the values defined by the default
layout plus the cumulative modifications of previous layout sections.
