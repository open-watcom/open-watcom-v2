:H3.Selecting Device Information
:P.
&WGML. must produce the output
for a specific device when a document is formatted.
Associated with a device is a default output file and a set of fonts.
The default output file is either a device (such as lpt1), a full file
name, or a file pattern (such as *.ps)
which specifies how to create the output
file name from the document name.
The set of fonts define the character sets and their attributes used
to produce the text in the document.
The fonts numbered zero through three are used by
the GML tags &gml.HP0 through &gml.HP3.
Font numbers greater than three may be referenced in the layout or
by the &gml.SF GML tag.
:NOTE.The device and font lists shown in the following examples may
not be the same as the lists shown on your screen.
As part of the &WGML. product installation, the devices and fonts
available at your site are selected.
Although the PostScript fonts are used in the following examples
to illustrate font selection, the operations performed are the same
for any device.
:P.
From the option screen,
select the :HP1.Device Information:eHP1. hotspot.
.screen 'frdev' 'Device Screen'
:P.
The first two areas specify the device name and default output file.
The rest of the screen specifies the fonts to be used in the document.
:NOTE.The previous screen has no fonts specified in the font area.
The fonts numbered zero through three are always defined by the
device.
If one or all of these fonts are not specified, the device defaults
are implied.
:P.
Press the &key.Tab&ekey. key and enter &mono.doc.ps&emono.\.
The output file name will now be 'doc.ps' for all document files processed.
Select &menu.Load device defaults&emenu. from the &menu.Device-Options&emenu.
menu.
.screen 'frdevld' 'Default device information'
:P.
The value of the output file area will be returned to the default
value.
The font area is also filled in with the default fonts for the device.
Selecting this menu item will delete any existing font definitions and
reset the font area to the defaults.
:H4.Selecting a New Device
:P.
You may enter a new device name in the device area or select
the :HP1.Device List:eHP1. hotspot.
If you select the device list function, a device browser will display
a list of available devices.
.screen 'frdevav' 'Device browser'
.choose
.mouse
Move the &ptr. pointer to the line with 'hplaserplus' and &op.\.
.keyboard
Press the &crsdn. key eight times and press the &g_enter. key.
.echoose
:P.
The device name HPLASERPLUS will be placed in the device area.
You are also asked if you want to set the device defaults.
In most cases you will press &g_enter. to reset the defaults.
.screen 'frdeval' 'Selecting a new device'
:H4.Select Device Fonts
:P.
Delete the name 'hplaserplus' from the device area, enter
the device name &mono.ps&emono., and press the &g_enter. key.
Press &g_enter. when asked about loading the defaults.
The default fonts for the device will be loaded into the font display area.
.choose
.mouse
Move the &ptr. pointer to the number area with a value of '1' and &op.\.
Select the :HP1.Font List:eHP1. hotspot.
.keyboard
Press the &key.Tab&ekey. three times followed by the &crsdn. key.
Select the :HP1.Font List:eHP1. hotspot.
.echoose
:P
.screen 'frdevfa' 'Available fonts'
:P.
The fonts available for the current device are displayed in a font browser.
.choose
.mouse
Move the &ptr. pointer to the line with 'helvetica' and &op.\.
.keyboard
Press the &crsdn. key twelve times and press &g_enter.\.
.echoose
:P.
The 'helvetica' font will be placed in the font name area, overwriting
the previous font name.
:P.
Move to the last font line and select the :HP1.Insert:eHP1. hotspot.
A new font line area will be created on the screen.
Enter the number four (4) and press the &key.Tab&ekey. key.
Use the font list function to select a font name for the new area.
Press the &key.Tab&ekey. and enter :HP1.uline:eHP1.\.
This will select the underlining attribute for the font.
The value :HP1.plain:eHP1. is usually used when a device has enough
variations in the available font style (for example, selecting an
italic font instead of underlining a roman font).
:P.
The next two columns specify the font :HP1.space:eHP1.
and :HP1.height:eHP1. values.
These values are numbers with up to two decimal places, and
represent point values.
A point is a unit of measurement used in typography.
&WGML. sets 72 points per inch.
:P.
The font space defines the amount of space between
lines of text which are in the font.
If no space was specified, the lines of text would be too close
together.
Small fonts (1 to 8 points) may require a one point space.
Medium fonts (9 to 13 points) may require a two point space.
Larger fonts may require three or more points of line space.
The amount of line space is adjustable to meet individual requirements.
Although this value can be entered for all devices,
it is usually applicable to
imaging printers such as a laser printer.
Line or matrix printers usually have a built in line space value.
:P.
The font height defines the height of the font characters.
This value may only be specified with :HP1.scaleable:eHP1. fonts.
Scaleable fonts are those for which you select a font name and
specify its height.
Non-scaleable fonts have a built in height value which cannot
be adjusted.
For example, all of the LaserJet device fonts are non-scaleable.
:P.
Enter the numbers two and eleven in the space and height columns.
The screen should appear as follows:
.screen 'frdevfn' 'Define a new font'
:P.
Positioning to a font line and selecting the :HP1.Delete:eHP1. hotspot
will delete a font from the list.
Pressing &g_enter. will accept the device and font changes and return
you to the options screen.
Pressing &g_escape. will return you to the options screen without
the changes.
