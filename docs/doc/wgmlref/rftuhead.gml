:H2 id='rftuhd'.Headings
:P.
The GML language considers a document to be composed of major
:I1.headings
sections with further levels of subdivision.
GML has seven levels of document division.
These divisions are indicated with the heading tags(:h0, :h1, ..., :h6).
:P.
The following restrictions apply to the use of headings&gml.
:dl
:I1.headings
:I2.restrictions
:dt.abstract
:dd.:hp2.&gml.h0:ehp2. and :hp2.&gml.h1:ehp2. tags may not be used.
:dt.preface
:dd.:hp2.&gml.h0:ehp2. and :hp2.&gml.h1:ehp2. tags may not be used.
:dt.body
:dd.All heading tags may be used.
:dt.appendix
:dd.:hp2.&gml.h0:ehp2. tags may not be used.
:dt.back material
:dd.:hp2.&gml.h0:ehp2. tags may not be used.
:edl.
:PC.
Consider the following example&gml.
:FIG id=rfghead place=inline frame=box.
 :INCLUDE file='rfghead'.
 :figcap.Sample Headings
:eFIG.
:PC.
The preceding example shows the structure of a fictitious
document.
One or more document elements could have been specified following
any of the
:hp2.&gml.h0:ehp2.,
:hp2.&gml.h1:ehp2.,
:hp2.&gml.h2:ehp2.
:hp2.&gml.h3:ehp2. or
:hp2.&gml.h4:ehp2. tags.
A common convention is to have the heading tags represent
the following entities&gml.
:dl.
:dt.&gml.h0
:I2 refid='tth0'.tth0
:dd.major parts of a document
:dt.&gml.h1
:I2 refid='tth1'.tth1
:dd.chapters or appendices
:dt.&gml.h2
:I2 refid='tth2'.tth2
:dd.sections
:dt.&gml.hn (n<7)
:I2 refid='tth3'.tth3
:I2 refid='tth4'.tth4
:I2 refid='tth5'.tth5
:I2 refid='tth6'.tth6
:dd.subsections of sections defined with
:hp2.&gml.h(n-1):ehp2. tags.
:edl.
:p.With an appropriate layout, the following table of contents
would be produced by the example&gml.
:fig place=inline frame=box.
 :INCLUDE file='rflhead'.
 :figcap.Sample Table of Contents
:efig.
:PC.Each heading level can be formatted in a different way.
The level zero headings have not been numbered, while the
heading levels one through four have been numbered
as directed by the layout.
:p.The layout with which the document is formatted determines the format
of headings.
Some of the formatting actions determined by the layout are
the number of lines skipped before
a heading is displayed and whether
the heading at a particular level is to
be included in the table of contents.
The GML tag used to obtain a table of contents will be discussed
later in the tutorial.
:p.The heading tags are immediately followed by the words which
make up the heading.  This heading text is called a
:hp2.text line:ehp2.
:I1.text line
in the reference section of this manual.
When a text line is presumed to follow a tag, the text is
usually given on the same line, immediately following the
period(.) after the tag.
The paragraph tag, :hp2.&gml.p:ehp2.,
:I2 refid='ttp'.ttp
introduced earlier in the
tutorial does not have a text line associated with it;
the text which follows the tag is used to form a paragraph,
and may be given on any number of lines.
