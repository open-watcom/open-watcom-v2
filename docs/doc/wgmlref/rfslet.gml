:H2.WATCOM Letter Format
:I1.letter format
:FIG place=inline frame=box.
&gml.GDOC sec = 'classification'.
      (sec is optional)
   &gml.FROM.
      address line
        (may occur several times)
   &gml.DATE align = 'value'
         depth = 'vert-space-unit'.date-text
        (align and depth are optional)
        (date-text is optional)
   &gml.DOCNUM.document number
   &gml.TO compact.
        (compact is optional)
      Recipient-line
        (may occur several times)
   &gml.ATTN.attention name
   &gml.SUBJECT.subject text
   &gml.OPEN.opening salutation
      basic document elements
   &gml.CLOSE depth = 'vert-space-unit'.closing salutation
         (depth is optional)
      author line
        (may occur several times)
   &gml.eCLOSE.typist mark
          (typist mark is optional)
   &gml.DISTRIB.
   &gml.DIST.label
      names
        (one per line)
   &gml.eDISTRIB.
&gml.eGDOC.
:eFIG.
