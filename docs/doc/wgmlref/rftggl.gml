:H2.GL
:XMP.
Format&gml. &gml.GL \(compact\)
            \(termhi=term-highlight\).
:eXMP.
:I1.glossary list
:I2 refid='gtgl' pg=major.gtgl
:P.
The glossary list tag signals the start of a glossary list,
and is usually used
in the back material section.
Each list item in a glossary list has two parts.
The first part is the glossary term and is defined with
the :HP2.&gml.gt:eHP2. tag.
The second part is the glossary description and is defined with
the :HP2.&gml.gd:eHP2. tag.
A corresponding :HP2.&gml.egl:eHP2. tag must be specified for each
:HP2.&gml.gl:eHP2. tag.
:INCLUDE file='rftgacon'.
:P.
The :HP2.termhi:eHP2.
:I2 refid='gatermh'.gatermh
attribute allows you to set the highlighting
level of the glossary term.
Non-negative integer numbers are valid highlighting values.
