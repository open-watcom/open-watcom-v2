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
.* 10-aug-92	Craig Eisler	initial draft
.*
:CHAPTER.Introduction
.if &ishelp eq 1 .do begin
:SECTION.Overview
.do end
&edname is a programmer's editor.  It is
loosely based on UNIX VI, but is much more powerful.
This manual may be used by someone without any knowledge
of VI, or by an experienced VI user.
People familiar with VI will find &edname is very similar
in its basic behaviour.
:P.
An editor is very personal thing.  Every person uses an editor
in a different way, or wants things to look slightly different.
&edname was designed with this in mind.  Most features in &edname
are configurable.
:P.
&edname has many powerful features.  Some of the more significant are:
:UL compact.
:LI.fully configurable
:UL compact.
:LI.The size of every window can be set by the user
:LI.The color of every window can be set by the user
:LI.A window's border can be set by the user
:LI.The menu bar may be enabled/disabled
:LI.The menu items in each menu can be set by the user
:LI.Keys can be changed and augmented to suit any configuration
:eUL.
:LI.unlimited undo and redo capability
:LI.unlimited file size
:LI.unlimited number of lines in a file
:LI.edit up to 250 files at the same time
:LI.multiple views on the same file
:LI.full mouse support
:LI.powerful script language
:UL compact.
:LI.local and global variables
:LI.structured constructs (loop, while, if/elseif/else )
:LI.arbitrary conditional expressions
:LI.file I/O
:eUL.
:LI.batch edit processing
:LI.regular expression search and replacement
:LI.text marks
:LI.keystroke macros
:LI.block delete, copy, change, case toggle, and shift operations
:eUL.
:P.
&edname is available for the following environments:
:UL.
:LI.DOS (real mode) (286 or higher)
:LI.DOS (protected mode, 386 or higher processors)
:LI.OS/2 1.x
:LI.OS/2 2.x
:LI.Windows NT
:LI.QNX
:LI.Windows 3.x GUI
:LI.Windows NT GUI
:eUL.
:SECTION.Terms and Notation
:SUBSECT.The Mouse
If you have a mouse, you may use it with &edname.
:PERIOD.
When you move your mouse, you will see a large block move around
on your screen.  This is called the mouse cursor.
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
When you start up &edname, you will see something like the following:
:PICTURE file='vistart'.
The &edname screen has several windows.
:UL.
:LI.The top line of the screen
is the
:KEYWORD.menu bar
:CONT.,
which contains all of the menu choices, the current mode, and the current
time.
:LI.The large window below the menu bar is the
:KEYWORD.edit window
:PERIOD.
Since
&edname is a multiple file editor, you may have more than one edit window.
Other edit windows are usually covered by the current edit window, unless
you change the size and position of the edit windows.
:LI.The lower left corner of the screen contains the
:KEYWORD.status window
:PERIOD.
This
window typically shows the current row and column.
:LI.The majority of the bottom of the screen contains the
:KEYWORD.message window
:PERIOD.
This window displays all errors and informational messages.
:eUL.
:P.
In the edit window will be a flashing line.  This flashing line is the
cursor, and it indicates which line you are on and the position
within the line.
:P.
On the border of the edit window are a number of special symbols.  These
special symbols are referred to as
:KEYWORD.gadgets
:PERIOD.
:UL.
:LI.In the top left corner of the edit window is the gadget ('ð') that opens
up the menu for the window.
:LI.In the lower right corner of the edit window is the resize gadget ('').
When you click on this gadget with your mouse and then drag the mouse, you
can resize the edit window.
:LI.On the right hand side of the edit window is the scroll bar.  The
scroll bar has several components:
:LI.The scroll up ('') gadget.  When you click on this with your mouse,
the edit window scrolls up one line.
:LI.The scroll down ('') gadget.  When you click on this with your mouse,
the edit window scrolls down one line.
:LI.The scroll thumb ('Û').  The scroll thumb indicates the relative position
of the current line in the file.  When you click on the scroll thumb with
your mouse and drag it, you can move to a new position in the file.  If
you click with your mouse on the scroll bar above the scroll thumb, you
move one page up in the file.  If you click on the scroll bar below the
scroll thumb, you move one page down in the file.
:eUL.
:cmt :eSUBSECT.
:cmt :SECTION.Edit Now
:cmt For those of you who cannot wait, this section describes a basic set of
:cmt commands that you can use to edit files right away.
