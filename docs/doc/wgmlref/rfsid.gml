:H2.Identifiers
:P.
Identifiers are used to "identify"
:I1.identifiers
:I1.id-name
certain types of document elements so that they
may be referenced.
For example, identifiers are useful with headings.
If an identifier is assigned to a heading with the
:HP1.id:eHP1. attribute, the heading can be referenced
with the :HP2.&gml.hdref:eHP2. tag.
The heading to be referenced is "identified" by the
identifier name assigned to the heading.
If the heading text is later changed, the heading reference
will still be valid, and automatically use the new heading text.
:P.
An identifier name should not be longer than seven characters and
must consist of letters and numbers.
If the identifier name is longer than seven characters, a warning message
will be issued.
