:H3.Selecting/Editing a Document
:P.
Selecting the :HP1.Select a different document:eHP1. menu
item from the :HP1.Document:eHP1. menu will display
on the screen a file browser for GML source files.
.screen 'frdocse' 'Select Document file browser'
:P.
The file browser presents you with an area at the top of the browser
where you may enter a new name.
The available files are shown in a list, with the file extension of 'gml'
being used to selectively display the files in the directory.
If your files have an extension other than 'gml', you may edit
the file pattern to specify a different pattern (such as *.doc) by pressing
the &f3. key.
You may also choose a new file from the list displayed on the screen.
.choose
.mouse
Move the &ptr. pointer to the appropriate file name and &op.\.
If there are more source files than can be displayed at one time,
&op. on the bar along the right side of the browser screen to
view the other files.
.keyboard
Use the &key.Tab&ekey. and cursor keys to select the appropriate file
name.
The file name area at the top of the browser will be updated to show
the currently selected file.
The cursor down (&crsdn.) and cursor up (&crsup.)
keys may be used to view files beyond the browser window.
.echoose
:PC.
After entering or selecting the appropriate file name, press &g_enter.
to place the name in the document file name area.
:P.
Selecting the :HP1.Edit Document:eHP1. hotspot will also bring up the file
browser.
The name area of the browser will contain the
name of the last edit file (or the main document name if
you are using the edit function for the first time).
Pressing &g_enter. after selecting a file name will edit the file.
If you are using the WATCOM Editor, entering &mono.exit&emono. will
save the file and return you to &WGML..
:P.
A number of different select operations will use the file browser to obtain
a file name.
Selecting a file is performed in the same way for each case.
:H3.Printing a Document
:P.
An output file is usually created as the document is processed.
By selecting the :HP1.Print a document file:eHP1. menu item from
the :HP1.Control:eHP1. menu, the document may be reprinted at a later time.
.screen 'frdocpr' 'Print a Document'
:P.
The name area of the browser will contain the name of the output file
for the currently selected document.
Your may enter or select from the browser file list another name for printing.
Pressing the &g_enter. key will
copy the file to the printer.
:NOTE.Some network systems require a separate command to actually send the
file to the printer.
You must issue this command yourself by selecting &menu.DOS Services&emenu.
from the &menu.Control&emenu. menu.
This will place you in DOS where you can issue the command.
Entering the &mono.exit&emono. command will return you to &WGML..
See :HDREF refid='frconf'. for information on configuring the print action.
