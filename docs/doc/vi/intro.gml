.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Introduction to VI.
.*
.* Date         By              Reason
.* ----         --              ------
.* 10-aug-92    Craig Eisler    initial draft
.* 26-sep-05    L. Haynes       reformatted for hlp, figures
.*
.***************************************************************************
.chap 'Introduction to the &edname'
.np
&edname (&edvi) is a programmer's editor.  It is
loosely based on UNIX &edvi, but is much more powerful.
This manual may be used by someone without any knowledge
of &edvi, or by an experienced &edvi user.
People familiar with &edvi will find the &edname is very similar
in its basic behaviour.
.np
An editor is very personal thing.  Every person uses an editor
in a different way, or wants things to look slightly different.
&edvi was designed with this in mind.  Most features in &edvi
are configurable.
.np
&edvi has many powerful features.  Some of the more significant are:
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
.np
&edname is available for the following environments:
:UL compact.
:LI.DOS (real mode) (286 or higher)
:LI.DOS (protected mode, 386 or higher processors)
:LI.OS/2 1.x
:LI.OS/2 2.x
:LI.Windows NT
:LI.QNX
:LI.Windows 3.x GUI
:LI.Windows NT GUI
:LI.Linux (multiple CPU architectures)
:eUL.
.* ******************************************************************
.section 'Terms and Notation'
.* ******************************************************************
.if &e'&dohelp ne 0 .do begin
.np
This section describes the terms and notations used in this
document.
.do end
.*
.beglevel
.* ******************************************************************
.section 'The Mouse'
.* ******************************************************************
.np
If you have a mouse, you may use it with &edvi..
When you move your mouse, you will see a large block move around
on your screen.  This is called the mouse cursor.
.np
These terms are used when referring to things that may be done with
the mouse.
:DL break.

:DT.click
:DD.Pressing a mouse button once.

:DT.double click
:DD.Pressing a mouse button twice in rapid succession.

:DT.dragging
:DD.Holding down a mouse button and then moving the mouse.

:eDL.
.* ******************************************************************
.section 'The Screen'
.* ******************************************************************
.np
When you start up &edvi, you will see something like the following:
.figure *depth='2.47' *scale='59' *file='vi001' &edvi after start-up
The &edvi screen has several windows.
:UL.

:LI.The top line of the screen is the
.keyword menu bar
which contains all of the menu choices, the current mode, and the current
time.

:LI.The large window below the menu bar is the
.keyword edit window.
Since
&edvi is a multiple file editor, you may have more than one edit window.
Other edit windows are usually covered by the current edit window, unless
you change the size and position of the edit windows.

:LI.The lower left corner of the screen contains the
.keyword status window.
This window typically shows the current row and column.

:LI.The majority of the bottom of the screen contains the
.keyword message window.
This window displays all errors and informational messages.

:eUL.
.np
In the edit window will be a flashing line.  This flashing line is the
cursor, and it indicates which line you are on and the position
within the line.
.np
On the border of the edit window are a number of special symbols.  These
special symbols are referred to as
.keyword gadgets
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

:LI. The scroll thumb ('Û').  The scroll thumb indicates the relative position
of the current line in the file.  When you click on the scroll thumb with
your mouse and drag it, you can move to a new position in the file.  If
you click with your mouse on the scroll bar above the scroll thumb, you
move one page up in the file.  If you click on the scroll bar below the
scroll thumb, you move one page down in the file.

:eUL.
.*
.endlevel
.*

