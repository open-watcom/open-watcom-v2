.chap The &company Dialog Editor
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'Dialog Editor'
Dialogs are windows that contain controls such as push buttons, check
boxes, list boxes, and combo boxes. Dialogs are the principle medium
for the exchanging of information between the user and the
application. You include dialogs in your application to prompt the
user for information, enable the user to change settings, and allow
the user to make choices. Dialogs provide the medium through which
users and your application can communicate with each other.
.np
The &company Dialog Editor enables you to create dialogs. With this
editor you can design and test your dialogs so you can see exactly how
they will look and work in your application.
.np
The Dialog Editor allows you to create a new dialog or modify an
existing dialog. When you complete the dialog you can save it as a
resource script or binary resource file. You can also save your dialog
directly to an .EXE or .DLL file if you originally loaded that dialog
from one of these files.
.*
.section Using the Dialog Editor
.*
.np
This section discusses the following topics:
.np
.begbull $compact
.bull
Starting the Dialog Editor
.bull
Quitting the Dialog Editor
.bull
Saving a Resource File with the Dialog Editor
.bull
The Dialog Editor Menu Bar
.bull
The Dialog Editor Toolbar
.endbull
.*
.beglevel
.*
.section Starting the Dialog Editor
.*
.np
.ix 'Dialog Editor' 'start'
.ix 'start' 'Dialog Editor'
The Dialog Editor is a stand-alone application, so you can invoke it
without first starting the Resource Editor. To start the Dialog
Editor, double click on the Dialog Editor icon in your Windows Program
Manager. An empty &company Dialog Editor window opens. To create a
dialog resource you must first open a resource file. You can either open a
new or existing resource file.
.np
How you proceed depends on whether you are opening a new resource file or
working with an existing resource file.
.figure *depth='3.20' *scale=50 *file='dia1' The Dialog Editor window
.*
.beglevel
.*
.section Opening a New Resource File with the Dialog Editor
.*
.np
.ix 'New'
.ix 'Dialog Editor' 'open new resource file'
.bd Choose New from the File menu
of the &company Dialog Editor window.
.np
Default dialog information appears in the editor window of the Dialog
Editor window. The Dialog Editor window is ready for you to create a
new dialog resource.
.*
.section Opening an Existing Resource File with the Dialog Editor
.*
.np
.ix 'Dialog Editor' 'open existing resource file'
.ix 'Open option'
.begstep
.step Double click on the Dialog Editor icon.
.result
The &company Dialog Editor window opens.
.step Choose Open
from the File menu.
.result
An Open Dialog Resource dialog appears listing all resource files.
.step Double click on the resource file you want to open
.orstep Click on the resource file you want to open
to select it, then
.bd click on OK
.ct .li .
.result
A Select Dialog(s) dialog appears listing all of the dialog resources
in the selected resource file. You can select one, several, or all of the
dialog resources in the resource file. All of the dialog resources you
select will open on the editor window of the Dialog Editor window.
.step Double click on the dialog you want to open.
.orstep Click on the dialogs you want to open
to select them, then
.bd click on OK
.ct .li .
.result
The selected dialog(s) appears in the &company Dialog Editor window in
the editor window.
.endstep
.figure *depth='1.78' *scale=66 *file='dia2' You can select one or more resources to edit
.*
.endlevel
.*
.section Quitting the Dialog Editor
.*
.np
.ix 'leave' 'Dialog Editor'
.ix 'Dialog Editor' 'leave'
.ix 'quit' 'Dialog Editor'
To exit the Dialog Editor, choose Exit from the File menu of the
&company Dialog Editor window.
.np
When you exit the Dialog Editor, it remembers the size and position of
its main window and the position of the toolbox. It also remembers
whether the toolbar and toolbox were hidden or not. The next time you
open the Dialog Editor, it will be set up the same way you left it.
The Dialog Editor also remembers the directory in which you last
opened or saved a dialog. This feature makes the Dialog Editor more
convenient and efficient to use.
.*
.section Saving a Resource File with the Dialog Editor
.*
.np
There are two functions you can use to save resource files with the Dialog
Editor:
.begbull $compact
.bull
Save
.bull
Save As
.endbull
.begnote
.note Important:
You can have several resource files open in the Dialog Editor window at one
time. The caption bar of the currently selected resource file appears
highlighted. Only this window is affected when you choose Save
or Save As from the File menu.
.endnote
.begnote
.note Note:
Save operations will always create a file containing the dialog script(s)
(.DLG) and a resource file (.RES).
.endnote
.*
.beglevel
.*
.section Dialog Editor: Save
.*
.np
.ix 'Save option'
.ix 'Dialog Editor' 'Save'
.bd Choose Save from the File menu
in the &company Dialog Editor window to save any changes made to the
resource file.
.np
After modifying individual dialog resources, you perform an update at
the resource level (the Save item in the Dialog menu). The resource file is
not saved until you perform a save at the editor level as well, using
the Save function. The Dialog Editor prompts you if you
attempt to exit the editor without saving changes. Choose Yes, No, or
Cancel to continue.
.*
.section Dialog Editor: Save As
.*
.ix 'Save As option'
.begstep
.step Choose Save As
from the File menu in the &company Dialog Editor window to save any
changes made to the resource file.
.result
A Save Resource dialog appears where you specify the file to which you
want to save the current resource file.
.step Specify the file to which you are saving
by selecting an existing file or entering the name of a new file.
.begnote
.note Note:
If you attempt to save the current resource file to an existing resource file, a
Save Resource message box appears informing you that the selected file
already exists. Choosing Yes on this dialog overwrites the selected
existing resource file with the contents of the current resource file.
Choosing No returns you to the Save Resource dialog.
.endnote
.step Click on OK
when completed.
.result
The dialog closes and the editor saves the resource file, returning you to
the Dialog Editor window.
.endstep
.*
.endlevel
.*
.section The Dialog Editor Menu Bar
.*
.figure *depth='3.20' *scale=50 *file='dia3' The Dialog Editor menu bar
.np
.ix 'Dialog Editor' 'menu bar'
.ix 'menu bar' 'Dialog Editor'
The menu bar consists of the following five menus:
.begpoint $break
.point File
Open and save resource file, load libraries, select custom controls, work
with symbols, and exit the editor
.point Edit
Access the clipboard functions, align and size the dialog controls,
configure the dialog, and delete dialogs
.point Dialog
Test, revert, and save the dialog, create a new dialog, copy the
dialog to another resource file, and set the order, tabs, and groups for
dialog controls
.point Tools
Select the desired tools to use to create the dialog
.point Window
Manipulate child windows, arrange icons, and hide/show the toolbox and
toolbar
.point Help
Access the on-line help information
.endpoint
.np
To see the function of an item in the menu bar or submenus, hold the
mouse button and drag the cursor over the desired menu item. A
description of the item appears in the status line at the bottom of
the screen. If you do not want to select the menu item, drag the
cursor off of the menu item before releasing the mouse button.
.*
.section The Dialog Editor Toolbar
.*
.np
.ix 'Dialog Editor' 'toolbar'
In the &company Dialog Editor window, the toolbar appears below the
menu bar when the Show Toolbar item in the Window menu is enabled.
Once enabled, the menu item changes to Hide Toolbar. To disable this
feature, choose Hide Toolbar from the File menu and the Toolbar
disappears.
.figure *depth='0.43' *scale=100 *file='dia4' The Dialog Editor Toolbar
.np
The following explains the function each icon performs, as well as the
equivalent function in the menu bar.
.begpoint
.point Create
Create a new resource file. This is equivalent to the item New in
the File menu.
.point Open
Open an existing resource file. This is equivalent to the item Open
in the File menu.
.point Save
Save the current resource file. This is equivalent to the item Save
in the File menu.
.point Cut
Cut the selected object(s) to the clipboard. This is equivalent to the
item Cut in the Edit menu.
.point Copy
Copy the selected object(s) to the clipboard. This is equivalent to the
item Copy in the Edit menu.
.point Paste
Paste the object(s) on the clipboard to the current resource file. This is
equivalent to the item Paste in the Edit menu.
.point Size
Size the selected control to the text within it. This is equivalent to
the item Size to Text in the Edit menu.
.point Test
Test the current dialog. This is equivalent to the item Test Mode in
the Dialog menu.
.point Order
Order the controls in the current dialog. This is equivalent to the
item Set Order in the Dialog menu.
.point Set Tab
Set tab stops in the current dialog. This is equivalent to the item Set
Tabs in the Dialog menu.
.point Set Groups
Set groups in the current dialog. This is equivalent to the item Set
Groups in the Dialog menu.
.endpoint
.np
To see the function of an item in the toolbar, position your cursor
over the desired icon. A description of the button's function appears
in the status line at the bottom of the window.
.*
.section The Dialog Editor Toolbox
.*
.np
The toolbox allows you to conveniently select the tool you want,
without having to access it through the Tools menu. Each toolbox
button represents an option in the Tools menu. Moving the cursor over
the toolbox buttons displays the function of the button in the status
bar.
.figure *depth='3.20' *scale=43 *file='dia5' The Dialog Editor toolbox
.*
.endlevel
.*
.section Using the Dialog Editor Window
.*
.np
.ix 'Dialog Editor' 'window'
The &company Dialog Editor window contains a menu bar, toolbar, toolbox,
and status line. The rest of the window is divided into three
sections:
.begbull $compact
.bull
The Information Bar
.bull
The Edit Window
.bull
The Workspace
.endbull
.figure *depth='3.20' *scale=44 *file='dia6' The Dialog Editor window contains a menu bar, toolbar, workspace, and status line.
.*
.beglevel
.*
.section The Dialog Editor Information Bar
.*
.np
.ix 'Dialog Editor' 'Information Bar'
The Information Bar is located under the toolbar. It provides
information about the currently selected object you are adding to the
dialog or modifying. This object can be a control or the dialog window
itself. There are three fields in the Information Bar. The contents of
two fields depends on whether the selected object is a dialog or a
control. You may press F9 to set the keyboard focus to the Information Bar.
While the Information Bar has the keyboard focus you may press ESCAPE to
have the focus return to the Editor.
.figure *depth='0.63' *scale=84 *file='dia7' The Dialog Editor information bar
.np
Changes made to these fields are only processed once you press ENTER
or click on the Set button. Reverse uncommitted changes by clicking on
the Discard button.
.begnote
.note Size
The Size field in this area contains three sets of numbers. These
numbers provide information on the size and position of the selected
dialog or object in the Edit Window. The default number sequence when
you create a new dialog is:
.millust begin
(20, 20) (115, 95) 95x75
.millust end
.np
(20, 20) indicates that the top left hand corner of the dialog is at
the position 20, 20 on the Edit Window grid.
.np
(115, 95) indicates that the bottom right hand corner of the dialog is
at the position 115, 95 on the Edit Window grid.
.np
95x75 indicates the width and height of the dialog.
.endnote
.*
.beglevel
.*
.section Dialogs
.*
.begnote
.note Caption
When the selected object is a dialog, this field contains the name of
the dialog that you want to appear on the dialog in the application.
Any changes you make to this field are not stored until you:
.begbull $compact
.bull
.bd Press ENTER
.ct , or
.bull
.bd Select the Set button
to the right of the field.
.endbull
.note Dlg Name
When the selected object is a dialog, this field contains the name of
the dialog resource. This is the name by which the dialog is stored in
the resource file. When you create a new dialog, the Dialog Editor assigns
the default name DIALOG_# to the new dialog. You change the name of
the dialog using the Dlg Name field.
.begstep To change the dialogs name:
.step Type the new dialog name
using double quotes if the dialogs name is to be an character string.
.orstep Type the symbol name
(without double quotes). New symbols will automatically be created.
.endstep
.endnote
.*
.section Controls
.*
.begnote
.note Text
When the selected object is a control, the Text field contains the
text appearing in the dialog with the selected object.
.figure *depth='0.47' *scale=62 *file='dia8' The information bar text field
.np
This field is not applicable for all objects. Any changes you make to
this field are not stored until you:
.begbull $compact
.bull
.bd Press ENTER
.ct , or
.bull
.bd Select the Set button
to the right of the field
.endbull
.note ID
There are two parts to the ID field. The field on the right contains
the numeric value of the currently selected control. Each control
always has a numeric identifier. You may enter a symbol here ( new
symbols will automatically be created ) or a numeric identifier.
.np
The symbol is a string used to specify the selected control. This is an
alternate way to identify the control in your source code.
.endnote
.*
.endlevel
.*
.section The Edit Window
.*
.np
The edit window is the window on which you create dialogs and
controls. It represents the resource file that your dialog is a part of.
.figure *depth='3.20' *scale=39 *file='dia9' The Dialog Editor edit window
.np
The edit window gives you space so you can temporarily move controls
off your dialog when it gets too crowded. For example, to rearrange
controls in your dialog, you can drag them onto the edit window and
then drag them back onto the dialog in the desired order.
.np
Although you can't see it on your screen, the edit window is an
infinite grid with the origin (0,0) initially at its top left. Dialogs
are always positioned relative to the edit window. The origin is in
the middle of the world co-ordinate system, which is the reason the
scroll thumbs are initially in the middle of the scroll bars.
.*
.section The Workspace
.*
.np
The workspace is the area under the Information Bar on which
you can manipulate several resource files. This area can be compared to
your desktop. For instance, if you minimize an edit window, an icon
representing that edit window appears in the workspace. The
workspace provides you with an area in which to open and close
edit windows.
.figure *depth='3.34' *scale=68 *file='dia10' The Dialog Editor workspace
.*
.endlevel
.*
.section Dialog Functions
.*
.np
.ix 'Dialog Editor' 'dialog functions'
This section describes the several functions you can perform with the
Dialog Editor. The following tasks are included in this section:
.begbull $compact
.bull
Creating New Dialogs
.bull
Opening Existing Dialogs
:cmt..bull
:cmt.Choosing Controls
.bull
Changing the Dialog Size
.bull
Changing the Location of a Dialog
.bull
Saving Dialogs
.bull
Changing the Name of a Dialog
.bull
Changing the Caption in a Dialog
.bull
Adding Controls to a Dialog
.bull
Modifying Controls in a Dialog
.bull
Ordering Controls in a Dialog
.endbull
.*
.beglevel
.*
.section Creating New Dialogs
.*
.np
.ix 'Dialog Editor' 'create new dialogs'
When you open the Dialog Editor, it displays an empty dialog box in
the editor window of the Dialog Editor window. You can change the size
and location of this dialog and add controls to it to meet the needs
of your application. However, if you are not happy with the changes
you have made and want to delete your dialog to start over again, or if
you want to modify more than one dialog at a time, you can create
another empty dialog in one of the following ways:
.begstep
.step Select New
from the File menu.
.step Select New
from the toolbar.
.step Draw another dialog
in the editor window with the "dialog" item from the
toolbox.
.endstep
.np
An empty dialog named "Untitled" appears in the editor window. Now you
can add controls to the empty dialog, and change its size and location
within the edit window.
.*
.section Opening Existing Dialogs
.*
.np
.ix 'Dialog Editor' 'open existing dialogs'
You can open a dialog from any .RES, .EXE, or .DLL file to your edit
window where you can modify it. To do this:
.begstep
.step Open the file you want,
in one of the following ways:
.begbull $compact
.bull
Select Open from the file menu, or
.bull
Select the Open button from the toolbar.
.endbull
.step Select the .RES, .EXE, or .DLL file
that contains the dialog you want to copy.
.result
The &company Dialog Editor Open Resource dialog appears.
.step Select the resource file you want.
.step Select the name of the file you want,
in one of the following ways:
.begbull $compact
.bull
Select the name and press OK, or
.bull
Double click on the name of the file.
.endbull
.np
If there are any dialog resources in the file, the Select Dialog
appears listing the names of all the dialog resources that exist in
that resource file.
.begnote
.mnote Note:
If the file does not contain any dialog resources, the main screen
appears with an empty edit window and dialog for you to add controls
and modify.
.endnote
.step Select one or more dialogs to place in the edit window
in one of the
following ways:
.begbull $compact
.bull
Double click on the dialogs you wish to modify, or
.bull
Select the dialogs you wish to modify and select OK.
.endbull
.np
The dialog you copied appears in the edit window of your main screen.
From here you can add controls to the dialog and modify its size and
location.
.endstep
.*
.section Changing the Dialog Size
.*
.np
To modify the size of a dialog,
.begstep
.step Select the dialog.
.step Position the mouse
above the appropriate sizing handle.
.step Drag the dialog's sizing handle
until the dialog is the size you want.
.endstep
.np
You will notice that the dialog size information in the Information
Bar will track the size of the dialog.
.*
.section Changing the Location of a Dialog
.*
.np
.ix 'Dialog Editor' 'change location of dialog'
.ix 'Dialog' 'change location of'
Since you will want all parts of your dialog to be visible to your
users and grab their attention, you can modify the location of your
dialog relative to the parent window. To do this:
.begstep
.step Position the mouse
either above the dialog caption or within several pixels of the
dialog's border (while remaining in the dialog).
.step Drag the dialog
to the position you want in the edit window.
.endstep
.np
You will notice that the dialog size information in the Information
Bar will track the location of the dialog.
.*
.section Saving Dialogs
.*
.np
.ix 'Dialog' 'save'
.ix 'Dialog Editor' 'saving dialogs'
It is a good idea to save changes to your dialog as you modify it
rather than wait for the Dialog Editor to prompt you when you close
your dialog or exit the Dialog Editor. There are five ways to save
dialogs. The two primary ways to save a dialog are to select one of
the following from the file menu:
.begbull $compact
.bull
Save
.bull
Save As
.endbull
.np
Selecting Save or Save As from the File menu saves the entire resource
file in which your dialog resides, updating any changes you made to
your dialog in the process.
.np
In addition, there are three other ways to save your dialog. You can
choose Save, Save As, or Copy To from the Dialog menu. Saving a dialog
from the Dialog menu only saves one resource &mdash. the currently
selected dialog &mdash. not the entire resource file which typically
contains numerous resources.
.begnote
.note Warning:
If you choose to select Save or Save As from the dialog menu you will
only be saving the current dialog and not any of the other resources
in the resource file. If you save the dialog under the same name as the
resource file, you will overwrite all the resources in that file and be
left with a resource file that contains nothing more than your dialog.
.endnote
.*
.section Changing the Name of a Dialog
.*
.np
.ix 'Dialog Editor' 'change name'
The name of a dialog is the name associated with the dialog resource
when you save it to a dialog resource file.
.np
To avoid name conflicts, (you can't have two dialog resources with the
same name, in the same file) you will want to change the name of your
dialog (the editor gives you one as a default). Type in the edit field
a name you want to give your dialog or, if symbols have been loaded,
you can select a symbol from the combo box. You can drop down the
combo list box and select a symbol from there.
.figure *depth='0.47' *scale=62 *file='dia11' The combo box is labelled "ID".
:cmt. For more information about loading and selecting symbols, see the
:cmt. sections entitled "Loading Symbols" and "Selecting Symbols".
.*
.section Changing the Caption in a Dialog
.*
.np
A dialog's caption is the text that appears in the title bar of the
dialog.
.np
You can type in the edit field of the Information Bar to change the
caption's name.
.*
.section Adding Controls to a Dialog
.*
.np
.ix 'Dialog Editor' 'add controls'
Select a tool that will allow you to create controls by selecting an
item from the Tools menu or from the toolbox.
.np
There are two ways in which controls may be drawn in the dialog.
.onestep
.step Click the left mouse button
at the position where you would like the top left corner of the
control to be. The control will be created in its default size.
.orstep Press the left mouse button and drag out a rectangle.
Release the left mouse button.
.endstep
.np
The tool box goes back to select mode when you draw a control (the
current tool becomes the select tool).
.np
To draw more than one of the same types of controls without having to
choose the control from the toolbar or menu every time, choose the
sticky tool which causes the tool to stay down.
.*
.section Modifying Controls in a Dialog
.*
.ix 'Dialog Editor' 'modifying dialog controls'
.np
Once you have created controls for your dialog you can modify the
controls as follows:
.begstep
.step Double-click the control.
.orstep Select the control and Press ENTER.
.result
A Style dialog box appears.
.endstep
.np
Once in the Dialog editor, you can define your dialog box. You can
also add, change, group, reorder, move, resize, ore delete dialog
controls so that your dialog box functions the way you want it to.
.*
.section Ordering Controls in a Dialog
.*
.ix 'Dialog Editor' 'ordering dialog controls'
.np
There is an ordering associated with the controls of a dialog. This
ordering is commonly referred to as the "tab order" and controls which
dialog control is highlighted next when you press the TAB or cursor
keys. As you add controls to a dialog, they are added to the end of
the list.
.np
The Dialog editor allows you to change this ordering using a simple
"point-and-click" technique. To do this:
.begstep
.step Select the dialog
.step Select Set Order
from the Dialog Menu dialog
.orstep Press the Set Order button
on the toolbar
.result
All of the controls in the dialog now have a button at their top-left
corner indicating their current position.
.step Click the dialog controls
in order until the ordering is what you would like.
.step Select Set Order
from the Dialog Menu dialog
.orstep Press the Set Order button
on the toolbar
.result
Reselecting Set Order will terminate the ordering session and record
any changes made.
.endstep
.np
As you click the dialog controls you will notice two things. First,
the buttons at the dialog controls top-left take on a "depressed"
look. This indicates a button you have already clicked. Second, the
unselected or unclicked buttons will reorder themselves to reflect any
changes you have made so far. This will allow you terminate the
ordering session without having to click all of the dialogs controls.
.np
Clicking on a dialog control that is in the "depressed" or selected
state will undo the new ordering you have given it and it will go back
to its original position. If a "depressed" dialog control already has
its original position then it finds the first available position. This
feature is handy if you make a mistake and would like to perform a
quick fix.
.np
There may be a point in the ordering process where you decide to start
over from the beginning. Instead of clicking over all the dialog
controls you have previously clicked or leaving and then re-entering
the Set Order mode you may simply double click the dialog control you
wish to be first. This will undo all that you have done and make the
selected dialog control the first one.
.np
You may run into a situation where the first n dialog controls are
correctly ordered but subsequent one are not. Before you begin to
click through the first n dialog controls note that there is a better
way. Simply shift-click the n'th control to accept the current
ordering up to that point and proceed.
.*
:cmt.######### this is the beginning of the commented out text #########
:cmt..np
:cmt..ix 'Dialog Editor' 'choosing Controls'
:cmt..ix 'Controls' 'choosing in Dialog Editor'
:cmt.Depending on the type of information you want the user to give your
:cmt.application, you must create the appropriate controls for your dialog. The
:cmt.following list helps you decide when to use one control and when to use
:cmt.another:
:cmt..begpoint $break Choosing Options
:cmt..point If only one option
:cmt.Use a push button
:cmt..point If between two exclusive options
:cmt.Use a check box
:cmt..point If between three exclusive options
:cmt.Use a radio button
:cmt..endpoint
:cmt..np
:cmt.Following are descriptions of the controls you can use to create your
:cmt.dialogs:
:cmt..np
:cmt..bd Push Buttons
:cmt..br
:cmt.If you want users to initiate some action, such as OK, Cancel, or Help, use a
:cmt.push button.
:cmt..np
:cmt..bd Check Boxes
:cmt..br
:cmt.If you want the user to choose binary, yes or no information, use a check
:cmt.box.
:cmt..np
:cmt..bd Radio Buttons
:cmt..br
:cmt.If you want the user to choose between three or more exclusive options, use
:cmt.a radio button.
:cmt..np
:cmt..bd Group Boxes
:cmt..br
:cmt.If you would like to group controls together such as radio buttons and check
:cmt.boxes to help users understand your dialogs, use one of the static controls:
:cmt..np
:cmt.If you want to give users more information about why a group of controls-
:cmt.such as radio buttons and check boxes-are grouped together, use a group
:cmt.box with a meaningful heading on it.
:cmt..np
:cmt..bd Frames
:cmt..br
:cmt.If you want to group controls together without a heading, use a frame.
:cmt..np
:cmt..bd List Boxes
:cmt..br
:cmt.If you want the users to be able to read a list of options and select one, use a
:cmt.list box.
:cmt..np
:cmt..bd Simple Combo Boxes
:cmt..br
:cmt.If you want users to be able to select an option from the list box or type the
:cmt.option they want directly into the edit field, use a simple combo box.
:cmt..np
:cmt..bd Drop Down Combo Boxes
:cmt..br
:cmt.If you want users to be able to.... Help?
:cmt.you have to press another button to make the list box drop down and then it
:cmt.works the same way. It has a button beside the edit field (a pop-box or
:cmt.drop-down button).
:cmt.drop down list combo box: same as drop down but not given an edit field,
:cmt.but are given a static text field that shows user what he selected from list
:cmt.box. You can't type in it.
:cmt..np
:cmt..bd Icon Control
:cmt..br
:cmt.Use this button to display an icon in your dialog for visual impact.
:cmt..np
:cmt..bd Horizontal and Vertical Scroll Bars
:cmt..br
:cmt.If you want to allow users to scroll within a large dialog quickly and, be able
:cmt.to see where they are within a large dialog, use horizontal and vertical scroll
:cmt.bars.
.*
:cmt..section Changing the Dialog Size
.*
:cmt..np
:cmt.To modify the size of a dialog,
:cmt..autonote
:cmt..note
:cmt.Select the dialog.
:cmt..note
:cmt.Drag the dialog's sizing handles until the dialog is the size you want.
:cmt..endnote
.*
:cmt..section Changing the Location of a Dialog
.*
:cmt..np
:cmt..ix 'Dialog Editor' 'change location of dialog'
:cmt..ix 'Dialog' 'change location of'
:cmt.Since you'll want all parts of your dialog to be visible to your users and grab
:cmt.their attention, you can modify the location of your dialog relative to the
:cmt.parent window. To do this:
:cmt..autonote
:cmt..note
:cmt.Drag the dialog to the position you want in the edit window.
:cmt..endnote
.*
:cmt..section Saving Dialogs
.*
:cmt..np
:cmt..ix 'Dialog' 'save'
:cmt..ix 'Dialog Editor' 'saving dialogs'
:cmt.It is a good idea to save changes to your dialog as you modify it rather than
:cmt.wait for the Dialog Editor to prompt you when you close your dialog or exit
:cmt.the Dialog Editor. There are five ways to save dialogs. The two primary
:cmt.ways to save a dialog are to select the following from the file menu:
:cmt..begbull $compact
:cmt..bull
:cmt.Select Save from the file menu.
:cmt..bull
:cmt.Select Save As from the file menu.
:cmt..endbull
:cmt..np
:cmt.Selecting Save or Save As from the File menu saves the entire .RES, .EXE, or .DLL file in which your dialog resides, updating any changes you made to
:cmt.your dialog in the process.
:cmt..np
:cmt.In addition, there are three other ways to save your dialog. You can choose
:cmt.Dialog Save, Dialog Save As, or Dialog Save Into from the Dialog menu.
:cmt.Saving a dialog from the Dialog menu only saves one resource-the current,
:cmt.selected dialog-not the entire .RES, .EXE, or .DLL file which typically
:cmt.contains numerous resources. To create your own private copy of a favorite
:cmt.resource from an executable in a library or in.... (This is wrong. When would
:cmt.you want to save from Dialog menu?)...choose save from the Dialog menu.
:cmt..begnote
:cmt..mnote Warning:
:cmt.If you choose to save from the dialog menu you will only be saving the
:cmt.current dialog and not any of the other resources in the .RES, .EXE, or .DLL
:cmt.file. If you save the dialog under the same name as the .RES, .EXE, or .DLL
:cmt.file, you will overwrite all the resources in that file and be left with a .RES, .EXE, or .DLL file that contains nothing more than your dialog.
:cmt..endnote

:cmt..section Changing the Caption Name of a Dialog
.*
:cmt..np
:cmt..ix 'Dialog Editor' 'change caption name'
:cmt.The caption name of a dialog is the name associated with the dialog
:cmt.resource when you save it to a dialog resource file.
:cmt..np
:cmt.To avoid name conflicts, (you can't have two dialog resources with the same
:cmt.name, in the same file) you will want to change the name of your dialog (the
:cmt.editor gives you one as a default) Type in the edit field a name you want to
:cmt.give your dialog, or , if you have loaded symbols, you can select a symbol
:cmt.from the combo box. You can drop down the combo list box and select a
:cmt.symbol from there. For more information about loading and selecting symbols,
:cmt.see the sections titled Loading Symbols and Selecting Symbols
.*
:cmt..section Changing the Caption in a Dialog
.*
:cmt..np
:cmt.A dialog's caption is the text that appears in the title bar of the dialog.
:cmt..np
:cmt.You can type in the edit field of the Information Bar to change the caption's
:cmt.name.
.*
:cmt..section Modifying Controls in a Dialog
.*
:cmt..ix 'Dialog Editor' 'modifying dialog controls'
:cmt..np
:cmt.Once you have created controls for your dialog you can modify the controls
:cmt.as follows:
:cmt..begbull
:cmt..bull
:cmt.Double-click the control.
:cmt..br
:cmt.The Style dialog box appears.
:cmt..endbull
:cmt..np
:cmt.Once in the Dialog editor, you can define your dialog box. You can also add,
:cmt.change, group, reorder, move, resize, ore delete dialog controls so that your
:cmt.dialog box functions the way you want it to.
.*
:cmt..section Ordering Controls in a Dialog
.*
:cmt..np
:cmt.The moment you add controls to a dialog, they are ordered (there is an
:cmt.ordering associated with the controls of a dialog) Ordered as you created
:cmt.them (if you open an existing Dialog, they will be ordered the same way
:cmt.they were when they were created).
.*
:cmt..section Adding Controls to a Dialog
.*
:cmt..np
:cmt..ix 'Dialog Editor' 'add controls'
:cmt.Select a tool that will allow you to create controls by selecting an item from
:cmt.the Tools menu or from the toolbox.
:cmt..np
:cmt.Drawing with the tool: drag out a rectangle with the mouse. Drag mouse
:cmt.across the dialog or edit window.
:cmt..np
:cmt.The tool box goes back to select mode when you draw a control (the current
:cmt.tool becomes the sect tool).
:cmt..np
:cmt.To draw more than one of the same types of controls without having to
:cmt.choose the control from the toolbar or menu every time, choose the sticky
:cmt.tool which causes the tool to stay down.
.*
.endlevel
