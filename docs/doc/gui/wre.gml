.chap *refid=resedit The &company Resource Editor
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'Resource Editor'
The &company Resource Editor is a full-featured editor that enables
you to easily create and edit resources and the files that contain
them. A file containing one or more resources will generally be
referred to as a resource file. A resource file may be an executable
(.EXE), resource file (.RES), dynamic link library (.DLL), bitmap file
(.BMP), cursor file (.CUR), or icon file (.ICO). The editor also
supports a variety of resource scripts. They include dialog (.DLG),
accelerator (.ACC), menu (.MNU), and string (.STR) scripts. This
support is accomplished by maintaining a synchronized resource file
(.RES) in the same directory as the resource script.
.ix 'Resource File' 'in Resource Editor'
.ix 'Resource Editor' 'resource file'
Opening a resource file reveals a Resource window in the Resource
Editor window. This window is a visual representation of the resource
file.
.np
.ix 'resource' 'defined'
A resource is a piece of data with a specific format that Windows uses
to create the visual and interactive elements of your application.
Most of the visual aspects of Windows applications are the result of
the application having resources, all of which can be created using
the &company Resource Editor. Use the Resource Editor to create and
edit the following resource types:
.begbull $compact
.bull
Accelerators
.bull
Bitmaps
.bull
Cursors
.bull
Dialogs
.bull
Icons
.bull
Menus
.bull
Strings
.endbull
.np
When designing your application's user interface, the resources are
separate from the application's source code. This makes changing the
user interface a simple task. Instead of accessing the application's
source code, you can make the changes directly to the resources. You
can manipulate some aspects of the resources, such as the memory flags
and resource names, without invoking the resource's associated editor.
Invoking the desired editor from the Resource Editor enables you to
change all aspects of the selected resource.
.np
Different applications often use the same or similar resources, such
as bitmaps, icons, dialogs, and menus. The Resource Editor enables you
to create resource files for a new application by reusing existing
resources. Copying a resource from existing applications into the new
resource file makes the task of creating your user interface quick and
simple.
.*
.section Using the &company Resource Editor
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting the Resource Editor
.bull
Quitting the Resource Editor
.bull
The Resource Editor Menu Bar
.bull
The Resource Editor Toolbar
.endbull
.*
.beglevel
.*
.section Starting the Resource Editor
.*
.np
.ix 'Resource Editor' 'start'
.ix 'start' 'Resource Editor'
To start the Resource Editor, double click on the Resource Editor icon
in your Windows Program Manager. The &company Resource Editor window
opens.
.keep 16
.figure *depth='3.20' *scale=56 *file='res1' The &company Resource Editor window acts as a workspace where you work with resources.
.*
.section Quitting the Resource Editor
.*
.np
.ix 'Resource Editor' 'leave'
.ix 'leave' 'Resource Editor'
.ix 'quit' 'Resource Editor'
To exit the Resource Editor, choose Exit from the File menu of the
&company Resource Editor window. If you made changes to the current
file, a message box appears prompting you to save the changes. Select
Yes to save the file and exit the Resource Editor, No to exit the
Resource editor without saving the file, or Cancel to return to the
Resource editor.
.keep 15
.figure *depth='1.49' *scale=67 *file='res2' As a safety feature, a message box appears when you select Exit from the File menu without saving a modified resource file.
.*
.section The Resource Editor Menu Bar
.*
.np
.ix 'Resource Editor' 'menu bar'
.ix 'menu bar' 'for Resource Editor'
The menu bar consists of the following five menus:
.begpoint
.point File
Open a new or existing resource file, save the current resource file,
and exit the Resource Editor
.point Edit
Access the clipboard functions, delete the selected resource, and set
the Resource Editor options
.point Resource
Open a new resource and work with existing resources
.point Window
Customize the Resource Editor workspace
.point Help
Access on-line help information
.endpoint
.np
To see the function of a menu item, hold the mouse button and drag the
cursor over the desired menu item. A description of the item appears
in the status line at the bottom of the screen. If you do not want to
select the menu item, drag the cursor off of the menu item before
releasing the mouse button.
.*
.section The Resource Editor Toolbar
.*
.ix 'Resource Editor' 'toolbar'
.ix 'Toolbar' 'in Resource Editor'
.np
In the &company Resource Editor window, the toolbar appears below the
menu bar. When the toolbar is shown, choose Hide Toolbar from the
Window menu to hide it. When the toolbar is hidden, you can display it
by choosing Show Toolbar from the Window menu.
.keep 16
.figure *depth='0.43' *scale=100 *file='res3' You can access six Resource Editor functions from the toolbar.
.np
The following explains the function each toolbar button performs, as
well as the equivalent menu item.
.begnote
.note CREATE
Create a new resource file. This is equivalent to the New item in the
File menu.
.note OPEN
Open an existing resource file. This is equivalent to the Open item in
the File menu.
.note SAVE
Save the resource file. This is equivalent to the Save item in the
File menu.
.note CUT
Cut the selected resource to the clipboard. This is equivalent to the
Cut item in the Edit menu.
.note COPY
Copy the selected resource to the clipboard. This is equivalent to the
Copy item in the Edit menu.
.note PASTE
Paste the resource on the clipboard to the specified resource file.
This is equivalent to the Paste item in the Edit menu.
.endnote
.np
To see the function of an item in the toolbar, position your cursor
over the desired button. A description of the button's function
appears in the status line at the bottom of the window.
.*
.endlevel
.*
.section Working with Resource Files
.*
.np
In the &company Resource Editor, the items in the File menu apply to
resource file. These items enable you to perform the following
functions:
.begbull $compact
.bull
Creating a New Resource File
.bull
Opening an Existing Resource File
.bull
Saving a Resource File
.endbull
.*
.beglevel
.*
.section Creating a New Resource File with the Resource Editor
.*
.ix 'Resource Editor' 'create new resource file'
.ix 'resource file' 'create new'
.ix 'New option'
.np
Create a new resource file each time you need a file to hold a
collection of resources. You can create the resources and add them to
the new file or you can copy existing resources from another file and
place them in the new file.
.keep 19
.figure *depth='3.20' *scale=56 *file='res4' Choosing New opens an untitled and empty Resource window into which you can store resources.
.np
To create a new resource file, choose New from the File menu in the
&company Resource Editor window. An untitled and empty Resource dialog
appears on the screen. From here, you can begin adding resources to
the resource file. You can also open an existing resource file and
copy selected resources to the new file. The file remains untitled
until you save it and assign it a file name.
.*
.section Opening an Existing Resource File with the Resource Editor
.*
.ix 'Resource Editor' 'open existing resource file'
.ix 'resource file' 'open existing'
.ix 'Open option'
.np
Open an existing resource file to view the resources for that file or
perform functions on the file's resources.
.figure *depth='3.20' *scale=56 *file='res5' Choosing Open permits you to access resources in an existing file.
.begstep To open an existing resource file:
.step Choose Open from the File menu
in the &company Resource Editor window.
.result
An Open File dialog appears where you select the file you want to
open.
.step Double click on the desired file
.orstep Click on the desired file
to select it, then
.bd click on OK
.ct .li .
.result
The Open File dialog closes and a Resource dialog for the selected
resource file appears in the Resource Editor window. This dialog
displays a button representing each type of resource that exists in
the resource file. Clicking on the resource buttons displays a list of
the resources of that type in the resource file.
.np
From here, you can perform any of the functions in the section
entitled :HDREF refid='workres'..
.endstep
.*
.section Saving a Resource File with the Resource Editor
.*
.np
There are two menu items that you can use to save resource files with
the Resource Editor:
.begbull $compact
.bull
Save
.bull
Save As
.endbull
.begnote
.note Important:
You can have several resource files open in the Resource Editor window
at one time. The caption bar of the currently selected resource file
appears highlighted. Only this window is affected when you choose Save
or Save As from the File menu.
.endnote
.*
.beglevel
.*
.section Resource Editor: Save
.*
.ix 'Save option'
.ix 'Resource Editor' 'Save'
.ix 'resource file' 'save'
.np
.bd Choose Save
from the File menu in the &company Resource Editor window to save any
changes made to the resource file.
.np
If the file is new and previously unsaved, refer to the section
entitled :HDREF refid='saveas'..
.np
After modifying individual resources, you perform an update at the
resource level. The resource file and its resources are not saved
until you select Save from File menu of the Resource Editor. The
Resource Editor prompts you if you attempt to exit the editor without
saving changes. Choose Yes to save the file and exit the Resource
Editor, No to exit the Resource Editor without saving the file, or
Cancel to return to the Resource Editor.
.*
.section *refid=saveas Resource Editor: Save As
.*
.ix 'Save As'
.ix 'Resource File' 'save as'
.ix 'Resource Editor' 'Save As'
.np
.begstep To save a resource file to another file:
.step Choose Save As
from the File menu in the &company Resource Editor window to save any
changes made to the resource file.
.result
A Save File dialog appears where you specify the file to which you
want to save the current resource file.
.step Specify the file
to which you are saving by selecting an existing file or entering the
name of a new file.
.step Click on OK
when completed.
.keep 17
.figure *depth='1.65' *scale=67 *file='res6' A Save File message box appears when you attempt to save the current resource file to an existing file.
.result
The dialog closes and the Resource Editor saves the resource file,
returning you to the Resource Editor window.
.begnote
.note Note:
If you attempt to save the current resource file to an existing file,
a Save File window appears informing you that the selected file
already exists. Choosing Yes on this dialog overwrites the selected
existing file with the contents of the current resource file.
.endnote
.result
The dialog closes and the Resource Editor saves the resource file,
returning you to the Resource Editor window.
.result
The dialog closes and the Resource Editor saves the resource file,
returning you to the Resource Editor window.
.endstep
.*
.endlevel
.*
.endlevel
.*
.section *refid=workres Working with Resources
.*
.np
The items in the Edit and Resource menus in the &company Resource
Editor window enables you to perform functions on resources within a
resource file. To use these functions, a resource file must be open in
the workspace. This resource file can be empty or can contain existing
resources. The functions you can perform are:
.begbull $compact
.bull
Creating a New Resource
.bull
Editing an Existing Resource
.bull
Saving a Resource
.bull
Using Shortcuts to Create a Resource
.bull
Deleting a Resource
.bull
Renaming a Resource
.bull
Changing Memory and Load Options
.endbull
.*
.beglevel
.*
.section Creating a New Resource
.*
.ix 'Resource Editor' 'create new resource'
.ix 'Resource' 'create new'
.ix 'New' 'in Resource menu'
.np
From the &company Resource Editor window, you can create any of the
following resources:
.begbull $compact
.bull
Accelerator
.bull
Bitmap
.bull
Cursor
.bull
Dialog
.bull
Icon
.bull
Menu
.bull
String
.endbull
.np
To create a new resource, choose the type of resource from the
cascading menu that appears when you choose New from the Resource
Menu. The editor for the selected resource type appears on the screen
on top of the Resource Editor window. From here, create the desired
resource, referring to the appropriate editor section in this guide
for more information.
.*
.section Editing an Existing Resource
.*
.np
.ix 'Resource' 'edit existing'
.ix 'Resource Editor' 'edit existing resource'
From the Resource Editor window, you can select any resource within a
resource file and edit it by opening the editor with which the
resource was created. This makes editing resources efficient as you
have access to every resource in a resource file and you can invoke
the appropriate editor quickly.
.begstep To edit an existing resource:
.step Select the desired resource file
on the Resource Editor window by selecting the appropriate Resource
window.
.result
This is the resource file whose resource you want to edit.
.step Select the resource type
by selecting on the appropriate line in the type listbox.
.result
A list of all resources of the selected type for this resource file
appears in the Resource listbox.
.keep 19
.figure *depth='3.20' *scale=56 *file='res7' Each resource type contained in the resource file appears in the Types listbox.
.step Double click on the resource you want to edit
.orstep Select the desired resource,
then
.bd choose Edit from the Resource menu
.orstep Select the desired resource,
then
.bd press ENTER
.ct .li .
.result
The appropriate editor for the selected resource opens on top of the
Resource Editor window. Use this editor to make changes to the
selected resource.
.step
Refer to the appropriate editor section in this guide for information
on using the editor to edit the selected resource.
.endstep
.*
.section Saving a Resource
.*
.np
.ix 'Resource Editor' 'save resource'
.ix 'Resource' 'save'
The Save As menu item in the Resource menu enables you to save a
resource to a new or existing resource file. Saving a resource to an
existing resource file replaces the existing resources for that
resource file with the resource you are saving. You can save only one
resource at a time with this function.
.begstep
.step Select the desired resource.
.result
You can select only one resource at a time and the selected resource
appears highlighted. This is the resource you want to save to another
resource file.
.step Choose Save As
from the Resource menu.
.result
A Save Resource dialog appears where you specify the resource file to
which you are saving the selected resource.
.step Specify the file to which you are saving
by selecting an existing file or entering the name of a new file.
.step Click on OK
when completed.
.result
How you proceed from here depends on whether you are saving the
resource to a new or existing resource file.
.endstep
.*
.beglevel
.*
.section Saving to a New Resource File
.*
.np
.ix 'Resource' 'save to new resource file'
.ix 'Resource Editor' 'save resource to new resource file'
Once you enter the name of the new resource file to which you are
saving the selected resource and click on OK in the Save Resource
dialog, the dialog closes.
.*
.section Saving to an Existing Resource File
.*
.np
.ix 'Resource Editor' 'save resource to existing resource file'
When you choose an existing file from the Save Resource dialog, a Save
Resource message box appears on the screen. This message box informs
you that the selected file already exists and asks if you want to
replace the existing file.
.keep 17
.figure *depth='1.65' *scale=67 *file='res8' A Save File As message box appears when you attempt to save the current resource file to an existing file.
.begstep To cancel the save:
.step Choose No
.result
The Save Resource dialog reappears.
.step Select another file
to which you can save the selected resource
.orstep Cancel the save function
.result
The dialog closes and the Resource Editor returns you to the Resource
Editor window.
.endstep
.begstep To replace the existing file:
.step Choose Yes
to replace the contents of the existing resource file with the
selected resource.
.result
The dialog closes and the Resource Editor saves the resource file,
returning you to the Resource Editor window.
.endstep
.*
.section Copying Into an Existing Resource File
.*
.np
.ix 'Resource Editor' 'copy resource into an existing file'
The Copy To function enables you to copy the selected resource into an
existing resource file. This does not remove the resource from its
current resource file and it does not delete the resources in the
existing resource file, as in the Save As function.
.begstep To copy a resource to another resource file:
.step Select the desired resource.
.result
The selected resource appears highlighted in the Resource window. This
is the resource you want to copy to another resource file. Copying a
resource does not remove it from its current resource file.
.step Choose Copy To from the Resource menu.
.result
A Copy Resource Into File dialog appears where you select an existing
resource file to which you want to copy the selected resource.
.keep 15
.figure *depth='2.69' *scale=65 *file='res9' In the Copy Resource Into File dialog, select the resource file to which you want to copy the selected resource.
.step Double click on the desired resource file
.orstep Click on the desired resource file
to select it, then
.bd click on OK
.ct .li .
.result
The Resource Editor copies the resource to the selected resource file.
The dialog closes and you return to the &company Resource Editor
window.
.begnote
.note Note:
If the resource file to which you copied the selected resource is open
on the Resource Editor window, the copied resource will not appear. To
view the updated resource file, simply open that resource file again.
The copied resource now appears in the Resource window for the
resource file.
.endnote
.endstep
.*
.endlevel
.*
.section Using Shortcuts to Create a Resource
.*
.np
.ix 'Resource Editor' 'shortcut to create resource'
The Edit menu on the &company Resource Editor window contains some
standard Windows items that you can use to quickly create resources.
The items are:
.begbull $compact
.bull
Cut
.bull
Copy
.bull
Paste
.endbull
.ix 'Resource' 'cut'
.ix 'Resource' 'copy'
.ix 'Resource' 'paste'
.ix 'Resource' 'copy to'
.np
These items allow you to cut or copy selected resources from one
resource file, then open a second resource file and paste them into
it. You can also paste them directly into the appropriate editor.
.begstep Cut
.step Select the resource
you want to cut to the clipboard.
.result
The selected resource appears highlighted in the Resource window.
.step Choose Cut
from the Edit menu of the Resource Editor window.
.result
The Resource Editor removes the selected resource from the Resource
window and places it on the clipboard.
.endstep
.np
Refer to
.us Paste
in this section for details on pasting information from the clipboard.
.begstep Copy
.step Select the resource
you want to copy to the clipboard.
.result
The selected resource appears highlighted in the Resource window.
.step Choose Copy
from the Edit menu of the Resource Editor window.
.result
The Resource Editor copies the selected resource to the
clipboard.
.endstep
.np
Refer to
.us Paste
in this section for details on pasting information from the clipboard.
.begstep Paste
.step Select the resource file in which you want to paste the resource
that is currently on the clipboard.
.step Choose Paste
from the Edit menu
of the Resource Editor window.
.result
The Resource Editor places the resource from the clipboard into the
current resource file in the Resource Editor window.
.endstep
.*
.section Deleting a Resource
.*
.np
.ix 'Resource Editor' 'deleting a resource'
The Delete item in the Edit menu enables you to quickly delete
resources from a resource file.
.begstep To delete an object from a resource file:
.step Select the resource you want to delete
from a resource file on the Resource Editor window.
.result
The selected resource appears highlighted in the Resource window.
.step Choose Delete
from the Edit menu.
.orstep Press the Delete key
on the keyboard.
.result
A Delete Resource dialog box appears displaying the name of the
resource chosen to delete.
.keep 14
.figure *depth='1.35' *scale=70 *file='res10' The Delete Resource dialog prompts you to verify that you want to delete the resource indicated.
.step Click on OK
to delete the selected resource.
.result
The Delete Resource dialog closes and the Resource Editor removes the
selected dialog from the Resource window.
.endstep
.begnote
.note Note:
Choosing Cancel on the Delete Resource dialog cancels the deletion and
returns you to the Resource Editor window.
.endnote
.*
.section Renaming a Resource
.*
.np
.ix 'Resource Editor' 'renaming a resource'
You can change the name of a resource from either the Resource Editor
or from the editor in which the resource was created.
.begstep To rename a resource:
.step Select the resource you want to rename.
.step Choose Rename
from the Resource menu in the &company Resource Editor window.
.result
A Rename Resource dialog appears containing two fields:
.begbull $compact
.bull
Old Name
.bull
New Name
.endbull
.keep 16
.figure *depth='1.49' *scale=68 *file='res11' In the Rename Resource dialog, you enter the new name of the selected resource.
.step Enter the new name for the resource
in the New Name field.
.step Click on OK
to close the Rename Resource dialog.
.result
The dialog closes and the new resource name appears in the Resource
window. The Resource Editor automatically reorders the resources into
alphabetical order.
.endstep
.*
.section Resource Editor: Changing Memory Options
.*
.np
.ix 'Resource Editor' 'Memory Flags option'
.ix 'Memory Flags options'
The Memory Flags item in the Resource menu of the Resource Editor
window enables you to change the memory options for the selected
resource without opening the editor for that resource. These options
indicate the memory type and loading instructions for the resource.
.begstep To set the memory options for a resource:
.step Select the resource
whose memory options you want to change.
.step Choose Memory Flags
from the Resource menu in the &company Resource Editor window.
.result
The Memory Options dialog opens displaying the current memory options
for the selected resource.
.keep 20
.figure *depth='2.20' *scale=65 *file='res12' In the Memory Options dialog, you change the memory options for the selected resource.
.step Select the required flags
in the Memory Options and Load Options sections of the dialog by
clicking on the check box or radio button beside the appropriate
field.
.result
Following is a description of each option.
.begpoint $break
.point Moveable
Selecting this option marks this resource to be stored in moveable
memory when it is loaded by an application.
.point Discardable
Selecting this option marks this resource to be stored in discardable
memory when it is loaded by an application.
.point Pure
When marked as Pure, Windows does not allow the memory into which the
resource is loaded to be modified.
.point Preload
All resources marked as Preload are loaded when the application for
which the resource is created is loaded.
.point LoadOnCall
Resources marked as LoadOnCall are loaded only when an application
issues a call to load them.
.endpoint
.endstep
.*
.endlevel
