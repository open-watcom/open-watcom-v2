.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 03-aug-92	Craig Eisler	initial draft
.*
:CHAPTER id=winmenu.Windows and Menus
This chapter describes the &cmdline
commands devoted to configuring
&edname.'s windows and menus.  All windows are fully configurable:
dimension, colors, existence of borders.  The menus are fully configurable:
all menu topics and menu items are user settable.
:P.
A window is configured first by specifying a window.  Once the
a window is specified, a number of properties may be set.  These properties
are described in the following section.
:SECTION.Window Properties
Some of the following &cmdline
commands accept colors as parameters.
These colors may be numbers in the range 0 through 15.  As well,
&edname has symbolic names for these colors, they are:
:UL compact.
:LI.black
:LI.blue
:LI.green
:LI.cyan
:LI.red
:LI.magenta
:LI.brown
:LI.white
:LI.dark_gray
:LI.light_blue
:LI.light_green
:LI.light_cyan
:LI.light_red
:LI.light_magenta
:LI.yellow
:LI.bright_white
:eUL

:FNLIST.Window Properties
:FUNC.BORDER
:SYNTAX.* BORDER <hasbord> <fg_clr> <bg_clr>
This command specifies the type of border and its colors.
:BLANKLINE.
&parm1 describes the type of border: if &parm1 is set to -1,
there is no border, if &parm1 is set to 1, there is a border.
:BLANKLINE.
&parm2 is used to specify the foreground color of the border (0-15).
It is ignored for &parm1 of -1.
:BLANKLINE.
&parm3 is used to specify the background color of the border (0-15).
It is ignored for &parm1 of -1.
:EXAMPLE.border -1
The currently selected window has no border.
:EXAMPLE.border 1 yellow black
The currently selected window has a yellow border with a black background.
:eFUNC.

:FUNC.DIMENSION
:SYNTAX.* DIMENSION <x1> <y1> <x2> <y2>
(&parm1, &parm2) specifies the coordinates of the upper left-hand corner
of the window, and (&parm3, &parm4) specifies the coordinates of the
lower right-hand corner of the window.
:BLANKLINE.
&edname editor automatically senses the number of lines and columns 
available.  The global variables %(SW) (screen width) and %(SH)
(screen height) are always set.  These are useful for coding
dimensions that are relative to the size of the screen.
All parameters may be coded as expressions.
:BLANKLINE.
All coordinates are 0-based.  The top left corner of the
screen is (0,0).  The bottom right corner of the screen is
(%(SW)-1,%(SH)-1).
:EXAMPLE.dimension 0 1 %(SW)-1 %(SH)-3
Makes the currently selected window the full width of the screen.
Its y dimensions are from the second line of the screen and
to the third last line of the screen.
:EXAMPLE.dimension %(SW)-10 5 %(SW)-1 10
Makes the currently selected window's x dimension start at the tenth
column from the right of the screen and end at the rightmost column of the
screen. Its y dimensions range from the fifth line to the tenth line of
the screen.
:eFUNC.

:FUNC.ENDWINDOW
:SYNTAX.* ENDWindow
Ends entry of properties for the currently selected window.  The currently
selected window is then redrawn with the new properties, if it
was previously visible.
:eFUNC.

:FUNC.HILIGHT
:SYNTAX.* HILIGHT <fg_clr> <bg_clr>
&parm1 sets the foreground color and &parm2 sets the background color
of highlighted text in the currently selected window.
:EXAMPLE.hilight bright_white black
Sets the highlighted color of the currently selected window to be bright white
text with a black background.
:eFUNC.

:FUNC.TEXT
:SYNTAX.TEXT <fg_clr> <bg_clr>
&parm1 sets the foreground color and &parm2 sets the background color
of text in the currently selected window.
:EXAMPLE.text white black
Sets the text color of the currently selected window to be white
text with a black background.
:eFUNC.
	 
:eFNLIST.

:SECTION.Window Types

This section describes all possible windows that may be selected.  These
&cmdline commands select the window to start setting properties.
See the previous section for properties that may be set.

:FNLIST.Window Types
:FUNC.COMMANDWINDOW
:SYNTAX.* COMMANDWindow
This is the window that is displayed whenever &edname is prompting
for a &cmdline,
a search string, a filter command or any other command or data.
:eFUNC.

:FUNC.COUNTWINDOW
:SYNTAX.* COUNTWindow
Window that opens when repeat counts are entered.  This window is
disabled if
:KEYWORD.repeatinfo
is not set.
:eFUNC.

:FUNC.DEFAULTWINDOW
:SYNTAX.* DEFAULTWindow
When the
:KEYWORD.defaultwindow
is selected, all windows get the properties that are set.
Note that this is best defined first to provide the default behaviour,
and all other windows that you want to be different from the
default can be specified after.
:eFUNC.

:FUNC.DIRWINDOW
:SYNTAX.* DIRWindow
This is the window that you select a file from whenever no file
is specified from the &cmdline
commands
:KEYWORD.edit
or
:KEYWORD.read
:PERIOD.
:eFUNC.

:FUNC.EDITWINDOW
:SYNTAX.* EDITWindow
This is the window that files are edited from.
:eFUNC.

:FUNC.EXTRAINFOWINDOW
:SYNTAX.* EXTRAINFOWindow
This window contains extra information about possible things that
may be done.
It is displayed when selecting results from the &cmdline commands
:KEYWORD.fgrep
:CONT.,
:KEYWORD.egrep
:CONT.,
and
:KEYWORD.files
:PERIOD.
:eFUNC.

:FUNC.FILEWINDOW
:SYNTAX.* FILEWindow
The window in which list of files currently being edited is displayed.
This is displayed when the &cmdline command
:KEYWORD.files
is executed.
:eFUNC.

:FUNC.FILECWINDOW
:SYNTAX.* FILECWindow
This window contains list of possible file choices when
file completion cannot match one specific file.  It is displayed
whenever the
:HILITE.TAB
key is pressed in a command window.
:eFUNC.

:FUNC.LINENUMBERWINDOW
:SYNTAX.* LINENUMBERWindow
This is the window that line numbers are displayed in.
The dimension of this window is ignored, the absolute position of the window
is decided by the position of the edit window that it is associated
with. However, the width of this window may be set using the
:KEYWORD.linenumwidth
setting.
:BLANKLINE.
This window is disabled if
:KEYWORD.linenumbers
is not set.
:eFUNC.

:FUNC.MENUWINDOW
:SYNTAX.* MENUWindow
This sets the properties of the windows that open whenever
a menu is activated.  Any
:KEYWORD.dimension
given with this window is ignored; the position of each
menu is variable.
:eFUNC.

:FUNC.MENUBARWINDOW
:SYNTAX.* MENUBARWindow
This window is the menu bar.
This is where all menus set up with the &cmdline
:KEYWORD.menu
are displayed.
:eFUNC.

:FUNC.MESSAGEWINDOW
:SYNTAX.* MESSAGEWindow
The window in which all all editor feedback is reported. 
The
:KEYWORD.hilight
color is the color that errors are reported in.
:BLANKLINE.
This window needs to have two lines in order to view
&edname feedback;  however, very few messages use two lines,
so you can get by with a message window that is only one line high.
:eFUNC.

:FUNC.SETWINDOW
:SYNTAX.* SETWindow
The window in which &edname settings are displayed.
This window is displayed when the &cmdline
:KEYWORD.set
command is entered without parameters or with
:HILITE.2
as its only parameter.
:eFUNC.

:FUNC.SETVALWINDOW
:SYNTAX.* SETVALWindow
The window in which the new value of an editor setting is entered.
This window is displayed if a value change is requested
after entering the &cmdline
:KEYWORD.set
with
:HILITE.2
as the parameter.
:eFUNC.

:FUNC.STATUSWINDOW
:SYNTAX.* STATUSWindow
This is the window where the current line and column are reported.
The current line number is displayed in the first line of the window,
and the current column is displayed in the second line of the window.
This window is disabled if
:KEYWORD.statusinfo
is not set.
:eFUNC.
:eFNLIST.

:SECTION.Sample Window Settings

The following examples are sample settings of the different types of
windows. These commands may be issued from the &cmdline
one at at time, or may be executed from a &edname
:KEYWORD.script
:PERIOD.
Typically, these commands will be found in the &edname
:KEYWORD.configuration script
:PERIOD.
:KEYWORD.Commandwindow
:CONT.:
:P.
:ILLUST.
commandwindow
   text yellow light_blue
   border 1 bright_white light_blue
   dimension 2 %(SH)-7 %(SW)-3 %(SH)-5
endwindow
:eILLUST.
    
:P.
:KEYWORD.Countwindow
:CONT.:
:ILLUST.
countwindow
    dimension 28 %(SH)-5 43 %(SH)-3
    border 1 yellow red
    text bright_white red
endwindow
:eILLUST.
    
:P.
:KEYWORD.Dirwindow
:CONT.:
:ILLUST.
dirwindow
    dimension 15 2 %(SW)-14 %(SH)-7
    border 1 bright_white light_red
    text yellow light_red
    hilight bright_white light_blue
endwindow
:eILLUST.

:P.
:KEYWORD.Editwindow
:CONT.:
:ILLUST.
editwindow
    border 1 bright_cyan blue
    dimension 0 1 %(SW)-1 %(SH)-3
    text white blue
    hilight light_red blue
endwindow
:eILLUST.

:P.
:KEYWORD.Extrainfowindow
:CONT.:
:ILLUST.
extrainfowindow
    border 1 bright_white light_green
    text bright_white light_green
endwindow
:eILLUST.
    
:KEYWORD.Filecwindow
:CONT.:
:ILLUST.
filecwindow
   border 1 yellow white
   dimension 4 8 %(SW)-5 %(SH)-8
   text black white
endwindow
:eILLUST.

:P.
:KEYWORD.Filewindow
:CONT.:
:ILLUST.
filewindow
    border 1 bright_white red
    text bright_white red
    hilight black yellow
    dimension 26 2 %(SW)-2 %(SH)-7
endwindow
:eILLUST.

:P.
:KEYWORD.Messagewindow
:CONT.:
:ILLUST.
messagewindow
    border -1
    dimension 8 %(SH)-2 %(SW)-1 %(SH)-1
    text white light_blue
    hilight blue light_blue
endwindow
:eILLUST.

:P.
:KEYWORD.Menuwindow
:CONT.:
:ILLUST.
menuwindow
    border 1 bright_white light_red
    text yellow light_red
    hilight bright_white light_blue
endwindow
:eILLUST.
    
:P.
:KEYWORD.Menubarwindow
:CONT.:
:ILLUST.
menubarwindow
    text bright_white white
    hilight yellow white
endwindow
:eILLUST.

:KEYWORD.Setvalwindow
:CONT.:
:ILLUST.
setvalwindow
    border 1 bright_white light_red
    text yellow light_red
    dimension 46 6 73 9
endwindow
:eILLUST.
    
:P.
:KEYWORD.Setwindow
:CONT.:
:ILLUST.
setwindow
    border 1 bright_white light_blue
    dimension 12 2 43 %(SH)-4
    text yellow light_blue
endwindow
:eILLUST.
    
:P.
:KEYWORD.Statuswindow
:CONT.:
:ILLUST.
statuswindow
   border -1
   dimension 0 %(SH)-2 7 %(SH)-1
   text black white
endwindow
:eILLUST.

:SECTION id='menucmd'.Menu Commands
&edname menus are set dynamically.  Any menu will
automatically be added to the menu bar when created, and removed when
destroyed.  There are some reserved menus:

:DEFLIST.
:DEFITEM.windowgadget
This menu is the one that appears when the upper left hand
corner of an edit window is clicked.
:DEFITEM.float<0-3>
These are floating (popup) menus.  They are made to appear when
the
:KEYWORD.script command
:KEYWORD.floatmenu
is used.
There are 4 floating menus,
:HILITE.float0
through
:HILITE.float3
:PERIOD.
:eDEFLIST.
:P.
Menus will attempt to open up where they are told to; however, if the menu
cannot fit, then it will try to open above the position that it was told
to, to the left of the position that it was told to, and above and to the
left of the position that it was told to.
:P.
The following sections describe each of the &cmdline
commands for controlling menus.

:FNLIST.Menu Commands

:FUNC.ADDMENUITEM
:SYNTAX.* ADDMENUItem <menuname> <itemname> <cmd>
Adds a new item to a previously created menu &parm1.
:PERIOD.
The item &parm2 is added to the menu.
:BLANKLINE.
This command is similar to the &cmdline command
:KEYWORD.menuitem
:CONT., only the
:KEYWORD.menuitem
command is used when first defining a menu.
:BLANKLINE.
&parm1 may be a quoted string, if the parameter
contains spaces. If &parm1 is specified as "", then
a solid bar is displayed in the menu.
:BLANKLINE.
A character in &parm2 preceded with an '&' will be the hot key
for activating the menu, e.g. &Control would have 'C' as
the hot key.
:BLANKLINE.
The command &parm3 may be any &edname &cmdline
command, and is run whenever the item is selected.
:SEEALSO.
:SEE.menuitem
:eSEEALSO.
:eFUNC.

:FUNC.DELETEMENU
:SYNTAX.* DELETEMENU <menuname>
Destroys menu with name &parm1.
:PERIOD.
:eFUNC.

:FUNC.DELETEMENUITEM
:SYNTAX.* DELETEMENUItem <menuname> <index>
Deletes item number &parm2 from menu &parm1.
:PERIOD.
The item number &parm2
is 0 based.
:BLANKLINE.
If &parm2 is specified as -1, then the last item
is removed from the menu.
:eFUNC.

:FUNC.ENDMENU
:SYNTAX.* ENDMENU
Finishes the creation of a new menu.
:SEEALSO.
:SEE.menu
:eSEEALSO.
:eFUNC.

:FUNC.MENU
:SYNTAX.* MENU <menuname>
Starts the creation of a new menu &parm1.
:PERIOD.
If a menu exists with the name already, it is destroyed and re-created.
A character preceded with an ampersand ('&') will be the hot key
for activating the menu, e.g. &Control would have 'C' as
the hot key.
:BLANKLINE.
&parm1 may be a reserved name: windowgadget, float0,
float1, float2, or float3
:SEEALSO.
:SEE.endmenu
:SEE.menuitem
:eSEEALSO.
:eFUNC.

:FUNC.MENUITEM
:SYNTAX.* MENUITEM <itemname> <cmd>
Adds the item &parm1 to the last menu started with the
:KEYWORD.menu
command.   Menu items may be added until the &cmdline command
:KEYWORD.endmenu
has been issued.
:BLANKLINE.
&parm1 may be a quoted string, if the parameter
contains spaces. If &parm1 is specified as "", then
a solid bar is displayed in the menu.
:BLANKLINE.
A character in &parm1 preceded
with an ampersand ('&') will be the hot key for activating the menu
item, e.g. &Exit would have 'E' as the hot key.
:BLANKLINE.
The command &parm2 may be any &edname &cmdline
command, and is run whenever the item is selected.
:eFUNC.
:SEEALSO.
:SEE.addmenuitem
:SEE.endmenu
:SEE.menu
:eSEEALSO.
:eFNLIST.

:SECTION.Sample Menus

The following examples are sample menu setups.
These commands may be issued from the &cmdline
one at at time, or may be contained in a
:KEYWORD.script
and executed in the
:KEYWORD.script
:PERIOD.
Typically, these commands will be found in the &edname
:KEYWORD.configuration script
:PERIOD.
:P.
This configures the menu that is popped up whenever the top left-hand
corner of an edit window is clicked with the mouse.
:ILLUST.
menu windowgadget
    menuitem &$AMP.Maximize maximize
    menuitem M&$AMP.inimize minimize
    menuitem ""
    menuitem "&$AMP.Save &$AMP. close" keyadd ZZ
    menuitem "Close &$AMP.no save" quit!
    menuitem &$AMP.Close quit
endmenu
:eILLUST.
:P.
This configures one of the floating (popup) menus.  This menu could
be displayed whenever a word is selected with the mouse.
:ILLUST.
menu float0
    menuitem "&$AMP.Open" edit %1
    menuitem ""
    menuitem "&$AMP.Change" keyadd cr
    menuitem "&$AMP.Delete" keyadd dr
    menuitem "&$AMP.Yank" keyadd yr
    menuitem ""
    menuitem "&$AMP.Fgrep" fgrep "%1"
    menuitem "&$AMP.Tag" tag %1
endmenu
:eILLUST.
:P.
This configures a menu bar menu item called Control.
:ILLUST.
menu &$AMP.Control
    menuitem "&$AMP.Tile windows" tile
    menuitem "&$AMP.Cascade windows" cascade
    menuitem "&$AMP.Reset windows" tile 1 1
    menuitem ""
    menuitem "Set &$AMP.booleans" set
    menuitem "Set &$AMP.other" set 2
    menuitem ""
    menuitem "&$AMP.Enter command" keyadd :
    menuitem &$AMP.System shell
    menuitem E&$AMP.xit quitall
endmenu
:eILLUST.

:P.
This configures a menu bar menu item called File.
:ILLUST.
menu &$AMP.File
    menuitem "&$AMP.File list" file
    menuitem "&$AMP.Next file" next
    menuitem "&$AMP.Edit new file" edit
    menuitem "&$AMP.Read file" read
    menuitem "&$AMP.Write current" write
    menuitem "&$AMP.Save and exit" keyadd ZZ
endmenu
:eILLUST.

:P.
This configures a menu bar menu item called Edit.
:ILLUST.
menu &$AMP.Edit
    menuitem "&$AMP.Start region" keyadd 
    menuitem "&$AMP.Delete region" keyadd dr
    menuitem "&$AMP.Copy (yank) region" keyadd yr
    menuitem "&$AMP.Paste (put)" put
    menuitem ""
    menuitem "&$AMP.Insert Text" keyadd i
    menuitem "&$AMP.Overstrike Text" keyadd R
    menuitem ""
    menuitem &$AMP.Undo undo
    menuitem &$AMP.Redo undo!
endmenu
:eILLUST.

:P.
This configures a menu bar menu item called Position.
:ILLUST.
menu &$AMP.Position
    menuitem "&$AMP.Start of file" 1
    menuitem "&$AMP.End of file" $
    menuitem "Line &$AMP.number" so lnum._vi
    menuitem ""
    menuitem "S&$AMP.tart of line" keyadd 0
    menuitem "En&$AMP.d of line" keyadd $
    menuitem ""
    menuitem "Search &$AMP.forwards" keyadd /
    menuitem "Search &$AMP.backwards" keyadd ?
    menuitem "&$AMP.Last search" keyadd n
    menuitem "&$AMP.Reverse last search" keyadd N
endmenu
:eILLUST.

:P.
This configures a menu bar menu item called Help.
:ILLUST.
menu &$AMP.Help
    menuitem "&$AMP.Command Line" help com
    menuitem "&$AMP.Key Strokes" help keys
    menuitem "&$AMP.Regular Expressions" help reg
    menuitem "&$AMP.Scripts" help scr
    menuitem "S&$AMP.ettings" help set
    menuitem "Starting &$AMP.Up" help start
endmenu
:eILLUST.
