:H2.Front Material
:pc.The front material starts with the :hp2.&gml.frontm:ehp2. tag
:I2 refid='ttfront'.ttfront
and may contain up to five entities&gml. title page
(:hp2.&gml.titlep:ehp2.),
abstract (:hp2.&gml.abstract:ehp2.),
:I2 refid='ttabstr'.ttabstr
preface (:hp2.&gml.preface:ehp2.),
:I2 refid='ttprefa'.ttprefa
table of contents
(:hp2.&gml.toc:ehp2.)
:I2 refid='tttoc'.tttoc
and list of figures (:hp2.&gml.figlist:ehp2.).
:I2 refid='ttfigli'.ttfigli
The particular entities to be included will depend upon the
requirements of a document.
When none of the five entities are necessary, the entire front material
segment may be omitted from the document.
The following subsections discuss each of the entities in the
front material.
:H3.Title Page
:pc.A title page specification begins with a :hp2.&gml.titlep:ehp2.
:I2 refid='tttitlp'.tttitlp
tag and concludes with an :hp2.&gml.etitlep:ehp2.
:I2 refid='ttetitl'.ttetitl
tag.
Between these two tags the document title
(:hp2.&gml.title:ehp2.),
:I2 refid='tttitle'.tttitle
the document number (:hp2.&gml.docnum:ehp2.),
:I2 refid='ttdocnu'.ttdocnu
the date of publication (:hp2.&gml.date:ehp2.)
:I2 refid='ttdate'.ttdate
and the author(s)
(:hp2.&gml.author:ehp2.)
:I2 refid='ttautho'.ttautho
with their address(es) (:hp2.&gml.address:ehp2.)
:I2 refid='ttaddre'.ttaddre
may be specified.
These tags may be given in any order;
the :hp2.&gml.author:ehp2., :HP2.&gml.title:eHP2.
and :hp2.&gml.address:ehp2.
tags may be specified a number of times.
The layout with which a document is formatted
establishes the style used to prepare the title page.
:p.A title page is illustrated in the following
example&gml.
:fig id=rfgtitp place=inline frame=box.
 :INCLUDE file='rfgtitpg'.
 :figcap.Sample Title Page
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box.
 :INCLUDE file='rfltitpg'.
 :figcap.
 :figdesc.Output of :figref refid=rfgtitp page=no.
:efig.
:pc.The following tags have been introduced in this section&gml.
:dl.
:dt.&gml.titlep
:I2 refid='tttitlp'.tttitlp
:dd.This tag indicates the start of the title page specification.
:dt.&gml.etitlep
:I2 refid='ttetitl'.ttetitl
:dd.This tag indicates the end of the title page specification.
:dt.&gml.title
:I2 refid='tttitle'.tttitle
:dd.This tag is used to give the title of the document,
and may be specified more than once for mulitiple title lines.
:dt.&gml.docnum
:I2 refid='ttdocnu'.ttdocnu
:dd.This tag is used to specify the document number.
:dt.&gml.date
:I2 refid='ttdate'.ttdate
:dd.This tag is used to specify the date printed
on the document.
The date on which the document is processed is used
when date text is not specified with the date tag.
:dt.&gml.author
:I2 refid='ttautho'.ttautho
:dd.This tag may be used to specify the name of an author.
It may be specified a number of times in the title page.
:dt.&gml.address
:I2 refid='ttaddre'.ttaddre
:dd.This tag is used to specify the start of an address entity.
The entity consists of a number of address lines given by
:hp2.&gml.aline:ehp2 tags.
:dt.&gml.eaddress
:I2 refid='tteaddr'.tteaddr
:dd.This tag is used to indicate the end of an address entity.
:dt.&gml.aline
:I2 refid='ttaline'.ttaline
:dd.This tag specifies an address line in an address entity,
and may be specified more than once.
:edl.
:H3.Abstract
:pc.An abstract may be specified following the
:hp2.&gml.abstract:ehp2 tag.
:I2 refid='ttabstr'.ttabstr
Heading tags (except :hp2.&gml.h0:ehp2.
and :hp2.&gml.h1:ehp2.)
may be used within the abstract.
The abstract section is implicitly terminated by the next major
document section.
:H3.Preface
:pc.A preface may be specified following the
:hp2.&gml.preface:ehp2 tag.
:I2 refid='ttprefa'.ttprefa
Heading tags (except :hp2.&gml.h0:ehp2. and :hp2.&gml.h1:ehp2.) may
:I2 refid='tth0'.tth0
:I2 refid='tth1'.tth1
be used within the preface.
The preface section is implicitly terminated by the next major
document section.
:H3.Table of Contents
:pc.The inclusion of the :hp2.&gml.toc:ehp2.
:I2 refid='tttoc'.tttoc
tag causes the table of contents to be printed.
The GML source files of some documents are processed
more than once
before they are actually printed.
In this case, the table of contents is placed in the front
material of the document.
When the source files of the GML document are processed only once,
the table of contents
cannot be placed in the front material.
For a one pass document where a table of contents is requested,
&WGML. will create the table of contents at the end of the document.
Documents are processed more than once when &WGML. is instructed
to use a number of passes
(see :HDREF refid='runpass'.).
:H3.List of Figures
:pc.The :hp2.&gml.figlist:ehp2.
:I2 refid='ttfigli'.ttfigli
tag causes a list of figures to be created.
The figure list is processed in the same way as the table of
contents when dealing with single versus multiple pass documents.
