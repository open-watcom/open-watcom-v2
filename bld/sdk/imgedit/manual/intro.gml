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
.* 04-may-93	Paul Fast	initial draft
.*
:CHAPTER.Introduction
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
The &imgname is a tool used by Windows programmers to design bitmaps,
icons and cursors.  Windows programmers must often use bitmaps to give a
graphical representation of an operation, icons to represent the
execution of commands and cursors to accentuate distinct windows in
their programs.  The &imgname provides an easy and very powerful method of
producing these images.  There are many
similarities between &imgname and other paint programs;
however, &imgname is not intended as a paint tool.  Instead,
its target use is as a developer's tool and hence it is not meant to
edit very large bitmaps.  While it does not excel at editing large
bitmaps, it is still capable of doing so.

:P.
The design philosophy of the &imgname was to give the user as much
flexibility as possible, providing as many functions as possible and
as few limitations as necessary.
:P.
Some of the significant features of &imgname are:
:UL compact.
:LI.allows editing of several images at one time
:LI.unlimited undo and redo capability
:LI.maximum bitmap size of 512 pixels by 512 pixels
:LI.fully sizeable edit windows
:LI.many editing functions such as:
:UL compact.
:LI.Cut, Copy and Paste
:LI.Image shifting
:LI.Snap image from the desktop
:LI.Reflection over horizontal or vertical axis
:LI.Rotation by 90 degrees
:eUL.
:LI.fully configurable:
:UL compact.
:LI.Pasting can clip into area or stretch into area
:LI.Edit windows can remain square
:LI.All view windows can be displayed, or only the current one
:LI.Tool window and color palette can be individually displayed or hidden
:eUL.
:LI.on line help
:eUL.
:P.
&imgname is available for the following environments:
:UL.
:LI.Windows 3.x 
:LI.Windows NT
:LI.Win 32s
:eUL.
:SECTION.Terms and Notation
:SUBSECT.The Mouse
A mouse is strongly recommended for use of &imgname..  While most
operations are attainable through key sequences, a mouse will be most
useful.
:P.
These terms are used when referring to things that may be done with
the mouse.
:DEFLIST.
:DEFITEM.click
Pressing a mouse button once.
:DEFITEM.double click
Pressing a mouse button twice in rapid succession.
:DEFITEM.dragging
Holding down a mouse button and then moving the mouse.
:eDEFLIST.
:eSUBSECT.
:SUBSECT.The Screen
When you start up &imgname, you will see something like the following:
.snap 'iestrt1' '3.53' 'WATCOM Image Editor on start up.'
The &imgname screen has several windows.
:UL.
:LI.The
:KEYWORD.main window
:CONT.
has the title 
:HILITE.WATCOM Image Editor
and represents the area in which
the image will be prepared.  The main window can be moved and sized
to the preference of the user.
:LI.The top line of the main window
is the
:KEYWORD.menu bar
:CONT.,
which contains all the menu choices.
:LI.The bar directly below the menu bar is the 
:KEYWORD.function bar
:PERIOD.
The function bar provides easy access to the most commonly used edit
features found under the menu options.  Each button on the function bar
represents a different operation.
:LI.At the bottom of the main window is the
:KEYWORD.status window
:PERIOD.
This window is divided into five sections with each section showing
different information about the current edit session.  The first
section is the
:KEYWORD.position
section and indicates the coordinates of the cursor in the edit window
(described below).  The second section is the
:KEYWORD.size
section and indicates the last size of a
shape being created.  The 
:KEYWORD.hotspot
section is the third section and it specifies the coordinates of
the hot spot for cursors.  If the image being edited is not a cursor,
this section remains blank.  The fourth section describes the image
type (bitmap, icon or cursor) and the dimensions of the image (in
pixels).  This is the
:KEYWORD.description
section.  The final section provides help messages and information on
various operations.  Both the information and this section are called
:KEYWORD.hint text
:PERIOD.
:LI.On the right is a long narrow window with numerous buttons on it
called the
:KEYWORD.tool window
:PERIOD.
While editing an image, the user can use the tools indicated in the
tool window to make operations such as drawing lines and circles much
easier.  The tool window is moveable and can be placed anywhere on
the screen.  The button that is down indicates the currently selected
tool.
:LI.Near the bottom is another moveable window which displays a
number of colored boxes.  This is the
:KEYWORD.color palette
and represents the location at which colors are selected and edited.
A highlighted box displays the current color selected for the left
mouse button (marked by 
:HILITE.L
:CONT.) and for the right mouse button (marked
by 
:HILITE.R
:CONT.)
:PERIOD.
The block of colors next to the current color
selection box represent the 
colors available to the user.
:eUL.
:P.
There are two more windows that need to be described; however they
do not appear until editing is about to begin.  When an image is
being editing, the screen will look something like:
.snap 'iestrt2' '3.53' 'Editing an image with WATCOM Image Editor.'
:UL.
:LI.The large window with the grid is the
:KEYWORD.edit window
:CONT.
and is where the user makes all modifications to the image being edited.
Essentially, the edit window contains an enlarged version of the
actual image being composed.  The caption of the edit window will
display the name of the file being edited if the file was opened.
Otherwise it will be given the caption
:ITALICS.(Untitled)(x)
where
:ITALICS.x
will be some number.
:LI.The final small window is the
:KEYWORD.view window
:CONT.
and contains the image in its actual size.
:eUL.
:P.
Both the edit window and the view window are moveable.  The view
window can be moved anywhere on the screen and the edit window can be
placed anywhere inside the area of the main window.  The edit window
can also be sized to the preference of the user.  
:UL.
:LI.The main window contains 
:KEYWORD.maximize
and
:KEYWORD.minimize
buttons at its top right and the edit window contains a minimize
button on its upper right corner.  The user can minimize the current
image to keep the main window looking clean or minimize the entire
application by clicking on the minimize buttons.  Minimizing an edit
window will produce a different icon depending on the type of image
being edited.  The user
can enlarge &imgname so that it occupies the entire screen by clicking
on the maximize button.
:LI.All the moveable windows have captions called
:KEYWORD.system menu boxes
at their upper left corners, which contain options for closing and
moving the window among other possible options.  Clicking on them
once displays the system menu and clicking on them twice closes
that particular window.
:eUL.      
:eSUBSECT.

:SUBSECT.Editing Terms
&imgname allows you to have a number of images open at one time. 
However there is only one view window (this can be changed later)
displaying the
:KEYWORD.current
or 
:KEYWORD.active
image.  The current image is the image currently being altered or
modified.  It is important to know which is
the current image since many of the
editing features (such as pasting or rotating)
take place on the current image.
The current image can be changed by clicking on the edit window of
the image you wish to become current or by selecting the image name
from the
:HILITE.Windows
menu.
:P.
Many edit operations can take place on the entire image or only a
:KEYWORD.selected region
:PERIOD.
When refered to in this manual, a selected region can be either the
entire edit area of the image or it can be a region specifed by the 
region selecting tool described at a later time.
:eSUBSECT.
:SUBSECT.The Function Bar
Throughout this manual the buttons on the function bar will be
referred to.  The function bar has the following appearance:
.snap 'funcbar' '0.34' 'WATCOM Image Editor Function Bar.'
and is always visible under the menu bar.  The buttons are displayed
in groups which deal with similar operations.  The following list gives the
names of the buttons from left to right and the operation they perform:
:UL.
:LI.The 
:KEYWORD.new
button creates a new image.
:LI.The
:KEYWORD.open
button opens an existing image.
:LI.The
:KEYWORD.save
button saves the current image.  If the file is named, it is
saved under the current file name.  If it is untitled, &imgname
requests that a filename be supplied.
:LI.The 
:KEYWORD.grid
button toggles the display of the grid in the edit window.  If the
button is down, the grid is displayed (this is the default). 
Clicking on the button will pop it back up and cause the grid to
disappear.
:LI.The
:KEYWORD.edit window maximize
button enlarges the current edit window to its maximum size, relative to
the main window.
:LI.The next three buttons perform clipboard operations.  The first
button is the
:KEYWORD.cut
button.  It cuts a region from the image and stores it in the
clipboard.
:LI.The 
:KEYWORD.copy 
button copies the selected region in the image to the clipboard.
:LI.The
:KEYWORD.paste
button allows the user to paste the bitmap in the clipboard into the
current image.  If there is no bitmap in the clipboard, clicking this
button will do nothing.
:LI.The next button is the
:KEYWORD.undo
button and represents an undo operation.  If there are no operations
to undo, this button does nothing.
:LI.Beside the undo button is the
:KEYWORD.redo
button.  Clicking on this button will redo an operation that has been
undone.  If no operations have been undone, this button does nothing.
:LI.The 
:KEYWORD.clear
button clears the entire image or a selected region of it.
:LI.Next to the clear button is the
:KEYWORD.snap
button.  Clicking this button will allow a user to grab bitmaps from
the desktop and bring them into the current image.
:LI.The next four buttons represent shift operations.  The first is
the
:KEYWORD.shift right
button which shifts the image a specified number of pixels to the
right.
:LI.The
:KEYWORD.shift left
button shifts the image to the left.
:LI.The
:KEYWORD.shift up
button shifts the image a specified number of pixels up.
:LI.The
:KEYWORD.shift down
button shifts the image down.
:LI.The next button is the
:KEYWORD.horizontal flip
button and it reflects a region in its horizontal axis.
:LI.The
:KEYWORD.vertical flip
button reflects a region in its vertical axis.
:LI.The
:KEYWORD.clockwise rotate
button rotates an image or selected region ninety degrees in the 
clockwise direction.  The rotation is performed about the center of
the region.
:LI.The last button is the
:KEYWORD.counter clockwise rotate
button.  Clicking this button rotates the image or region
ninety degrees in the counter
clockwise direction.  As with the clockwise rotation, the
rotation is performed about the center of the region.
:eUL.
A description of what each button does is displayed in the hint text
section of the status window when the left mouse button is pressed over
the button.
Releasing the mouse button after moving the cursor
off the function button will release the function button without
performing the operation.
:eSUBSECT.
:P.
For those of you without the patience to go through the
manual from cover to cover, the next section will give you enough
information to begin editing right away!
