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
.* 12-may-93	Paul Fast	initial draft
.*
:CHAPTER.File Operations
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
All file operations can be achieved through the
:HILITE.File
menu on the menu bar.  In addition to using the menu items, most of the
operations can be attained through key sequences or are represented by
buttons on the function bar.
:SECTION.Starting an Image
&imgname allows you to edit many images at one time.  Hence you are not
required to save and close an image before you begin editing another.  You
may perform any of the following
:ITALICS.new
or
:ITALICS.open
operations in succession,
without the restriction of closing other edit windows.
:SUBSECT.Creating a New Image
There are three ways to begin a new image.  The first is to select
:HILITE.New
from the
:HILITE.File
menu.  The second method is to hit
:HILITE.F2
and the final way is to click on the new
button in the function bar.
All methods will cause a dialog to appear requesting
that the type of image to create be indicated.  Once the image type is
selected, clicking the
:HILITE.OK
button will prompt you for more specific information.
:P.
If you are creating a bitmap image, a dialog box will appear prompting you
for the size of the image in pixels and the color scheme.  Both the width
and the height can range from one to 512 pixels.  The bitmap must be
either a two color (monochrome) bitmap or a 16 color bitmap.
Clicking
:HILITE.OK
will allow you to begin editing the bitmap.  As an alternative to entering
the size and color scheme, you may click on the
:HILITE.Screen Select
button.  This will cause &imgname to ignore the values for the width,
height and color scheme and allow the user to dynamically select a bitmap
to begin editing from the desktop.  Move the cross hairs cursor to the
point at which you want to begin selecting.  Then hold the left mouse button
down and move the cursor to the end point of selection.  The currently
selected area will be highlighted by a rectangle.  When you have selected the
area you wish to edit, release the mouse button.  &imgname will reappear
with the area you have selected in the edit window.  You can find out the
dimensions and color scheme of the bitmap by looking in the status window.
Since there is an upperbound on the size of bitmaps &imgname allows you
to edit, you will not be able to select a region larger than 512x512
pixels.
:P.
If you are creating an icon, you will be presented with a dialog box
containing a list box.  This box will list the dimensions and
color schemes of the icons you may create.
Windows uses 32x16 pixel
monochrome icons on CGA video adapters and 32x32 pixel monochrome icons
for other adapters (such as
EGA, VGA and 8514/A) running in monochrome mode.
For non-CGA adapters running
in color mode, Windows uses 32x32 pixel 16 color icons.
Determine the type of icon
you wish to edit and select that entry from the list box.  You will then
be presented with the edit window at which point editing may begin.
:P.
The final image type you may create is a cursor.  As with the icon selection,
selecting a cursor image to create will cause a dialog box to appear
requesting more information.  The type of cursor you may edit depends on
the graphics capabilities of the machine on which &imgname is being run.
The list box in the dialog box will contain the possible combinations of
cursor dimensions and color schemes you are allowed to edit.  Hence, once
the type of cursor to create is determined, you may select that type from
the list box.  Doing so will present you with the edit window, at which point
you may commence editing.
:eSUBSECT.
:SUBSECT.Editing an Existing Image
Instead of creating a new image, you may also edit an existing image.  To
do this you must select
:HILITE.Open
from the file menu or hit
:HILITE.Shift
and
:HILITE.F4
or click on the open button on the function bar.  
To open an image, you must give
the name of the file including the directory in which it can be
found.  The following dialog box displays this.
.snap 'open' '1.85' 'Dialog box to specify the name of the file to open.'
Different file types and their extensions are listed in a list box at the
bottom of the open dialog box.
The list box on the left lists the files in the
current directory matching the extension given.  The current
directory is displayed graphically on the right of the dialog box. 
Once you have chosen the image to open click on the
:HILITE.OK
button and the image will be displayed in the
view window and enlarged in the edit window.  
:P.
There are restrictions on the kinds of files that can be
opened.  The file must be in a valid Windows 3.x file format for
either bitmaps, icons or cursors and must have either two or 16 colors.
For bitmaps, the dimensions must be less than
or equal to 512x512 pixels.  If you attempt to open an image which
does not meet these specifications, an error message will appear
informing you why the image could not be opened.  Once an image is
opened, the status window will
contain information on the type of image, the dimensions of the image 
and the number of colors it has.
:eSUBSECT.
:SECTION.Saving Images
Once you are ready to save an image you can use the 
:HILITE.Save
or
:HILITE.Save As
options found in the
:HILITE.File
menu.  The difference between the two save operations is that
:HILITE.Save
assumes that the image is already named and the 
:HILITE.Save As
operation lets you specify a filename for the image.  The save
button on the function bar represents the 
:HILITE.Save
function.  There is no button to represent the 
:HILITE.Save As 
operation.
:P.
If the file you are editing is named (that is, the edit window
contains a name other than 
:ITALICS.(Untitled)(x)
where
:ITALICS x
is some number) then choosing
:HILITE.Save
:CONT.,
hitting 
:HILITE.F3
or clicking on the save button in the function bar will save the file
with its current name.  If the image you are editing is not named or
you wish to save the image to another file, you should choose the
:HILITE.Save As
item from the
:HILITE.File
menu or hold
:HILITE.Shift
and press
:HILITE.F3
:PERIOD.
Doing so, will present you with a dialog box similar to the open
dialog box, in which you can specify the directory for the file and
give it a filename.  If you specify a name that already exists, you
will be warned and asked to confirm the replacement of the old file.
:P.
If you click on the save button in the function bar and the
image you are editing is not named, &imgname will assume a
:HILITE.Save As
operation and will present you with a dialog box asking for the name of
the file to save.  Hence the only time you need to explicitly select the
:HILITE.Save As
item from the
:HILITE.File
menu is when you specifically want to save an existing file under a
different name.  If you fail to give the file
an extension when saving the image,
&imgname will append the default extension for the image
to the filename.

:SECTION.Closing Images
Since &imgname allows you to have a number of images open at the same
time, you must explicitly close an image when you are finished
editing it.  There are a variety of ways to close an image.  Choosing
:HILITE.Close
from the 
:HILITE.File
menu is the most obvious way to close the current 
image being edited.  Other ways to close an image are to double click on the 
system menu button in the top left corner of the edit window
or select 
:HILITE.Close 
from the system menu of the edit window you wish to close.  If the
image you wish to close is not saved, &imgname. will ask if you want
to save the file before closing it.  You will easily be able to determine
whether a file has been edited since it was last saved  by looking at
the title in the edit window.  If an asterisk (*) appears after the
filename, it is not saved. 
:P.
Choosing
:HILITE.Close All
from the
:HILITE.File
menu will close all of the images currently being edited.  If any of
the images are not saved, &imgname. will ask you to confirm the
closing of that image.  At this point, you may save the file or close
it without saving it first.  
:SECTION.Exiting &imgname.
To exit &imgname you can select 
:HILITE.Exit
from the 
:HILITE.File
menu.  This will close all the images once again asking you to
confirm the closing of any images that are not saved.  Once all
images are closed, &imgname. itself closes.  You can also exit
&imgname. by double clicking on the system menu box for the main
window or selecting
:HILITE.Close
from the system menu of the main window.

