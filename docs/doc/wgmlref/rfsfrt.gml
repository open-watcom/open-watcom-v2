:H2.Front Material
:I1.front material
:I1.frontm
:FIG place=inline frame=box.
&gml.FRONTM.
  title page
  abstract
  preface
  table of contents
  list of figures
:eFIG.
:H3.Title Page
:I1.title page
:I1.titlep
:I1.title
:I1.docnum
:I1.date
:I1.author
:I1.address
:I1.aline
:I1.stitle
:FIG place=inline frame=box.
&gml.TITLEP.
  &gml.TITLE stitle = short title.title text
        (stitle is optional)
  &gml.DOCNUM.document number
  &gml.DATE.date text
       (date text is optional)
  &gml.AUTHOR.author's name
  &gml.ADDRESS.
    &gml.ALINE.address line
      (may occur several times)
  &gml.eADDRESS.
&gml.eTITLEP.
:eFIG.
:H3.Abstract
:I1.abstract
:FIG place=inline frame=box.
&gml.ABSTRACT.
  basic document elements
  (headings :HP2.H2-H6:eHP2. allowed)
:eFIG.
:H3.Preface
:I1.preface
:FIG place=inline frame=box.
&gml.PREFACE.
  basic document elements
  (headings :HP2.H2-H6:eHP2. allowed)
:eFIG.
:H3.Table of Contents
:I1.table of contents
:I1.toc
:FIG place=inline frame=box.
&gml.TOC.
:eFIG.
:H3.List of Figures
:I1.list of figures
:I1.figlist
:FIG place=inline frame=box.
&gml.FIGLIST.
:eFIG.
