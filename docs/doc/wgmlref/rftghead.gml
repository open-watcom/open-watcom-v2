:H2.H0, H1, H2, H3, H4, H5, H6
:XMP.
Format&gml. &gml.H:HP1.n:eHP1. \(id='id-name'\)
            \(stitle='character string'\).<text line>
        (:HP1.n:eHP1.=0,1)
Format&gml. &gml.H:HP1.n:eHP1. \(id='id-name'\).<text line>
        (:HP1.n:eHP1.=0,1,2,3,4,5,6)
:eXMP.
:I2 refid='gth0' pg=major.gth0
:I2 refid='gth1' pg=major.gth1
:I2 refid='gth2' pg=major.gth2
:I2 refid='gth3' pg=major.gth3
:I2 refid='gth4' pg=major.gth4
:I2 refid='gth5' pg=major.gth5
:I2 refid='gth6' pg=major.gth6
:P.
These tags are used to create headings for
sections and subsections of text.
A common convention uses the headings as follows&gml.
:Dl.
:DT.&gml.H0
:DD.Major part of document.
:DT.&gml.H1
:DD.Chapter.
:DT.&gml.H2
:DD.Section.
:DT.&gml.H3, &gml.H4, &gml.H5, &gml.H6
:DD.Subsections.
:eDL.
:PC.
The specific layout with which a document is formatted will determine
the format of the headings.
Some layouts cause the headings to be automatically numbered according to
a chosen convention.
The heading text specified with the tag may also be used in the
creation of top and/or bottom page banners.
:P.
A heading may be used where a basic document element is
permitted to appear, with the following restrictions&gml.
:OL.
:LI.:HP2.&gml.h0:eHP2. tags may only be used in the body of a document.
:LI.:HP2.&gml.h1:eHP2. tags may not be used in the preface or the abstract.
:eOL.
:P.
The :HP2.stitle:eHP2.
:I2 refid='gastitl'.gastitl
attribute allows you to specify a short title for the heading.
The short title will be used instead of the heading text when creating
the top and/or bottom page banners.
The short title attribute is valid with a level one or level zero
heading.
:P.
The :hp2.id:ehp2.
:I2 refid='gaid'.gaid
attribute assigns an identifier name to the heading.
The identifier name is used when processing a heading reference, and
must be unique within the document.
