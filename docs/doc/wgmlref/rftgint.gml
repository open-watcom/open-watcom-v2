:P.
This chapter contains a subsection on each of the tags supported by
the &WGML. language. The tags are presented in alphabetical order
and in several forms:
:OL compact.
:LI.tag.
:P.
This form is used when other data is not associated with the tag.
For example, the :HP2.&gml.body:eHP2. tag is used when defining
the structure of the document, but has no text specified with it.
:LI.tag.<paragraph elements>
:P.
This form is used when paragraph elements, such as text, are assumed
to follow the tag.
The :HP2.&gml.pc:eHP2. tag is an example of this type of tag.
:LI.tag.<text line>
:I1.text line
:P.
Some tags have a single line of text associated with it, such as
with the :HP2.&gml.h0:eHP2. tag.
In this situation, the processing rules are as follows:
:I1.tags
:I2.rules for processing
:OL.
:LI.When nothing follows the tag,
except an optional period(.),
the next input line is used.
:LI.Otherwise, the text following the period or space after the tag
is used.
:eOL.
:PC.As these rules are somewhat complicated, it is best
to always place the
text line on the same line as the tag, immediately following a period.
:eOL.
:P.
Use two periods when a tag ends a sentence.
The first period will be processed as part of the tag.
The second period will be treated as part of the text.
If only one period is specified, the period to end the sentence
will not appear in the resulting document.
:P.
Basic document elements, such as highlighting tags, cannot appear as
part of a <text line>.
If a tag is specified, it will be processed as if it had been entered
on the next input record.
:p.
Some tags have attributes which are used to modify or define
the behavior of the tag.
Tag attributes will be presented in the following way&gml.
:XMP.
&gml.tag  attribute-one
      \(attribute-two\)
:eXMP.
:PC.
Attributes not enclosed in brackets(\(), such as attribute-one,
are required and must be present with the tag.
Most attributes are optional, and will have brackets
as illustrated by attribute-two.
No other text is allowed between a tag and its attributes.
