:set symbol="msg_list" value="event list"
:set symbol="mark_menu" value="output"
:set symbol="appname" value="&dde"
.*
.chap &dde
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&dde'
&dde is a debugging tool that allows you to watch all DDE (Dynamic
Data Exchange) activity occurring in the system. &dde also allows you
to monitor DDEML errors and registration of various DDE objects, such
as strings, conversations, links, and servers. DDE is one way for
different applications running under Windows to communicate with each
other.
.np
By watching the communications between applications, you can determine
if the information being sent is correct. If it does not seem correct
you can locate the cause of the error.
.*
.section Using &dde
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting &dde
.bull
Quitting &dde
.bull
The &dde Menu Bar
.bull
The &dde Toolbar
.endbull
.*
.beglevel
.*
.section Starting &dde
.*
.np
.ix '&dde' 'start'
.ix 'start' '&dde'
To start &dde, double click on the &dde icon. This opens the &dde
window. When you start the application, the &company &dde window is
always empty. This window displays the menu bar and a toolbar.
.keep 19
.figure *depth='2.56' *scale=71 *file='dde1' The &company &dde window contains the event list, menu bar, and toolbar.
.*
.section Quitting &dde
.*
.np
.ix 'leave' '&dde'
.ix '&dde' 'leave'
To exit &dde, choose Exit from the File menu of the &company &dde
window.
.*
.section The &dde Menu Bar
.*
.np
.ix 'Menu bar' '&dde'
.ix '&dde' 'Menu bar'
The menu bar consists of the following six menus:
.begpoint
.point File
Save to a file, start and pause logging, and configure the session
.point Output
Work with the event list and control its output behaviour
.point Monitor
Select the information you want &dde to write to the event list
.point Track
Display windows summarizing information for DDE objects tracked by &dde
.point Aliases
Replace numeric values with meaningful text
.point Help
Access on-line help information
.endpoint
.figure *depth='0.72' *scale=71 *file='dde1a' The &company &dde menu bar.
.*
.section The &dde Toolbar
.*
.np
.ix 'Show Toolbar' 'in &dde'
.ix '&dde' 'Show Toolbar'
In the &company &dde window, the toolbar appears below the menu bar
when the Show Toolbar item in the File menu is checked. To disable
this feature, choose the item again from the File menu and the
Toolbar disappears.
The following explains the function each toolbar button performs,
as well as the equivalent menu item.
.begnote
.note TRACK STRINGS
Display or hide the strings tracking window.  This window displays a
list of all strings currently registered with the operating system.
This button is equivalent to the Strings item in the Track menu.
.note TRACK CONVERSATIONS
Display or hide the conversations tracking window. This window
displays a list of all conversations currently active in the system.
This button is equivalent to the Conversations item in the Track menu.
.note TRACK LINKS
Display or hide the links tracking window.  This window displays
a list of all links currently active in the system. This
button is equivalent to the Links item in the Track menu.
.note TRACK SERVERS
Display or hide the servers tracking window.  This window displays
a list of all servers currently registered with the operating system.
This button is equivalent to the Servers item in the Track menu.
.note LOG
Start Logging. This button is equivalent to the Log to
File item in the File menu.
.note MARK
Record your own comments in the event list. This button is
equivalent to the Mark item in the Output menu.
.note CLEAR
Clear the event list.  This button is equivalent to the Clear item
in the Output menu.
.note ADD HWND ALIAS
Replace window handle values with text. This is equivalent to the
Add Hwnd Alias item in the Alias menu.
.note ADD TASK ALIAS
Replace task id values with text. This is equivalent to the
Add Task Alias item in the Alias menu.
.note ADD CONVERSATION ALIAS
Replace conversation id values with text. This is equivalent to the
Add Conversation Alias item in the Alias menu.
.endnote
.figure *depth='0.72' *scale=71 *file='dde1a' The &company &dde toolbar.
.*
.endlevel
.*
.section The &dde Event List
.*
.np
.ix 'event list' 'in &dde'
.ix '&dde' 'event list'
Opening &dde reveals an empty window. This is the event list where
&dde logs the information you define in the Monitor menu. Some
examples of events are strings being created or deleted and
conversations and links being established or terminated.
.keep 19
.figure *depth='2.56' *scale=71 *file='dde2' Once you start spying, the event list displays the information selected for monitoring.
.np
This section describes the tasks you can perform in relation to the
event list.
.*
.beglevel
.*
.section &dde: Setting the Font
.*
.np
.ix 'fonts' 'set in &dde'
.ix '&dde' 'set fonts'
The Font item in the File menu allows you to set the font, style, and
size for the text in the &dde window. Choosing this item opens the Font
dialog box. Select the desired font details and click on OK to
close the dialog and apply the fonts.
.*
.section &dde: Saving the Event List to a File
.*
.np
.ix 'Save' 'in &dde'
.ix '&dde' 'save'
The Save items in the File menu allow you to save the contents of the
event list to a text file. Saving information to a file enables you to
print the output later or annotate the text file on-line.
.np
Choose Save from the File menu in the &company &dde window to save
the event list information into the current working directory. A
message box appears indicating the path to which &dde saved the file.
.np
.ix 'Save As' 'in &dde'
Choose Save As from the File menu to specify the file to which you
want to save the event list information. This opens a Save As dialog
box where you select the desired file. Click on OK when completed. A
message box appears indicating the path to which &dde saved the file.
.*
.section &dde: Clearing the Event List
.*
.np
.ix 'event list' 'clear in &dde'
.ix '&dde' 'clear event list'
.ix 'Clear option'
Choose Clear from the File menu to delete all information from the
current event list.
.*
.section &dde: Marking the Event List
.*
:include file='wguimark.gml'
.*
.section &dde: Pausing the Event List Output
.*
.np
.ix 'Output to Screen'
.ix 'Screen' 'output to'
.ix '&dde' 'output to screen'
The Output to Screen item in the Output menu allows you to temporarily
stop writing information to the event list. When this menu item is
unchecked output will not be written to the event list. Although &dde
stops writing information to the event list, it still writes it to the
log file if you are currently creating a log file. When &dde is not
writing information to the event list applications will run sightly
faster so when you are not interested in viewing the event list you
may want to stop writing to it.
.begnote
.note Note:
If you mark the event list while the Output to Screen feature is
deactivated, the text you enter does not appear in the event list.
.endnote
.*
.section &dde: Automatic Scrolling
.*
.np
.ix 'Auto Scroll'
.ix 'Scroll' 'automatic'
.ix '&dde' 'automatic scrolling'
As &dde writes new information to the event list, the information is
added to the bottom of the list. Checking the Auto Scroll item in
the Output menu tells &dde to automatically scroll the information
in the event list so that the most recent information written to
the event list always appears on the screen.
.np
When the Auto Scroll menu item not checked you must use the scroll
bar to scroll down and view any new information that is added to
the event list.
.*
.endlevel
.*
.section Selecting Information to Monitor
.*
.np
.ix '&dde' 'Monitor menu'
.ix 'Monitor menu' 'in &dde'
The monitor menu contains items that let you specify the types
of DDE events that &dde will report on by recording them in the
event list. Limiting the amount of information logged makes reading
the event list more efficient.
.np
When an item appears in the menu with a checkmark, &dde reports
on that type of activity. The following describes various types of
activity &dde can monitor.
.begpoint $break
.point Posted Messages
When the Posted Messages item is checked, &dde makes an entry in
the event list whenever a DDE message that has been selected using the
Message Filter dialog is posted to an application's message queue.
.point Sent Messages
When the Sent Messages item is checked, &dde makes an entry in the
event list whenever a DDE message that has been selected using the
Message Filter dialog is sent to an application.
.point String Handle Data
DDE applications use strings to communicate with other applications.
These strings are registered with the operating system by the
applications using them. When done with the strings, the applications
tell the operating system they are done and the system destroys the
strings. &dde reports on all string operations that occur within the
system when the String Handle Data menu item is checked.
.point Callbacks
Callbacks are special functions called by the operating system to
process DDE messages. When the Callback item is checked &dde makes an
entry in the event list each time the operating system calls a DDE
Callback for a transaction type that has been selected using the
Callback Filter dialog.
.point Errors
A DDE error usually indicates that a DDE operation has failed. When
the Errors item is checked, &dde reports all DDE errors that occur
within the system.
.point Conversations
A conversation is the connection established between two applications
that is necessary to perform DDE operations. When the Conversations
item is checked, &dde makes an entry in the event list each time a
conversation is established or terminated.
.point Links
A link is a relationship between two DDE applications: a server and a
client. A link is established for a single value maintained by the
server. When this value changes the server notifies the client. When
the Links item is checked, &dde logs notifications in the event list
each time a link is established or terminated.
.endpoint
.*
.beglevel
.*
.section Setting the Message Filter
.*
.np
.ix 'Set Message Filter'
.ix 'Message Filter' 'set'
.ix '&dde' 'set message filter'
The Set Message Filter item opens the Message Filter dialog which
lists the various DDE messages that can be posted or sent. This
dialog allows you to select the specific messages you want &dde to
write to the event list.
.keep 16
.figure *depth='2.22' *scale=75 *file='dde5' In the Set Message Filter dialog, select the messages on which you want &dde to report.
.np
Select the messages that you want &dde to report on when posted or
sent. A message is activated when its check box contains a check.
.begbull $compact
.bull
Use the Set All button in this dialog to activate all of the messages.
.bull
Use the Clear All button in this dialog to deactivate all of the
messages.
.endbull
.np
Click on OK when the desired messages are activated.
.*
.section Setting the Callback Filter
.*
.np
.ix 'Callback Filter' 'set'
.ix '&dde' 'set Callback Filter'
The Set Callback Filter menu item opens the Callback Filter dialog
which lists the various types of transactions for which a DDE
callback function may be called. This dialog allows you to specify
the transaction types you want &dde to report in the event list.
.keep 19
.figure *depth='2.48' *scale=68 *file='dde6' In the Set Callback Filter dialog, select the transaction types on which you want &dde to report.
.np
Select the transaction types on which you want &dde to report. A
transaction type will be reported on when its check box contains a
check.
.begbull $compact
.bull
Use the Set All button in this dialog to activate all of the
transaction types.
.bull
Use the Clear All button in this dialog to deactivate all of the
transaction types.
.endbull
.np
Click on OK when the desired transaction types are activated.
.*
.endlevel
.*
.im wguilog &dde
.*
.section Tracking with &dde
.*
.np
In the process of DDE interactions programs register and unregister
several types of DDE objects such as:
.begbull $compact
.bull
.ix '&dde' 'track strings'
Strings
.bull
.ix '&dde' 'track conversations'
Conversations
.bull
.ix '&dde' 'track links'
Links
.bull
.ix '&dde' 'track servers'
Servers
.endbull
.np
The tracking windows offer you an easy way to determine at any time
which objects are currently registered with the system.
.np
To open a tracking window choose Strings, Conversations, Links or
Servers from the Track menu. This opens a tracking window that
displays all the objects of the selected type that have been
registered but not unregistered since &dde started. The tracking
windows are automatically updated whenever an event occurs that
changes the displayed information.
.np
Each tracking window lists its information in tabular form under
several headings. You can sort the displayed information based on any
column by clicking on the column heading for that column.
.*
.beglevel
.*
.section Tracking Strings
.*
.np
.ix 'Strings' 'track in &dde'
.ix '&dde' 'track strings'
Choose Strings from the Track menu to open a Strings window which
tracks all strings currently registered with the operating system. As
strings are destroyed, &dde removes them from the list.
.keep 15
.figure *depth='2.1' *scale=90 *file='dde9' The Strings window tracks all strings currently registered with the operating system.
.np
The information in this window is:
.begpoint $break
.point Handle
The unique numeric identifier for the string
.point Count
The current usage count of the string.  A string is deleted when its
usage count is zero.
.point String
The text of the registered string
.endpoint
.*
.section Tracking Conversations
.*
.np
.ix 'Conversations' 'tracking'
.ix '&dde' 'track conversations'
Choose Conversations from the Track menu to open a Conversations
window which tracks all conversations currently active in the system.
.keep 12
.figure *depth='1.46' *scale=91 *file='dde10' The Conversations window tracks all conversations currently active in the system.
.np
The information in this window is:
.begpoint $break
.point Client
The unique numeric identifier for the client
.point Server
The unique numeric identifier for the server
.point Service
The service name specified when the conversation was established
.point Topic
The topic specified by the client when the conversation was established
.endpoint
.*
.section Tracking Links
.*
.np
.ix 'Links' 'tracking'
.ix '&dde' 'track links'
Choose Links from the Track menu to open a Links window which tracks
all links currently active in the system.
.keep 9
.figure *depth='1.09' *scale=68 *file='dde11' The Links window tracks all links currently active in the system.
.np
The information in this window is:
.begpoint $break
.point Client
The unique numeric identifier for the client
.point Server
The unique numeric identifier for the server
.point Type
A value indicating if the link is Hot or Warm. When a link is
established between a client and server, the server notifies the
client whenever the value of the item changes. When the link type is
Warm, the only information sent to the client is notification that the
value of the item has changed. When the link type is Hot, then the
notification that the value has changed is sent to the client as well
as the new value of the item.
.point Service
The service name specified when the conversation on which this link
exists was established
.point Topic
The topic specified by the client when the conversation was
established.
.point Item
The name of the item for which the link is established.
.point Format
The format of the information that is being communicated between
applications.
.endpoint
.*
.section Tracking Servers
.*
.np
.ix 'Servers' 'tracking'
.ix '&dde' 'track servers'
Choose Servers from the Track menu to open a Servers window which
tracks all servers currently registered with the operating system.
.keep 12
.figure *depth='1.46' *scale=92 *file='dde12' The Servers window tracks all servers currently registered with the operating system.
.np
The information in this dialog is:
.begpoint $break
.point Server Name
The name of each server that is registered with the operating system.
This name is generally descriptive of the type of server.
.point Instance Name
The server name with a numeric identifier added by the operating
system. This name uniquely identifies the server in the system.
.endpoint
.*
.endlevel
.*
.section Working with Aliases
.*
.np
.ix 'Aliases' 'in &dde'
.ix '&dde' 'Aliases in'
The event list displays an overwhelming array of numeric identifiers.
The alias functions allow you to replace these numeric identifiers
with meaningful text.  This makes the event list easier for
you to understand. For example, if you are monitoring a server
application and you know it's task id is 0x1234 then you can replace
every occurrence of that task id with the text "server".
.*
.beglevel
.*
.section Defining Aliases
.*
.np
Using the alias functions you can define aliases for window handles,
task id's and conversation id's.
.begstep
.step Choose Add hwnd alias, Add task alias or Add conversation alias
from the Alias menu
.orstep Click on the appropriate icon on the toolbar.
.result
This opens the appropriate Add Alias dialog. The dialog contains a
list box that lists all identifiers of the chosen type that &dde
has encountered.
.keep 28
.figure *depth='3.45' *scale=79 *file='dde13' In the Add Alias dialog box, you replace the selected window handle, task, or conversation id with descriptive text.
.step Click once on the window handle, task, or conversation
for which you want to add an alias
.orstep Enter a numeric value in the Value field.
.step Position the cursor in the Current Text field.
.step Enter the text
with which you want to replace this numeric identifier.
.step Click on OK
to register the alias and close the dialog
.orstep Select the Do More button
to register the alias and leave the dialog open.
.result
This allows you to create aliases for more numeric identifiers.
.endstep
.*
.section Disabling Aliases
.*
.np
Choosing Disable Aliases from the Aliases menu changes all aliased
values back to their original numeric identifiers. The Disable Aliases
menu item is checked when this option is active. Choosing the menu
item again disables it. Numeric identifiers are then replaced by their
text aliases again.
.*
.section Purging Aliases
.*
.np
Choosing Purge Aliases from the Aliases menu deletes all aliases set
up in &dde.. Any values replaced by aliases will revert to their
original numeric values.
.*
.endlevel
