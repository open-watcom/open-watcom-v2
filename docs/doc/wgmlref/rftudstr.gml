:pc.To this point in the tutorial, the general structure of
a document has been simplified as summarized in the
following figure.
:fig place=inline frame=box.
&gml.GDOC.
  &gml.BODY.
    . . . body of document
&gml.eGDOC.
:figcap.Simplified Document Structure
:efig.
:pc.The complete structure of a document may be summarized as follows.
:fig id='overall' place=inline frame=box.
&gml.GDOC.
  &gml.FRONTM.
    . . . front material
  &gml.BODY.
    . . . main body of document
  &gml.APPENDIX.
    . . . appendices
  &gml.BACKM.
    . . . back material
&gml.eGDOC.
:figcap.Overall Structure of Document
:efig
:pc.As illustrated in :FIGREF refid='overall'.,
there are four major segments with
the following contents&gml.
:dl
:dt.Front Material
:I1.front material
:dd.This segment contains entities such as the title page,
abstract, preface, table of contents and list of figures.
:dt.Body
:I1.body
:dd.This segment contains the main text for the document.
:dt.Appendix
:I1.appendix
:dd.This segment contains the appendices for the document.
:dt.Back Material
:I1.back material
:dd.This segment contains any ending text which follows
the appendices, such as the index.
:edl.
:pc.Each of the four segments is optional.
The complexity of a document determines which segments
will be used.
