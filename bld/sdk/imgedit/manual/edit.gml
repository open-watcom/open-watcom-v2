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
:CHAPTER.Edit Features
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
In addition to the various tools,
&imgname provides many very useful edit
features which make image editing quick and easy.  This
chapter discusses these edit features which are found under the
:HILITE.Edit
menu in the menu bar and are displayed on the function bar.
:SECTION.Undo and Redo Operations
The first edit feature that &imgname supplies is a multiple undo and
redo capacity.  The number of operations that &imgname stores is
dependent on the amount of memory available in the current Windows
session, the number of images currently being edited and 
the size of the images being edited.  The storage of large bitmaps
requires more memory than for small images.  Hence the number of
recorded operations for large bitmaps may be less than the number of
operations stored for smaller images.
When &imgname detects that memory is getting low it
discards the oldest operations it has recorded, making room to store
more recent operations.  You can gain a longer operation history by
considering the following rules:
:UL compact.
:LI.close any Windows applications not currently being used as these
expend memory
:LI.have as few images open as possible in &imgname 
:LI.keep the image dimensions small if at all possible
:eUL.
While these rules are useful to bear in mind, the worst that will
happen if they are not followed is that the number of operations
recorded will be less than the total number of operations.  This 
means that you may not be able to undo
:ITALICS.all
operations.
:P.
An operation can be a number of things.  For most of the tools an
operation consists of the time from when the mouse button is pressed
down until it is released.  In general, whenever the image is altered
an operation is recorded.  The exception is the setting of the hot
spot for a cursor.  This operation cannot be undone.  
:P.
To undo an operation select 
:HILITE.Undo
from the 
:HILITE.Edit
menu or hit
:HILITE.Ctrl
and
:HILITE.U
:PERIOD.
Alternatively, you can click on the undo
button in the function bar.  The image will change to the form it
took before the last operation was performed.  You can undo as many
operations as are recorded by &imgname
:PERIOD.
The number of remaining operations will be displayed in the
:ITALICS.hint text
section of the status window.  
:P.
An operation that is undone, can afterwards be redone.  This is
useful if you have changed an image and forget what the change was. 
You are able to undo the operations and then redo them again.  Since
the redo operation undoes an undo operation, the
redo operation can only be performed after an undo operation.  As
soon as an operation is performed
other than an undo, all redo operations
are removed.  As consecutive undo operations are performed, the redo
stack grows.  To redo an undone operation, select
:HILITE.Redo
from the 
:HILITE.Edit
menu or hit
:HILITE.Ctrl
and
:HILITE.R
:PERIOD.
Alternatively, you can click on the redo
button which is beside the undo button on the function bar.  Again,
the
:ITALICS.hint text
will display the number of possible redo operations remaining.

:P.
A final undo operation pertains only to opened files or new files
that have been saved.  The 
:HILITE.Restore
item can be selected from the
:HILITE.Edit
menu to restore an image to its most recently saved
form.  This saves the user
from selecting undo numerous times when much editing has been
performed.  Moreover, if some undo operations get discarded because
of low memory, this function assures the user that the original image can
be restored.

:SECTION.Clipboard Functions
&imgname makes use of the Windows clipboard by allowing users to cut
bitmaps, copy bitmaps and paste bitmaps that exist in the Windows
clipboard into the current image.  The clipboard is consistent
throughout other Windows applications, so, if an application copies a
bitmap to the clipboard, that bitmap is then available to &imgname
:PERIOD.
Hence the clipboard can be thought of as a method for moving bitmaps
between different applications.

:SUBSECT.Cutting and Copying
Cutting and copying are similar operations, both being used in
conjunction with the region selecting tool.  The difference between
the functions is that cutting a region will fill that region with whiteness
and copying a region will leave the image unchanged.  Both functions
copy the selected region to the Windows clipboard. 
To cut a
region, indicate the region to be cut with the region selecting tool.
Select
:HILITE.Cut
from the 
:HILITE.Edit
menu, press
:HILITE.Ctrl-X
or click on the cut button in the function bar shown with the
scissors.  The rectangle that was selected will become white and that
region will be stored in the Windows clipboard.  Selecting
:HILITE.Cut
without a region specified will cut the entire image to the clipboard.
:P.
To copy a selected region, simply select
:HILITE.Copy
from the 
:HILITE.Edit
menu, press
:HILITE.Ctrl-C
or click on the copy button in the function bar.  The rectangle
displaying the selected region will disappear and that region will be
copied to the Windows clipboard; however, the image will not change. 
If a copy is selected without a region specified, the entire image is
copied to the clipboard.  The
:ITALICS.hint text
will display whether the cut or copy was successful.
:eSUBSECT.
:SUBSECT.Pasting from the Clipboard
There are a few variations of pasting images from the
clipboard.  All pasting operations are performed on the current image
being edited.
:P.
The first and most common form of pasting is the normal paste which
one obtains by selecting
:HILITE.Paste
from the 
:HILITE.Edit
menu, pressing
:HILITE.Ctrl-V
key or clicking on the paste button in the function bar.  For the
normal paste, no region should be selected in the current edit
window.  Once the paste operation is selected, the cursor changes to
a pointer and moving it around the edit window shows an outline of
the bitmap to be pasted into the current image.  Moreover, the
:ITALICS.hint text
section of the status window states the dimensions of the bitmap in
the clipboard.  Move the cursor to the position at which the bitmap
should be pasted and click the left mouse button.  This pastes the
bitmap in the position selected over the existing image.  If the
normal paste is selected and you wish to cancel it, you can hit the
:HILITE.ESC
key.
:P.
The second method of pasting involves selecting a region first with
the region selecting tool.  This allows you the flexibility to first
choose the location and size of the pasted bitmap.  Once a region has
been selected, the result of selecting the paste operation depends on
the current settings of the editing session.
&imgname can be set to stretch the
bitmap into the region or clip the bitmap into the region.  The preference
can be set in the
:HILITE.Current Settings
window under the
:HILITE.Options
menu (see
:HILITE.Image Pasting
in the
:HILITE.Options
chapter).
If &imgname is set to stretch the image into the region,
the paste operation will either stretch the
clipboard bitmap into the region specified or compress it into the
region.
While stretching small bitmaps into large regions is usually
no problem, attempting to compress very large bitmaps into small
regions will often produce a less than accurate likeness of the
clipboard bitmap.  If the settings for the current session are set to
clip the bitmap into the selected area, the image will be copied into
the selected region.  If the region is smaller than the bitmap, the
bitmap dimensions will be truncated to fit inside the region.  If the
region is larger than the clipboard bitmap, the extra area will be
filled with whiteness.
:P.
You may wonder what the results would be of pasting a color bitmap
into a black and white image.  Try it.  You will see that &imgname
translates the color image into a monochrome one.  &imgname
translates white pixels in the clipboard bitmap to white pixels in
your image, and any other color gets translated to black.
:P.
If you are editing an icon or cursor you may still copy bitmaps from
the clipboard into your image.  If the bitmap in the clipboard has been
copied from another icon or cursor in &imgname, the screen and inverse
colors will be preserved.  However, if the bitmap has been put into the
clipboard by another application, no screen colors are assumed to be
in the bitmap.  If a portion of an icon or cursor containing screen
colors is placed into the clipboard and then pasted into a bitmap, the
screen colors will be interpreted as normal solid colors in the bitmap.
:eSUBSECT.

:SECTION.Transformations on Images
&imgname allows the basic geometric transformations of shifting,
rotating and reflecting.  These operations can alter the entire image
or only a selected region.  
:SUBSECT.Shifting an Image
One can shift an image a specified number of pixels in any of four
directions: up, down, left or right.  The number of pixels the image
is shifted can be changed by choosing the
:HILITE.Current Settings
item from the
:HILITE.Options
menu.  The user can also decide whether the shift should fill the vacant
space with whiteness or whether the shift should wrap around to the other
side.  This choice too is made in the
Current Settings dialog.
The four arrow buttons on the function bar represent the
shift operation in the specified direction.  If you prefer to use the
keyboard, the same results can be obtained by holding
:HILITE.Shift
and the arrow key that indicates the direction you wish to shift the
image.  You can also find the four directions listed under
the
:HILITE.Shift
item in the
:HILITE.Edit
menu.
:eSUBSECT.
:SUBSECT.Flipping an Image
An image can also be reflected along either the horizontal or
vertical axis.  If a region is selected with the selection tool, only
this portion of the image will be reflected.  To flip the image
click on one of the flip buttons in the function bar.  Alternatively,
you may choose the desired axis under the
:HILITE.Flip
item in the
:HILITE.Edit
menu.  
:eSUBSECT.
:SUBSECT.Image Rotation
&imgname also supports the rotation of an image by 90 degrees in
either the clockwise or counter clockwise direction.  Since &imgname
is not targeted for use on very large bitmaps, it should be noted
that attempting to rotate a very large image may take a considerable
amount of time.  The
:ITALICS.hint text
section of the status window indicates the percentage of completion
for the rotation.  There are three rotation types and you can choose
the type you wish to use by selecting it from the
Current Settings
dialog.  The difference between these rotations only becomes apparent
when the region for rotating is not perfectly square.
:P.
The first type of rotation is a
:ITALICS.simple
rotation.  This type of rotation will rotate the image around the
center of the selected area, only clipping the region if necessary. 
If the region is not perfectly square, part of the previous location
of the selected area will be filled with whiteness.  If you are
rotating only a selected region, the selected region will also change
to be consistent with the rotation.  This is the
default type of rotation.
:P.
The second type of rotation could be called a 
:ITALICS.stretch
rotation.  When this type of rotation is selected from the Current
Settings dialog, the selected area is rotated and then stretched or
compressed to fit into the same selected area.  The image lying outside
the selected region will never be altered and the area inside the region
will never be clipped or filled with whiteness.
:P.
The final rotation type is a
:ITALICS.clipping
type of rotation.  When an area is selected for rotation, this final
type of rotation assures that nothing outside of this area will be
altered, while not distorting the image in any way.
If the region selected is not square, the area inside the region
is clipped to fit into that region.
This type of rotation will not stretch or compress the
bitmap in any way.
:eSUBSECT.
It is best when rotating to try the different types of rotation -
using undo when necessary - to find out which type is best suited
for your situation.  Once again, it should be noted that if the
region to be rotated is a perfect square, all these rotation types
will have the same effect.
:P.
To rotate a region either click on the desired rotation button on the
function bar or select the rotation direction from the 
:HILITE.Rotate
item in the
:HILITE.Edit
menu.  The Current Settings dialog contains a check box indicating
whether or not the region should remain selected after a rotation. 
If this box is checked, the area you rotate will remain selected
after it is rotated.  If it is not checked, the selected region will
no longer remain highlighted after a rotation.
:SECTION.Other Image Alterations
Two other image functions need to be described.  These alterations can
affect the entire image or simply a selected area.

:SUBSECT.Clearing an Image
&imgname allows the user to clear the image (erase the contents of the
image window).  To clear the entire image select
:HILITE.Clear
from the 
:HILITE.Edit
menu.  You can also hit the
:HILITE.Delete
key or click on the clear button in the function bar.  To clear just
a portion of the image, select the region to clear with the region
selection tool.  When the region you have selected is correct,
proceed to click on the clear button, hit the
:HILITE.Delete
key or choose
:HILITE.Clear
from the 
:HILITE.Edit
menu.
:eSUBSECT.
:SUBSECT.Capturing from the Screen
Another useful edit function is the snapping utility.  This function
allows you to snatch images from the desktop and bring them into your
edit window as part of your image.  To use this function, click on
the snap button in the function bar or choose
:HILITE.Snap
from the
:HILITE.Edit 
menu.  Upon doing so, &imgname will minimize itself and you will see
a rectangle on the screen which you may move around with the mouse. 
This rectangle represents the size of the image you are snapping.  Move
the rectangle so that it covers the image you wish to snap and click
on the left mouse button.  &imgname will return with the area under
the rectangle enlarged in the edit window.  If you are editing a
monochrome image you may still snap from the desktop.  However, the
results may not be what you expect.  As with pasting, the snap
utility will translate all colors other than white into black
pixels.  White pixels remain white.  Hence,
an image with many colors may appear quite different when converted
into a monochrome image.
:P.
You can also restrict the snap function to a region which you select. 
By selecting a region first and then selecting the snap function the
image you capture will fill only the region you have selected.  As
with the rotation, the region you have selected will remain displayed
if you have the
:ITALICS.keep region selected
item checked in your Current Settings dialog.
