.np
.ix 'graphics library'
The &company &lang Graphics Library consists of a large number of
&routines that provide graphical image support under DOS and QNX.
This chapter provides an overview of this support.
The following topics are discussed.
.begbull
.bull
Graphics &rroutines
:CMT. :HDREF refid='grov1'.
.bull
Graphics Adapters
:CMT. :HDREF refid='grov2'.
.bull
Classes of Graphics &rroutines
:CMT. :HDREF refid='grov3'.
.autopoint
.point
Environment &rroutines
:CMT. :HDREF refid='grov4'.
.point
Coordinate System &rroutines
:CMT. :HDREF refid='grov5'.
.point
Attribute &rroutines
:CMT. :HDREF refid='grov6'.
.point
Drawing &rroutines
:CMT. :HDREF refid='grov7'.
.point
Text &rroutines
:CMT. :HDREF refid='grov8'.
.point
Graphics Text &rroutines
:CMT. :HDREF refid='grov9'.
.point
Image Manipulation &rroutines
:CMT. :HDREF refid='grov10'.
.point
Font Manipulation &rroutines
:CMT. :HDREF refid='grov11'.
.point
Presentation Graphics &rroutines
:CMT. :HDREF refid='grov12'.
.illust begin
Display &rroutines
:CMT. :HDREF refid='grov13'.
Analyze &rroutines
:CMT. :HDREF refid='grov14'.
Utility &rroutines
:CMT. :HDREF refid='grov15'.
.illust end
.endpoint
.bull
.if '&lang' eq 'C' .do begin
Graphics Header Files
:CMT. :HDREF refid='grov16'.
.do end
.el .do begin
Include Files
:CMT. :HDREF refid='grov16'.
.do end
.endbull
.*
.section *refid=grov1 Graphics &rroutines
.***************************
.np
.ix 'graphics &routines'
Graphics &routines are used to display graphical images such as lines
and circles upon the computer screen.
&rroutines are also provided for displaying text along with the
graphics output.
.*
.section *refid=grov2 Graphics Adapters
.*************************
.np
.ix 'graphics adapters'
Support is provided for both color and monochrome screens which are
connected to the computer using any of the following graphics adapters:
.begbull
.bull
IBM Monochrome Display/Printer Adapter (MDPA)
.bull
IBM Color Graphics Adapter (CGA)
.bull
IBM Enhanced Graphics Adapter (EGA)
.bull
IBM Multi-Color Graphics Array (MCGA)
.bull
IBM Video Graphics Array (VGA)
.bull
Hercules Monochrome Adapter
.bull
SuperVGA adapters (SVGA) supplied by various manufacturers
.endbull
.*
.section *refid=grov3 Classes of Graphics &rroutines
.**************************************
.np
.ix 'classes of &routines'
The &routines in the &company &lang Graphics Library can be organized into
a number of classes:
.begnote
.note Environment &rroutines
.sk 0
These &routines deal with the hardware environment.
.note Coordinate System &rroutines
.sk 0
These &routines deal with coordinate systems and mapping coordinates
from one system to another.
.note Attribute &rroutines
.sk 0
These &routines control the display of graphical images.
.note Drawing &rroutines
.sk 0
These &routines display graphical images such as lines and ellipses.
.note Text &rroutines
.sk 0
These &routines deal with displaying text in both graphics and text modes.
.note Graphics Text &rroutines
.sk 0
These &routines deal with displaying graphics text.
.note Image Manipulation &rroutines
.sk 0
These &routines store and retrieve screen images.
.note Font Manipulation &rroutines
.sk 0
These &routines deal with displaying font based text.
.note Presentation Graphics &rroutines
.sk 0
These &routines deal with displaying presentation graphics
elements such as bar charts and pie charts.
.endnote
.np
The following subsections describe these &routine classes in more detail.
Each &routine in the class is noted with a brief description of its
purpose.
.*
.beglevel
.*
.section *refid=grov4 Environment &rroutines
.******************************
.np
These &routines deal with the hardware environment.
The
.kw _getvideoconfig
&routine returns information about the current video mode and the
hardware configuration.
The
.kw _setvideomode
&routine selects a new video mode.
.np
.ix 'graphic page'
Some video modes support multiple pages of screen memory.
The visual page (the one displayed on the screen) may be different than
the active page (the one to which objects are being written).
.keep begin
.np
.fdbeg
.fd _getactivepage
get the number of the current active graphics page
.fd _getvideoconfig
get information about the graphics configuration
.fd _getvisualpage
get the number of the current visual graphics page
.fd _grstatus
get the status of the most recently called graphics library &routine
.fd _setactivepage
set the active graphics page (the page to which graphics objects are drawn)
.fd _settextrows
set the number of rows of text displayed on the screen
.fd _setvideomode
select the video mode to be used
.fd _setvideomoderows
select the video mode and the number of text rows to be used
.fd _setvisualpage
set the visual graphics page (the page displayed on the screen)
.fdend
.keep end
.*
.section *refid=grov5 Coordinate System &rroutines
.************************************
.np
These &routines deal with coordinate systems and mapping coordinates
from one system to another.
The &company &lang Graphics Library supports three coordinate systems:
.ix 'coordinate systems'
.autonote
.note
Physical coordinates
.note
View coordinates
.note
Window coordinates
.endnote
.np
.ix 'physical coordinates'
Physical coordinates match the physical dimensions of the screen.
The physical origin, denoted (0,0), is located at the
top left corner of the screen.
A pixel to the right of the origin has a positive x-coordinate and a
pixel below the origin will have a positive y-coordinate.
The x- and y-coordinates will never be negative values.
:cmt..sk
:cmt..millust begin
:cmt.                      increasing
:cmt.       (0,0)           x-values
:cmt..bx on 13 42
:cmt.
:cmt.
:cmt.
:cmt.increasing
:cmt. y-values
:cmt.
:cmt.
:cmt.
:cmt..bx off
:cmt..millust end
.np
.ix 'view coordinates'
The view coordinate system can be defined upon the physical coordinate
system by moving the origin from the top left corner of the screen
to any physical coordinate
.gfuncref setvieworg .
In the view coordinate system, negative x- and y-coordinates are allowed.
The scale of the view and physical coordinate systems is identical (both are
in terms of pixels).
.np
.ix 'window coordinates'
The window coordinate system is defined in terms of a range of
user-specified values
.gfuncref setwindow .
These values are scaled to map onto the physical coordinates of the screen.
This allows for consistent pictures regardless of the
resolution (number of pixels) of the screen.
.keep begin
.np
.fdbeg
.fd _getcliprgn
get the boundary of the current clipping region
.fd _getphyscoord
get the physical coordinates of a point in view coordinates
.fd _getviewcoord
get the view coordinates of a point in physical coordinates
.fd _getviewcoord_w
get the view coordinates of a point in window coordinates
.fd _getviewcoord_wxy
get the view coordinates of a point in window coordinates
.fd _getwindowcoord
get the window coordinates of a point in view coordinates
.fd _setcliprgn
set the boundary of the clipping region
.fd _setvieworg
set the position to be used as the origin of the view coordinate system
.fd _setviewport
set the boundary of the clipping region
and the origin of the view coordinate system
.fd _setwindow
define the boundary of the window coordinate system
.fdend
.keep end
.*
.section *refid=grov6 Attribute &rroutines
.****************************
.np
These &routines control the display of graphical images such as
lines and circles.
Lines and figures are drawn using the current color
.gfuncref setcolor ,
the current line style
.gfuncref setlinestyle ,
the current fill mask
.gfuncref setfillmask ,
and the current plotting action
.gfuncref setplotaction .
.keep begin
.np
.fdbeg
.fd _getarcinfo
get the endpoints of the most recently drawn arc
.fd _getbkcolor
get the background color
.fd _getcolor
get the current color
.fd _getfillmask
get the current fill mask
.fd _getlinestyle
get the current line style
.fd _getplotaction
get the current plotting action
.fd _remapallpalette
assign colors for all pixel values
.fd _remappalette
assign color for one pixel value
.fd _selectpalette
select a palette
.fd _setbkcolor
set the background color
.fd _setcolor
set the current color
.fd _setfillmask
set the current fill mask
.fd _setlinestyle
set the current line style
.fd _setplotaction
set the current plotting action
.fdend
.keep end
.*
.section *refid=grov7 Drawing &rroutines
.**************************
.np
These &routines display graphical images such as lines and ellipses.
&rroutines exist to draw straight lines
.gfuncsref lineto ,
rectangles
.gfuncsref rectangle ,
polygons
.gfuncsref polygon ,
ellipses
.gfuncsref ellipse ,
elliptical arcs
.gfuncsref arc
and pie-shaped wedges from ellipses
.gfuncsref pie .
.np
These figures are drawn using the attributes described in the previous
section.
The &routines ending with
.id _w
or
.id _wxy
use the window coordinate system; the others use the view coordinate system.
.keep begin
.np
.fdbeg
.fd _arc
draw an arc
.fd _arc_w
draw an arc using window coordinates
.fd _arc_wxy
draw an arc using window coordinates
.fd _clearscreen
clear the screen and fill with the background color
.fd _ellipse
draw an ellipse
.fd _ellipse_w
draw an ellipse using window coordinates
.fd _ellipse_wxy
draw an ellipse using window coordinates
.fd _floodfill
fill an area of the screen with the current color
.fd _floodfill_w
fill an area of the screen in window coordinates with the current color
.fd _getcurrentposition
get the coordinates of the current output position
.fd _getcurrentposition_w
get the window coordinates of the current output position
.fd _getpixel
get the color of the pixel at the specified position
.fd _getpixel_w
get the color of the pixel at the specified position in window coordinates
.fd _lineto
draw a line from the current position to a specified position
.fd _lineto_w
draw a line from the current position to a specified position in window coordinates
.fd _moveto
set the current output position
.fd _moveto_w
set the current output position using window coordinates
.fd _pie
draw a wedge of a "pie"
.fd _pie_w
draw a wedge of a "pie" using window coordinates
.fd _pie_wxy
draw a wedge of a "pie" using window coordinates
.fd _polygon
draw a polygon
.fd _polygon_w
draw a polygon using window coordinates
.fd _polygon_wxy
draw a polygon using window coordinates
.fd _rectangle
draw a rectangle
.fd _rectangle_w
draw a rectangle using window coordinates
.fd _rectangle_wxy
draw a rectangle using window coordinates
.fd _setpixel
set the color of the pixel at the specified position
.fd _setpixel_w
set the color of the pixel at the specified position in window coordinates
.fdend
.keep end
.*
.section *refid=grov8 Text &rroutines
.***********************
.np
These &routines deal with displaying text in both graphics and text modes.
This type of text output can be displayed
in only one size.
.np
This text is displayed using the
.kw _outtext
and
.kw _outmem
&routines..
The output position for text follows the last text that was displayed
or can be reset
.gfuncref settextposition .
Text windows can be created
.gfuncref settextwindow
in which the text will scroll.
Text is displayed with the current text color
.gfuncref settextcolor .
.keep begin
.np
.fdbeg
.fd _clearscreen
clear the screen and fill with the background color
.fd _displaycursor
determine whether the cursor is to be displayed after a graphics &routine
completes execution
.fd _getbkcolor
get the background color
.fd _gettextcolor
get the color used to display text
.fd _gettextcursor
get the shape of the text cursor
.fd _gettextposition
get the current output position for text
.fd _gettextwindow
get the boundary of the current text window
.fd _outmem
display a text string of a specified length
.fd _outtext
display a text string
.fd _scrolltextwindow
scroll the contents of the text window
.fd _setbkcolor
set the background color
.fd _settextcolor
set the color used to display text
.fd _settextcursor
set the shape of the text cursor
.fd _settextposition
set the output position for text
.fd _settextwindow
set the boundary of the region used to display text
.fd _wrapon
permit or disallow wrap-around of text in a text window
.fdend
.keep end
.*
.section *refid=grov9 Graphics Text &rroutines
.********************************
.np
These &routines deal with displaying graphics text.
Graphics text is displayed
as a sequence of line segments, and
can be drawn in different sizes
.gfuncref setcharsize ,
with different orientations
.gfuncref settextorient
and alignments
.gfuncref settextalign .
The &routines ending with
.id _w
use the window coordinate system; the others use the view coordinate system.
.keep begin
.np
.fdbeg
.fd _gettextextent
get the bounding rectangle for a graphics text string
.fd _gettextsettings
get information about the current settings used to display graphics text
.fd _grtext
display graphics text
.fd _grtext_w
display graphics text using window coordinates
.fd _setcharsize
set the character size used to display graphics text
.fd _setcharsize_w
set the character size in window coordinates used to display graphics text
.fd _setcharspacing
set the character spacing used to display graphics text
.fd _setcharspacing_w
set the character spacing in window coordinates used to display graphics text
.fd _settextalign
set the alignment used to display graphics text
.fd _settextorient
set the orientation used to display graphics text
.fd _settextpath
set the path used to display graphics text
.fdend
.keep end
.*
.section *refid=grov10 Image Manipulation &rroutines
.*************************************
.np
These &routines are used to transfer screen images.
The
.kw _getimage
&routine transfers a rectangular image from the screen into memory.
The
.kw _putimage
&routine transfers an image from memory back onto the screen.
The &routines ending with
.id _w
or
.id _wxy
use the window coordinate system; the others use the view coordinate system.
.keep begin
.np
.fdbeg
.fd _getimage
store an image of an area of the screen into memory
.fd _getimage_w
store an image of an area of the screen in window coordinates into memory
.fd _getimage_wxy
store an image of an area of the screen in window coordinates into memory
.fd _imagesize
get the size of a screen area
.fd _imagesize_w
get the size of a screen area in window coordinates
.fd _imagesize_wxy
get the size of a screen area in window coordinates
.fd _putimage
display an image from memory on the screen
.fd _putimage_w
display an image from memory on the screen using window coordinates
.fdend
.keep end
.*
.section *refid=grov11 Font Manipulation &rroutines
.************************************
.np
These &routines are for the display of fonts compatible with
Microsoft Windows.
Fonts are contained in files with an extension of
.mono .FON.
Before font based text can be displayed, the fonts must be registered
with the
.kw _registerfonts
&routine,
and a font must be selected with the
.kw _setfont
&routine..
.keep begin
.np
.fdbeg
.fd _getfontinfo
get information about the currently selected font
.fd _getgtextextent
get the length in pixels of a text string
.fd _getgtextvector
get the current value of the font text orientation vector
.fd _outgtext
display a string of text in the current font
.fd _registerfonts
initialize the font graphics system
.fd _setfont
select a font from among the registered fonts
.fd _setgtextvector
set the font text orientation vector
.fd _unregisterfonts
frees memory allocated by the font graphics system
.fdend
.keep end
.*
.section *refid=grov12 Presentation Graphics &rroutines
.****************************************
.np
These &routines provide a system for displaying and manipulating
presentation graphics elements such as bar charts and pie charts.
The presentation graphics &routines can be further divided into
three classes:
.begnote
.note Display &rroutines
.sk 0
These &routines are for the initialization of the presentation graphics
system and the displaying of charts.
.note Analyze &rroutines
.sk 0
These &routines calculate default values for chart elements without
actually displaying the chart.
.note Utility &rroutines
.sk 0
These &routines provide additional support to control the appearance
of presentation graphics elements.
.endnote
.np
The following subsections describe these &routine classes in more detail.
Each &routine in the class is noted with a brief description of its
purpose.
.*
.beglevel
.*
.section *refid=grov13 Display &rroutines
.**************************
.np
These &routines are for the initialization of the presentation graphics
system and the displaying of charts.
The
.kw _pg_initchart
&routine initializes the system and should be the first
presentation graphics &routine called.
The single-series &routines display a
single set of data on a chart; the multi-series &routines
(those ending with
.id ms)
display several sets of data on the same chart.
.keep begin
.np
.fdbeg
.fd _pg_chart
display a bar, column or line chart
.fd _pg_chartms
display a multi-series bar, column or line chart
.fd _pg_chartpie
display a pie chart
.fd _pg_chartscatter
display a scatter chart
.fd _pg_chartscatterms
display a multi-series scatter chart
.fd _pg_defaultchart
initialize the chart environment for a specific chart type
.fd _pg_initchart
initialize the presentation graphics system
.fdend
.keep end
.*
.section *refid=grov14 Analyze &rroutines
.**************************
.np
These &routines calculate default values for chart elements without
actually displaying the chart.
The &routines ending with
.id ms
analyze multi-series charts; the others analyze single-series charts.
.keep begin
.np
.fdbeg
.fd _pg_analyzechart
analyze a bar, column or line chart
.fd _pg_analyzechartms
analyze a multi-series bar, column or line chart
.fd _pg_analyzepie
analyze a pie chart
.fd _pg_analyzescatter
analyze a scatter chart
.fd _pg_analyzescatterms
analyze a multi-series scatter chart
.fdend
.keep end
.*
.section *refid=grov15 Utility &rroutines
.**************************
.np
These &routines provide additional support to control the appearance
of presentation graphics elements.
.keep begin
.np
.fdbeg
.fd _pg_getchardef
get bit-map definition for a specific character
.fd _pg_getpalette
get presentation graphics palette (colors, line styles, fill patterns and
plot characters)
.fd _pg_getstyleset
get presentation graphics style-set (line styles for window borders and
grid lines)
.fd _pg_hlabelchart
display text horizontally on a chart
.fd _pg_resetpalette
reset presentation graphics palette to default values
.fd _pg_resetstyleset
reset presentation graphics style-set to default values
.fd _pg_setchardef
set bit-map definition for a specific character
.fd _pg_setpalette
set presentation graphics palette (colors, line styles, fill patterns and
plot characters)
.fd _pg_setstyleset
set presentation graphics style-set (line styles for window borders and
grid lines)
.fd _pg_vlabelchart
display text vertically on a chart
.fdend
.keep end
.*
.endlevel
.*
.endlevel
.*
.if '&lang' eq 'C' .do begin
.section *refid=grov16 Graphics Header Files
.do end
.el .do begin
.section *refid=grov16 Include Files
.do end
.******************************
.np
.if '&lang' eq 'C' .do begin
.ix 'graphics header files'
All program modules which use the Graphics Library should
include the header file
.filename graph.h.
This file contains prototypes for all the &routines in the library
as well as the structures and constants used by them.
.np
Modules using the presentation graphics &routines should also
include the header file
.filename pgchart.h.
.do end
.el .do begin
.ix 'graphics include files'
All program modules which use the Graphics Library should
include the file
.filename graphapi.fi.
This file contains definitions of all the &routines in the library.
As well, each &routine should include
.filename graph.fi
which contains all the structure and constant definitions.
.np
Modules using the presentation graphics &routines should also
include the file
.filename pgapi.fi.
As well, each &routine should include
.filename pg.fi.
.do end
