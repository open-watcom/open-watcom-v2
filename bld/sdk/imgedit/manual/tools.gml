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
.* 11-may-93	Paul Fast   	initial draft
.*
:CHAPTER id='tools' Tools
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
Any image can be designed by simply setting each pixel to the
desired color with the pencil tool.  However, this can be a long
and tedious process and it is not very practical for larger bitmaps. 
Hence &imgname provides various tools to aid in drawing.  
:P.
Recall that the tool window looks like the following:
.snap 'toolwnd' '1.37' 'WATCOM Image Editor Tool Window.'
The tools are displayed in the tool window as buttons.  To select any tool
click the left mouse button over the tool to be selected.  That
button will appear pushed down.  Alternatively, the desired tool can
be selected from the
:HILITE.Tools
menu.  Tools selected from the tool window must be selected with the
left mouse button.  However, once the tool is selected, the user has
the option to use either the left or right mouse button.  Using the
left button will display the results of the draw with the color
currently selected for the left button and using the right button
will draw in the current right color.

:SECTION.Free hand drawing
The pencil is the tool used for drawing free hand.  When in the edit
window, the cursor changes to the shape of a pencil.  When either
mouse button is pressed down the pixel under the cursor is changed to
the currently selected color for that button.  By holding either
the left or right mouse button
down and moving the mouse around the edit window, a trail of the
selected color is left according to where the mouse travelled.  For
the best results when drawing free hand on large bitmaps
move the mouse slowly to gain more
accuracy.
:SECTION.Drawing Lines
The line drawing tool is represented by the button on the tool window
with the line on it.  The cursor corresponding to the line drawing
tool is a cross hairs cursor.  To draw a line, move the cursor to
where one of the end points of the line is to be in the edit window. 
Hold the mouse button down and move the mouse to the location of the
other end point.  A thin line approximating the results of the line
draw appears and alters every time the mouse is moved.  When the
mouse button is released, the line is drawn.  
:P.
If at any time while
holding the button down, the line is no longer desired, hitting the 
:HILITE.ESC
key will cancel the operation.  While moving the mouse around with
the mouse button held down, the
:HILITE.Size
section in the status window indicates the size of the line in the 
:ITALICS.x
and
:ITALICS.y
directions.
:SECTION.Drawing Ellipses and Rectangles
Ellipses and rectangles
can be drawn in either filled or outlined form.  These are
in the 
:HILITE.Tools
menu and are shown on the tool window with a filled and outlined ellipse 
and similar
rectangles.  The edit cursor for all four of these drawing tools are
the cross hairs.  
:P.
A rectangle is drawn by clicking on the rectangle tool and moving to
the desired location of one of the rectangle corners in the edit window.
Holding the
mouse button down and moving the cursor around shows the rectangle
that will result from the draw.  When the desired rectangle is
established, the mouse button should be released.
:P.
An ellipse
is drawn by specifying a bounding rectangle and fitting the ellipse
inside the rectangle.  To draw an ellipse select one of the ellipse
buttons from the tool window and move the cursor in the edit
window to the location of one of the corners of the desired bounding
rectangle.  Hold the mouse button down and move the cursor to the
corner diagonal to the initial corner.  As the mouse is moved with
the button held down, an approximation of the ellipse is shown in the
edit window.  
Releasing the mouse button will cause
the ellipse to be drawn according to the mouse position.
:P.
For both ellipses and rectangles, the 
:ITALICS.Size
section of the status window will display the current size of the
shape in the
:ITALICS.x
and
:ITALICS.y
directions while the mouse button is held down.
Once
again, hitting the 
:HILITE.ESC
key while holding the mouse button down will cause the draw to be
cancelled.
:SECTION.Region Selecting
The button in the tool window with the dashed rectangle represents
the tool used to select a region.  A region is simply a rectangle
in which an edit operation is performed.  Once a region is selected,
that area can be cleared, cut to the clipboard, copied to the
clipboard, the recipient of a paste, rotated or reflected.  The
cursor for the region selector is also the cross hairs.
:P.
A region is selected exactly the same way that a rectangle is drawn. 
However, when the mouse button is released the outlined region stays
there until an edit operation is performed.  Attempting to select
multiple regions will simply cause the previous region selected to
disappear.  As with the other shape tools, hitting
:HILITE.ESC
while holding the mouse button down will cancel the current region
selection and the size of the selected region is given in the 
:ITALICS.Size
section of the status window.  
The current color selection for the region selecting tool has
no effect on the operation.
:SECTION.Paint Brush
The paint brush tool is very similar to the pencil tool.  However,
the paint brush paints more than 1 pixel simultaneously.  Depending
on the currently selected brush width (indicated in the
:HILITE.Brush Width
submenu of the 
:HILITE.Options
menu) the brush draws in 2x2, 3x3, 4x4 or 5x5 pixel brush sizes. 
This tool is used when thick pen width is desired or
when erasing a certain portion of the image.  To draw with the paint
brush, follow the instructions for drawing with the pencil.  The edit
cursor for the paint brush is a paint roller.

:SECTION.Filling Regions
The filling tool is used to fill a specific area with a selected
color.  The fill tool is represented on the tool window with a pouring
can of paint which is also the cursor for the paint tool.  To fill an
area move the cursor to the area that is to be filled.  An area is
considered to be a set of pixels identical in color that are 
adjacent to each other.  Clicking the
mouse button over an area will cause all the pixels in that area to
become the color of the currently selected color.  Note that breaks in
lines of even one pixel will cause the fill to
:ITALICS.leak
through the line.  An area that is a dithered color (see the
:HILITE.Working with Colors
chapter for a definition of dithered color), cannot be filled with another
color.

:SECTION.Hot Spot Tool
The hot spot tool is only displayed on the tool window when the
current image being edited is a cursor.  Moreover, the menu item is
disabled if the the current image is not a cursor.  The hot spot tool
allows the user to specify the coordinates of the hot spot for the
cursor image.  To select a hot spot, select the hot spot tool and
move the cursor to the location in the edit window of the desired hot
spot.  Click the left mouse button down to select that set of
coordinates as the hot spot.  The
:ITALICS.HotSpot
section of the status window will display the coordinates of the
hot spot for the current cursor image.  Holding the mouse button down
and moving the the cursor around has no effect when using the hot
spot tool.  Similarly, the current color selection has no bearing on
the hot spot.
