.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 11-may-93	Paul Fast	initial draft
.*
:CHAPTER.Bitmaps, Icons and Cursors 
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
While it is not necessary to totally understand bitmaps, icons and
cursors a brief discussion here may clear up some misconceptions and
explain why &imgname has some of the operations it does.  All three
image types have similarities but each image has its designed purpose
and appropriate place.
:SECTION.Bitmaps
The simplest of the images to understand are bitmaps.  A bitmap is an
ordered set of colored pixels representing a picture.  In theory, a
bitmap is limitless in its dimensions.  However, due to constraints
in memory and screen sizes, bitmaps rarely get larger than 800x600
pixels.  &imgname can open, edit and save bitmaps ranging in size up
to 512x512 pixels.
:P.
All pixels in a bitmap must have a color given to them.  When
displaying a bitmap on a screen, it is possible to make a portion of
the bitmap the same as the screen background, giving the appearance
of 
:ITALICS.transparency
:PERIOD.
However, there is no way to store this transparency.  Thus when
designing bitmaps with &imgname, all pixels must be given a color
(note that the default color is white).  Windows stores bitmaps according
to the number of colors contained in the bitmap.  A bitmap can be either
a two color, 16 color, 256 color or 2^24 color
bitmap.  &imgname allows editing of
only two color and 16 color bitmaps.
:P.
The default file extension for bitmap files in Windows is .BMP.
A bitmap file, as it is stored on disk, begins with a structure
containing information about the bitmap size and dimensions. 
Following this structure is a block of information about the color
scheme of the bitmap, dictating how the pixel values need to be
interpreted.  At the end of the file are the values for all the
pixels.  A bitmap file can contain only one bitmap image.
:P.
The Windows programmer will most commonly use bitmaps in their
program to give them a graphical dimension.  Bitmaps can be used to
represent anything from buttons to brush patterns to company logos.
:SECTION.Icons
Icons are most commonly used to represent the execution of a command
or the contents of a file.  Icons are not limitless in size but are
restricted to 32x16, 32x32 or in some cases 64x64 pixels.  The
dimensions of an icon depend
on the graphics capabilities of the machine
on which they are displayed.
:P.
Unlike bitmaps, icons may have pixels that do not have a color value
associated with them.  Pixels in an icon may be assigned to be the
color of the screen over which they are displayed.  Similarly, they
may take on the inverted color of the screen over which they are
displayed.  This apparent transparency allows icons to take on shapes
other than rectangular.  &imgname allows a user to assign a pixel to
be the screen color or inverted screen color by having a particular color
represent the screen.  When a user selects the screen color to be
the currently selected color, any pixels set with this color will
appear transparent when used in an application.  Icon images must be either
two color or 16 color images.
:P.
Icons are stored in file formats very similar to that of bitmaps.  One
exception is that icon files store two copies of the pixels.  The
first copy represents the colors of each of the pixels.  The second
copy dictates whether the first value stored should be interpreted as
a normal color, a screen color or an inverse color.  An additional
difference is that an icon file may contain data for more than one
icon.  Hence &imgname allows you to add icons to an icon file or
remove icons when more than one exists.  While a file may contain more
than one icon, it may only contain one icon of a particular dimension
and color scheme.  For example, a valid icon file could have both a
16 color 32x32 icon and a monochrome 32x32 icon; however, it could
not contain two 16 color 32x32 pixel icons.
The default extension for icon files is .ICO.
:SECTION.Cursors
Cursors are used to represent the location of the active point on the
screen as given by the mouse.  The most common cursor used in
Windows is the arrow, but Windows programmers may design their own
cursors to suit their particular needs.  Cursors are very similar to
icons in that they are restricted to a 32x32 or 64x64 pixel dimension
and they are permitted to use screen colors.  However, the only
other colors they may contain are black and white.
:P.
Like icons, cursors can take on shapes other than rectangular by
specifying a transparent color for the outer edges of the image.
It is also quite useful 
that pixels can be the inverted screen color since this can be used
to guarantee
that the cursor will always be visible regardless of the screen color. 
Moreover, a cursor has a
:HILITE.hot spot
:PERIOD.
Because of the size of cursors, it is often unclear which point the
user is attempting to select when the mouse button is clicked.  A hot
spot is a set of 
:ITALICS.x, y
coordinates relative to the cursor dimensions that indicates to
Windows the point it should use when the mouse button is clicked.
&imgname allows a user to specify the hot spot for a cursor by
selecting the hot spot tool and clicking over the desired pixel.
:P.
Cursor files have extension .CUR 
and are started in a format very similar to that of icons.  However,
cursor files also store the coordinates of the hot spot.
