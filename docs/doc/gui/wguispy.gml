:set symbol="msg_info" value="Viewing Message Information"
:set symbol="win_info" value="Viewing Information About a Window"
:set symbol="mark_menu" value="List"
:set symbol="appname" value="&spy"
:set symbol="msg_list" value="message list"
.*
.chap &spy
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&spy'
Most Windows programs are message based. Messages are sent to windows
that programs create and the programs are expected to react to the
messages appropriately.
.np
When a program does not react as expected, you must spend time
debugging the program to find and correct the errors. &spy is a tool
designed to help you debug Windows programs by letting you view the
messages sent to windows in Win16 and Win32 applications. By watching how
a program reacts to the messages it receives, you can isolate the
parts of your code that are causing problems and then make the
required corrections.
.np
With &spy:
.begbull
.bull
you can determine if your window is receiving the messages you expect
it to receive.
.bull
you can verify that your window is receiving messages in the order you
expect.
.bull
novice Windows programmers can become familiar with Windows messages,
by viewing the messages and their parameters.
.endbull
.np
As you run your Windows programs, &spy displays messages in the &spy
window. You control the window using the menu items or the &spy
toolbar.
.*
.section Using &spy
.*
.np
This section discusses the following topics:
.begbull $compact
.bull
Starting &spy
.bull
Quitting &spy
.bull
The &spy Menu Bar
.bull
The &spy Toolbar
.endbull
.*
.beglevel
.*
.section Starting &spy
.*
.np
.ix '&spy' 'start'
.ix 'start' '&spy'
To start &spy, double click on the &spy icon.
This opens the &spy window.
When you start the application, the &spy window is always empty.
This window displays the menu bar, message list and a toolbar.
.figure *depth='3.32' *scale=79 *file='spy1' The &spy window contains the menu bar, message list and toolbar.
.*
.section Quitting &spy
.*
.np
.ix '&spy' 'leave'
.ix 'leave' '&spy'
To exit &spy, choose Exit from the File menu of the &spy window.
.*
.section The &spy Menu Bar
.*
.np
.ix '&spy' 'Menu Bar'
.ix 'Menu Bar' '&spy'
The menu bar consists of the following five menus:
.begpoint
.point File
Save to a file, start and pause logging, and configure the session
.point List
Manipulate the message list by clearing, marking, and autoscrolling it
.point Spy
Start, stop, and pause the &spy session, select the windows you want
to monitor, and gain access to information in windows.
.point Messages
Select the messages you want to monitor or stop on.
.point Help
Gain access to on-line help information
.endpoint
.figure *depth='0.77' *scale=79 *file='spy1a' The &spy menu bar.
.*
.section The &spy Toolbar
.*
.np
.ix '&spy' 'toolbar'
The &spy toolbar provides quick access to commonly used menu items.
The following explains the function each tool bar button performs
as well as the equivalent menu item.
.begnote
.note SELECT WINDOW
Select a single window and start spying on it or add a window to the
list of windows being spied on. This is equivalent to the Select
Window and Add Window items in the Spy menu.
.note STOP
Stop monitoring the selected window(s) and cancel the window
selections. This is equivalent to the Stop Spying item in the Spy
menu.
.note PEEK AT WINDOW
Obtain information about a selected window. This is equivalent to the
Peek at Window item in the Spy menu.
.note MARK
Record your own comments in the &spy window. This is equivalent to the
Mark item in the List menu.
.note CLEAR MESSAGE LIST
Erase all messages from the &spy window. This is equivalent to the
Clear Message List item in the List menu.
.note SHOW SELECTED WINDOWS
Obtain information about the windows open on your desktop. This is
equivalent to the Show Selected Windows item in the Spy menu.
.note MESSAGES TO WATCH
Select the messages you want to monitor and log. This is equivalent to
the Messages to Watch item in the Messages menu.
.note MESSAGES TO STOP ON
Specify the messages you want &spy to stop on when it encounters them.
This is equivalent to the Messages to Stop on item in the Messages menu.
.endnote
.figure *depth='0.77' *scale=79 *file='spy1a' The &spy toolbar.
.np
Only one of the following three toolbar buttons appears at a time.
.begnote
.note NOT SPYING
This button appears when &spy is not monitoring any windows.  Pressing
this button has no effect.  It is simply an indicator to you that
spying is not in progress.
.note STOP SPYING TEMPORARILY
This button appears when &spy is in the process of monitoring one or
more windows. Choosing this button temporarily suspends monitoring
operations. This is equivalent to the Stop Spying Temporarily item in
the Spy menu.
.note CONTINUE SPYING
This button appears when you instruct &spy to temporarily suspend
monitoring operations. Choosing this button resumes monitoring
operations. This is equivalent to the Continue Spying item in the Spy
menu.
.endnote
.*
.endlevel
.*
.section The &spy Window
.*
.np
.ix '&spy' 'Message List'
.ix 'Message List' 'in &spy'
The &spy window contains the menu bar and the toolbar discussed above.
However, most of the &spy window is occupied by the message list.  This
is where &spy logs the messages it monitors.
.figure *depth='3.32' *scale=79 *file='spy2' Once you start spying, the message list displays the messages selected for monitoring.
.np
For the messages it monitors &spy displays the following information in
the message list:
.begpoint $break
.point Message Name
Name of the message sent.
.point hWnd
The handle of the window that received the message.
.point MsgID
The numeric identifier of the message.
.point wParam
Data specific to the message. This is the value that will be passed
as the wparam parameter of the window's callback function.  To find
the meaning of this parameter see the section &msg_info for information
on how to get help on a message or refer to your Windows 3.x or Win32
programmer's documentation.
.point lParam
Data specific to the message. This is the value that will be passed
as the lparam parameter of the window's callback function. To find
the meaning of this parameter see the section &msg_info for information
on how to get help on a message or refer to your Windows 3.x or Win32
programmer's documentation.
.endpoint
.np
Several spy functions affect the appearance or behavior of the &spy
window.
.begbull $compact
.bull
Setting the Fonts
.bull
Saving
.bull
Clearing the Message List
.bull
Marking the Message List
.bull
Automatic Scrolling
.bull
Always On Top
.endbull
.*
.beglevel
.*
.section Spy: Setting the Fonts
.*
.np
.ix '&spy' 'set fonts'
.ix 'Fonts' 'set in &spy'
The Set Font item in the File menu allows you to set the font, style,
and size for the text in the &spy window. Choosing this item opens
the Font dialog box. Select the desired font details and click on OK
to close the dialog and apply the fonts.
.*
.section Spy: Saving the Contents of the Message List
.*
.np
.ix '&spy' 'saving in'
.ix 'save' 'in &spy'
The Save and Save as items under the File menu in &spy allow you to
save the contents of the message list to a text file. Saving
information to a file enables you to print the output later or
annotate the text file on-line.
.np
Choose Save from the File menu in the &spy window to save the message
list information into the current working directory. A message box
appears indicating the path to which &spy saved the file.
.np
.ix '&spy' 'Save As option'
.ix 'Save As' 'in &spy'
Choose Save As from the File menu to specify the file to which you
want to save the currently displayed information. This opens a Save As
dialog box where you select the desired file. Click on OK when
completed. A message box appears indicating the path to which &spy
saved the file.
.*
.section Spy: Clearing the Message List
.*
.np
.ix '&spy' 'clear message list'
.ix 'Message List' 'clear in &spy'
Choose Clear from the File menu to delete all information from the
message list.
.*
.section Spy: Marking the Message List
.*
.ix '&spy' 'mark message list'
.ix 'Message List' 'mark in &spy'
.*
:INCLUDE file='wguimark.gml'
.*
.section Spy: Automatic Scrolling
.*
.np
.ix '&spy' 'automatic scrolling'
.ix 'Auto Scroll Message List'
As &spy writes new information to the message list, the information
is added to the bottom of the list. Choosing Auto Scroll Message List
from the List menu tells &spy to automatically scroll the information
in the message list so that the most recent information written to the
message list always appears on the screen. The Auto Scroll Message List
menu item is checked when this feature is active.
.np
When this feature is deactivated, new messages written to the
message list are added to the bottom of the list and you must use
the scroll bar to scroll down and view the information.
.*
.section Spy: Always On Top
.*
.np
.ix 'Always On Top'
.ix '&spy' 'always on top'
The &spy window behaves like all other windows&mdash.it moves into the
background when other windows are selected. To ensure the &spy window
is visible at all times, select the Always On Top item from the File
menu. The Always On Top menu item is checked when this feature is
enabled. To turn this feature off, select the item again from the File
menu.
.*
.endlevel
.*
.section Selecting Messages to Monitor
.*
.np
.ix 'messages' 'watching'
.ix 'messages' 'stopping on'
.ix '&spy' 'selecting messages to monitor'
Each window receives an overwhelming volume of messages. Generally,
you will only be interested in a small subset of these. &spy lets
you limit the messages on which it will report so that you do not have
to sort through all the extraneous data yourself.
.np
&spy maintains two lists of messages: the messages to watch list and
the messages to stop on list.  When spy monitors a message in the
messages to watch list it adds information about that message to
the Message List in the &spy window. When spy monitors a message
in the messages to stop on list it displays a message box reporting
that the message has been encountered and pauses spying operations
until you restart them by selecting Continue Spying from the spy menu.
Any message that does not appear on either list is ignored by &spy.
.np
To modify the messages to watch list choose Messages To Watch from the
Messages menu. To modify the messages to stop on list choose Messages
To Stop On from the Messages menu. In either case, the process of
modifying the list is the same. The rest of this section explains the
dialogs you use to do this.
.*
.beglevel
.*
.section *refid=selinms Selecting Individual Messages
.*
.np
.ix 'messages' 'watching'
.ix 'messages' 'stopping on'
The most precise way to modify the messages to watch or messages to
stop on lists is simply to select a set of individual messages to
appear on the list. To select a set of individual messages to
monitor follow these procedures.
.begstep
.step Choose Messages To Watch or Messages To Stop On
from the Messages Menu.
.result
This opens a Message Class dialog box.  The caption of
the dialog box reflects the menu item selected.
.figure *depth='3.23' *scale=80 *file='spy5' In the Message Class dialog, select the Individual Message Control button.
.step Click on the Individual Message Control button.
.result
This opens a dialog that contains a check box for each message.
.figure *depth='2.55' *scale=52 *file='spy6' In the Message Range dialog, you select the individual messages you want &spy to watch or stop on.
.step Add or remove messages from the list of messages.
.result
A message appears on the selected list if it is checked.
The following buttons are provided for your convenience:
.begnote
.note Clear All
Uncheck all messages on this dialog.
.note Set All
Check all messages on this dialog.
.endnote
.step Choose an action button.
.result
The following buttons allow you to proceed from this dialog:
.begnote
.note Next
Close the current dialog and open the one for the alphabetic
range of messages immediately following this one. Changes made
on the current dialog will not be saved until you press the
OK button on some future message range dialog.
.note Prev
Close the current dialog and open the one for the alphabetic
range of messages immediately preceding this one. Changes
made on the current dialog will not be saved until you press
the OK button on some future message range dialog.
.note OK
Close the current dialog and save all changes made in it and
in previous dialog that were exited using the Next or Prev
button.
.note Cancel
Close the current dialog and ignore all changes made in it
and in previous dialog that were exited using the Next or
Prev button.
.endnote
.step Select OK on the Message Class dialog.
.result
The message class dialog closes and your message selections take effect.
If you choose Cancel on the Message Class dialog your message selections
will be discarded.
.endstep
.*
.section *refid=selmscl Selecting Message Classes
.*
.np
Although modifying the list of messages to watch or stop on by
selecting individual messages is more precise, you may find this
process tedious.  To speed up the process several message classes
have been defined for you. These classes group messages by their
function. The messages contained in each message class are listed in
the section entitled :HDREF refid='msgcls'.
.np
Selecting a message class adds all the individual
messages in that class to the selected list.  Similarly,
de-selecting a message class removes all the individual
messages in that class from the selected list.
.np
To select messages to monitor by class follow these procedures:
.begstep
.step Choose Messages To Watch or Messages To Stop On
from the Messages Menu.
.result
This opens a Message Class dialog box.  The caption of
the dialog box reflects the menu item selected.
.ix 'messages' 'watching'
.ix 'messages' 'stopping on'
.figure *depth='3.23' *scale=80 *file='spy5' In the Message class dialog, choose the message classes you want &spy to monitor.
.step Click on the check box
beside the message classes you want to monitor.
.result
A message class is selected when an X appears in the check box. The
following buttons are available for your convenience:
.begnote
.note Clear All
De-select all of the message classes.
.note Set All
Select all of the message classes.
.endnote
.step Click on OK
to save your choices and close the dialog.
.result
Selecting Cancel on this dialog exits without saving your selections.
.endstep
.*
.section *refid=msgcls Predefined Spy Message Classes
.*
.np
The sections that follow list the messages contained in each message class
that &spy defines for you.
.np
Messages followed by (*) are undocumented messages.
.br
Messages followed by (**) are available under Win32 only.
.*
.tb set $
.tb &INDlvl.+2 &syscl./2+1
.*
.beglevel
.*
.section Clipboard Messages
.*
.np
.monoon
$WM_ASKCBFORMATNAME $WM_CHANGECBCHAIN
$WM_CLEAR           $WM_CLIPBOARDUPDATE (**)
$WM_COPY            $WM_CUT
$WM_DESTROYCLIPBOARD$WM_DRAWCLIPBOARD
$WM_HSCROLLCLIPBOARD$WM_PAINTCLIPBOARD
$WM_PASTE           $WM_RENDERALLFORMATS
$WM_RENDERFORMAT    $WM_SIZECLIPBOARD
$WM_UNDO            $WM_VSCROLLCLIPBOARD
.monooff
.*
.section DDE Messages
.*
.np
.monoon
$WM_DDE_ACK         $WM_DDE_ADVISE
$WM_DDE_DATA        $WM_DDE_EXECUTE
$WM_DDE_INITIATE    $WM_DDE_POKE
$WM_DDE_REQUEST     $WM_DDE_TERMINATE
$WM_DDE_UNADVISE
.monooff
.*
.section Init Messages
.*
.np
.monoon
$WM_INITDIALOG      $WM_INITMENU
$WM_INITMENUPOPUP   $WM_UNINITMENUPOPUP (**)

.monooff
.*
.section Input Messages
.*
.np
.monoon
$WM_APPCOMMAND (**)    $WM_BEGINDRAG (*)
$WM_CHAR               $WM_CHARTOITEM
$WM_COMMAND            $WM_CONTEXTMENU (**)
$WM_COPYDATA (**)      $WM_DEADCHAR
$WM_DRAGLOOP (*)       $WM_DRAGMOVE (*)
$WM_DRAGSELECT (*)     $WM_DROPOBJECT (*)
$WM_EXITSIZEMEOVE (*)  $WM_HSCROLL
$WM_INPUT (**)         $WM_INPUT_DEVICE_CHANGE (**)
$WM_INPUTLANGCHANGE (**)$WM_INPUTLANGCHANGEREQUEST (**)
$WM_KEYDOWN            $WM_KEYUP
$WM_LBTRACKPOINT (*)   $WM_MENUCHAR
$WM_MENUCOMMAND (**)   $WM_MENUDRAG (**)
$WM_MENUGETOBJECT (**) $WM_MENURBUTTONUP (**)
$WM_MENUSELECT         $WM_NEXTMENU (*)
$WM_NOTIFY (**)        $WM_NOTIFYFORMAT (**)
$WM_QUERYDROPOBJECT (*)$WM_TCARD (**)
$WM_UNICHAR (**)       $WM_VSCROLL
.monooff
.*
.section MDI Messages
.*
.np
.monoon
$WM_ISACTIVEICON (*)$WM_MDIACTIVATE
$WM_MDICASCADE      $WM_MDICREATE
$WM_MDIDESTROY      $WM_MDIGETACTIVE
$WM_MDIICONARRANGE  $WM_MDIMAXIMIZE
$WM_MDINEXT         $WM_MDIRESTORE
$WM_MDISETMENU      $WM_MDITILE
.monooff
.*
.section Mouse Messages
.*
.np
.monoon
$WM_CAPTURECHANGED (**)$WM_LBUTTONDBLCLK
$WM_LBUTTONDOWN     $WM_LBUTTONUP
$WM_MBUTTONDBLCLK   $WM_MBUTTONDOWN
$WM_MBUTTONUP       $WM_MOUSEACTIVATE   
$WM_MOUSEHOVER (**) $WM_MOUSEHWHEEL (**)
$WM_MOUSELEAVE (**) $WM_MOUSEMOVE
$WM_MOUSEWHEEL (**) $WM_RBUTTONDBLCLK
$WM_RBUTTONDOWN     $WM_RBUTTONUP
$WM_SETCURSOR       $WM_XBUTTONDBLCLK (**)
$WM_XBUTTONDOWN (**)$WM_XBUTTONUP (**)
.monooff
.*
.section NC Misc Messages
.*
.np
.monoon
$WM_NCACTIVATE      $WM_NCCALCSIZE
$WM_NCCREATE        $WM_NCDESTROY
$WM_NCPAINT
.monooff
.*
.section NC Mouse Messages
.*
.np
.monoon
$WM_NCHITTEST       $WM_NCLBUTTONDBLCLK
$WM_NCLBUTTONDOWN   $WM_NCLBUTTONUP
$WM_NCMBUTTONDBLCLK $WM_NCMBUTTONDOWN
$WM_NCMBUTTONUP     $WM_NCMOUSEHOVER (**)
$WM_NCMOUSELEAVE (**)$WM_NCMOUSEMOVE
$WM_NCRBUTTONDBLCLK $WM_NCRBUTTONDOWN
$WM_NCRBUTTONUP     $WM_NCXBUTTONDBLCLK (**)
$WM_NCXBUTTONDOWN (**)$WM_NCXBUTTONUP (**)
.monooff
.*
.section Other Messages
.*
.np
.monoon
$WM_CHANGEUISTATE (**)$WM_COALESCE_FIRST
$WM_COALESCE_LAST     $WM_COMMNOTIFY
$WM_CONVERTREQUEST (*)$WM_CONVERTRESULT (*)
$WM_DROPFILES         $WM_INTERIM (*)
$WM_PENWINFIRST       $WM_PENWINLAST
$WM_PRINT (**)        $WM_PRINTCLIENT (**)
$WM_QUERYSAVESTATE (*)$WM_QUEUESYNC
$WM_QUERYUISTATE (**) $WM_SIZEWAIT (*)
$WM_SYNCTASK (*)      $WM_TESTING (*)
$WM_UPDATEUISTATE (**)$WM_YOMICHAR (*)
.monooff
.*
.section System Messages
.*
.np
.monoon
$WM_CANCELJOURNAL (**)$WM_COMPACTING
$WM_DEVMODECHANGE     $WM_DEVICECHANGE (**)
$WM_DISPLAYCHANGE (**)$WM_ENDSESSION
$WM_ENTERIDLE         $WM_ENTERMENULOOP
$WM_EXITMENULOOP      $WM_FILESYSCHANGE (*)
$WM_NULL              $WM_POWER
$WM_POWERBROADCAST (**)$WM_SPOOLERSTATUS
$WM_SYSCHAR           $WM_SYSCOLORCHANGE
$WM_SYSCOMMAND        $WM_SYSDEADCHAR
$WM_SYSKEYDOWN        $WM_SYSKEYUP
$WM_SYSTEMERROR (*)   $WM_SYSTIMER (*)
$WM_THEMECHANGED (**) $WM_TIMECHANGE
$WM_TIMER             $WM_USERCHANGED (**)
$WM_WININICHANGE      $WM_WTSSESSION_CHANGE (**)
.monooff
.*
.section User Messages
.*
.np
.monoon
$WM_USER
.monooff
.*
.section Unknown Messages
.*
.np
.monoon
$None
.monooff
.*
.section Window Messages
.*
.np
.monoon
$WM_ACTIVATE           $WM_ACTIVATEAPP
$WM_ACTIVATESHELLWINDOW (*)$WM_ALTTABACTIVE (*)
$WM_CANCELMODE         $WM_CREATE
$WM_CHILDACTIVATE      $WM_CLOSE
$WM_COMPAREITEM        $WM_CTLCOLOR
$WM_CTLCOLORBTN (**)   $WM_CTLCOLORDLG (**)
$WM_CTLCOLOREDIT (**)  $WM_CTLCOLORLISTBOX (**)
$WM_CTLCOLORMSGBOX (**)$WM_CTLCOLORSCROLLBAR (**)
$WM_CTLCOLORSTATIC (**)$WM_DELETEITEM
$WM_DESTROY            $WM_DRAWITEM
$WM_DWMCOLORIZATIONCHANGED (**)$WM_DWMCOMPOSITIONCHANGED (**)
$WM_DWMNCRENDERINGCHANGED (**)$WM_DWMWINDOWMAXIMIZEDCHANGE (**)
$WM_ENABLE             $WM_ENTERSIZEMOVE (*)
$WM_ERASEBKGND         $WM_FONTCHANGE
$WM_GETDLGCODE         $WM_GETFONT
$WM_GETHOTKEY          $WM_GETICON (**)
$WM_GETMINMAXINFO      $WM_GETTEXT
$WM_GETTEXTLENGTH      $WM_GETTITLEBARINFOEX (**)
$WM_HELP (**)          $WM_ICONERASEBKGND
$WM_KILLFOCUS          $WM_MEASUREITEM
$WM_MOVE               $WM_MOVING (**)
$WM_NEXTDLGCTL         $WM_OTHERWINDOWCREATED
$WM_OTHERWINDOWDESTROYED$WM_PAINT
$WM_PAINTICON          $WM_PALETTECHANGED
$WM_PALETTEISCHANGING  $WM_PARENTNOTIFY
$WM_QUERYDRAGICON      $WM_QUERYENDSESSION
$WM_QUERYNEWPALETTE    $WM_QUERYOPEN
$WM_QUERYPARKICON (*)  $WM_QUIT
$WM_SETFOCUS           $WM_SETFONT
$WM_SETHOTKEY          $WM_SETICON (**)
$WM_SETREDRAW          $WM_SETTEXT
$WM_SETVISIBLE (*)     $WM_SHOWWINDOW
$WM_SIZE               $WM_SIZING (**)
$WM_STYLECHANGED (**)  $WM_STYLECHANGING (**)
$WM_SYNCPAINT (*)      $WM_VKEYTOITEM
$WM_WINDOWPOSCHANGED   $WM_WINDOWPOSCHANGING
.monooff
.*
.endlevel
.*
.tb set
.tb
.*
.section Saving and Loading Message Configurations
.*
.np
.ix 'Message Configuration'
.ix '&spy' 'save and load message configuration'
Selecting messages to monitor can be an involved procedure because of
the number of options available. Once selected, the group of message
selections is called a message configuration.
.np
&spy allows you to save message configurations and recall them again.
This section describes the functions relating to message
configurations:
.begbull $compact
.bull
Using the Auto Save Feature
.bull
Saving a Message Configuration
.bull
Loading a Message Configuration
.endbull
.*
.beglevel
.*
.section Using the Auto Save Feature
.*
.np
.ix '&spy' 'auto save feature'
.ix 'auto save' 'in &spy'
When the Auto Save Message Configuration feature is active &spy
saves your message configuration each time you exit. The next
time you start &spy, the message configuration is automatically
reloaded.
.np
To activate or deactivate this feature choose the Auto Save Message
Configuration item from the Messages menu.  When this feature is active
its menu item is checked.
.*
.section Saving a Message Configuration
.*
.np
.ix 'message configuration' 'save'
.ix '&spy' 'save message configuration'
With &spy you can save your message configurations so you can easily
load a previous message configuration instead of reselecting the
desired messages.
.np
To save a message configuration follow these procedures:
.begstep
.step Select the messages you want to monitor
as described in the sections entitled :HDREF refid='selmscl'.
and :HDREF refid='selinms'..
.step Choose Save Message Configuration
from the Messages menu.
.result
This opens a Save As dialog box.
.step Specify the desired file name and path
for this message configuration.
.step Click on OK
in the Save As dialog box when completed.
.result
This saves the message configuration as specified and closes the
dialog.
.endstep
.*
.section Loading a Message Configuration
.*
.np
.ix '&spy' 'load message configuration'
.ix 'message configuration' 'load'
To load a previously saved message configuration, follow these
procedures:
.begstep
.step Choose Load Message Configuration
from the Messages menu.
.result
The Open dialog box appears.
.step Select the message configuration you previously saved.
.step Click on OK.
.result
The Open dialog box closes and &spy loads the selected message
configuration.
.endstep
.*
.endlevel
.*
.endlevel
.*
.section Spying
.*
.np
Once you have selected the messages you want &spy to monitor, you
select windows for &spy to watch.  This starts spying operations.
.*
.beglevel
.*
.section Start Spying
.*
.np
Message logging begins when you select a window to monitor. &spy
monitors the selected window for the messages specified. You can start
spying by selecting one window to spy on, or choosing to spy on all
windows.
.begstep To spy on all windows:
.step Choose All Windows
.ix '&spy' 'All Windows'
.ix 'All Windows'
from the Spy menu.
.result
&spy begins immediately monitoring all windows for the specified
messages. The caption bar specifies that you are monitoring all
windows.
.endstep
.begnote
.note Note:
You can use the All Windows menu item at any time, not just to start
spying.
If you begin spying by choosing one window, then want to monitor all
windows, choose All Windows from the Spy menu.
.endnote
.begstep To spy on one window:
.step Choose Select Window
.ix '&spy' 'Select Window'
.ix 'Select Window'
from the Spy menu.
.result
This minimizes the &spy window and opens the Select Window dialog
which prompts you to choose a window to monitor.
.figure *depth='2.46' *scale=100 *file='spy8' The Select Window dialog displays information for the window on which the cursor rests.
.step Position the cursor inside the icon in the dialog box.  Press the mouse button and hold it down.
.result
As you move the cursor across the screen, the window on which the
cursor is positioned has a highlighted border and information about
the selected window appears in the dialog box.
.step Position the cursor on the window you want to monitor.  Release the mouse button.
.begnote
.note Note:
You can select only one window at a time using the Select Window
dialog box.
.endnote
.step Click on OK
to save this window selection.
.result
This closes the Select Window dialog and returns you to the &spy
window. The caption bar indicates the window you are monitoring.
.endstep
.*
.section Spying on More Windows
.*
.np
Once you have started spying you can select additional windows to
monitor, unless you chose All Windows to start the spy process. You
select additional windows in the same way you select one window.
.ix 'Add Window'
.begstep
.step Choose Add Window
from the Spy menu.
.result
This minimizes the &spy window and opens the Select Window dialog.
.step Position the cursor inside the icon in the dialog box.  Press the mouse button and hold it down.
.result
Information about the currently selected window appears in the dialog.
.step Position the cursor on the window you want to monitor.  Release the mouse button.
.begnote
.note Note:
You can select only one window at a time using the Select Window
dialog box.
.endnote
.step Click on OK
to save this window selection.
.result
This closes the Select Window dialog and opens the &spy window. The
window heading specifies that you are monitoring multiple windows.
.endstep
.*
.section Stop Spying
.*
.np
.ix 'Done Spying'
Choose Done Spying from the Spy menu to stop spying on the selected
windows. When you choose this item, &spy does not retain the list of
windows selected for spying.
.*
.section Pause Spying
.*
.ix 'Stop Spying Temporarily'
.np
Choose Stop Spying Temporarily from the Spy menu to pause message
logging. When you choose this item, &spy retains the list of windows
selected for spying.
.np
The Stop Spying Temporarily menu item changes to Continue Spying when
the pause is active. Choose Continue Spying from the Spy menu to start
spying again. This starts spying and changes the menu item back to
Stop Spying Temporarily.
.*
.endlevel
.*
.section &msg_info
.*
.np
Double clicking on a message in the message list reveals a Message
Information dialog box which contains additional information about the
selected message. This dialog displays the message, the number of
times it has been issued, and the window that received the selected
instance of the message.
.figure *depth='3.62' *scale=100 *file='spy9' The Message Information dialog box displays additional information about the selected message.
.ix 'Message Information dialog'
.ix '&spy' 'Message Information dialog'
.np
From the Message Information dialog you can perform the following
actions:
.begbull $compact
.bull
Reset the usage count
.bull
Display Windows help for the message.
.bull
Change the monitoring state for the message.
.bull
Highlight the window that received the message.
.bull
Display information about the window that received the message.
.endbull
.*
.beglevel
.*
.section Resetting the Usage Count
.*
.np
You can reset the usage count for the message to zero by pressing
the Reset Count button on the Message Information dialog.  The usage
count will also be automatically reset to zero when you start spying
or clear the message list.
.*
.section Displaying Help for the Message
.*
.np
You can get more information about most messages, such as the
meaning of their parameters, by the Help for Message
button on the Message Information dialog.  This starts the
Windows help facility to display the information. Help is not
available for all messages.
.*
.section Changing the Monitoring State of the Message
.*
.np
The watch and stop on check boxes in the Message Information dialog
indicate if the message appears in the messages to watch or messages
to stop on lists.  You can add the message to either of these lists
by checking the appropriate check box or remove it by removing the
check.
.*
.section Highlighting the Receiving Window
.*
.np
If the window that received this message has not been destroyed you
can highlight it by selecting the Highlight Window button.
This changes the border &colour of the window so it is easy to locate.
.*
.section Displaying Information about the Receiving Window
.*
.np
If the window that received this message has not been destroyed you
can get additional information about it by selecting the Show Information
button. This opens a Window Information dialog that displays
information such as the window's class, style, size and position. For
more information on the Window Information dialog see the section
entitled :HDREF refid='viewinf'..
.*
.endlevel
.*
.im wguilog &spy
.*
.section *refid=viewinf Viewing Window Information
.*
.np
.ix 'window information' 'viewing in &spy'
.ix '&spy' 'view window information'
In &spy you can view different window information using items in the
Spy menu. You can view:
.begbull $compact
.bull
information about a specific window
.bull
a list of all windows open on your desktop
.endbull
.*
.beglevel
.*
.section *refid=wininfo &win_info
.*
.np
.ix 'Peek at Window'
.ix 'Window' 'Peek At in &spy'
To view information about a window:
.begstep
.step Choose Peek At Window
from the Spy menu.
.result
This minimizes the &spy window and opens the Peek At Window dialog
box.
:cmt. .figure *depth='1.xx' *file='spy12' The Peek at Window dialog displays information about the window on which the cursor rests.
.step Position the cursor inside the icon in the dialog box.  Press the mouse button and hold it down.
.result
As you move the cursor across the screen, the window on which the
cursor is positioned has a highlighted border and information about
the selected window appears in the dialog box. The following
information is displayed:
.begpoint $break
.point Window
the handle of the window and its title, if it has one
.point Class
the name of the window class to which this window belongs
.point Parent
the handle of the window's parent if it has one
.point Size
the screen coordinates of the top right and bottom left corners of the
window, followed by the window's size
.point Window Style
the window's style bits
.point Class Style
the style bits for the window's class
.point Child ID
a numeric identifier associated with the window when it is created
(This only appears if the selected window is a child window.)
.endpoint
.step Position the cursor on the window you want to monitor.  Release the mouse button.
.step Click on OK
to close the dialog box.
.result
This closes the Peek at Window dialog box and returns you to the
&spy window.
.endstep
.*
.section Listing All Windows
.*
.np
To view a list of all windows open on your desktop:
.begstep
.step Choose Show Selected Windows
from the Spy menu.
.ix 'Show Selected Windows'
.result
This opens the Window Selection dialog box which displays the
following:
.begbull $compact
.bull
window handles
.bull
window captions
.endbull
.figure *depth='3.20' *scale=87 *file='spy13' The Window Selection dialog displays a list of all windows currently open on the desktop.
.np
The information appears in a hierarchical manner. Windows with a parent
are listed, indented, directly below their parent. Windows currently
being monitored by &spy are marked with an asterisk.
.step
From this dialog you can perform the following functions:
.begbull $compact
.bull
Add a window for &spy to monitor
.bull
Delete a window that &spy is monitoring
.bull
Select all windows for monitoring
.bull
View information about a window in the list
.endbull
.endstep
.*
.beglevel
.*
.section Adding a Window
.*
.np
.ix 'window' 'adding in &spy'
.ix '&spy' 'adding a window'
You can add a window for &spy to monitor in two ways:
.begstep
.step Double click on the line
for the window you want to add, or
.step Click once on the line
for the desired window to select it and then select the Add button.
.endstep
.np
The window you added is now marked with an asterisk indicating that
&spy is monitoring that window.
.np
Click on OK to close the dialog and save the updated window list.
.*
.section Deleting a Window
.*
.np
.ix 'window' 'deleting'
.ix '&spy' 'deleting a window'
To delete a window that &spy is monitoring, do one of the following:
.begstep
.step Double click on the line
for the window you want to delete, or
.step Click once on the line
for the window that you no longer want &spy to monitor and then select
the Delete button.
.endstep
.np
This removes the asterisk from beside the window handle, indicating
that &spy is no longer monitoring that window.
.np
Click on OK to close the dialog and save the updated window list.
.*
.section Monitoring All Windows
.*
.np
To instruct spy to start spying on all windows open on the desktop,
check the "Spy on all windows" check box.  Then chose the OK button
to close the dialog and accept this selection.
.*
.section Viewing Information
.*
.np
.ix 'Show Info button' 'in &spy'
.ix '&spy' 'Show Info button'
To view information about a window in the list, do the following:
.begstep
.step Click once on the line
for the window whose information you want to view.
.result
The selected line appears highlighted in the list.
.step Select the Show Info button
on the dialog.
.result
The Window Information dialog box opens. The information in this
dialog is identical to the information in the Peek at Window dialog
box. Refer to the section entitled :HDREF refid='wininfo'. for a
description of the fields in this dialog.
.step Click OK.
.result
The Window Information dialog closes and you return to the Window
Selection dialog.
.step Click OK
on the Window Selection dialog to close it.
.endstep
.*
.endlevel
.*
.endlevel
.*
