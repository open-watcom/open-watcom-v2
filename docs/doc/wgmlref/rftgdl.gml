:H2.DL
:XMP.
Format&gml. &gml.DL \(compact\)
            \(break\)
            \(headhi=head-highlight\)
            \(termhi=term-highlight\)
            \(tsize='hor-space-unit'\).
:eXMP.
:I1.definition list
:I2 refid='gtdl' pg=major.gtdl
:P.
The definition list tag signals the start of a definition list.
Each list item in a definition list has two parts.
The first part is the definition term and is defined with
the :HP2.&gml.dt:eHP2. tag.
The second part is the definition description and is defined with
the :HP2.&gml.dd:eHP2. tag.
A corresponding :HP2.&gml.edl:eHP2. tag must be specified for each
:HP2.&gml.dl:eHP2. tag.
:INCLUDE file='rftgacon'.
:P.
The :HP2.break:eHP2. attribute indicates that the
:I2 refid='gabreak'.gabreak
definition description should be started on a new output line
if the size of the definition term exceeds the maximum horizontal
space normally allowed for it.
If this attribute is not specified, the definition description
will be placed after the definition term.
The break attribute is one of the few &WGML. attributes which
does not have an attribute value associated with it.
:P.
The :HP2.headhi:eHP2. attribute allows you to set the highlighting
:I2 refid='gaheadh'.gaheadh
level of the definition list headings.
Non-negative integer numbers are valid highlighting values.
:P.
The :HP2.termhi:eHP2. attribute allows you to set the highlighting
:I2 refid='gatermh'.gatermh
level of the definition term.
Non-negative integer numbers are valid highlighting values.
:P.
The :HP2.tsize:eHP2. attribute allows you to set the minimum
:I2 refid='gatsize'.gatsize
horizontal space taken by the definition term.
Any valid horizontal space unit may be specified.
The attribute value is linked to the font of the
&gml.DT tag if the termhi attribute is not specified
(see :HDREF refid='fnlink'.).
