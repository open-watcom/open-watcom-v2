.chap The &company Accelerator Editor
.*
.ix 'Accelerator Editor'
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
The Accelerator Editor enables you to create and edit accelerator
items so you can invoke menu items without using a mouse. Accelerators
define hot keys for menu items in your application. These enhance the
user interface for more advanced users of the application.
.np
Accelerators are keyboard equivalents for menu items, such as Ctrl+S
or F2. For example, many applications associate the accelerator Ctrl+C
with the Copy item in the Edit menu.
.book The Windows Interface: An Application Design Guide
suggests some standard accelerators.
.np
An accelerator item is made up of a key value, menu item identifier,
and flags. These components define the key combination the user
presses to access the specified menu item. The Accelerator Editor
stores the accelerator items in an accelerator table. You can have
multiple accelerator tables for a resource file.
.np
The accelerator editor allows you to create new accelerators and edit
existing accelerators without having to create a resource script or
use a resource compiler. Without the accelerator editor, you would
have to use the resource compiler to add accelerators to your
applications.
.*
.section Using the Accelerator Editor
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting the Accelerator Editor
.bull
Quitting the Accelerator Editor
.bull
The Accelerator Editor Menu Bar
.bull
The Accelerator Toolbar
.endbull
.*
.beglevel
.*
.section Starting the Accelerator Editor
.*
.np
.ix 'Accelerator Editor' 'start'
.ix 'start' 'Accelerator Editor'
The Accelerator Editor may be invoked in one of two ways. The first
involves first starting the Resource Editor then starting the Accelerator
Editor. The second way the editor may be started is from the &company IDE.
.np
To start the Accelerator Editor from the Resource Editor,
you must first open the Resource Editor.
How you proceed depends on whether you are
opening a new (empty) accelerator table or an existing accelerator
table.
:CMT. This is a new screen snap shot
.figure *depth='3.20' *scale=56 *file='acc1' The Accelerator Editor window
.*
.beglevel
.*
.section Creating a New Accelerator Table
.*
.ix 'Accelerator Editor' 'create new table'
.begstep To create a new accelerator table:
.step
.bd Double click on the Resource Editor icon
in your Windows Program Manager.
.result
The &company Resource Editor window opens.
.step Open a new or existing resource file.
.if &e'&dohelp eq 0 .do begin
Refer to the chapter entitled :HDREF refid='resedit'. for more
information.
.do end
.step Choose Accelerator
from the cascading menu that appears when you choose New from the
Resouce menu.
.result
The &company Accelerator Editor window opens and displays an empty
accelerator item list.
.endstep
.*
.section Opening an Existing Accelerator Table
.*
.ix 'Accelerator Editor' 'open existing table'
.begstep To open an existing accelerator table:
.step Double click on the Resource Editor icon
in your Windows Program Manager.
.result
The &company Resource Editor window opens.
.step Open an existing resource file.
.if &e'&dohelp eq 0 .do begin
Refer to the chapter entitled :HDREF refid='resedit'. for more
information on the steps involved in opening a resource file.
.do end
.result
The Resource window for the existing resource file displays buttons
representing each existing resource type.
.step Select the accelerator table button
to view the existing accelerator tables.
.orstep Choose Accelerator
from the cascading menu that appears when you choose View from the
Resource menu.
.result
The Accelerator Resources list appears revealing all accelerator
tables in this resource file.
.step Double click on the existing accelerator table
you want to open.
.orstep Click on the desired accelerator table
to select it, then
.bd press ENTER
.orstep Click on the desired accelerator table to select it and choose Edit from the Resource menu
on the &company Resource Editor window.
.result
The &company Accelerator Editor window opens and displays the existing
accelerator items for the selected accelerator table.
.endstep
.*
.endlevel
.*
.section Quitting the Accelerator Editor
.*
.np
.ix 'Accelerator Editor' 'leave'
.ix 'leave' 'Accelerator Editor'
To exit the Accelerator Editor, choose Exit from the File menu of the
&company Accelerator Editor window.
.np
If you made changes to accelerator items but did not save the
information, a message box prompts you to save before exiting. Choose
Yes to save the changes and exit, No to exit without saving the changes,
or Cancel to return to the &company Resource Editor window.
.np
When you exit the Accelerator Editor, the application remembers the
directory in which you last opened or saved an accelerator table (not
the resource file). The next time you open an accelerator
table, the last directory accessed appears in the File Selection dialog
so you do not have to reenter its path. This feature makes the
Accelerator Editor more convenient and efficient to use.
.*
.section The Accelerator Editor Menu Bar
.*
:CMT. This is a new screen snap shot
.figure *depth='3.20' *scale=56 *file='acc2' The Accelerator Editor menu bar
.np
.ix 'Accelerator Editor' 'menu bar'
.ix 'menu bar' 'Accelerator Editor'
The menu bar consists of the following five menus:
.begpoint
.point File
Clear and save the accelerator table, and exit the Accelerator Editor
.point Edit
Access the clipboard functions, hide or show the toolbar, and delete
an accelerator item
.point Resource
Rename the accelerator table and change its memory flags
.point Accelerator
Insert and edit accelerator items
.point Help
Access on-line help information
.endpoint
.np
To see the function of an item on the menu bar or submenus, hold the
mouse button and drag the cursor over the desired menu item. A
description of the item appears in the status line at the bottom of
the screen. If you do not want to select the menu item, drag the
cursor off of the menu item before releasing the mouse button.
.*
.section The Accelerator Editor Toolbar
.*
.np
.ix 'Accelerator Editor' 'toolbar'
In the &company Accelerator Editor window, the toolbar appears below the
menu bar. When the toolbar is shown choose Hide Toolbar from the
Edit menu to hide it.  When the toolbar is hidden you can display
it by choosing Show Toolbar from the Edit menu.
.figure *depth='0.45' *scale=100 *file='acc3' The Accelerator Editor toolbar
.np
The following explains the function each toolbar button performs, as
well as the equivalent menu item.
.begnote
.note CLEAR
Clear the accelerator table. This is equivalent to the Clear item in the File
menu.
.note UPDATE
Update the resource file with this accelerator table. This is equivalent to
the Update item in the File menu.
.note CUT
Cut the current accelerator table to the clipboard. This is equivalent
to the Cut item in the Edit menu.
.note COPY
Copy the current accelerator table to the clipboard. This is
equivalent to the Copy item in the Edit menu.
.note PASTE
Paste the accelerator table from the clipboard to the current
accelerator table. This is equivalent to the Paste item in the Edit menu.
.note INSERT
Insert the new accelerator item into the accelerator table. This is
equivalent to the Insert New Item item in the Accelerator menu.
.note DELETE
Delete the selected accelerator item from the accelerator table. This
is equivalent to the Delete item in the Edit menu.
.note KEY VALUE
Set the accelerator item's key value using the keyboard. This is
equivalent to the Key Value item in the Accelerator menu.
.endnote
.np
To see the function of a button on the toolbar, position your cursor
over the desired icon. A description of the button's function appears
in the status line at the bottom of the window.
.endlevel
.*
.section Using the Accelerator Editor Window
.*
.np
.ix 'Accelerator Editor' 'using the window'
The &company Accelerator Editor window contains a menu bar, toolbar,
and status line to help you create and edit accelerators. The rest of
the window, the workspace, is divided into four sections:
.begbull $compact
.bull
Resource Name Field
.bull
Attributes Area
.bull
Accelerator Item List
.bull
Action Buttons
.endbull
:CMT. .figure *depth='4.86' *scale=180 *file='acc3' The Accelerator Editor window contains a menu bar, toolbar, status line, and workspace.
.figure *depth='3.20' *scale=56 *file='acc4' The Accelerator Editor window contains a menu bar, toolbar, status line, and workspace.
.*
.beglevel
.*
.section Accelerator Editor: Resource Name Field
.*
.np
.ix 'Resource Name field' 'in Accelerator Editor'
The Resource Name field in the &company Accelerator Editor window
contains the name of the accelerator table currently being edited. A
resource file can contain more than one accelerator table, so it is
important that the resource names for the accelerator tables be
unique. The Resource Name field is not case sensitive and can hold up
to 255 characters. You can enter a textual name, a number or
a symbol in this field.
.np
When you create a new accelerator table, the Resource Editor assigns
the default name ACCELERATOR_# to it, where # is a decimal number
starting at one and increasing for each new resource of that type
opened. You can rename the accelerator table using the Rename item
in the Resource menu.
.*
.section Accelerator Editor: Attributes Area
.*
.np
.ix 'Accelerator Editor' 'Attributes Area'
.ix 'Attributes Area'
The attributes area provides information about the currently selected
accelerator item. You can edit accelerator items using only the fields
in this area.
:CMT. This is a new screen snap shot
.figure *depth='2.13' *scale=70 *file='acc5' The Accelerator Editor attributes area
.np
Accelerator item attributes are displayed in four sections. Following
is an explanation of each section.
.begnote
.note Key
The Key field contains the key value used to activate the menu item.
.note Menu Item ID
The two Menu Item ID fields contain:
.begbull $compact
.bull
the number of the menu item that is activated by the entry in the
Key field.
.bull
the symbol information for the selected menu item.
.endbull
.note Key Type
This section indicates if the key value is in ASCII or virtual key.
.note Modifiers
An X appears in the check box for the modifiers that are part of the
accelerator key value.
The available modifier keys are:
.begbull $compact
.bull
Alt
.bull
Control
.bull
Shift
.endbull
.endnote
.np
The option Flash Top-level Menu Item indicates that when the specified
key value is used, the Accelerator Editor briefly highlights the
corresponding menu bar item.
.*
.section Accelerator Editor: Accelerator Item List
.*
.np
.ix 'Accelerator Item List'
.ix 'Accelerator Editor' 'Item list'
The accelerator item list displays all of the accelerator items in the
current accelerator table. Use this list to view the attributes of the
accelerator items. Select an accelerator item from the list by
clicking on it. Details of the selected accelerator item appear in the
attributes area of the window.
:CMT. This is a new screen snap shot
.figure *depth='1.76' *scale=57 *file='acc6' The Accelerator Editor item list
.*
.section Accelerator Editor: Action Buttons
.*
.np
.ix 'Action Buttons'
.ix 'Accelerator Editor' 'Action Buttons'
The action buttons control how the information in the attributes area is
handled by the editor.
The Insert button will use the contents of the
attributes area to create a new entry in the table.
The Change button will use the contents of the
attributes area to modify the current entry in the table.
The Reset button will discard the contents of the
attributes area and reset them to those of the current entry in the table.
The Insert button is the default button when the ENTER key is pressed
and no other button has the focus.
:CMT. This is a new screen snap shot
.figure *depth='0.43' *scale=67 *file='acc7' The Accelerator Editor action buttons
.*
.endlevel
.*
.section Accelerator Functions
.*
.np
You can perform several functions with the Accelerator Editor. This
section describes the following tasks:
.begbull $compact
.bull
Adding a New Accelerator Item
.bull
Changing an Existing Accelerator Item
.bull
Deleting Accelerator Items
.bull
Renaming an Accelerator Table
.bull
Saving Accelerator Items
.bull
Using Shortcuts to Create Accelerator Tables
.bull
Changing Memory Options
.endbull
.*
.beglevel
.*
.section Adding a New Accelerator Item
.*
.np
.ix 'Accelerator Editor' 'adding a new item'
.ix 'add' 'new Accelerator Item'
The steps for adding a new accelerator item to the accelerator table
are the same, regardless of whether the accelerator table is empty or
already contains accelerator items.
.begstep To add a new accelerator item:
.step Position the cursor in the Key field.
.step Enter the desired key value
that will activate the menu item.
.result
You can enter a single key, such as a letter or number, or a key
sequence. A key sequence may be made up of a modifier and a key, such
as ^k for Ctrl+k.
.step Enter a symbol or numeric identifier
for the menu item
that is activated by the entry in the Key field.
.result
The symbol or numeric identifier you enter here is the identifier
associated with the menu item.
This relationship is defined in the Menu Editor. If you do not
know the desired menu item identifier, open the appropriate menu and
look up the identifier. It is not possible for the Accelerator Editor to
get this information for you.
.step Specify ASCII or Virtual Key
in the Key Type section of the window.
.begnote
.note ASCII Keys
.ix 'ASCII keys'
When the Key Type specified is ASCII, keys you press appear as
characters, such as
.us a, B, 2,
or
.us $.
In the Key field, the ASCII characters are surrounded by quotation
marks. For example,
.us c
appears as
.us "c"
and
.us Ctrl+C
appears as
.us "^C".
When using ASCII characters as accelerators, you may combine them with
the Ctrl key to form key sequences, such as Ctrl+X or Ctrl+P. There is
no symbol to represent the Shift key in ASCII. Applying the Shift key
with an ASCII character results in an upper case character in the Key
field. For example,
.us Shift+d
appears as
.us "D".
:cmt. To apply the Alt key to an ASCII character, mark the appropriate check
:cmt. box in the Modifiers section of the attributes area.
.note Virtual Keys
.ix 'Virtual Keys'
When the Key Type specified is Virtual Key, you enter the standard
Windows identifier for the desired key. For example, the Windows
identifier for the Home key is
.us VK_HOME.
The header file WINDOWS.H defines these identifiers. To avoid looking
up these identifiers yourself, use the Key Value function explained in
the section
.us Editing the Key Value.
:cmt. There is no virtual key equivalent for the Alt, Ctrl,
:cmt. and Shift keys.
To specify whether the Alt, Ctrl, or Shift keys must be pressed with
another key to form a key sequence, mark the appropriate check box in
the Modifiers section of the attributes area.
.endnote
.step Select the desired modifiers
in the Modifiers section.
.result
Modifiers are flags indicating keys that you must press in addition to
the specified key value to activate the menu item. When the ASCII
option is selected, the Control and Shift modifiers are not available.
.step Press the Insert action button
.orstep Choose Insert New Item
from the Accelerator menu.
.orstep Select the Insert New Item button on the toolbar.
.result
The Accelerator Editor adds the new accelerator item to the
accelerator table. When accelerator items exist in the accelerator
table, one item is always selected. Choosing the Insert New Item menu
item or button positions the new accelerator item directly after the
selected accelerator item in the list.
.endstep
.*
.section Changing an Existing Accelerator Item
.*
.np
.ix 'Accelerator Editor' 'change existing item'
.ix 'Accelerator Item' 'change existing'
You may want to change the key value or the menu item identifier
associated with an accelerator item.
.begstep To modify an existing accelerator item:
.step Click the accelerator item you want to modify.
.result
The selected accelerator item appears highlighted in the accelerator
item list and the accelerator item details appear in the attributes area
of the window.
.step Enter the new accelerator item details
in the attributes area.
.step Press the Change action button
.result
These actions update the selected accelerator item with the modified
accelerator item attributes.
.endstep
.*
.beglevel
.*
.section Editing the Key Value
.*
.np
.ix 'Key Value' 'edit'
.ix 'Accelerator Editor' 'edit the key value'
When editing only the key value of an accelerator item, use the Key
Value function. With this function, the Accelerator Editor looks up
the correct virtual key value for the selected key and inserts it into
the accelerator item list for you. This is a quick way to make
accelerator item changes, but it only applies to accelerator items
whose key type is Virtual Key.
.begstep To use the Key Value function:
.step Choose the accelerator table you want to work with
from the Resource window on the &company Resource Editor window.
.result
The Accelerator Editor window appears displaying all of the
accelerator items for the selected accelerator table.
.step Click on the accelerator item you want to modify.
.result
The selected accelerator item appears highlighted in the accelerator
table and the accelerator item details appear in the attributes area
of the window.
.step Choose Select Key Value
from the Accelerator menu
.orstep Select the Key Value button on the toolbar.
.result
The accelerator table disappears and instructions for using the Key
Value function appear in its place.
.step Press the desired key on the keyboard.
.result
This is the key that you want to activate the menu item specified in
the Menu Item field.
The instructions disappear from the window revealing the modified
accelerator table. The updated accelerator item is still selected.
.step Change modifiers,
if required.
.endstep
.*
.endlevel
.*
.section Deleting Accelerator Items
.*
.np
There are two functions you can use to delete accelerator items from
the accelerator table:
.begbull $compact
.bull
Delete
.bull
Clear
.endbull
.np
The Delete item in the Edit menu deletes the selected accelerator
item from the accelerator item list. You can delete only one
accelerator item at a time with this function.
.np
The Clear item in the File menu deletes all accelerator items in
the current accelerator table. This is a powerful function and must be
used carefully to avoid accidentally deleting all accelerator items.
.*
.beglevel
.*
.section Deleting an Accelerator Item
.*
.np
.ix 'Accelerator Editor' 'delete item'
.ix 'Accelerator Item' 'delete'
To delete accelerator items from the current accelerator table one at
a time:
.begstep
.step Select the accelerator item you want to delete
from the accelerator item list by clicking on it.
.result
The selected accelerator item is highlighted.
.step Choose Delete
from the Edit menu
.orstep Select the Delete button from the toolbar.
.result
The Accelerator Editor removes the selected item from the accelerator
item list.
.step Choose Update
from the File menu
to save the accelerator table changes.
.endstep
.*
.section Deleting All Accelerator Items
.*
.np
To delete all accelerator items in the current accelerator table:
.begstep
.step Choose Clear
from the File menu
.orstep Select the Clear button from the toolbar.
.result
A message box appears prompting you to verify that you want to clear
all accelerator items from the current accelerator table.
.step Choose Yes or No
to continue.
.result
Choosing Yes removes all accelerator items from the current
accelerator table. The accelerator item list is empty. Choosing
No cancels the operation.
.endstep
.figure *depth='1.46' *scale=88 *file='acc8' As a safety feature, a message box appears when you choose the Clear function.
.*
.endlevel
.*
.section Renaming an Accelerator Table
.*
.np
.ix 'Accelerator Table' 'rename'
.ix 'Accelerator Editor' 'rename table'
When you create an Accelerator Table, the Resource Editor assigns a
default name to it. Assign a new name to the accelerator table using
the Rename function.
.begstep To rename an accelerator table:
.step Choose Rename
from the Resource menu on the &company Accelerator Editor window.
.orstep Double click or right click on the Resource Name field.
.result
A Rename Resource dialog box appears containing two fields:
.begbull $compact
.bull
Old Name
.bull
New Name
.endbull
.figure *depth='1.31' *scale=72 *file='acc9' In the Rename Resource dialog, you enter the new name of the accelerator table.
.step Enter the desired accelerator table name
in the New Name field.
.step Click on OK
to close the Rename Resource dialog.
.result
The dialog closes and the new accelerator table name appears in the
Resource Name field.
.endstep
.*
.section Saving Accelerator Items
.*
.np
There are two functions you can use to save accelerator tables with
the Accelerator Editor:
.begbull $compact
.bull
Update
.ix 'Update'
.ix 'Accelerator Editor' 'update'
.bull
Save As
.bull
Copy To
.endbull
.np
Each of these options performs a different type of save function. This
section describes how to use each function.
.*
.beglevel
.*
.section Accelerator Editor: Update
.*
.np
After modifying an accelerator item in an accelerator table, choose
Update from the File menu on the Accelerator Editor window to
save the updated accelerator table to the current resource file. This
prevents the changes from being lost when you exit the Accelerator
Editor.
.np
The Update function updates the resource file only at the resource
level. The resource file is not saved until you perform a save at the
Resource Editor level as well. Exiting the Resource Editor without
saving causes you to lose the accelerator table changes made.
.*
.section Accelerator Editor: Save As
.*
.np
Choose Save As from the File menu on the Accelerator Editor window to
save the current accelerator table to another resource file.
.np
If you added any new symbol names (Item ID's), a Save Symbol Header
File dialog  appears which allows you to create a new header file which
includes the newly added symbol names.
.figure *depth='2.24' *scale=66 *file='acc10' On the Save Symbol Header File dialog, specify the header file to which you want to save symbol ID's.
.np
Afterwards, a Save Accelerator Table dialog appears where you specify
the resource file to which you want to save the current accelerator
table.
.figure *depth='2.24' *scale=66 *file='acc11' On the Save Accelerator Table dialog, specify the resource file to which you want to save the accelerator table.
.begnote
.note Important:
If the resource file to which you are saving the accelerator table already
contains resources, the existing resources will be discarded and
replaced with the accelerator table you are saving.
.endnote
.*
.section Accelerator Editor: Copy To
.*
.np
Choose Copy To from the Resource menu of the Accelerator Editor window
to copy the current accelerator table into an existing resource file.
.np
If you added any new symbol names (Item ID's), a Save Symbol Header
File dialog  appears which allows you to create a new header file which
includes the newly added symbol names.
.figure *depth='2.24' *scale=66 *file='acc10' On the Save Symbol Header File dialog, specify the header file to which you want to save symbol ID's.
.np
Afterwards, a Copy To dialog appears where you specify the resource
file to which you want to copy the current accelerator table. Unlike
the Save As function, copying an accelerator table to a resource file
adds an entry to the list of resources rather than replacing the
existing resources with the new entry.
.figure *depth='2.24' *scale=66 *file='acc12' On the Copy To dialog, specify the resource file to which you want to copy the accelerator table.
.*
.endlevel
.*
.section Using Shortcuts to Create Accelerator Tables
.*
.np
.ix 'Accelerator Editor' 'shortcuts to create tables'
.ix 'Accelerator Tables' 'shortcuts to create'
The Edit menu on the &company Accelerator Editor window contains some
standard Windows options that you can use to quickly create
accelerator resources. The options are:
.begbull $compact
.bull
Cut
.bull
Copy
.bull
Paste
.endbull
.np
These options allow you to perform functions on accelerator tables but
not on individual accelerators items.
.begnote
.note Cut
Choose Cut from the Edit menu of the Accelerator Editor
window to cut the accelerator item list from the current accelerator
table and place it on the clipboard.
.note Copy
Choose Copy from the Edit menu of the Accelerator Editor window to
copy the accelerator item list in the current accelerator table
to the clipboard.
.note Paste
Choose Paste from the Edit menu of the Accelerator Editor window to
copy the accelerator item list on the clipboard into the current
accelerator table.
.endnote
.*
.section Accelerator Editor: Changing Memory Options
.*
.np
.ix 'Accelerator Editor' 'change memory options'
The Memory Flags option allows you to change the selected memory
options for the current accelerator table. These options indicate the
memory type and loading instructions.
.begstep To set the flags for an accelerator table resource:
.step Choose Memory Flags
from the Resource menu
.ix 'Memory Flags'
of the &company Accelerator Editor window.
.result
The Memory Options dialog opens displaying either the current or
default memory options.
.figure *depth='1.89' *scale=67 *file='acc13' On the Memory Options dialog, you change the memory options for the accelerator table.
.step Select the required flags
in the Memory Options and Load Options sections of the dialog by
clicking on the check box beside the field. Following is a description
of each option.
.begpoint $break
.point Moveable
Selecting this option marks this accelerator table to be stored in
moveable memory when it is loaded by an application.
.point Discardable
Selecting this option marks this accelerator table to be stored in
discardable memory when it is loaded by an application.
.point Pure
When marked as Pure, Windows will not allow the memory that the
accelerator table is loaded into to be modified.
.point Preload
All accelerator tables marked as Preload will be loaded when
the application for which you are creating the accelerator table is loaded.
.point LoadOnCall
Accelerator tables marked as LoadOnCall will only be loaded when an
application issues a call to load them.
.endpoint
.step Click on OK
when all memory option selections are made.
.result
The Memory Options dialog closes returning you to the Accelerator
Editor window.
.endstep
.*
.endlevel
