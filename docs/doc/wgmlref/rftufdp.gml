:P.
The main document screen is displayed when WGMLUI is first started.
There is one input area where you may enter the name of the GML
source document.
In the following document screen,
the file name &mono.manual&emono. has been entered.
.screen "frdoc" "Main Document Screen"
:PC.
At this point you can process the document with the selected options.
:I1.hotspots
.choose
.mouse
Point to the highlighted area near the bottom of the screen labeled
'F7=Format and Print Document' and &op.\.
.keyboard
Press the &key.F7&ekey. key.
.echoose
:NOTE.The highlighted areas with labels such as
'F7=Format and Print Document' are called :HP1.hotspots:eHP1..
The actions indicated by the hotpots can be activated by either selecting
on the hotspot with a mouse pointer, or by pressing the key
indicated by the label.
Further references to hotspots in this document will be of the form
'select the :HP1.Format and Print Document:eHP1. hotspot'.
:P.
The formatted document is directed to a specific output location.
This may be a physical device such as the printer or terminal screen,
or a file on the disk.
The device selected for this tutorial is the PostScript printer, which
directs the output to a file with the same name as the source document and
a &mono.PS&emono. file type.
In the previous example, the disk file &mono.manual.ps&emono. would contain
the formatted output.
:P.
Pressing the &f7. key causes the document to be formatted.
If there are no errors, the resulting document file is printed.
If the printing process seems to take a very long time,
the most common reason is that a printer is not connected to your computer.
In most cases, an error will be reported after approximately ninety seconds.
:H3.Obtaining Help
:P.
While you are learning WGMLUI,
:IH1.WGMLUI program
:I2.help
the most useful feature
of the system will probably be the help facility.
Pressing the &key.F1&ekey. key will give you help on the area of the screen
which is currently selected.
Pressing the &key.F1&ekey. key again will tell you how to use the help facility.
After pressing the &key.F1&ekey. key twice you will see the following screen:
.screen "frhelp" "Help about the Help Facility"
:P.
Press the &g_escape. key to remove the help screen.
Note that the bottom line on the screen contains an area displaying
a hint on how to proceed.
The hint line is updated as you select different areas on the screen.
:P.
Help on the current area may also be obtained
by selecting &menu.Help&emenu. from the &menu.Control&emenu. menu.
The menu bar appears on the top line of the screen.
Each menu name can be 'pulled down' to select from a number of
menu item choices.
.choose
.mouse
Point to the &menu.Control&emenu. menu and press the &ptr. &bt.\.
&udr. the &ptr. to the &menu.Help&emenu. item and release the &bt.\.
Pressing the &g_escape. key will return you to the document screen.
.keyboard
While pressing the &key.Alt&ekey. key, pressing the &key.C&ekey. key
will pull down the &menu.Control&emenu. menu.
Use the cursor keys to select the &menu.Help&emenu. item and press
&g_enter.\.
Pressing the &g_escape. key will return you to the document screen.
:P.
While pressing the &key.Alt&ekey. key,
pressing the highlighted letter of a menu name
will pull down that menu.
.echoose
:H3.Control Menu
:P.
All of the screens displayed by WGMLUI have &menu.Control&emenu. as
the first menu on the menu bar.
The &menu.Help&emenu. menu item was described in the previous section.
.screen "frdocct" "Control menu"
:P.
Selecting the &menu.DOS services&emenu. menu item will place you in the DOS
environment.
Enter the &mono.exit&emono. command to return from DOS.
The &menu.Configure&emenu. menu item is selected when you wish to change
some of the default values used by the WGMLUI program, and is discussed
in more detail in a later section (see :HDREF refid='frconf'.).
The &menu.About...&emenu. menu item displays some information about
the WGMLUI program.
Selecting &menu.Quit&emenu. is the same as pressing the &g_escape. key,
except while in the main document screen.
Selecting &menu.Quit&emenu. in the main screen will exit the WGMLUI program.
