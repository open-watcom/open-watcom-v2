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
.* 07-may-93	Paul Fast   	initial draft
.*
:CHAPTER id='basic'.Basic Usage
:SECTION.Invoking &imgname
&imgname is invoked by double clicking on the &imgname icon.  No
command line parameters are used.

:SECTION.Getting Help
Before beginning, it is useful to know where help can be found.
The easiest way to get help is to look in the status window at the
section displaying the hint text.  
When selecting any menu option the
hint text gives a brief description of the results of selecting that
option.  Furthermore, information about any buttons can be shown
by moving the cursor over the desired button and holding the mouse button
down.  The operation will not be performed until the mouse button is
released and only if it is released over the function button.  If the
cursor is moved off the function button before the mouse button is
released, the button will pop back up without performing the
operation.
:P.
If you have not already done so, now would be a good time to explore
some of the options available to you by highlighting some of the menu
options or clicking some of the buttons and reading the displayed
hint text.

:SECTION.Getting Started
If you are familiar with any paint programs then getting
started with &imgname will be easy since there are many
similarities between &imgname and most paint programs.  If you
are not familiar with these types of programs, you will be very soon.
:P.
To begin with, let us create a new image.  Follow these steps:
:UL.
:LI.Click on the 
:ITALICS.new
button located on the far left of the function bar.
:LI.After doing so you will be presented with a dialog box such as
the following:
.snap 'selimg' '1.27' 'Dialog allowing the user to select the type of image to create.'
At this point you must indicate the type of
image you wish to create.  For now, just use the default which is a bitmap
image and click on the 
:HILITE.OK
button.  
:LI.At this point another dialog box will appear:
.snap 'selbmp' '1.48' 'Dialog allowing the user to specify the bitmap information.'
This is the place to specify
the size of the bitmap to create and the number of
colors in the bitmap.  Again, just use the default to create a
bitmap that is 32 pixels by 32 pixels and has 16 colors.  Click on the
:HILITE.OK
button.
:eUL.
You should now see the edit window in the top left corner of the main
window and the view window will appear on the right of the screen. 
Both are initially white to begin with and the edit window will
contain a grid.  The status window will show that a new image has
been created and that this is a 32 by 32 pixel bitmap with 16
colors.  You can change the size and location of the edit window to
whatever suits you.  The initial configuration ensures that the grid
will always remain square.  Hence, even if you attempt to make the window
rectangular, the window will always resize itself so that the grid is
square.  You can change this configuration later.
:P.
If you move the cursor around the edit window, you will notice that
the cursor looks different.  Depending on the tool selected
from the tool window, the cursor will change to represent that tool's
operation.  For example, if 
the pencil is selected, the cursor will also look like
a pencil.  Different tools will have different cursors. 
:P.
Before drawing, we need to select a tool to use.  
If the pencil button is not pressed on the tool window, click
that button to select the pencil as the drawing tool.  
:P.
To choose a color with which to draw, move to the color
palette and click the left mouse button on the desired color.  You can
do the same with the right mouse button.  When selecting a color,
you should notice the color of the box with the 
:HILITE.L 
change to the
selected color.  Similarly the color of the right box will change
when selecting a color with the right mouse button.
:P.
Now move to the edit window and press either the right or left
button.  That square in the edit window will change to the selected
color of the mouse button you clicked.  Notice that drawing with
white on a white background does not show a difference in the image
appearance.
Simultaneously, the view
window will display the pixel corresponding to that point with the
selected color.  You can experiment by holding down a mouse button
and moving around.  The results of what you do in the edit window
will appear in the view window.
:P.
You can experiment with the pencil and different colors or try other
tools.  The other tools will be described in a later chapter.  When
you are finished you can save the file by clicking on the 
save button.  Doing so will cause a dialog box similar to the following
to appear:
.snap 'save' '1.85' 'Dialog box to indicate the name of the file to be saved.'
At this time you must decide on a filename for the bitmap.  Enter 
the name of the file in the
:HILITE File Name
box and be sure that the directory specified is correct.
:P.
After the file is saved the title of the edit window will be set to
the name of the file you specified.  To exit &imgname you can choose
:HILITE.Exit
from the 
:HILITE.File
menu.  &imgname will always prompt you to save any images that are
not saved when you try to quit.  In this case, you have just saved
the file so it will not ask you if you want to save it again.  If you
edit the image after you have saved it, you will notice that an asterisk
(*) is placed beside the name of the file in the title of the edit
window.  This is simply to let you know that you have made changes to
the image.  
:P.
A feature worth mentioning here is the undo operation.
The undo
button will cause the last operation performed to become undone. 
This can be performed multiple times.  To redo an undone operation
click on the redo
button.  This should allow you the freedom to experiment with the
various operations without having to worry about causing irreparable
damage to your bitmap.
:P.
If you have mastered the above steps, then using the &imgname should 
be no problem
to you.  
