.section *refid='artwork' artwork
.ix 'artwork'
.ix 'In-line' 'artwork'
.ix 'Figures' 'images'
.tag begin
.tdesc
The artwork tag declares a bit-mapped image to be included in the help file. Windows and OS/2 BMP files 
are supported. &ipfcmd looks for images in the directories pointed to by the IPFCARTWORK environment 
variable (see :HDREF refid='ipfenv'.).
.tattrbs
.tattr name='filename.ext'
The name of the bitmap image file. This attribute is required.
.tattr align=left | center | right
Specifies how the image is to be aligned in the current window. Can be one of left, center, or right.
.tattr linkfile='filename.ext'
A direct reference to a file containing artlink/eartlink tags and its associated link tags. This allows the 
image to act as a hyper-graphic link.
.tattr runin
This flag places the image within the line of text surrounding it. Otherwise, it acts as a block with 
the text appearing above and below.
.tattr fit
This flag causes the image to be scaled to fill the window. Note that this may distort the image if the
aspect ratio of the window and image differ.
.tclass In-line
.tcont None
.tseealso
:HDREF refid='artlink'.
.tag end

