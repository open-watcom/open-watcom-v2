.chap The &company String Editor
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'String Editor'
To group together text strings in your application, you can use string
tables. A string table is a list of strings such as error messages,
prompts, and other text you want your application to display. Each
string in the string table has a unique number, called an identifier,
associated with it. When the application presents the user with a
message, it searches for the appropriate identifier and displays the
associated string, regardless of the textual content of the string.
.np
The &company String Editor enables you to create string tables for your
application. The String Editor stores these tables in the resource file. You
can have multiple string tables in a resource file, which allows you to
logically group the text strings for your application. Since a string
table is simply another Windows resource that is separate from the
source code, you can use the String Editor to do such things as
translate your strings into another language or make your strings more
concise without having to change the source code.
.*
.section Using the String Editor
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting the String Editor
.bull
Quitting the String Editor
.bull
The String Editor Menu Bar
.bull
The String Editor Toolbar
.endbull
.*
.beglevel
.*
.section Starting the String Editor
.*
.np
.ix 'String Editor' 'start'
.ix 'start' 'String Editor'
The String Editor may be invoked in one of two ways. The first
involves first starting the Resource Editor then starting the String
Editor. The second way the editor may be started is from the &company IDE.
.np
To start the String Editor from the Resource Editor,
you must first open the Resource Editor.
How you proceed depends on whether you are
opening a new (empty) string table or an existing string table. The
String Editor works on resources found in resource files.
:CMT. This is a new screen snap shot
.figure *depth='3.20' *scale=53 *file='str1' The String Editor window
.*
.beglevel
.*
.section Creating a New String Table
.*
.ix 'String Editor' 'create string table'
.ix 'String Table' 'create'
.begstep To create a new string table:
.step Double click on the Resource Editor icon
in your Windows Program Manager.
.result
The &company Resource Editor window opens.
.step Open a new or existing resource file.
.result
.if &e'&dohelp eq 0 .do begin
Refer to the chapter entitled :HDREF refid='resedit'. for more
information.
.do end
.step Choose New
from the Resource menu.
.result
A cascading menu appears from the New menu item.
.step Choose String
from the New cascading menu.
.result
The &company String Editor window opens which contains an empty Strings list.
.endstep
.*
.section Opening an Existing String Table
.*
.ix 'String Table' 'open'
.ix 'String Editor' 'open existing table'
.begstep To edit the contents of an existing string table:
.step Double click on the Resource Editor icon
in your Windows Program Manager.
.result
The &company Resource Editor window opens.
.step Open an existing resource file.
.result
.if &e'&dohelp eq 0 .do begin
Refer to the chapter entitled :HDREF refid='resedit'. for more
information.
.do end
The Resource window for the existing resource file displays buttons
representing each existing resource type.
.step Select the string table button
to view all existing string tables.
.orstep Choose Menu
from the cascading menu that appears when you choose View from the
Resource menu.
.result
The String Tables list appears revealing all string tables in this resource file.
.step Double click on the existing string table you want to open.
.orstep Click on the desired string table
to select it, then
.bd choose Edit from the Resource menu
on the &company Resource Editor window.
.result
The &company String Editor window opens and displays the existing string
items for the selected string table.
.endstep
.*
.endlevel
.*
.section Quitting the String Editor
.*
.np
.ix 'String Editor' 'leave'
.ix 'leave' 'String Editor'
To exit the String Editor, choose Exit from the File menu of the &company String
Editor window.
.np
If you made changes to strings but did not save the information, a
message box prompts you to save before exiting. Choose Yes to save the
changes and exit the String Editor, No to exit the String Editor
without saving the changes, or Cancel to return to the String Editor.
.np
When you exit the String Editor, the application remembers the
directory in which you last opened or saved a string table (not the
resource file). The next time you open a string table, the last directory
accessed appears in the File Selection dialog so you do not have to
reenter its path. This feature makes the String Editor more convenient
and efficient to use.
.*
.section The String Editor Menu Bar
.*
:CMT. This is a new screen snap shot
.figure *depth='1.67' *scale=66 *file='str2' The String Editor menu bar
.np
.ix 'String Editor' 'Menu bar'
.ix 'Menu bar' 'String Editor'
The menu bar consists of the following five menus:
.begpoint $break
.point File
Clear and save the string table, and exit the String Editor
.point Edit
Access the clipboard functions, hide or show the toolbar, and delete a
string item
.point Resource
Change the string table memory flags and copy it into another resource file
.point String
Insert a new string item
.point Help
Access on-line help information
.endpoint
.np
To see the function of an item in the menu bar, position your cursor
over the item and hold down the mouse button. A description of the
item appears in the status line at the bottom of the screen. If you do
not want to select the menu item, drag the cursor off of the menu item
before releasing the mouse button.
.*
.section The String Editor Toolbar
.*
.np
.ix 'String Editor' 'toolbar'
In the &company String Editor window, the toolbar appears below the menu
bar. When the toolbar is shown choose Hide Toolbar from the Edit menu
to hide it. When the toolbar is hidden you can display it by choosing
Show Toolbar from the Edit menu.
:CMT. .figure *depth='4.86' *scale=180 *file='str2' You can access seven String Editor functions from the toolbar.
.figure *depth='0.88' *scale=76 *file='str3' You can access seven String Editor functions from the toolbar.
.np
The following explains the function each toolbar button performs,
as well as the equivalent menu item.
.begpoint
.point Clear
Clear the string table. This is equivalent to the item Clear in the
File menu.
.point Update
Update the resource file with this string table. This is equivalent to the
item Update in the File menu.
.point Cut
Cut the current string table to the clipboard. This is equivalent to
the item Cut in the Edit menu.
.point Copy
Copy the current string table to the clipboard. This is equivalent to
the item Copy in the Edit menu.
.point Paste
Paste the string table from the clipboard to the current string table.
This is equivalent to the item Paste in the Edit menu.
.point Insert
Insert the new string item into the string table. This is equivalent
to the item Insert New Item in the Strings menu.
.point Delete
Delete the selected string item from the string table. This is
equivalent to the item Delete in the Edit menu.
.endpoint
.np
To see the function of an item in the toolbar, position your cursor
over the desired button. A description of the button's function
appears in the status line at the bottom of the window.
.*
.endlevel
.*
.section Using the String Editor Window
.*
.np
.ix 'String Editor' 'window'
The &company String Editor window contains a menu bar, toolbar, and
status line to help you create and edit string items. The rest of the
window, the workspace, is divided into four sections:
.begbull $compact
.bull
String Text Field
.bull
String ID Field
.bull
Action Buttons
.bull
Strings List
.endbull
:CMT. .figure *depth='4.86' *scale=180 *file='str2' The &company String Editor window contains a menu bar, toolbar, workspace, and status line.
.figure *depth='3.20' *scale=53 *file='str4' The String Editor window contains a menu bar, toolbar, workspace, and status line.
.*
.beglevel
.*
.section String Text Field
.*
.np
The String Text field in the &company String Editor window contains the
text for the string item you are defining. Whatever appears in this
field will appear in your application.
.*
.section String ID Field
.*
.np
.ix 'String ID field'
.ix 'String Editor' 'String ID field'
The two String ID fields contains:
.begbull $compact
.bull
the identifier associated with the string item
.bull
the symbol information for the specified string
.endbull
.np
Creating a string item automatically creates a relationship between
the string item and its identifier, the String ID. In your source code
you reference the string item by its String ID.
.begnote
.note Note:
Both String ID fields currently contain the same numerical information
because the String Editor does not yet support symbols.
.endnote
.*
.section String Editor: Action Buttons
.*
.np
.ix 'Action Buttons'
.ix 'String Editor' 'Action Buttons'
The action buttons control how the information in the attributes area is
handled by the editor.
The Insert button will use the contents of the
attributes area to create a new entry in the string table.
The Change button will use the contents of the
attributes area to modify the current entry in the string table.
The Reset button will discard the contents of the
attributes area and reset them to those of the current entry in the
string table.
The Insert button is the default button when the ENTER key is pressed
and no other button has the focus.
.*
.section Strings List
.*
.np
.ix 'Strings List'
.ix 'String Editor' 'strings list'
The Strings list displays all of the string items in the current
string table. The string items appear in ascending numerical order by
String ID.
.*
.endlevel
.*
.section String Functions
.*
.np
.ix 'String functions'
.ix 'String Editor' 'string functions'
You can perform several functions with the String Editor. This section
describes the following tasks:
.begbull $compact
.bull
Adding a New String Item
.bull
Changing an Existing String Item
.bull
Deleting String Items
.bull
Saving String Items
.bull
Using Shortcuts to Create String Tables
.ix 'String Editor' 'shortcuts to create string tables'
.ix 'String Tables' 'shortcuts to creating'
.bull
Changing Memory Options
.endbull
.*
.beglevel
.*
.section Adding a New String Item
.*
.np
The steps for adding a new string item to the Strings list are the
same, regardless of whether the Strings list is empty or already
contains string items.
.begstep To add a new string item:
.step Enter the text
that you want to appear in the application in the String Text Field of
the &company String Editor window.
.result
The text that will be displayed in your application appears exactly as
you enter it in this field.
.step Enter symbol or numeric identifier
fo the string.
.result
The identifier you enter here is associated with the string
item. This is the identifier by which your source code references the
string.
.step Press the Insert action button
.ix 'Insert New Item'
.ix 'String Editor' 'insert new item'
.orstep Choose Insert New Item
from the Strings menu.
.orstep Select the Insert New Item button on the toolbar.
.result
The String Editor adds the new string item to the Strings list. The
editor automatically arranges the strings in order by String ID.
.endstep
.*
.section Changing an Existing String Item
.*
.np
.ix 'String Editor' 'change existing string'
.ix 'String Item' 'change existing'
You may want to change the text or identifier for a string item in the
Strings list.
.begstep To modify an existing sting table item:
.step Choose the string table you want to work with
from the Resource window on the &company Resource Editor window.
.result
The String Editor window appears displaying all of the string items
for the selected string table.
.step Click on the string item
from the Strings list that you want to modify.
.result
The selected string item appears highlighted in the Strings list.
.step Change the String Item or String ID field.
.step Press the Change action button
.result
These actions update the selected string item with the modified string
item details.
.endstep
.*
.section Deleting String Items
.*
.np
.ix 'String Editor' 'delete string items'
.ix 'String Items' 'delete'
.ix 'String Items' 'clear'
.ix 'String Editor' 'Clear option'
These are two functions you can use to delete string items from the
Strings list:
.begbull $compact
.bull
Delete
.bull
Clear
.endbull
.np
The Delete item in the Edit menu deletes the selected string item. You
can delete only one string item at a time with this function.
.np
The Clear item in the File menu deletes all string items in the
current string table. This is a powerful function and must be used
carefully to avoid accidentally deleting an entire String list.
.*
.beglevel
.*
.section Deleting a String Item
.*
.np
To delete string items from the current string table one at a time:
.begstep
.step Select the string item you want to delete
from the Strings table by clicking on it.
.result
The selected string item is highlighted.
.step Choose Delete
from the Edit menu
.orstep Select the Delete button from the toolbar.
.result
The String Editor removes the selected string item from the Strings list.
.step Choose Update
from the File menu
to save the string table changes.
.endstep
.*
.section Deleting All String Items
.*
.np
To delete all string items in the current string table:
.begstep
.step Choose Clear
from the File menu
.orstep Select the Clear button on the toolbar.
.result
A message box appears prompting you to verify that you want to clear
all of the string items from the current string table.
.step Choose Yes, No, or Cancel
to continue.
.result
Choosing Yes removes all string items from the current string table.
The Strings list is left empty.
.endstep
:CMT. .figure *depth='2.22' *scale=200 *file='str3' As a safety feature, a message box appears when you choose the Clear function.
.figure *depth='1.46' *scale=68 *file='str5' As a safety feature, a message box appears when you choose the Clear function.
.*
.endlevel
.*
.section Saving String Items
.*
.np
.ix 'String Editor' 'save string items'
.ix 'String Items' 'save'
There are three functions you can use to save string tables with the
String Editor:
.begbull $compact
.bull
Update
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
.section String Editor: Update
.*
.np
After modifying a string item in a string table, choose Update
from the File menu on the String Editor window to save the updated
string table to the current resource file. This prevents the changes from
being lost when you exit the String Editor.
.np
The Update function updates the resource file only at the resource
level. The resource file is not saved until you perform a save at the
Resource Editor level as well. Exiting the Resource Editor without
saving causes you to lose the string table changes made.
.*
.section String Editor: Save As
.*
.np
Choose Save As from the File menu on the String Editor window to save
the current string table to another resource file. A Save Menu dialog
appears where you specify the resource file to which you want to save the
current string table.
:CMT. .figure *depth='2.78' *scale=150 *file='str4' On the Save Menu dialog, specify the resource file to which you want to save the string table.
.figure *depth='2.24' *scale=65 *file='str6' On the Save Menu dialog, specify the resource file to which you want to save the string table.
.begnote
.note Important:
If the resource file to which you are saving the string table already
contains resources, the existing resources will be discarded and
replaced with the string table you are saving.
.endnote
.*
.section String Editor: Copy To
.*
.np
Choose Copy To from the Resource menu on the String Editor window to
copy the current string table into an existing resource file. A Copy To
dialog appears where you specify the resource file to which you want to copy
the current string table. Unlike the Save As function, copying a
string table to a resource file adds an entry to the list of resources
rather than replacing the existing resources with the new entry.
:CMT. .figure *depth='3.24' *scale=165 *file='str5' On the Copy To dialog, specify the resource file to which you want to copy the String list.
.figure *depth='2.24' *scale=65 *file='str7' On the Copy To dialog, specify the resource file to which you want to copy the String list.
.*
.endlevel
.*
.section Using Shortcuts to Create String Tables
.*
.np
.ix 'String Tables' 'shortcuts to creating'
.ix 'String Editor' 'shortcuts to creating string tables'
The Edit menu on the &company String Editor window contains some
standard Windows options that you can use to quickly create string
tables. The options are:
.begbull $compact
.bull
Cut
.bull
Copy
.bull
Paste
.endbull
.np
These options allow you to perform functions on a string table but not
on individual string items.
.begnote
.note Cut
Choose Cut from the Edit menu of the String Editor window to cut the
Strings list from the current string table and paste it to the
clipboard.
.note Copy
Choose Copy from the Edit menu of the String Editor window to copy the
String list in the current string table to the clipboard.
.note Paste
Choose Paste from the Edit menu of the String Editor window to copy
the string table on the clipboard into the current String list.
.endnote
.*
.section String Editor: Changing Memory Options
.*
.np
.ix 'Memory Options' 'in String Editor'
.ix 'String Editor' 'change memory options'
The Memory Flags option allows you to change the selected memory
options for the current string table. These options indicate the
memory type and loading instructions.
:CMT. This is a new screen snap shot
.figure *depth='1.89' *scale=66 *file='str8' Changing memory options
.begstep To set the memory flags for a string table:
.step Choose Memory Flags
from the Resource menu
of the &company String Editor window.
.result
The Memory Options dialog opens displaying either the current or
default memory options.
.step Select the required flags
in the Memory Options and Load Options sections of the dialog by
clicking on the check box beside the field.
.result
Following is a description of each option.
.begpoint $break
.point Moveable
Selecting this option marks this string table to be stored in moveable
memory when it is loaded by an application.
.point Discardable
Selecting this option marks this string table to be stored in
discardable memory when it is loaded by an application.
.point Pure
When marked as Pure, Windows will not allow the memory that the string
table is loaded into to be modified.
.point Preload
All string tables marked as Preload will be loaded when the
application for which you are creating the string table is loaded.
.point LoadOnCall
String tables marked as LoadOnCall will be loaded only when an
application issues a call to load them.
.endpoint
.step Click on OK
when all memory option selects are made.
.result
The Memory Options dialog closes returning you to the String Editor window.
.endstep
.*
.endlevel
.*
