.chap Zoom
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'Zoom'
Zoom is a tool designed to magnify selected sections of your screen.
This allows you to view icons more closely and examine windows to
detect errors in the graphics. With Zoom you can also watch a program,
while magnified, paint its windows to detect subtle errors.
.*
.section Using Zoom
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting Zoom
.bull
Quitting Zoom
.bull
The Zoom Menu Bar
.bull
The Zoom Window
.endbull
.*
.beglevel
.*
.section Starting Zoom
.*
.ix 'Zoom' 'start'
.ix 'start' 'Zoom'
.np
To start Zoom double-click on the Zoom icon. This opens the
&company Zoom window. When you start Zoom, the Zoom window contains a
magnification of the upper left hand corner of the current screen.
.figure *depth='4.64' *scale=100 *file='wzm1' The Zoom window displays, in magnification, the selected screen area.
.*
.section Quitting Zoom
.*
.ix 'Zoom' 'exit'
.ix 'leave' 'Zoom'
.np
To close the Zoom session, choose Exit from the File menu of the
&company Zoom window.
.*
.section The Zoom Menu Bar
.*
.ix 'Zoom' 'Menu bar'
.ix 'Menu bar' 'Zoom'
.np
The menu bar consists of the following:
.begpoint
.point File
Configure the session and paste the window contents to the clipboard
.point <<
Decrease the magnification of the object in the Zoom window
.point >>
Increase the magnification of the object in the Zoom window
.endpoint
.*
.section The Zoom Window
.*
.ix 'Zoom' 'using the window'
.np
The &company Zoom Window has both a vertical and horizontal scroll bar
which enable you to view, in magnification, the area around the
selected screen space. Once you have selected an area to view in
magnification, you can decrease and increase the magnification using
the symbols on the menu bar.
.begbull $compact
.bull
Click on the decrease symbol (<<) to lessen the magnification of the
selected area.
.bull
Click on the increase symbol (>>) to get a closer view of the selected
area.
.endbull
.*
.endlevel
.*
.section Zoom Operations
.*
.np
There are a number of functions you can perform with Zoom, including
zooming, resizing the window, and pasting magnified screen sections to
the clipboard. This section describes each Zoom function.
.*
.beglevel
.*
.section Zooming
.*
.ix 'zooming'
.np
The following procedures describe the standard steps you perform to
zoom.
.begstep
.step Position the mouse pointer in the Zoom window.
.step Press the left mouse button and drag the mouse
outside of the Zoom window.
.result
Attached to the cursor is a box which acts as a magnifier. As you move
the mouse, the screen sections over which you drag this magnifier
appear magnified in the Zoom window.
.step Position the magnifier on top of the screen area
you want to capture in magnification.
.step Release the mouse button.
.result
Zoom captures the selected screen section in the Zoom window in
magnification.
.step Make fine adjustments,
such as centering the captured image, using the scroll bars of
the Zoom window.
.result
Scrolling is a standard Windows feature. However, with Zoom, you can
see the magnifier move on the screen as you scroll the captured image in
the Zoom window.
.endstep
.*
.section Resizing the Zoom Window
.*
.ix 'Zoom' 'resizing window'
.ix 'Window' 'resizing in Zoom'
.np
Resizing the Zoom window can be done at two different times: before you
zoom an area or while you are zooming an area.
.np
Resizing the window before you zoom is a standard Windows operation.
.begstep
.step Position the cursor on the border of the window
until it turns into a double-pointed arrow.
.step Press the left mouse button and drag the mouse
until the window is the desired size.
.step Release the mouse button.
.endstep
.np
While positioning the magnifier over an area of the screen to select it
for magnification, you may find you want to increase or decrease the
size of the area you capture.
.begstep To resize the window while zooming:
.step Position the cursor in the Zoom window.
.step Press the left mouse button and drag the mouse
outside of the Zoom window.
.step Position the magnifier
over the desired screen area to select.
.step Press the right mouse button
while continuing to hold down the left mouse button.
.result
If the Sticky Magnifier option is activated, you do not need to hold
down the left mouse button. For details on the Sticky Magnifier option,
refer to the section entitled :HDREF refid='confzm'..
.step Move the mouse to increase or decrease the size
of the magnifier.
.result
There is a maximum and minimum size for the magnifier. The resizing
interaction is similar to resizing a regular window. The Zoom window
does not change in size until you release the right mouse button.
.step Release the right mouse button
when the magnifier is the desired size.
.endstep
.begnote
.note Note:
When resizing the window while zooming, the Zoom window may overlap
the selected screen area when you release the right mouse button. When
this occurs, the magnifier captures the image of the resize window,
not the original screen area. This only happens if the screen area you
are selecting is close to the Zoom window.
.endnote
.*
.section The Begin Zoom Item
.*
.ix 'Begin Zoom Item'
.np
The Begin Zoom item in the File menu allows you to scan the screen
area without holding down the left mouse button. This is similar to
the Sticky Magnifier feature that is offered in the Configure dialog.
.np
.begnote
.mnote Note:
The Begin Zoom item is not available in Zoom for Win32.
.endnote
.np
To Zoom using the Begin Zoom item:
.begstep
.step Select Begin Zoom from the File menu.
.result
This reveals the magnifier attached to your cursor.
.step Position the magnifier over the screen area you want to magnify.
.step Resize the magnifier
as desired using the right mouse button.
.step Click the left mouse button
to capture the selected screen area in the Zoom window.
.endstep
.*
.section Pasting with Zoom
.*
.ix 'Zoom' 'pasting'
.ix 'Pasting' 'in Zoom'
.np
The paste function in Zoom allows you to paste whatever is in the Zoom
window onto the clipboard. From here you can paste the clipping to
other applications, such as the Image Editor.
.np
To paste the contents of the Zoom window to the clipboard, choose
Paste to Clipboard from the File menu of the &company Zoom window.
.*
.endlevel
.*
.section *refid=confzm Configuring Zoom
.*
.ix 'Zoom' 'configuring'
.ix 'configure' 'Zoom'
.np
The Configure item under the File menu opens the Configure dialog. On
this dialog you set certain options that dictate how the Zoom
application behaves. Once you have selected the desired configure
options, click on OK to store this information and close the Configure
dialog. The following sections explain the options presented in the
configure dialog.
.figure *depth='2.90' *scale=100 *file='wzm2' On the Configure dialog, you customize the behaviour of the Zoom application.
.*
.beglevel
.*
.section Configuring Zoom: Always On Top
.*
.ix 'Always on Top option Configuring Zoom:'
.np
Selecting the Always On Top option Configuring Zoom: ensures that the Zoom window will
always appear on top of other windows. If this option Configuring Zoom: is not selected,
then when working with several windows at a time the Zoom window can
get pushed behind other windows.
.*
.section Configuring Zoom: Sticky Magnifier
.*
.ix 'Sticky Magnifier'
.ix 'Zoom' 'Sticky Magnifier'
.ix 'Begin Zoom item'
.np
Selecting Sticky Magnifier is the same as using the Begin Zoom menu
item. When this box is checked, the zooming interaction Configuring Zoom: changes.
.np
The Sticky Magnifier option Configuring Zoom: dictates that you zoom by pressing the
left mouse button once in the Zoom window to activate the zoom
function Configuring Zoom: and then press it once more to capture the selected screen
area. With this option Configuring Zoom: you are able to drag the magnifier into
position Configuring Zoom: without holding down the mouse button.
.begnote
.note Note:
.ix 'Win32'
The Sticky Magnifier option Configuring Zoom: is not available in the Win32 version
of Zoom.
.endnote
.*
.section Configuring Zoom: Auto Refresh
.*
.ix 'Zoom' 'Auto Refresh'
.ix 'Auto Refresh'
.ix 'Refresh' 'auto in Zoom'
.np
Normally, Zoom only refreshes the selected area in the Zoom window
when the magnifier moves. When the Auto Refresh option Configuring Zoom: is selected,
Zoom refreshes the image displayed in the Zoom window at regular
intervals, as well as when the magnifier moves. This feature is useful
when magnifying screen images that change frequently, such as the
seconds on the clock application Configuring Zoom:.
.np
When the enable auto refresh box in the configuration Configuring Zoom: dialog is
checked you can enter a value in the Interval field. This value
specifies the interval in tenths of seconds that Zoom will wait
between image refreshes.
.*
.endlevel
