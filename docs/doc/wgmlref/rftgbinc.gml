:H2.BINCLUDE
:XMP.
Format&gml. &gml.BINCLUDE file='file name'
                  depth='vert-space-unit'
                  reposition=start
                             end.
:eXMP.
:I1.binary include
:I1.graphic include
:I2 refid='gtbincl' pg=major.gtbincl
:P.
The binary include tag causes the data in the
specified file to be included into the document
without being processed by &WGML..
This tag provides the means to include graphic or
non-textual data in the document (see also :HDREF refid='gtgraph'.).
:P.
The required attribute :HP2.file:eHP2.
:I2 refid='gafile'.gafile
specifies the name of the file to include.
The value of the attribute is a character string, and may
be any valid file name.
The input file is processed as containing binary data.
If the input is text data, a record type such as "(t:80)"
must be prefixed to the file name
(see :HDREF refid='files'.).
:P.
The required attribute :HP2.depth:eHP2.
:I2 refid='gadepth'.gadepth
specifies the vertical size of the contents of the file.
The value of the attribute is any non-zero vertical space unit.
This depth value must be the exact depth of the file contents when
placed in the formatted output, and
is used to reserve the required amount of space
on the page.
The depth attribute is linked to the current font
being used in the document (see :HDREF refid='fnlink'.).
:P.
The required attribute :HP2.reposition:eHP2.
:I2 refid='garepos'.garepos
specifies the place in the formatted output that new text would be placed
after the content of the file is processed.
With some devices, a graphic will not change the current position
on the output page when it is processed.
In this case, &WGML. must ensure that following text is started
on the output page after the graphic.
The attribute value :HP1.start:eHP1. indicates that the current
position on the output page will be unchanged
after the graphic is processed.
The attribute value :HP1.end:eHP1. indicates that the current
position on the output page will
be immediately following the graphic
after the graphic is processed.
If the included data is within a framed figure, and the frame
is formed with characters, the value of the reposition attribute
must be the value :HP1.start:eHP1..
