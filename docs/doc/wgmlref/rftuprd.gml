:H2.Processing Displays
:P.
A status screen is presented by &WGML. while it is processing
a document.
Accumulated statistics and information about the document section
currently being processed is displayed.
.screen 'wgmlmain' '&WGML. Status Screen'
:PC.
To stop document processing before WGML has completed formatting:
:I1.mouse, using
:I1.stopping &WGML.
:I1.break
.choose
.mouse
Move the &ptr. pointer to the &g_escape. area at the bottom left
corner of the screen and &op.\.
An error screen indicating that processing has been stopped will be displayed.
Point to the &g_enter. area at the bottom right corner of the screen
and &op.\.
.keyboard
Press the &g_escape. key.
Alternatively, pressing the &key.Ctrl&ekey. and &key.Break&ekey. keys
at the same time will stop document processing.
After pressing the key(s), an error screen will be displayed.
Press the &g_enter. key to continue.
.echoose
:NOTE.&WGML. can be used with or without a &ptr. pointer device.
Throughout this chapter both methods are illustrated
using the style shown above.
If you are using a &ptr., read the paragraph marked "Mouse:",
otherwise, read the paragraph marked "Keyboard:".
:H3.Display Areas
:P.
The top third of the screen displays the
status of the document being processed.
Each information area is updated to
indicate the part of the document &WGML. is processing.
:P.
The middle third of the screen displays document statistics.
When processing is completed, this area will show the statistics
for the entire document.
The message 'Text lines produced' reflects the number of lines in
the formatted document which are not blank.
The message 'Output records' reflects the number of records sent
to the output device.
With some devices, many formatted document lines may be sent to
the device in one output record.
:P.
The bottom third of the screen displays &WGML. messages.
These messages are warnings about possible
errors in the source document.
:H3.Error Screen
:P.
When an error is detected in the GML document,
an error screen of the following form is displayed:
.screen 'wgmlerr' '&WGML. Error Screen'
:P.
In this example the ending tag of a definition list is missing.
&WGML. shows where in the document source the error occurs and
at what point in the document the starting tag (&gml.DL in this case)
was specified.
After reading the message:
.choose
.mouse
Move the &ptr. pointer to the &g_enter. area at the bottom right
corner of the screen and &op.\.
.keyboard
Press the &g_enter. key.
.echoose
