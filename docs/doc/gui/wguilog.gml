.*
.section &*: Working with Log Files
.*
.np
By writing the information in the &msg_list to a log file, you have a
file you can look at in case a program you are running terminates your
applications. In such a situation, your &msg_list is lost so you have
no information to refer to when debugging. Writing to a log file also
allows you to print the output later or annotate the text file
on-line.
.*
.beglevel
.*
.section &*: Configuring the Log File
.*
.np
.ix '&appname' 'configure log file'
.ix 'Configure Log'
Choosing Configure Log from the File menu opens the Log Configuration
dialog where you customize &appname.'s behaviour when it logs to a
file. This dialog contains four sections where you specify
information.
.keep 20
.figure *depth='3.07' *scale=92 *file='spy10' In the Log Configuration dialog you set the desired behaviour for &appname when logging to a file.
.*
.beglevel
.*
.section &*: Default Log Name
.*
.np
.ix 'Default Log Name'
.ix 'Log' 'Default Name in &appname'
In the Default Log Name field, specify the file to which &appname
writes when you begin logging. Clicking the Browse button next to this
field opens a Choose Log Name dialog that lets you browse your file
structure while choosing the log name.
.*
.section &*: Log Name
.*
.np
The settings in the Log Name section of the dialog indicate to
&appname how the name of the log file should be determined when the
Log to File menu item is chosen.
.np
.ix 'Query for Log Name'
.ix 'Choose Log Name dialog'
When the Query for Log Name option is selected, &appname presents a
prompt each time you select Log to File from the File menu or the
toolbar. This reveals a Choose Log Name dialog where you specify the
file to which &appname writes the log information.
.np
When the Use Default Log Name option is selected, &appname writes the
log information to the file indicated in the Default Log Name field in
the Log Configuration dialog. No prompt appears.
.*
.section &*: Action When Log File Exists
.*
.np
.ix 'Log Configuration dialog'
The options in the Action When Log File Exists section of the Log
Configuration dialog dictate how &appname behaves when you choose Log
to File and a log file already exists.
.np
.begpoint
.point Replace
&appname writes over the existing log file. All information in the
existing file is lost.
.point Append
&appname adds the new information to the end of the existing log file.
.point Query
&appname prompts you with a message box that allows you to choose a
course of action&mdash.Replace, Append, or Cancel.
.endpoint
.keep 14
.figure *depth='2.14' *scale=100 *file='spy11' The Log Information message box appears when logging if the Query option is selected on the Log Configuration dialog.
.*
.endlevel
.*
.section &*: Start Logging
.*
.np
.ix 'Logging' 'start in &appname'
.ix '&appname' 'start logging'
Choosing Log to File from the File menu tells &appname to write all
new information added to the &msg_list to the log file. Depending on
the options selected in the Log Configuration dialog, the following
dialogs may appear when you choose Log to File:
.begbull
.bull
Choose Log Name dialog&mdash.On this dialog you specify the file to
which &appname writes the log information.
.bull
Log Information dialog&mdash.On this dialog you choose a course of
action for logging when the specified log file already exists.
.endbull
.begnote
.note Note:
Choosing Cancel on either of these dialogs closes the dialog and
prevents logging from starting.
.endnote
.*
.section &*: Logging Type
.*
.np
There are two ways to write information to the log file:
.ix 'Logging' 'continuous and periodic'
.ix '&appname' 'continuous and periodic logging'
.begbull $compact
.bull
Continuous Logging
.bull
Periodic Logging
.endbull
.np
If the Continuous Logging option is activated, then as &appname writes
information to the &msg_list, it simultaneously writes to a log file.
.np
When the Periodic Logging option is activated, &appname writes
information to the log file only after a reasonable amount of
information, as determined by the application, has been recorded in
the &msg_list.
.np
Periodic logging is more convenient because it is faster than
continuous logging. However, if your system goes down, some
information will be lost. By simultaneously writing to the log file
and the message list, continuous logging allows you to store all
information in a log file without the chance of losing information.
.*
.section &*: Stop Logging
.*
.np
.ix 'Logging' 'stop in &appname'
.ix '&appname' 'stop logging'
Once the log is started, &appname continues writing to the log file
until you stop logging. A check mark beside the Log to File item in
the File menu indicates that a log is being created. Choose this item
again to stop logging.
.*
.section &*: Pause Logging
.*
.np
.ix 'Logging' 'pause in &appname'
.ix '&appname' 'pause logging'
To temporarily stop writing information to the log file you can choose
the Pause Log item from the File menu. A check mark appears beside the
item indicating that the log is currently paused. To restart logging,
choose this item again from the File menu.
.*
.endlevel
.*
