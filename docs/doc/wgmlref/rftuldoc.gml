:H2.Large Documents
:pc.It is often convenient when preparing large documents to
use more than one GML source file.
For example, each chapter in a book may be a separate GML file.
The entire book is prepared by causing &WGML. to process a
primary input file containing :hp2.&gml.include:ehp2.
:I2 refid='ttinclu'.ttinclu
tags which
cause the individual chapters to be included into the document.
:fig place=inline frame=box.
&gml.GDOC.
&gml.BODY.
 &gml.INCLUDE file='chap1'.
 &gml.INCLUDE file='chap2'.
 &gml.INCLUDE file='chap3'.
&gml.APPENDIX.
 &gml.INCLUDE file='app1'.
 &gml.INCLUDE file='app2'.
&gml.eGDOC.
:figcap.Illustration of &gml.include Tag
:efig
:pc.The preceding figure illustrates one such organization for a
book.
The :hp2.&gml.include:ehp2.
:I2 refid='ttinclu'.ttinclu
tag has been used to include the text for
three chapters and two appendices.
Replacing each :hp2.&gml.include:ehp2. tag with the
contents of the referenced file would produce the same document.
:p.The :hp2.&gml.include:ehp2.
:I2 refid='ttinclu'.ttinclu
tag causes &WGML. to process the associated
file at the place the tag occurs.
An included file may itself cause other files to be included.
This capability allows the author to organize the document into
separate data files.
Using a number of files is important in complex documents since
a number of small files is usually easier to maintain than would
be the case if the entire document were specified as a single large file.
:p.When a document is organized into small files,
a subset of the input files may be included into a "skeleton"
file containing the major tags (such as :hp2.&gml.gdoc.:ehp2.,
:hp2.&gml.body:ehp2. and :hp2.&gml.egdoc:ehp2.).
The subset of input files may then be processed independently
from the rest of the document as it is being written and revised.
This capability is useful for a number of reasons&gml.
:ol.
:li.&WGML. will format a small document in less time
    than would be required for a large document.
:li.Several authors may work simultaneously on independent
    parts of a document.
:li.&WGML. runs on a number of different machines.
    A document can be prepared on a micro-computer in sections, and then
    transferred to a mainframe computer for final production
    of the document.
:eol.
:pc.Consequently, most documents of a non-trivial nature are
specified using a number of input files.
:p.The :hp2.&gml.include:ehp2.
:I2 refid='ttinclu'.ttinclu
tag is not a standard tag; it has been
added to the tags accepted by &WGML. to provide the
capability to include GML source files.
Other GML processors may use alternative tags or methods to include
additional files.
