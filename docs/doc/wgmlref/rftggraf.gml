:H2 id='gtgraph'.GRAPHIC
:XMP.
Format&gml. &gml.GRAPHIC file='file name'
                 \(depth='vert-space-unit'\)
                 \(width=page
                        column
                        'hor-space-unit'\)
                 \(scale=number\)
                 \(xoff='hor-space-unit'\)
                 \(yoff='vert-space-unit'\).
:eXMP.
:I2 refid='gtgrap' pg=major.gtgrap
:P.
This tag is used to include a graphic image file into
the document.
&WGML. supports two types of graphic include files.
If the first two characters in the file are percent(%)
followed by an exclamation mark(!), then the file
is a PostScript graphic.
A PostScript graphic file will only produce an image if
the document is produced for a PostScript device.
If the image file is not a PostScript graphic,
a special validity check is performed on the file to determine
if it is a WATCOM GKS PXA image file.
If it is not a PXA file, it is assumed to be a PostScript graphic file.
PXA files are supported with PostScript, HP LaserJet Plus, and
IBM PC Graphic printers, although grey scales are only supported
with a PostScript device.
Documents can be proofed on devices which are not supported
by the graphic tag.
If the device is not supported,
the appropriate amount of white
space is left for the graphic.
All space value attributes are linked to the current font
being used in the document (see :HDREF refid='fnlink'.).
:P.
The required attribute :HP2.file:eHP2.
:I2 refid='gafile'.gafile
specifies the name of the graphic file to include.
The value of the attribute is a character string, and may
be any valid file name.
:P.
The :HP2.depth:eHP2. attribute
:I2 refid='gadepth'.gadepth
specifies the vertical size of the graphic image.
The value of the attribute is any valid vertical space unit,
and must be specified if the graphic is a PostScript image.
If the specified depth is less than the size of the actual graphic,
the difference in size is taken off the top of the graphic image.
If the depth is not specified when including a PXA file,
the graphic depth is obtained from
information within the image file.
A PXA file is assumed to be defined in a vertical direction
with 150 dots per inch (dpi)
for PostScript and HP LaserJet devices, and 72dpi for PC Graphics
printers.
:P.
The :HP2.width:eHP2. attribute allows you to specify
:I2 refid='gawidth'.gawidth
the width of the graphic.
The attribute value :HP1.page:eHP1. specifies that the
graphic will be as wide as the page, even if the document
is formatted for more than one column.
The attribute value :HP1.column:eHP1. specifies that the
graphic shall be one column wide.
If a horizontal space unit is used as the attribute value,
the graphic will have the width specified by the attribute value.
If the graphic is larger than the specified width, the difference
in size is taken off the right hand side of the graphic image.
A PXA file is assumed to be defined in a horizontal direction
with 150 dots per inch (dpi)
for PostScript and HP LaserJet devices, and 120dpi for PC Graphics
printers.
:P.
The :HP2.scale:eHP2. attribute allows you to alter
:I2 refid='gascale'.gascale
the size of the graphic.
The scale operation is performed after all depth and offset calculations
are completed, and is supported with PostScript and HP LaserJet devices
only.
The attribute value is a positive integer number which represents
a percentage of the original graphic size.
Therefore, the value '100' will result in no scaling.
With the HP LaserJet, only the values :HP1.50, 100, 150:eHP1.
and :HP1.200:eHP1. are valid.
:P.
The :HP2.xoff:eHP2. and :HP2.yoff:eHP2. attributes specify
:I2 refid='gaxoff'.gaxoff
:I2 refid='gayoff'.gayoff
an offset into the graphic.
Some images are saved so that they will print in the middle
of a blank page.
By specifying the amount of space from the lower left corner of
this blank page to the lower left hand corner of the printable
graphic with the offset attributes,
&WGML. can shift the graphic to position it properly on
the page.
The value of the attributes can be a vertical space unit, with
negative values being allowed.
