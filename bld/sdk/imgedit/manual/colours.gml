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
.* 13-may-93	Paul Fast	initial draft
.*
:CHAPTER.Working with Colors
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
When using a graphically intensive tool such as &imgname. it is
important to understand something about how colors work in Windows.
This chapter
looks at selecting different colors, using screen and inverse
colors, editing colors and saving, loading and restoring color
palettes.  
:P.
Windows stores colors as
:ITALICS.RGB values
:PERIOD.
An RGB value is a three-tuple value, indicating the intensity of red,
green and blue in the color.  Each value in the three-tuple can
range from zero to 255.  For example: an RGB value of (0, 0, 0)
represents black, an RGB value of (255, 0, 0) represents bright red
and an RGB value of (255, 255, 255) represents white.  Under this
scheme, there are almost seventeen million colors representable. 
Unfortunately, most display drivers cannot display that many colors
so Windows resorts to
:ITALICS.dithering
colors.  Dithering is a process whereby Windows simulates a color
by combining pixels of two or more pure colors.  A pure color is a
color that Windows can display without dithering (such as black or
white).  A pixel can only be set to a pure color.  However, by
dithering a number of pixels an area can appear to be almost any
color.
:SECTION.The &imgname Color Palette
:P.
In &imgname, all color operations take place in the color palette window
which looks like the following:
.snap 'clrpal' '0.70' 'WATCOM Image Editor Color Palette.'
This window can be moved around anywhere on the screen.  To hide this
window, you can select the
:HILITE.Show Color Palette
from the
:HILITE.Options
menu, hit
:HILITE.F7
or double click on the system menu box at the upper left of the
color palette.  Colors can only be selected and edited if the
color palette is visible.
:P.
A highlighted box in the left  side of the color palette window
displays the current color selection for drawing.  Inside the
highlighted box are two labels:
:HILITE.Draw
and
:HILITE.Fill
:PERIOD.
The difference between the colors specified in these two boxes is
that the color indicated in the 
:HILITE.Draw
box is always a pure color and the color given in the 
:HILITE.Fill
box can be a dithered color.  The fill color will always be the color
that was selected.  The draw color will be the pure color that most
accurately represents the fill color.  &imgname uses this color
when performing drawing
operations such as using the pencil, drawing lines,
outlined rectangles, outlined ellipses and using the paint brush. 
Since these operations deal with individual pixels instead of an
area of pixels, they must be performed with a pure color.  The color
given under the
:HILITE.Fill
label will be used when performing a fill, drawing solid rectangles
and solid ellipses.  These operations work with an entire area of
pixels and hence, can use the dithered colors.  The user should exercise
caution when filling a region in a large bitmap with a dithered color.
Once filled with a dithered color, the same region cannot be filled with
another color using the fill
tool.  This result is due to the dithered nature of the region and the
fact that a fill acts only on solid colors.
:P.
On the right side of the color palette is a display of the colors
available for use.
To select different colors, move the cursor to the 
available colors.  The
cursor will change into a hand and you will be allowed to pick a
color.  If you wish to choose a color for the left mouse button,
click the left mouse button and if you wish to choose a color for
the right button, click the right button.  As you click a button, you
will see the current colors box reflect the new color selection you
have made.  If the color selected is a dithered color, the 
:HILITE.Draw
and
:HILITE.Fill
colors will be different.  Otherwise they will be exactly the same. 
Under the default settings, the pure colors reside on the left
side of the available colors and the dithered colors on the right. 
This may change as you edit colors.
:P.
When editing a two color bitmap,
black and white are the only pure colors available.  Hence, the
remaining available colors are gray scales made up of dithered black
and white pixels.

:SECTION.Editing Colors
&imgname allows you to edit the colors available for drawing if you are
not editing a monochrome bitmap.
If the machine executing &imgname is capable of displaying 256
colors or more, you will be able to choose different solid colors
as well as dithered colors.  If &imgname is running on a machine
capable of only displaying 16 colors or less, you will be restricted
to adding dithered colors to the palette.
:P.
To edit a color, move to that color in the display of the available
colors.  Double click the left mouse button or select that color
as the left mouse color and choose
:HILITE.Edit Current Color
from the 
:HILITE.Palette
menu.  You will be
presented with a window similar to the following:
.snap 'clredit' '2.40' 'Edit the currently selected color.'
This window allows you to create custom colors.  The custom color
boxes initially contain various gray scales.  To customize a color,
first select a box from the
:HILITE.Custom Colors
:PERIOD.
The dithered and solid colors are displayed on the right in the
:HILITE.Color/Solid
box.  You may now edit this color by dragging the
cross-hairs around the large dithered color box and sliding the gray scale
indicator on the far right of the dialog box
up and down the gray scale.  As you change these values,
the color will change and the RGB value will be displayed on the far
right of the window.  When you have established the new color, click the
left mouse button on the 
:HILITE.Add to Custom Colors
button.  The display of custom colors will show the new color you
have added.
:P.
You may add up to 16 custom colors by simply repeating the process
for the other custom color boxes.  Once you have added all the
colors you wish, select the custom color you would like to have
replace the current color in the &imgname color palette with the
left mouse button.  Clicking on the 
:HILITE.OK
button will exit the color window and you will see that the color
selected with the left mouse button will be the custom color created
in the previous window.  You can repeat this process many times in
order to tailor the color palette to your liking.  If at any time you
decide you want to discard all of the color editing you have done,
you can choose
:HILITE.Reset Current Palette
from the
:HILITE.Palette
menu.  Doing so will return the color palette back to its original
state.  The custom colors you have created will remain stored until
you exit from &imgname:PERIOD.
:SECTION.Screen and Inverse Colors
When editing icons or cursors, two colors are added to the color
palette.  These colors represent the color of the screen
(or background) and the
inverse color of the screen.  The two colors are often collectively
referred to as screen colors.  Since it is often desireable to have 
icons and cursors take on shapes other than rectangular, the edges of
the icon or cursor image can take on the color of the screen over which
they are displayed.  Moreover, to give emphasis to parts of icons or
cursors or to guarantee their visibility on any screen color, you
can choose to draw with the inverse of the screen color.  To do so
with &imgname, you simply select either the screen color or the
inverse color as the current color.  
:P.
Initially, the screen color is set to be white and the inverse
color black.  You may change this by either double clicking on the
screen or inverse color with the left mouse button, or choosing
:HILITE.Set Screen Color
from the
:HILITE.Palette
menu.  You will then be presented with a dialog box displaying the current
screen and inverse color and the colors you may choose from for the
new color representations:
.snap 'setscrn' '1.29' 'Choose the color to represent the screen.'
This dialog box only lets you select the
background color only.
The inverse color will always be set to the inverse
of the color you have selected.  You may choose different
screen colors by clicking on them with the left mouse button.  As
you select different colors that color and its inverse are
displayed as the currently selected screen colors.  Click on the 
:HILITE.OK
button when you have established the colors to represent the screen
and inverse colors.  The new screen and inverse colors are displayed
in the color palette and if editing with the screen colors has
already taken place, the edit windows and view windows will reflect
the newly chosen screen colors.
:P.
For the best results, you should choose colors to represent the
screen and inverse that do not appear predominantly in the image you
are editing.  This avoids confusion between which pixel is a true color
and which is a screen color.  For example, if you choose red as the screen
color, it is still possible to have red in the image you are
editing.  However, this can become confusing so it is safest to
choose another color to represent the screen.  If you are editing a
monochrome image and your machine displays more than two colors, it
is possible (and in fact recommended) to choose a color other than
black or white to represent the screen.
:SECTION.Loading, Saving and Restoring Color Palettes
To make image editing easier and more consistent,
&imgname allows users to save and load color palettes from files.
A color palette file has default file extension .PAL
and consists of a file header followed by 28 RGB values.  Palette files
have a standard Windows format and hence can be loaded from other Windows
applications or by other Windows applications.
:P.
If you have edited the color palette and prefer the palette you have
created over the default palette at all times you can save it and load it
again when you begin &imgname
:PERIOD.
To save the current color palette, choose
:HILITE.Save Color Palette
from the
:HILITE.Palette
menu.  You will be presented with a save dialog box similar to the
one used for saving images.  The default file extension of .PAL
is automatically appended to the filename if an extension is not
given.  The palette file you have saved will then be available to other
Windows applications that use color palettes such as Windows Paint
Brush.
:P.
In order to load a color palette to replace the existing one, choose
:HILITE.Load Color Palette
from the 
:HILITE.Palette
menu.  Again, you will be presented with a dialog box requesting the
filename of the palette to load.  Once this is specified, &imgname
will read the file and replace the existing palette in the color
palette window with the new palette.  You may still edit the colors
in the current palette with the method described above.  When
resetting the color palette from the 
:HILITE.Palette
menu, the palette will return to its last loaded form.  To return the
color palette to the default palette of &imgname, choose
:HILITE.Restore Color Palette
from the
:HILITE.Palette
menu.
