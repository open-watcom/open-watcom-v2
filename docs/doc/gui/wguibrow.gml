.chap The &brname
.*
:CMT. :helplvl level=3.
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&br'
Imagine being assigned to a project with thousands of lines of C++
source code.
Learning the relationship between the objects of such an application
can be very difficult.
The &br was developed to solve this very problem &mdash. to help
developers better understand the source code they are working with.
Information such as the class inheritance hierarchy and the call tree
for an application can be studied while using the &br..
Once you have located a symbol, you can quickly view the source file
that contains its definition or list all the files that reference it.
Viewing these relationships helps you understand how the program works.
.np
This chapter describes the many tasks you can perform with the &br,
including creating and opening a Browser database file, configuring
the Browser session, and viewing the list of all symbols in your
program, the class inheritance hierarchy, and call structure.
.*
.section Using the &br
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting the &br
.bull
Starting the &br through the &wide
.bull
Quitting the &br
.bull
The &br Menu Bar
.endbull
.*
.beglevel
.*
.section Starting the &br
.*
.np
.ix '&br' 'start'
.ix 'start' '&br'
To start the &br, double click on the &br icon.
This opens the &br window.
The caption bar of this window displays the current option file and
Browser database file.
The message
.mono no browser file
appears on this line if a database file is not currently selected.
.np
The &br is an integrated tool that you can open from the &vip so that
you can browse the source code of the project you are working on.
Refer to the &vip guide for further information on the &ide..
.*
.section *refid=brquit Quitting the &br
.*
.np
.ix '&br' 'leave'
.ix 'leave' '&br'
To exit the &br, choose
.us Exit
from the
.us File
menu of the &br window.
If you made changes to the options during the &br session, a message
box appears prompting you to save the changes.
.np
Choose
.ul
No
in the message box to close the &br session without saving.
Any options changed within the session are lost.
.np
Choose
.ul
Yes
in the message box to save the current options to the current option
file, if one exists, and exit the &br session.
If no option file exists, a
.us Save As
dialog appears that allows you to specify the option file to which you
want to save the new options.
.onestep
.step Select
.us Cancel
to return to the main &br window without saving the options.
.orstep Select
.us OK
to close the dialog and exit the &br session.
.endstep
.*
.section The &br Menu Bar
.*
.np
.ix '&br' 'Menu Bar'
The Browser's menu bar consists of the following eight menus:
.begpoint $break
.point File
Create, open, and configure Browser files; save and load options
.point View
Choose a global view for the current database file
.point Detail
View detailed information for a symbol
.point Tree
Choose the nodes to display for a tree view
.point Locate
Find symbols within the current global view
.point Options
Configure the &br session
.point Windows
Select from the list of &br windows currently open
.point Help
Access on-line help information
.endpoint
.*
.endlevel
.*
.section Browser Files
.*
.np
.ix '&br' 'files'
.ix 'module file' 'in &br'
To browse your source code, you must first create a &br module
file for each source file that you wish to browse.
Currently, browsing is supported by the &watc compilers only
(&watf does not support browsing).
To create a &br module file, specify the "db" option when you
compile the source file.
If you are using the &wide, select
.us Emit Browser Information
from the
.us Debugging Switches
panel of the
.us Compiler Switches
dialog.
The &br module file will have the same name as the source file and
an extension of ".MBR".
.np
Once the &br module files have been created, these files are merged
and a &br database file is created.
This process eliminates redundant information such as duplicate
definitions that occur when a header file is included by many source
files.
Browser database files have the extension ".DBR".
.np
The &br allows you to configure your session and saves this
configuration to an options file.
Options files have the extension ".OBR".
The file name
.fi setup.obr
is the default name of the options file.
The default options file is automatically loaded by the Browser when
the Browser is started.
.*
.beglevel
.*
.section Creating a Browser Database File within the Browser
.*
.ix '&br' 'create database file'
.ix 'database file' 'create in &br'
.begstep
.step Choose
.us New
from the
.us File
menu.
.result
This opens the
.us New Browser File
dialog where you enter the name of the Browser database file you are
creating.
.figure *depth='1.32' *scale=56 *file='brow1' Use the New Browser File dialog to create a new Browser database file.
.step Type the name and path
of the new Browser database file.
.orstep Click on the
.us Files
button to open a second dialog that allows you to browse the directory
structure for an existing Browser database file.
Select an existing file and change its name.
If you do not change its name, a message box appears when you press
.us OK
asking if you want to overwrite the
existing file.
.step Click on
.us OK.
.result
This opens a
.us Module
window that will eventually contain a list of the module files that
will make up the database file.
.step Click on the
.us Add
button in the
.us Module
window.
.result
This opens the
.us Select Module File(s)
dialog where you choose the module files to add to the database file.
.step Select the module files
to add to the database file and click on
.us OK.
.orstep Double click
on the desired module file.
.result
This closes the
.us Select Module File(s)
dialog and adds the selected module file to the Browser file component
list.
Each module file on the components list has a check box.
When added, this check box is marked with an X, indicating that the
module file is enabled.
.step Continue with steps 4 and 5
until you have added all desired module files to the component list.
.figure *depth='2.32' *scale=52 *file='brow2' The Modules dialog displays the selected module files.
.step Click on
.us OK
on the
.us Module
window.
.result
This closes the
.us Module
window and creates the database file.
.endstep
.*
.section Creating a Browser Database File from the Command Line
.*
.np
.ix '&br' 'create database file from command line'
.ix 'database file' 'create from command line'
It is also possible to create the Browser database file from the
command line.
This allows you to make Browser database file creation
part of your standard build procedure.
When you do this, the batch build
procedure will automatically update your database file.
Updating the Browser database file occurs only if changes have been
made to a module file within the database file.
.np
.ix '&br' 'merger utility'
.ix 'merger' 'with &br'
A separate utility, called the merger, is used to create the database
file.
The name of the merger program is
.fi wbrg.exe.
.ix 'MBR files'
Its command line consists of the name of the database file and a list
of the module files (.MBR files) to be merged.
The name of the database file must be preceded by a
.mono database
command.
.ix 'DBR files'
The default extension given to the database file is ".DBR".
The list of module files must be preceded by a
.mono file
command.
The module file names must be separated by commas or enclosed by curly
braces and separated by spaces.
The list of module file names can contain wild cards.
The following are examples of valid merger commands.
In each case, the module files
.fi m1.mbr
and
.fi m2.mbr
will be processed and the database file
.fi db.dbr
will be created.
.millust begin
wbrg database db file m1, m2
wbrg database db file { m1 m2 }
wbrg file m1, m2 database db
wbrg file { m1 m2 } database db
.millust end
.np
It is also possible to specify a command file that contains merger
commands.
.ix 'CBR files'
Command files have the extension ".CBR".
Consider a command file, called
.fi merge.cbr,
containing the following merger commands.
.millust begin
database db
file m1
file m2
.millust end
.np
The following example will achieve the same results as the previous
example.
.millust begin
wbrg @merge
.millust end
.np
.ix 'merger' 'quiet option'
.ix 'quiet option'
If you want to suppress the listing of file names that the merger
produces as it is working, you can include the "quiet" option on the
command line or in the command file.
.*
.section Opening an Existing Browser Database File
.*
.ix '&br' 'open database file'
.ix 'database file' 'open in &br'
.ix 'open' 'database file in &br'
.begstep
.step Choose
.us Open
from the
.us File
menu.
.result
This opens the
.us Open Browser Database File
dialog where you select the database file you want to open.
.step Select a database file to open and click on
.us OK.
.orstep Double click
on the desired database file.
.result
This closes the
.us Open Browser Database File
dialog.
.begnote
.mnote Note:
You can have only one database file open at a time.
If you open a second database file, the &br discards the first and
displays information for the second.
.endnote
.endstep
.*
.section Working with Browser Module Files
.*
.np
.ix 'Module file' 'in &br'
.ix '&br' 'working with module files'
Once a Browser database file is loaded, you might want to edit the list
of modules originally used to create the database file.
Editing the list of modules allows you to temporarily remove modules
from the database or add modules to the database.
For example, you may decide that you only want to browse a particular
module or set of modules or you may have forgotten a module when the
database was originally created.
.np
The
.us Modules
menu item in the
.us File
menu allows you to view the list of module files that make up the
current database file.
From this list you can perform the following functions on the module
files:
.begbull $compact
.bull
Add
.bull
Remove
.bull
Disable
.bull
Enable
.endbull
.figure *depth='2.32' *scale=52 *file='brow3' On the Modules dialog, you can add, remove, disable, and enable module files.
.*
.beglevel
.*
.section Adding a Module File
.*
.ix '&br' 'add Module file'
.ix 'Module file' 'add in &br'
.begstep
.step Choose
.us Modules
from the
.us File
menu.
.result
This opens the
.us Modules
dialog for the current database file.
This dialog lists all module files that make up the database file.
Each module file has a check box.
An X in this box indicates that the module file is enabled.
The &br browses only the enabled module files.
.step Click on the
.us Add
button in the
.us Modules
dialog.
.result
This opens the
.us Select Module File(s)
dialog where you choose the module files to add to the current Browser
file.
.step Select the module files
to add to the Browser file and click on
.us OK.
Select multiple module files by holding the Shift key while
you click on the desired files.
.result
Clicking on OK closes the
.us Select Module File(s)
dialog and adds the selected module file to the database file.
Each module file in the module list has a check box.
When added, this check box is marked with an X, indicating that the
module file is enabled.
.step Continue with steps 2 and 3
until you have added all desired module files to the component list.
.step Click on
.us OK
in the
.us Module
dialog.
.result
This closes the
.us Modules
dialog and updates the database file.
.endstep
.*
.section Removing a Module File
.*
.begstep
.step Choose
.us Modules
from the
.us File
menu.
.result
This opens the
.us Modules
dialog for the current database file.
This dialog lists all module files that make up the database file.
Each module file has a check box.
An X in this box indicates that the module file is enabled.
The &br browses only the enabled module files.
.step Click once on the module file
you want to remove from the database file.
.result
This highlights the selected module file.
.step Click on the
.us Remove
button in the
.us Modules
dialog.
.result
This removes the selected module file from the list.
.step Click on
.us OK
in the
.us Modules
dialog.
.result
This closes the
.us Modules
dialog and updates the database file.
.endstep
.*
.section Disabling a Module File
.*
.ix '&br' 'disable module file'
.ix 'Module file' 'disable in &br'
.begstep
.step Choose
.us Modules
from the
.us File
menu.
.result
This opens the
.us Modules
dialog for the current database file.
This dialog lists all module files that make up the database file.
Each module file has a check box.
An X in this box indicates that the module file is enabled.
The &br browses only the enabled module files.
.step Click in the check box
of the module file you want to disable.
Alternatively, use the up and down arrow keys to select the module file
you wish to disable.
Press the space bar to disable the currently selected module file.
.result
This removes the X.
The blank box indicates that the module file is disabled and will not
be browsed.
.step Repeat step 2
until you have disabled all desired module files.
.step Click on
.us OK
on the
.us Modules
dialog.
.result
This closes the
.us Modules
dialog and updates the database file.
.endstep
.np
To disable all of the module files in the
.us Modules
dialog, click on the
.us Disable All
button.
Click on
.us OK
to update the database file and close the
.us Modules
dialog.
.*
.section Enabling a Module File
.*
.begstep
.step Choose
.us Modules
from the
.us File
menu.
.result
This opens the
.us Modules
dialog for the current database file.
This dialog lists all module files that make up the database file.
Each module file has a check box.
An X in this box indicates that the module file is enabled.
The &br browses only the enabled module files.
.step Click in the check box
of the module file you want to enable.
Alternatively, use the up and down arrow keys to select the module file
you wish to enable.
Press the space bar to enable the currently selected module file.
.result
This places an X in the box indicating that the module file is enabled
and will be browsed.
.step Repeat step 2
until you have enabled all desired module files.
.step Click on
.us OK
on the
.us Module
dialog.
.result
This closes the
.us Modules
dialog and updates the database file.
.endstep
.np
To enable all of the module files on the
.us Modules
window, click on the
.us Enable All
button.
Click on
.us OK
to update the database file and close the
.us Modules
window.
.*
.endlevel
.*
.endlevel
.*
.section Global Views
.*
.np
The menu items under the View menu let you display a global view of
your program.
A global view is one that displays relationships between all symbols
in your program.
The following are global views.
.begpoint $break
.point List
Displays a list of all symbols in your program
.point Inheritance
Displays the class inheritance graph for your program
.point Call
Displays the call graph for your program
.endpoint
.np
Once a global view has been displayed, you can view detailed
information for the symbols in the global view.
Refer to the section entitled :HDREF refid='brview'. for a discussion
on displaying detail information.
.np
It is possible to specify a query that restricts the symbols displayed
in global views.
See the section entitled :HDREF refid='brglob'. for more information.
.*
.beglevel
.*
.section *refid=brlist The List View
.*
.np
.ix '&br' 'browsing symbols'
.ix 'symbols' 'browsing'
Using the &br to view the symbols in your program is much faster than
searching through your source code for symbol information.
From the symbols list you can quickly access detailed information on a
symbol that tells you where the symbol is used and where it is
defined.
.np
To view a list of all symbols in the current Browser database file,
choose
.us List
from the
.us View
menu.
This displays a window which can list all symbols in your program.
The window has a vertical scroll bar that allows you to scroll through
the list of symbols in your program.
Since your program may contain a very large number of symbols, the
Browser does not load all the symbols in your program from the
database.
Instead, only the number of symbols that can be displayed in the
window are loaded.
As far as the Browser is concerned, this list is infinite.
For this reason, the scroll thumb on the vertical scroll bar is
positioned in the middle of the vertical scroll bar and cannot be
moved.
Click below the vertical scroll thumb to view the next page of symbols
and above the vertical scroll thumb to view the previous page of
symbols.
.figure *depth='2.66' *scale=51 *file='brow4' The List window displays all symbols in the current browser database file.
.np
Each symbol has a icon to its left.
A letter marking each icon indicates the symbol type in the source
code as follows:
.begpoint
.point F
Function
.point C
Class
.point T
Typedef
.point V
Variable
.point E
Enum
.endpoint
.np
There are several ways to display the detail view of symbols.
When you reveal the detail view, the file folder icon changes to an
open file folder.
To close the detail view dialog, click on the file folder.
This changes the icon back to a closed folder and closes the dialog.
To reveal the detail view of a symbol you can perform any of the
following actions:
.begbull
.bull
The file folder icons are hot spots that display the detail view of the
selected symbol.
To activate the hot spot, click once on the symbol name to select it
and press ENTER to reveal the detail view.
.bull
Click once on the file folder to reveal the detail view for that
symbol.
.bull
Click once on the symbol name to select it and choose
.us Detail
from the
.us Detail
menu.
.bull
Double click on the symbol name.
.endbull
.*
.section The Class Inheritance View
.*
.np
.ix '&br' 'browsing classes'
.ix 'browsing classes'
.ix 'inheritance menu item'
Selecting
.us Inheritance
from the
.us View
menu displays the inheritance hierarchy of all of the C++ classes in
your program using a tree.
This allows you to see the relationships between base classes and
derived classes.
In the inheritance hierarchy, each node represents a class.
.figure *depth='2.66' *scale=51 *file='brow5' The Inheritance view displays the hierarchy of all C++ classes in your program (graph view shown).
.np
There are two different tree views you can choose from to display the
class hierarchy:
.begpoint $break
.point Graph view
The graph view displays each class in a box.
A box, or node, is connected to another node if one is a derived class
of the other.
The root node of the tree is the base class for all nodes in the tree.
The tree is initially fully expanded.
See the section entitled :HDREF refid='brtree'. for more information
on removing and expanding nodes from the tree.
.point Outline view
The outline view displays the same information as the graph view but
in a different way.
The outline view is initially fully collapsed.
That is, only the base classes are displayed.
In order to view the derived classes of a node, you must single click
on the node.
This expands the node, displaying all its immediate derived classes.
See the section entitled :HDREF refid='brtree'. for more information
on removing and expanding nodes from the tree.
.endpoint
.*
.section The Function Call Tree View
.*
.np
.ix '&br' 'browsing functions'
.ix 'browsing functions'
.ix 'call menu item'
Selecting
.us Call
from the
.us View
menu displays the function call tree for all functions in your
program.
This allows you to see all the functions that a given function calls
and conversely all functions that call a certain function.
In the call tree, each node represents a function.
.figure *depth='2.66' *scale=52 *file='brow6' The Call view displays a call tree for all functions in your program (outline view shown).
.np
There are two different tree views you can choose from to display the
function call tree:
.begpoint $break
.point Graph view
The graph view displays each function in a box.
A box, or node, is connected to another node if one function calls the
other.
The tree is initially fully expanded.
See the section entitled :HDREF refid='brtree'. for more information
on removing and expanding nodes from the tree.
.point Outline view
The outline view displays the same information as the graph view but
in a different way.
The outline view is initially fully collapsed.
In order to view the functions called by the function specified in the
node, you must single click on the node.
This expands the node, displaying all functions it calls.
See the section entitled :HDREF refid='brtree'. for more information
on removing and expanding nodes from the tree.
.endpoint
.*
.section *refid=brtree Manipulating the Tree Views
.*
.np
.ix '&br' 'graph view'
.ix '&br' 'outline view'
With both the graph view and the outline view you can change the
information displayed in the following ways:
.begbull $compact
.bull
select the root nodes you want to view
.bull
expand any node in the view
.bull
collapse any node in the view
.endbull
.np
You expand and collapse nodes in the graph and outline views to hide
and reveal descendant and ancestor nodes of the selected node.
There are two ways to expand and collapse nodes:
.begbull $compact
.bull
choose the desired function from the
.us Tree
menu
.bull
click on the node (applies only to the outline view)
.endbull
.np
In addition to these functions, you can force the graph to be redrawn
when a node in the tree view is collapsed.
By default, collapsed nodes will leave a gap in the graph.
Redrawing the graph removes these gaps.
This feature is controlled by an option.
See the section entitled :HDREF refid='brconf'. for more information.
.np
Clicking on the right mouse button when the mouse cursor is in a tree
view will automatically display the
.us Tree
menu.
This allows you to perform the actions in the
.us Tree
menu without actually going to the menu bar.
.*
.beglevel
.*
.section Selecting Root Nodes
.*
.np
.ix 'Root Nodes'
.ix 'Nodes' 'root'
.ix '&br' 'selecting root nodes'
Select
.us Select Root Nodes
from the
.us Tree
menu to change the root nodes that are displayed.
The root nodes you select appear in the graph or outline view; all
other root nodes are hidden.
.np
Choosing
.us Select Root Nodes
from the
.us Tree
menu opens the
.us Select Root Nodes
dialog.
This dialog lists all of the symbols that appear as a root node.
Each symbol in the
.us Select Root Nodes
dialog has a check box.
When enabled, this check box is marked with an
X, indicating that the symbol will appear as a root node in the
display.
.figure *depth='2.32' *scale=52 *file='brow7' On the Select Root Nodes dialog, choose the symbols you want to appear as root nodes.
.*
.beglevel
.*
.section Disabling Root Nodes
.*
.ix 'Root Nodes' 'disable'
.ix '&br' 'disable root nodes'
.begstep
.step Click in the check box
of the root node you want to disable.
Alternatively, use the up and down arrow keys to select the root node
you wish to disable.
Press the space bar to disable the currently selected root node.
.result
The X disappears meaning that the root node is disabled and will not
appear in the display.
.step Repeat step one
until you have disabled all desired root nodes.
.step Click on
.us OK
in the
.us Select Root Nodes
dialog.
.result
The
.us Select Root Nodes
dialog closes and the display is updated.
.endstep
.np
To disable all of the root nodes in the
.us Select Root Nodes
dialog, click on the
.us Disable All
button.
This removes Xs from each box in the
.us Select Root Nodes
dialog.
Click on
.us OK
to close the dialog and update the display.
.begnote
.mnote Note:
This disables the selected root nodes only for the active view window.
.endstep
.*
.section Enabling Root Nodes
.*
.ix '&br' 'enable root nodes'
.ix 'Root Nodes' 'enable'
.begstep
.step Click in the check box
of the root node you want to enable.
Alternatively, use the up and down arrow keys to select the root node
you wish to enable.
Press the space bar to enable the currently selected root node.
.result
An X appears in the box indicating that the root node is enabled and
will appear in the display.
.step Repeat step one
until you have enabled all desired root nodes.
.step Click on
.us OK
in the
.us Select Root Nodes
dialog.
.result
The
.us Select Root Nodes
dialog closes and the display is updated.
.endstep
.np
To enable all of the root nodes in the
.us Select Root Nodes
dialog, click on the
.us Enable All
button.
An X appears in each box on the
.us Select Root Nodes
dialog.
Click on
.us OK
to close the dialog and update the display.
.begnote
.mnote Note:
This enables the selected root nodes only for the active view window.
.endstep
.*
.endlevel
.*
.section Expanding Nodes
.*
.np
.ix 'Nodes' 'expanding'
.ix '&br' 'expanding nodes'
You can expand the display all at once, one level at a time, or one
branch at a time.
To expand by levels and branches, you must first select the node you
want to expand.
Do this by clicking once on the desired node.
.begpoint $break
.point Expand One Level
Choose
.us Expand One Level
from the
.us Tree
menu to display all of the immediate children for the selected node.
.point Expand Branch
Choose
.us Expand Branch
from the
.us Tree
menu to display all descendants of the selected node.
.point Expand All
Choose
.us Expand All
from the
.us Tree
menu to fully expand all of the enabled root nodes.
Disabled root nodes do not appear in the display.
.endpoint
.*
.section Collapsing Nodes
.*
.np
.ix 'Nodes' 'collapsing'
.ix '&br' 'collapsing nodes'
You can collapse the display all at once or one branch at a time.
To collapse by branches, you must first select the node you want to
collapse.
Do this by clicking once on the desired node.
.begpoint $break
.point Collapse Branch
Choose
.us Collapse Branch
from the
.us Tree
menu to hide all descendants of the selected
node.
.point Collapse All
Choose
.us Collapse All
from the
.us Tree
menu to collapse all of the nodes and display only the root nodes.
.endpoint
.*
.beglevel
.*
.section Using the Keyboard
.*
.np
.ix 'expanding and collapsing nodes' 'using the keyboard'
.ix 'using the keyboard' 'to expand and collapse nodes'
You can expand and collapse the symbols using key sequences.
Click once on the symbol name to highlight it and press one of the
following keys:
.begpoint $break
.point +
Expand the symbol one level
.point -
Collapse the symbol one level
.point *
Expand all descendants of the selected node.
This is the same as choosing
.us Expand Branch
from the
.us Tree
menu.
.point CTRL *
Expands all of the enabled root nodes.
This is the same as choosing
.us Expand All
from the
.us Tree
menu.
.point CTRL -
Collapses all of the symbols and displays only the root nodes.
This is the same as choosing
.us Collapse All
from the
.us Tree
menu.
.endpoint
.*
.endlevel
.*
.endlevel
.*
.endlevel
.*
.section *refid=brview Viewing Detail Information
.*
.np
.ix '&br' 'view detail information'
Once a global view is displayed, you can view detailed information for
a symbol in the global view in several ways:
.begbull $compact
.bull
Double click on the desired symbol.
.bull
Click once on the desired symbol to select it; then choose
.us Detail
from the
.us Detail
menu.
.bull
Click once on the desired symbol to select it then press ENTER.
.endbull
.np
From the list view, you can reveal the detail view if you:
.begbull
.bull
Click once on the icon to the left of the symbol.
.endbull
.np
Performing any of these actions reveals a detail view window for the
selected symbol.
The same information appears in this window regardless of the method
used to access it.
.figure *depth='4.20' *scale=50 *file='brow8' The detail view displays detailed information for the selected symbol.
.np
The detail view window displays the source file where the symbol is
defined or declared and it shows you what the symbol looks like in
your source code.
.np
A list box appears in the detail view window when you select a symbol
whose type is a function, class, or enum.
The list box contains information specific to the symbol type, as
follows.
.begpoint $break
.point Functions
The list box displays, where applicable, local variables for the
function.
.point Classes
The list box displays member variables and member functions for the
selected
class and any inherited classes.
The &br divides this information into three categories:
.begbull $compact
.bull
Public
.bull
Private
.bull
Protected
.endbull
.point Enums
The list box displays enumerator values for the selected enumerator.
.endpoint
.np
You can view detailed information for symbols in the list box by
double-clicking on the symbol.
Other symbols in the header information for the detail view are
highlighted.
Double-clicking on these symbols also displays a detail view.
.*
.beglevel
.*
.section Goto Definition
.*
.np
.ix '&br' 'Goto Definition function'
.ix 'Goto Definition function'
From a detail view,
.us Goto Definition
in the
.us Detail
menu allows you to edit the file that contains the symbol's definition.
The editor is positioned on the line and column containing the symbols
definition.
This allows you to make edits to your source code while you are
browsing.
When you are done, save any changes made and exit the editor to return
to the &br session.
.np
Note that any changes to your source code will make the Browser
database out-of-date.
.*
.section Source References
.*
.np
.ix '&br' 'source references'
.ix 'Source References'
Selecting
.us Source References
from the
.us Detail
menu displays all locations in the source code where a symbol is
referenced.
This allows you to analyze all uses of a particular symbol.
.np
When modifying a symbol, you can use this feature to locate all
occurrences of the symbol in the source code so you can update them.
.begstep To view the source references for a symbol:
.step Position yourself at the detail view of the symbol
whose source references you want to view.
.step Choose
.us Source References
from the
.us Detail
menu.
.result
This opens the
.us Source References
window for the selected symbol.
This window displays the filename, line number, and column number of
each occurrence of the selected symbol in the source code.
.endstep
.figure *depth='4.20' *scale=50 *file='brow9' Use the Source References window to view all the occurrences of the selected symbol in the source code.
.*
.section Symbol References
.*
.np
.ix 'Symbol references'
.ix '&br' 'symbol references'
.us Symbol References
allows you to view a list of all symbols that use a particular
symbol.
When modifying a symbol you can use this feature to locate all
symbols using the modified symbol.
This allows you to determine if all referencing symbols need to be
updated.
.begstep To view the symbol references for a symbol:
.step Position yourself at the detail view of the symbol
whose symbol references you want to view.
.step Choose
.us Symbol References
from the
.us Detail
menu.
.result
This opens the
.us Symbol Referencing
window for the selected symbol.
This window displays a list of all symbols that use the selected
symbol.
The display is the same as the list view of symbols.
Each symbol has a icon indicating its symbol type.
.figure *depth='4.20' *scale=50 *file='brow10' Use the Symbols window to view a list of all symbols that use the selected symbol.
.np
You can perform the same actions in this window as from the
.us List
window.
Refer to the section entitled :HDREF refid='brlist'. for more
information.
.endstep
.np
From this window you can invoke the editor to make changes to the
source code that contains the symbols that reference a particular
symbol.
Select the reference you want to edit in one of two ways:
.begbull
.bull
Double click on the desired reference.
.bull
Click once on the desired reference to select it and press ENTER.
.endbull
.np
This invokes the editor and positions the cursor at the line that
contains the selected reference.
.*
.endlevel
.*
.section Locating Symbols
.*
.np
.ix 'Symbols' 'locating'
.ix '&br' 'locating symbols'
When a global view is displayed and your program contains many
symbols, it can be difficult to scroll through the global view to
locate a particular symbol.
There are two functions that can help you locate a symbol in a global
view:
.begbull $compact
.bull
Find
.bull
Find Selected
.endbull
.*
.beglevel
.*
.section Find
.*
.np
Choose
.us Find
from the
.us Locate
menu to open the
.us Find
dialog.
Use this dialog to specify the criteria to be used to locate a symbol.
For example, you can search for a particular symbol or a set of
symbols with common characteristics.
.figure *depth='1.52' *scale=55 *file='brow11' Use the Find dialog to locate one symbol or a set of symbols.
.*
.beglevel
.*
.section Find Pattern
.*
.np
.ix '&br' 'find pattern'
.ix 'find pattern'
In the
.us Pattern
field of the
.us Find
dialog, enter a pattern for the symbol(s) you wish to locate.
The matching behaviour depends on the switches set in this dialog.
.begpoint $break
.point Match Case
When enabled, the &br performs a case sensitive compare when
attempting to find a match.
.point Match Whole Symbols Only
When enabled, the &br locates symbols that exactly match the specified
pattern.
This switch only applies when the
.us Use Regular Expressions
switch is disabled.
.point Use Regular Expressions
When enabled, the &br interprets the pattern specified as a regular
expression.
The
.us Edit
button, when pressed, displays the
.us Regular Expression Options
dialog.
This dialog allows you to specify the regular expression.
.endpoint
.np
Dropping the combo box of the
.us Pattern
field displays a list of previous patterns entered.
You can select a pattern from this list instead of entering a new one.
.*
.section Find Filters
.*
.np
.ix 'find filters'
.ix '&br' 'find filters'
Clicking on the
.us Filters
button on the
.us Find
dialog opens the
.us Find Filters
dialog.
This dialog allows you to specify the characteristics of the symbol(s)
to be located.
.figure *depth='2.47' *scale=52 *file='brow12' Use the Find Filters dialog to specify characteristics of the symbol(s) you want to locate.
.np
In the
.us Symbol Type
section of the dialog click on the symbol types you want to view.
To search for all symbol types, select the
.us All
button.
.np
In the
.us Symbol Scope
section, you can specify the scope for locating symbols.
.begpoint $break
.point Member of Class:
Specify the class that the symbol must be a member of in order to be
located.
.point Local Symbols of Function:
Specify the function that the symbol must be local to in order to be
located.
.endpoint
.*
.beglevel
.*
.section Find File Filters
.*
.np
The
.us Source Files
button, when pressed, displays the
.us Source Files
dialog that allows you to specify the files to be searched when
attempting to locate a symbol.
This dialog lists
all source files that make up the database file.
Each source file in the list has a check box.
An X in this box indicates that the source file is searched when
trying to locate a symbol.
Click in the check box of the source file you want to disable.
This removes the X. The blank box indicates that the source file will
not be searched when trying to locate a symbol.
To re-enable the source file,
click in the check box again.
This places an X in the box indicating that the source file will be
searched.
.figure *depth='2.89' *scale=52 *file='brow13' Choose the source files to be searched using the Source Files dialog.
.np
The
.us Pattern
section of the dialog, allows you to specify wild card directory
specifications for files that are to be included or excluded in the
search.
For example, specifying "d:\watcom\h\*.h" and pressing the
.us Exclude
button, will prevent any file in the "d:\watcom\h" directory with
extension ".h" from being searched.
To include these files in the search again, specify the same pattern
and press the
.us Include
button.
.np
Pressing the
.us Set All
button includes all files in the search.
.np
Pressing the
.us Clear All
button excludes all files in the search.
This is useful, for example, if you wish to only search files in the
current directory.
Simply press the
.us Clear All
button, specify "*.*" in the Pattern section, and press the
.us Include
button.
.*
.endlevel
.*
.section Performing the Find
.*
.np
Once the filter criteria are set, perform the find operation by
clicking on the
.ul
OK
button on the
.us Find
dialog.
This closes the dialog and performs the search.
The &br examines the symbols in the current global view and selects the
first symbol that matches the search criteria.
.np
To find the next symbol that matches the search criteria, choose
.us Find Next
from the
.us Locate
menu.
.*
.endlevel
.*
.section Find Selected
.*
.np
.ix '&br' 'find selected'
Because the symbols displayed in a global view are often many, only a
portion of the display is visible in the window at any time.
When you temporarily scroll away from the selected symbol or node, a
quick method of locating that symbol is to select
.us Find Selected
from the
.us Locate
menu.
This will immediately locate and display the currently selected symbol
in the global view.
.*
.endlevel
.*
.section *refid='brconf' Configuring the &br
.*
.np
.ix '&br' 'configure'
There are several options in the &br that you can configure:
.begbull $compact
.bull
Regular expression processing for find and query
.bull
Global Symbol Queries
.bull
Enumerator styles for detail views of enumeration constants
.bull
Member filters for detail views of classes
.bull
Auto-arranging of graph views
.bull
Line drawing method for graph views
.bull
Orientation of graph views
.bull
Line styles and &colour.s for inheritance graphs
.bull
Line styles and &colour.s for call graphs
.bull
Selection of text editor
.bull
Automatic saving of options on exit
.endbull
.np
This section describes each configuration option.
.*
.beglevel
.*
.section Regular Expressions for Find and Query
.*
.np
.ix '&br' 'set regular expressions for find'
.ix '&br' 'set regular expressions for queries'
.ix 'regular expressions' 'in &br'
.us Regular Expressions
in the
.us Options
menu allows you to configure the regular expression used to find a
symbol and specify a query.
.figure *depth='2.37' *scale=52 *file='brow14' Use the Regular Expressions Options dialog to configure a regular expression for Find and Query.
.np
Choose
.us Regular Expressions
from the
.us Options
menu to open the
.us Regular Expressions Options
dialog.
In the
.us Search String Meaning
section of the dialog you
select the anchoring method used to find a match.
.begpoint $break
.point Starts With
Matches only if the pattern is found at the beginning of the symbol
.point Contains
Matches if it occurs anywhere in the symbol
.endpoint
.np
In the
.us Regular Expression Characters
section of the dialog you select the characters you want the &br to
interpret as meta-characters.
.np
Select the desired characters by clicking once in the corresponding
check box.
An X in the check box indicates that the character will be interpreted
as a meta-character.
Unchecked characters are matched as standard keyboard characters.
To de-select a character, click again on its check box.
.np
The other buttons on this dialog are:
.begpoint $break
.point Set All
Click on Set All to enable all of the characters.
.point Clear All
Click on Clear All to disable all of the characters.
.point Defaults
Click on Defaults to discard the current settings in this dialog and
replace them with the settings configured at the start of the &br
session.
.point OK
Click on
.us OK
to close this dialog.
This changes the configuration for the current &br session.
.endpoint
.*
.section *refid='brglob' Global Symbol Queries
.*
.np
Choose
.us Query
from the
.us Options
menu to open the
.us Query
dialog.
Use this dialog to specify the criteria to be used to display symbols
in a global view.
.figure *depth='1.52' *scale=55 *file='brow15' Use the Query dialog to configure global view symbol queries.
.*
.beglevel
.*
.section Query Pattern
.*
.np
.ix '&br' 'specifying the query pattern'
.ix 'query pattern'
In the
.us Pattern
field of the
.us Query
dialog, enter a pattern for the symbol(s) you
wish to display in the global views.
The matching behaviour depends on the switches set in this
dialog.
.begpoint $break
.point Match Case
When enabled, the &br performs a case sensitive compare when
attempting to find a match.
.point Match Whole Symbols Only
When enabled, the &br includes symbols that exactly match the specified
pattern.
This switch only applies when the
.us Use Regular Expressions
switch is disabled.
.point Use Regular Expressions
When enabled, the &br interprets the pattern specified as a regular
expression.
The
.us Edit
button is enables and, when pressed, displays the
.us Regular Expression Options
dialog.
This dialog allows you to specify the regular expression.
.endpoint
.np
Dropping the combo box of the
.us Pattern
field displays a list of previous patterns entered.
You can select a pattern from this list instead of entering a new one.
.*
.section Query Filters
.*
.np
.ix 'query filters'
.ix '&br' 'query filters'
Clicking on the
.us Filters
button on the
.us Query
dialog opens the
.us Query Filters
dialog.
This dialog allows you to specify the characteristics of the symbol(s)
to be displayed in the global views.
.np
In the
.us Symbol Type
section of the dialog click on the symbol types you want to view.
To display symbols of all types, click on the
.us All
button.
.np
In the
.us Symbol Scope
section, you can specify the scope of the symbols to be displayed.
.begpoint $break
.point Member of Class:
Specify the class that the symbol must be a member of in order to be
displayed.
.point Local Symbols of Function:
Specify the function that the symbol must be local to in order to be
displayed.
.endpoint
.np
In the
.us Symbol Attributes
section, you can specify the attributes of the symbols to be displayed.
Selecting
.us Artificial
will cause compiler-generated symbols to be displayed.
Selecting
.us Anonymous
will cause unnamed types to be displayed.
Unnamed types will be displayed as square brackets enclosing the
symbols whose type they define.
Selecting
.us Declared Only
will cause only symbols that have been defined to be displayed.
For example, a function prototype for an unreferenced function will
not be displayed when
.us Declared Only
is selected.
.*
.beglevel
.*
.section Query File Filters
.*
.np
The
.us Source Files
button, when pressed, displays the
.us Source Files
dialog that allows you to specify the files that a symbol must be
defined in in order to be displayed.
This dialog lists
all source files that make up the database file.
Each source file in the list has a check box.
An X in this box indicates that all symbols defined in the source file
will be displayed.
Click in the check box of the source file you want to disable.
This removes the X.
The blank box indicates that any symbols defined in the
source file will not be displayed.
Alternatively, use the up and down arrow keys to select the source file
you wish to disable.
Press the space bar to disable the currently selected source file.
To re-enable the source file,
click in the check box again or press the space bar.
This places an X in the box again.
.np
The
.us Pattern
section of the dialog, allows you to specify wild card directory
specifications for files.
Any symbols defined in these files will be displayed in the global
views.
For example, specifying
.fi d:\watcom\h\*.h
and pressing the
.us Exclude
button, will prevent any symbol defined in any file in the
.fi d:\watcom\h
directory with extension ".h" from being displayed.
To display symbols from these files again, specify the same pattern
and press the
.us Include
button.
.np
Pressing the
.us Set All
button displays all symbols in all files.
.np
Pressing the
.us Clear All
button causes no symbols to be displayed.
This is useful, for example, if you wish to only display symbols
defined in files in the current directory.
Simply press the
.us Clear All
button, specify
.fi *.*
in the Pattern section, and press the
.us Include
button.
.*
.endlevel
.*
.endlevel
.*
.section Enumerator Styles
.*
.np
.ix 'enumeration styles'
.ix '&br' 'enumeration styles'
Selecting
.us Enumeration Styles
from the
.us Options
menu displays the
.us Enumeration Styles
dialog.
This dialog allows you to specify the format for displaying enumeration
constants in the detail views for enumeration constants.
.figure *depth='1.37' *scale=56 *file='brow16' Specify enumerator styles for detail views of enumerator constants using the Enumeration Styles dialog.
.*
.section Member Filters for Classes
.*
.np
.ix '&br' 'member filters for class details'
.ix 'class details' 'member filters'
.us Member Filters
in the
.us Options
menu allows you to specify the members you want to appear in the
detail view of a class.
For example, you may not want the detail class to contain private
members.
Alternatively, you may only wish to see function members and not data
members.
.begstep
.step Choose
.us Member Filters
from the
.us Options
menu.
.result
This opens the
.us Member Filters
dialog where you specify the information you want
to appear in detail views for classes.
.figure *depth='2.30' *scale=52 *file='brow17' Use the Member Filters dialog to select the information to appear in detail views for classes.
.step Choose the inheritance level
from the
.us Inherited Members
section of the dialog.
.result
The options are:
.begpoint $break
.point None
Do not show inherited members.
.point Visible
Show the local members of a class and visible members of inherited
classes.
.point All
Show all local and inherited members of a class.
.endpoint
.step Click on the check boxes
in the
.us Access Level
section of the dialog to select the desired access levels.
.result
The options are Public, Protected, and Private.
Only members with the selected
attributes will appear in the detail view for a class.
.step Select the desired members
in the
.us Members
section of the dialog.
.result
Show data members in a class by enabling the
.us variables
check box.
When the
.us variables
check box is enabled, you may also enable or disable static data
members by clicking on the
.us static
check box.
.np
Show function members in a class by enabling the
.us functions
check box.
When the
.us functions
check box is enabled, you may also enable or disable static and virtual
function members by clicking on the
.us static
and
.us virtual
check boxes.
.step Click on OK
to accept the member filter query.
.result
This closes the dialog and returns you to the active window.
.endstep
.begnote
.mnote Note:
The Default button on the Member Filter dialog resets the query
to the default settings.
.endnote
.*
.section Auto-arranging of Graph Views
.*
.np
.ix '&br' 'auto-arranging the graph view'
.ix 'graph views' 'auto-arranging'
When collapsing a graph view, nodes that become hidden are replaced by
gaps in the graph.
.us Arrange Graph
in the
.us Tree
menu compacts the graph view to remove the spaces left vacant by
hidden nodes.
.np
When enabled,
.us Graph Auto-arrange
of the
.us Options
menu causes the &br to automatically compact the graph view each time
you perform a collapse operation.
To enable the automatic compaction of the graph view after a collapse
operation, select
.us Graph Auto-arrange
from the
.us Options
menu.
A check mark appears beside the menu item when it is enabled.
Select the menu item again to disable it.
.*
.section Line Drawing Method for Graph Views
.*
.np
.us Graph Square Lines
in the
.us Options
menu allows you to select the type of line that connects the nodes in
a graph view.
The default is to connect nodes of the graph using diagonal lines.
Choosing
.us Graph Square Lines
from the
.us Options
menu causes nodes to be connected using square lines (combinations of
vertical and horizontal lines).
A check mark beside the menu item indicates this method of drawing
lines is enabled.
To disable this option, select it again.
.figure *depth='2.83' *scale=51 *file='brow18' The Graph Square Lines option changes the connecting lines from diagonal to square.
.*
.section Changing the Graph Orientation
.*
.np
.ix '&br' 'change graph orientation'
In the
.us Options
menu you can select whether the trees on the graph view grow
horizontally or vertically.
A graph view that grows horizontally is one where the root node is at
the left and the leaf nodes at the right.
A graph view that grows vertically is one where the root node is at the
top and the leaf nodes at the bottom.
By default, graphs grow vertically.
.np
Select
.us Graph Horizontal
from the
.us Options
menu to change the graph orientation to horizontal.
Selecting this option changes the menu item name to
.us Graph Horizontal
in the
.us Options
menu.
Select this menu item to change the graph orientation back to
vertical.
.figure *depth='2.83' *scale=51 *file='brow19' The Graph Horizontal option displays the graph with the root node at the left.
.*
.section Defining Graph View Legends
.*
.np
.ix 'graph view legends' 'in &br'
.ix 'inheritance legend menu item'
.ix 'call legend menu item'
.ix '&br' 'defining graph view legends'
The
.us Inheritance Legend
and
.us Call Legend
menu items in the
.us Options
menu allow you to change the &colour.s, line styles, and node styles
used in the graph views.
Changing the styles updates all open graph views in the current
session, as well as any new ones you open.
Saving the session configuration to an option file saves changes
made to the &colour.s and the lines and node styles.
.np
The
.us Inheritance Legend
and
.us Call Legend
dialogs are designed differently, but the
procedures for changing the graph styles are identical.
.np
To change the line and node styles for inheritance graphs, choose
.us Inheritance Legend
from the
.us Options
menu.
The
.us Inheritance Legend
dialog appears.
.np
To change the line and node styles for call graphs, choose
.us Call Legend
from the
.us Options
menu.
The
.us Call Legend
dialog appears.
.begstep
.step Click once on the line or node style
or use the up, down, right and left arrow keys to select
the line or node style you want to change.
.result
A box appears around the selected line.
.step Click on the Modify button.
.result
This opens the
.us Draw Style
dialog for the selected line or node style.
.step Click once on the desired line or node style
.orstep use the up and down arrow keys
to select the desired line or node style.
.result
The sample appearing in the
.us Example
field at the top of the dialog now reflects the currently selected
line or node style.
.step Click once on the desired &colour or use the up and down
arrow keys to select the desired &colour..
.result
The sample appearing in the
.us Example
field at the top of the dialog now reflects the currently selected
&colour..
.step Click on OK
to accept the new style and &colour setting.
.result
The
.us Draw Style
dialog closes, returning you to the
.us Inheritance Legend
or
.us Call Legend
dialog.
Select another line or node style to change and repeat this procedure.
.step Click on OK in the
.us Inheritance Legend
or
.us Call Legend
dialog when you have changed all desired &colour.s and styles for lines
and nodes.
.result
Clicking
.us OK
closes the dialog and updates all open inheritance or call graph
views with the selected &colour.s and line and node styles.
.endstep
.*
.section Selecting a Text Editor
.*
.np
.ix '&br' 'selecting a text editor'
.ix 'set text editor'
You can use your own favourite text editor from within the &br..
.begstep To select your own text editor:
.step Choose Set Text Editor
from the Options menu.
.result
The Set Text Editor dialog appears.
You can enter the name of the text editor in the first field.
You must also indicate whether the text editor is an executable
file or a Dynamic Link Library (DLL).
If the editor is an executable file (rather than a DLL), then you can
enter an argument line in the second field.
The argument line will be supplied to the editor whenever it is
started by the &br..
The argument line can include any of three different macros which will
be filled in by the &br..
The macros are:
.begnote
.note %f
The name of the file to be edited.
.note %r
The row in the file at which to position the cursor.
If the editor is invoked from a diagnostic message which contains a
line number then the row value is extracted from the message;
otherwise the row value is 1.
.note %c
The column in the file at which to position the cursor.
If the editor is invoked from a diagnostic message which contains a
column number then the column value is extracted from the message;
otherwise the column value is 1.
.endnote
.np
For example, if the editor argument line that you specified was:
.millust begin
file='%f' row='%r' col='%c'
.millust end
.pc
and you double click on an error message in the Log window that names
the file
.id foobar.c
with an error at line 215 and column 31, then the argument line
that is passed to your editor is:
.millust begin
file='foobar.c' row='215' col='31'
.millust end
.np
This flexibility allows you to specify the name of the file to edit
and the row and/or column at which to position the text cursor.
If no row or column is available, then the &br will supply the value
of 1 as a default.
.step Select OK
when you wish to confirm the selection of a new editor.
.orstep Select Cancel
when you wish to cancel the selection of a new editor.
.orstep Select Default
when you wish to restore the default editor selection
and then select OK or Cancel.
.endstep
.*
.section Automatically Saving Options on Exit
.*
.np
.ix '&br' 'saving options on exit'
.us Save Options on Exit
in the
.us Options
menu instructs the &br to automatically save the current options to an
options file.
To enable the automatic saving of options, select
.us Save Options on Exit
from the
.us Options
menu.
A check mark beside the menu item indicates that it is enabled.
To disable this option, select it again.
.np
See the section entitled :HDREF refid='brsave'. for more information
on saving options to a file.
.*
.endlevel
.*
.section Loading Options
.*
.np
.ix '&br' 'loading options'
.ix 'options' 'loading'
Use the
.us Load Options
menu to load an option file into your &br session.
.np
Choose
.us Load Options
from the
.us File menu.
The
.us Load Options File
dialog appears where you select the option file you want to load for
the current &br session.
.*
.section *refid=brsave Saving Options
.*
.np
.ix '&br' 'saving options'
There are three ways to save the current options to an option file:
.begbull $compact
.bull
Save Options
.bull
Save Options As
.bull
Save Options on Exit
.endbull
.np
An asterisk beside the option file name in the caption bar indicates
that changes were made to the options during the &br session and
should be saved.
This section describes each method for saving options to a file.
.begnote
.note Note:
If you have made option changes during the &br session and you choose
.us Exit
from the
.us File
menu without first saving the options, the &br prompts you to save the
options to a file.
Refer to the section entitled :HDREF refid='brquit'. for more
information.
.endstep
.begpoint $break
.point Save Options
Selecting
.us Save Options
from the
.us File
menu saves the updated options information to the current options
file.
To save options in this way, an options file must be specified in the
caption bar.
.ix '&br' 'save options'
.point Save Options As
.us Save Options As
in the
.us File
menu opens the
.us Save Options As
dialog.
Specify the filename of the option file to which you want to save the
current options.
Click on
.us OK
to close the dialog and save the options.
.point Save Options on Exit
.us Save Options on Exit
in the
.us Options
menu allows you to specify that the &br is to automatically save the
current options to an options file, if one exists.
When enabled, a check mark appears beside this menu item.
.np
When enabled and an option file exists, the &br saves the changes
without prompting when you close the session.
.np
When enabled and no option file exists, the &br displays a message box
prompting you to save the new options.
Choose
.ul
No
in this box to close the &br session without saving.
Choose
.ul
Yes
in this box to display a
.us Save As
dialog where you specify the option file to which you want to save the
new options.
.endpoint
.*
.section Setting Source Search Paths
.*
.np
.ix 'path option'
.ix '&br' 'path option'
.ix 'source search path'
.ix 'search path'
You can specify a command line option to the &br that allows you to
specify alternate source file search paths.
This option is useful when a database file is created on a system
different from the one that is used to browse the application source
code.
The syntax of the command line option is:
.millust begin
path path_spec1;path_spec2;...
.millust end
.np
Since the database files record explicit paths to source files, it is
likely that the path will no longer be valid once the software is
moved to another system.
.np
.ix 'path search'
When the &br cannot locate the specified file using its explicit path,
it will search the paths listed in the path option.
